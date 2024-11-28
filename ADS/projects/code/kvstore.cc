#include "kvstore.h"
#include <string>

KVStore::KVStore(const std::string &dir) : KVStoreAPI(dir)
// 初始化的工作：
// 1.从磁盘文件里读取sstables 把sstables里的数据缓存到内存中 三种策略（不缓存 缓存索引 缓存bf和索引）
// 2.初始化vlog的路径 并读取vlog的头尾信息 不读取vlog文件 只有当查询或者插入的时候缓存中的sstables命中了才会去vlog中查询 减少了磁盘IO
// 3.初始化memtable
{

	this->dir = dir;
	this->timeStamp = 0;
	uint64_t maxtime = 0;
	sstables = std::vector<std::vector<SSTable *>>(sst_level);
	// 读取sstables 缓存到内存中
	if (!utils::dirExists(dir)) // 如果目录不存在，创建目录
		utils::_mkdir(dir);

	for (int i = 0; i < sst_level; i++)
	{
		// level-i目录路径
		std::string level_dir = dir + "/level-" + std::to_string(i);
		// 如果目录不存在，创建目录
		if (!utils::dirExists(level_dir))
			utils::_mkdir(level_dir);
		// 扫描目录下的所有sst文件
		std::vector<std::string> sst_files;
		utils::scanDir(level_dir, sst_files);
		// 读取sst文件
		for (auto file : sst_files)
		{
			if (file.substr(file.size() - 4, 4) == ".sst")
			{
				std::string fullpath = level_dir + "/" + file;
				SSTable *sst = new SSTable(fullpath);		   // 读取sst文件更新到缓存里
				sstables[i].push_back(sst);					   // 放入第i层
				maxtime = std::max(maxtime, sst->header.time); // 这一步之前漏了 沟槽的调试了我半天不知道gc出现了什么bug 结果是这里的maxtime没有更新
			}
		}
	}
	timeStamp = maxtime; // 更新时间戳为所有sstables中的最大时间戳 即恢复之前存储的最大时间戳

	// 初始化vlog路径 并读取vlog的头尾信息
	vlog_path = dir + "/vlog";
	vlog = new VLog(vlog_path);
	// 初始化memtable
	memtable = new Memtable();
}

KVStore::~KVStore()
// 析构函数的工作：
// 1.把memtable的数据写入vlog文件和sst文件持久化存储
// 2.释放memtable的内存
{
	// 如果memtable不为空，将memtable的数据写入sst文件
	if (memtable->head->next[0])
		memtable2sst();

	// 释放memtable的内存
	delete memtable;
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
// 插入的工作：
// 1.尝试在memtable中插入 如果memtable大小没有超过阈值 直接插入（如果存在key 则覆盖）
// 2.如果超过sst文件的大小限制，那么先将memtable转换为entry并写入vlog文件，同时根据offset写入sst文件以及缓存
// 3. 如果level0的sst文件数目超过了level0的最大文件数目，那么进行合并
// 4.最后重新初始化memtable 然后插入新的key value
{
	// checksize 严格来说 要检查是覆盖还是插入 如果是覆盖是不改变size的
	// 现在的实现会导致每一个文件的上限是sst_max_size-20 byte
	if (memtable->size + 20 <= sst_max_size)
	{
		memtable->put(key, s);
		return;
	}
	// memtable大小超过阈值，首先将memtable转换为vlog entries并获取offset 以及把sst写入缓存
	memtable2sst();

	// 如果level0的sst文件数目超过了level0的最大文件数目，那么进行合并
	compaction();

	// 最后重新初始化memtable 然后插入新的key value
	memtable->reset();
	memtable->put(key, s);
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
// 查询的工作：
// 1.先在memtable中查找
// 2.再在缓存的sstables中查找　每一个sst根据bf和key范围判断是否存在key以节省时间
// 3.如果在sst中找到了key 则根据offset在vlog中查找value
// 4.如果没找到 返回空字符串
{
	// 先在memtable中查找
	value_t result = memtable->get(key);
	if (result == delete_tag) // 如果是删除标记，返回空字符串
		return "";
	if (result != "") // 如果在memtable中找到了key
		return result;

	/* assert(result == "") */
	// 否则就是memtable里不存在key 在缓存的sstables中查找 对所有sst文件进行遍历 包括每一层
	findinSSTables(key, result);

	// 也有可能是没找到 返回空字符串 result == ""
	// 或者说找到了 返回result 两者可以合并
	return result;
}

/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
// 删除的工作：
// 1.先在memtable中查找 如果找到了key 则插入删除标记
// 2.如果没找到 返回false
{
	if (get(key) == "") // 如果key不存在
		return false;
	put(key, delete_tag); // 否则插入删除标记
	return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
// 重置kvstore的工作：
// 1.清空memtable
// 2.删除所有sstables文件、vlog文件
// 3.清除sstables的缓存
// 4.重置时间戳
// 5.重新初始化vlog
{
	// 清空memtable
	memtable->reset();

	// 删除所有sstables文件
	for (std::vector<SSTable *> ssts : sstables)
		for (auto sst : ssts)
			remove(sst->filepath.c_str());

	// 删除vlog文件
	remove(vlog->filepath.c_str());

	// 清除sstables的缓存
	sstables = std::vector<std::vector<SSTable *>>(sst_level);

	// 重置时间戳
	timeStamp = 0;

	// 重新初始化vlog 包括创建文件
	vlog = new VLog(vlog_path);
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list)
// 扫描kvstore的工作：
// 1.先扫描memtable（有序的）
// 2.再扫描sstables（注意 每个sstable的内部应该是有序的 因为sstable是只能由memtable转换过来的）
// 3.将扫描结果存入list
{
	std::map<uint64_t, std::string> scan_result; // 用map来存储扫描结果，方便排序 而且map的key是唯一的
	// 扫描sstables 分为两步：1 遍历sstables得到offset 2 读取vlog得到value

	// 1. 遍历sstables
	std::map<uint64_t, std::tuple<uint64_t, uint64_t, uint32_t>> offset; // key->offset,time,vlen
	for (std::vector<SSTable *> ssts : sstables)
		for (auto sst : ssts)
		{
			if (key1 > sst->header.max_key || key2 < sst->header.min_key) // 如果key1大于sst文件的最大key 或者key2小于sst文件的最小key
				continue;
			for (int i = 0; i < sst->header.tuple_num; i++)
			{
				Tuple t = sst->tuples[i];
				if (t.key >= key1 && t.key <= key2) // 如果key在范围内
				{
					// 如果key不存在或者时间戳更大 则更新offset
					if (offset.find(t.key) == offset.end() || std::get<1>(offset[t.key]) < sst->header.time)
						if (t.vlen == 0) // 如果value长度为0 说明是删除标记 尝试从map中删除
						{
							if (offset.find(t.key) != offset.end()) // 如果key存在
								offset.erase(t.key);				// 删除key
						}
						else
							offset[t.key] = std::make_tuple(t.offset, sst->header.time, t.vlen);
				}
				else if (t.key > key2) // 如果key超出范围
					break;
			}
		}

	// 2. 读取vlog
	std::ifstream in(vlog_path, std::ios::binary | std::ios::in); // 以二进制读方式打开vlog文件
	if (!in)
	{
		std::cerr << "open vlog file failed" << std::endl;
		return;
	}
	size_t tail = vlog->tail; // 减少调用
	for (auto it = offset.begin(); it != offset.end(); it++)
	{
		uint64_t key = it->first;
		uint64_t offset = std::get<0>(it->second);
		uint32_t vlen = std::get<2>(it->second);

		in.seekg(offset); // 定位到offset
		char buffer[vlen];
		in.read(buffer, vlen); // 读取value
		scan_result[key] = std::string(buffer, vlen);
	}
	in.close();

	// 3. 扫描memtable
	memtable->scan(key1, key2, scan_result); // 扫描memtable 会覆盖sstables的结果 因为memtable是最优先的

	for (auto it = scan_result.begin(); it != scan_result.end(); it++) // 将map中的结果转移到list中
		list.push_back(std::make_pair(it->first, it->second));
}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid value.
 * chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size)
// 回收vlog的工作：
// 1.读取vlog文件从tail开始的前chunk_size字节部分的entry（严格大于等于chunk_size 即最后一个如果不够chunk_size 要多读一个）
// 2.对于每一个entry，使用key在缓存的sst中查找，如果找到了并且offset对应上了，说明是最新的
// 3.将最新的entry重新插入memtable 不是最新的则跳过
// 4.全部扫描完成后，强制把memtable转换为sstable和vlog并写入缓存和磁盘
// 5.对扫描过的区域打洞
{
	// 读取vlog文件的前chunk_size字节部分的entry
	std::ifstream in(vlog_path, std::ios::binary | std::ios::in); // 以二进制读方式打开vlog文件
	if (!in)
	{
		std::cerr << "open vlog file failed" << std::endl;
		return;
	}
	// 文件大小
	size_t filesize = vlog->filesize;
	size_t tail = vlog->tail; // gc前的tail
	in.seekg(tail);			  // 定位到tail
	vLog_entry entry;
	uint64_t offset = tail; // 记录当前的偏移量
	size_t n = std::min(tail + chunk_size, filesize);
	while (in.tellg() < n) // 读取vlog文件的前chunk_size字节部分的entry 并且不超过文件大小
	{

		in.read((char *)&entry.magic, sizeof(entry.magic)); // 跳过magic
		in.seekg(sizeof(entry.checksum), std::ios::cur);	// 跳过checksum
		in.read((char *)&entry.key, sizeof(entry.key));		// 读取key
		in.read((char *)&entry.vlen, sizeof(entry.vlen));	// 读取vlen
		offset = in.tellg();								// 记录当前的偏移量

		char buffer[entry.vlen];
		in.read(buffer, entry.vlen); // 读取value 待会插入memtable
		entry.value = std::string(buffer, entry.vlen);

		// 使用key在缓存的sst中查找最新的offset
		// 先在memtable中查找
		key_type key = entry.key;
		if (memtable->get(key) != "") // 如果在memtable中找到了key 说明vlog里的这个entry肯定会被memtable里的覆盖 因此是无效的
			continue;

		// 否则在缓存的sstables中查找
		uint64_t maxtime = 0;
		uint64_t real_offset = 0;
		bool found = false;
		for (std::vector<SSTable *> ssts : sstables)
		{
			std::sort(ssts.begin(), ssts.end(), [](SSTable *a, SSTable *b)
					  { return a->header.time > b->header.time; }); // 按照时间戳降序排序
			for (auto sst : ssts)
			{
				if (key < sst->header.min_key || key > sst->header.max_key) // 如果key不在sst文件的key范围内，或者时间戳小于等于maxtime(即最新的sst文件) 则跳过
					continue;
				if (sst->bloomfilter.check(key)) // 这是第3种方法 如果bloomfilter命中
				{
					int l = 0, r = sst->header.tuple_num - 1;
					while (l <= r)
					{
						int mid = (l + r) >> 1;
						if (sst->tuples[mid].key == key) // 找到了
						{
							found = true;
							maxtime = sst->header.time;		// 更新maxtime
							if (sst->tuples[mid].vlen == 0) // 如果value长度为0 说明是删除标记
								real_offset = 0;
							else // 否则是新的offset
								real_offset = sst->tuples[mid].offset;
							break;
						}
						else if (sst->tuples[mid].key < key)
							l = mid + 1;
						else
							r = mid - 1;
					}
				}
				if (found)
					break;
			}
			if (found)
				break;
		}
		if (real_offset == offset)
			put(key, entry.value); // 如果是最新的entry 则插入memtable
	}

	vlog->tail = in.tellg() == -1 ? filesize : (size_t)in.tellg(); // 更新tail 也就是gc后的tail 如果读到了文件末尾 则tail为filesize 这个很重要 一旦弄错了 后续就会偏离导致错误
	in.close();
	// 强制把memtable转换为sstable和vlog并写入缓存和磁盘
	memtable2sst();
	// 对扫描过的区域打洞
	if (vlog->tail > tail)
		utils::de_alloc_file(vlog_path, tail, vlog->tail - tail);
}

/*
 * 将memtable中的数据转移到sst文件以及vlog文件 并且更新sstables的缓存
 * 注意 只有这个函数会产生新的sst文件
 */
void KVStore::memtable2sst()
{
	std::fstream vlog_file(vlog->filepath, std::ios::binary | std::ios::out | std::ios::app); // 以二进制写方式打开文件 app表示在文件末尾追加
	vLog_entry tmp_entry;																	  // 一个entry 用于转换
	Tuple tmp_tuple;																		  // 一个tuple 用于转换
	Header h;																				  // 一个header 用于转换
	h.max_key = 0;
	h.min_key = UINT64_MAX;
	h.tuple_num = 0;
	h.time = ++timeStamp;																												  // 更新时间戳
	node *cur = memtable->head->next[0];																								  // 跳过头节点
	SSTable *sst = new SSTable(dir + "/level-0/" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".sst"); // 创建一个新的sst文件 以时间戳命名

	vlog_file.seekp(vlog->filesize); // 定位到文件头部
	while (cur != nullptr)
	{
		std::vector<unsigned char> v; // key vlen value组成的二进制序列

		if (cur->val == delete_tag) // 如果是删除标记 直接写入sst文件 不写入vlog文件
		{
			tmp_tuple.key = cur->key;
			tmp_tuple.vlen = 0; // 删除标记的value长度为0
			tmp_tuple.offset = 0;
			sst->bloomfilter.insert(tmp_tuple.key); // 插入bloomfilter 如果不插入 会导致查询的时候这个key不命中 这里是一个很奇怪的点 理论上这个删除的key在之前插入时应该设置了bloomfilter
			sst->tuples.push_back(tmp_tuple);
			h.max_key = std::max(h.max_key, tmp_tuple.key); // 更新max_key 不更新的话 到时候get的时候会忽略掉这个key 导致删除失败
			h.min_key = std::min(h.min_key, tmp_tuple.key); // 更新min_key
			cur = cur->next[0];
			continue;
		}

		tmp_entry.key = cur->key;
		tmp_entry.vlen = cur->val.size();
		tmp_entry.value = cur->val;

		tmp_tuple.key = tmp_entry.key;
		tmp_tuple.vlen = tmp_entry.vlen;

		h.max_key = std::max(h.max_key, tmp_tuple.key);
		h.min_key = std::min(h.min_key, tmp_tuple.key);

		// 把key vlen value组成的二进制序列写入v
		v.resize(sizeof(tmp_entry.key) + sizeof(tmp_entry.vlen) + tmp_entry.vlen);
		memcpy(v.data(), &tmp_entry.key, sizeof(tmp_entry.key));
		memcpy(v.data() + sizeof(tmp_entry.key), &tmp_entry.vlen, sizeof(tmp_entry.vlen));
		memcpy(v.data() + sizeof(tmp_entry.key) + sizeof(tmp_entry.vlen), tmp_entry.value.c_str(), tmp_entry.vlen);
		// 计算校验和
		tmp_entry.checksum = utils::crc16(v);
		// 写入vlog文件
		vlog_file.write((char *)&vlog->magic, sizeof(vlog->magic));
		vlog_file.write((char *)&tmp_entry.checksum, sizeof(tmp_entry.checksum));
		for (unsigned char c : v) // 逐个写入v 也就是key vlen value
		{
			vlog_file.write((char *)&c, sizeof(c));
		}
		vlog->filesize += sizeof(vlog->magic) + sizeof(tmp_entry.checksum) + v.size(); // 更新文件大小
		tmp_tuple.offset = vlog_file.tellp() - tmp_entry.vlen;						   // 计算offset 是相对于文件开头的偏移量
		sst->tuples.push_back(tmp_tuple);											   // 将tuple加入sst缓存
		sst->bloomfilter.insert(tmp_tuple.key);										   // 插入bloomfilter
		cur = cur->next[0];
	}
	h.tuple_num = sst->tuples.size();
	vlog_file.close();

	// 然后转换成sst
	std::fstream sst_file(sst->filepath, std::ios::binary | std::ios::out); // 以二进制写方式打开文件
	// 写入sst文件头部信息 注意顺序
	sst_file.write((char *)&h.time, sizeof(h.time));
	sst_file.write((char *)&h.tuple_num, sizeof(h.tuple_num));
	sst_file.write((char *)&h.min_key, sizeof(h.min_key));
	sst_file.write((char *)&h.max_key, sizeof(h.max_key));
	// 写入bloomfilter
	sst_file.write((char *)&(sst->bloomfilter.bloom_filter_), sizeof(sst->bloomfilter.bloom_filter_));
	// 写入tuple
	for (int i = 0; i < h.tuple_num; i++)
	{
		sst_file.write((char *)&sst->tuples[i].key, sizeof(sst->tuples[i].key));
		sst_file.write((char *)&sst->tuples[i].offset, sizeof(sst->tuples[i].offset));
		sst_file.write((char *)&sst->tuples[i].vlen, sizeof(sst->tuples[i].vlen));
	}
	sst_file.close();

	// 同时更新sstables的缓存 对于析构函数来说 这一步是多余的
	sst->header = h;
	sstables[0].push_back(sst); // 放入0层
}

/*
 * 在sstables中查找key
 * 有三种方法：1.全部访问 2.只根据min_key和max_key决定是否访问 3.根据bloomfilter判断是否访问
 */
void KVStore::findinSSTables(key_type key, value_t &result)
{

	switch (cache_policy)
	{
	case 1: // 这是第1种策略 访问硬盘里的全部sst文件
	{
		uint64_t maxtime = 0;
		if (!utils::dirExists(dir)) // 如果目录不存在
		{
			std::cout << "dir not exists" << std::endl;
			return;
		}
		Header header; // 读取某个sst文件的头部信息
		Tuple tuple;   // 读取某个sst文件的某一个tuple
		for (int i = 0; i < sst_level; i++)
		{
			// level-i目录路径
			std::string level_dir = dir + "/level-" + std::to_string(i);
			// 如果目录不存在
			if (!utils::dirExists(level_dir))
			{
				std::cout << "level" << i << " dir not exists" << std::endl;
				return;
			}
			// 扫描目录下的所有sst文件
			std::vector<std::string> sst_files;
			utils::scanDir(level_dir, sst_files);
			bool found = false; // 记录当前level是否找到了key 因为level越小时间戳越大 所以在某一层找到后就不用再找了
			// 读取sst文件
			for (auto file : sst_files)
			{
				if (file.substr(file.size() - 4, 4) == ".sst")
				{
					std::string fullpath = level_dir + "/" + file;
					std::ifstream in(fullpath, std::ios::binary | std::ios::in); // 以二进制读方式打开文件
					// 读取SSTable文件头部信息 注意顺序
					in.read((char *)&header.time, sizeof(header.time));
					if (header.time <= maxtime) // 如果时间戳小于等于maxtime 则跳过
					{
						in.close();
						continue;
					}
					in.read((char *)&header.tuple_num, sizeof(header.tuple_num));
					in.read((char *)&header.min_key, sizeof(header.min_key));
					in.read((char *)&header.max_key, sizeof(header.max_key));
					// 跳过bloomfilter
					in.seekg(sizeof(BloomFilter::bloom_filter_), std::ios::cur);
					int n = header.tuple_num;
					for (int i = 0; i < n; i++) // 读取tuple
					{
						in.read((char *)&tuple.key, sizeof(tuple.key));
						if (tuple.key == key) // 找到了 更新offset和vlen
						{
							in.read((char *)&tuple.offset, sizeof(tuple.offset));
							in.read((char *)&tuple.vlen, sizeof(tuple.vlen));
							maxtime = header.time; // 更新maxtime
							found = true;
						}
						else
							in.seekg(sizeof(tuple.offset) + sizeof(tuple.vlen), std::ios::cur); // 跳过offset和vlen
					}
					in.close();
				}
			}
			if (found)
				break;
		}
		if (maxtime == 0) // 如果maxtime为0 说明没找到
			return;
		// 到vlog文件中读取value
		std::ifstream in(vlog_path, std::ios::binary | std::ios::in); // 以二进制读方式打开vlog文件
		in.seekg(tuple.offset);										  // 定位到offset
		char buffer[tuple.vlen];
		in.read(buffer, tuple.vlen); // 读取value
		if (tuple.vlen == 0)		 // 如果value长度为0 说明是删除标记
			result = "";
		else // 否则是value
			result = std::string(buffer, tuple.vlen);
		in.close();
		return;
	}
	case 2:
	{
		uint64_t maxtime = 0;
		for (std::vector<SSTable *> ssts : sstables)
		{
			std::sort(ssts.begin(), ssts.end(), [](SSTable *a, SSTable *b)
					  { return a->header.time > b->header.time; }); // 按照时间戳降序排序
			for (auto sst : ssts)
			{
				// 这是第2种策略 如果key在sst文件的key范围内 就进行二分查找
				if (key < sst->header.min_key || key > sst->header.max_key)
					continue;
				// 二分查找
				int l = 0, r = sst->header.tuple_num - 1;
				while (l <= r)
				{
					int mid = (l + r) >> 1;
					if (sst->tuples[mid].key == key) // 找到了
					{
						maxtime = sst->header.time;									  // 更新maxtime
						std::ifstream in(vlog_path, std::ios::binary | std::ios::in); // 以二进制读方式打开vlog文件
						in.seekg(sst->tuples[mid].offset);							  // 定位到offset
						char buffer[sst->tuples[mid].vlen];
						in.read(buffer, sst->tuples[mid].vlen); // 读取value
						if (sst->tuples[mid].vlen == 0)			// 如果value长度为0 说明是删除标记
							result = "";
						else // 否则是value
							result = std::string(buffer, sst->tuples[mid].vlen);
						in.close();
						return;
					}
					else if (sst->tuples[mid].key < key)
						l = mid + 1;
					else
						r = mid - 1;
				}
			}
		}
		return;
	}
	case 3:
	{
		uint64_t maxtime = 0;
		for (std::vector<SSTable *> ssts : sstables)
		{
			std::sort(ssts.begin(), ssts.end(), [](SSTable *a, SSTable *b)
					  { return a->header.time > b->header.time; }); // 按照时间戳降序排序
			for (auto sst : ssts)
			{
				// 这是第3种策略 如果key在sst文件的key范围内 并且bloomfilter命中 就进行二分查找
				if (key < sst->header.min_key || key > sst->header.max_key)
					continue;
				if (sst->bloomfilter.check(key)) // 二分查找
				{
					int l = 0, r = sst->header.tuple_num - 1;
					while (l <= r)
					{
						int mid = (l + r) >> 1;
						if (sst->tuples[mid].key == key) // 找到了
						{
							maxtime = sst->header.time;									  // 更新maxtime
							std::ifstream in(vlog_path, std::ios::binary | std::ios::in); // 以二进制读方式打开vlog文件
							in.seekg(sst->tuples[mid].offset);							  // 定位到offset
							char buffer[sst->tuples[mid].vlen];
							in.read(buffer, sst->tuples[mid].vlen); // 读取value
							if (sst->tuples[mid].vlen == 0)			// 如果value长度为0 说明是删除标记
								result = "";
							else // 否则是value
								result = std::string(buffer, sst->tuples[mid].vlen);
							in.close();
							return;
						}
						else if (sst->tuples[mid].key < key)
							l = mid + 1;
						else
							r = mid - 1;
					}
				}
			}
		}
		return;
	}
	}
}

/*
 * 合并sstables
 * 从0层开始 进行以下操作：（以level-0为例，先操作缓存，全部操作完成后再操作磁盘）
 * 1.统计0层所有sst的key区间（通过min_key和max_key） 选出下一层与区间相交的sstables（如果不是0层 则应该只选出超出阈值的sstables）
 * 2.将选出的sstables合并成一个大的map 在合并过程中需要根据时间戳判断哪条记录是最新的 可以通过一些技巧来减少时间复杂度
 * 3.遍历这个map 将其中的记录x写入新的sstables 每16kb一个文件
 * 4.将新的sstables全部放入1层
 * 5.删除0层的sstables（对于其他层来说是删除超出阈值的sstables）
 * 6.如果1层的sstables数量超过了阈值 选出时间戳最小、键最小的文件 向下一层合并（如果没有下一层则新建一层）
 * 7.直到某一层无需合并 则结束 把缓存里的sstables全部写入磁盘
 */
void KVStore::compaction()
{
	if (sstables[0].size() <= 2) // 如果0层的sstables数量小于等于2 则不需要合并
		return;
	// 先选出需要合并的sstables
	std::vector<SSTable *> merge_ssts = sstables[0]; // 本层需要与下一层合并的sstables 对于0层来说就是全部
	std::vector<SSTable *> next_ssts;				 // 下一层需要与本层合并的sstables
	std::map<key_type, Tuple> merge_map;			 // 合并后的map key->tuple
	key_type min_range = UINT64_MAX, max_range = 0;	 // key的范围
	uint64_t maxtime = 0;							 // 最大时间戳
	int lvl = 0;									 // 受影响的最高层数
	sstables[0].clear();							 // 清空0层的sstables
	// 从0层开始 合并缓存中的sstables
	for (int i = 0; i < sst_level - 1; i++)
	{
		lvl = i + 1; // 合并i层和i+1层 受影响的最高层数是i+1

		// 预处理：先把i层需要合并的sstables按照时间戳升序排序 方便后续合并
		std::sort(merge_ssts.begin(), merge_ssts.end(), [](SSTable *a, SSTable *b)
				  { return a->header.time < b->header.time; });
		maxtime = merge_ssts[merge_ssts.size() - 1]->header.time; // 更新maxtime为i层的最大时间戳

		// 统计i层所有sst的key区间（通过min_key和max_key）
		for (auto sst : merge_ssts)
		{
			min_range = std::min(min_range, sst->header.min_key);
			max_range = std::max(max_range, sst->header.max_key);
		}

		// 选出下一层与区间相交的sstables加入next_ssts 并从缓存中删除
		for (int j = 0; j < sstables[i + 1].size(); j++)
		{
			if (sstables[i + 1][j]->header.min_key <= max_range && sstables[i + 1][j]->header.max_key >= min_range)
			{
				next_ssts.push_back(sstables[i + 1][j]);
				sstables[i + 1].erase(sstables[i + 1].begin() + j);
				j--;
			}
		}

		// 预处理：先把i+1层需要合并的sstables按照时间戳升序排序 方便后续合并
		std::sort(next_ssts.begin(), next_ssts.end(), [](SSTable *a, SSTable *b)
				  { return a->header.time < b->header.time; });

		// 将选出的sstables合并成一个大的map 为了方便 这里不用建议的归并排序 而是逐个插入map
		// 由归纳可以得出 层数越小的sst文件越新 又因为预处理把每一层都按照时间戳升序排序 所以接下来的顺序就是按照时间戳升序进行的 即后插入的一定是新的 直接覆盖即可
		for (auto sst : next_ssts) // 下一层的sstables
		{
			for (int j = 0; j < sst->header.tuple_num; j++)
			{
				Tuple t = sst->tuples[j];
				merge_map[t.key] = t; // 插入map
			}
		}
		for (auto sst : merge_ssts) // 本层的sstables
		{
			for (int j = 0; j < sst->header.tuple_num; j++)
			{
				Tuple t = sst->tuples[j];
				merge_map[t.key] = t; // 插入map
			}
		}

		// 遍历这个map 将其中的记录x写入新的sstables(缓存中) 每16kb一个
		uint64_t filesize = sst_bf_size + sst_header_size; // 当前文件大小 初始化为bloomfilter和header的大小
		Tuple tmp_tuple;								   // 一个tuple 用于转换
		Header h;										   // 一个header 用于转换
		h.max_key = 0;
		h.min_key = UINT64_MAX;
		h.tuple_num = 0;
		h.time = maxtime;
		SSTable *sst = new SSTable(); // 创建一个新的sst 不关联文件

		for (auto it = merge_map.begin(); it != merge_map.end(); it++)
		{
			if (filesize + 20 > sst_max_size) // 如果文件大小超过了阈值 写入header filesize等信息
			{
				h.tuple_num = sst->tuples.size();
				sst->header = h;
				sst->filesize = filesize;
				sstables[i + 1].push_back(sst);			  // 放入下一层
				filesize = sst_bf_size + sst_header_size; // 重新初始化文件大小
				h.max_key = 0;
				h.min_key = UINT64_MAX;
				sst = new SSTable(); // 创建一个新的sst 不关联文件
			}
			tmp_tuple = it->second;

			h.max_key = std::max(h.max_key, tmp_tuple.key);
			h.min_key = std::min(h.min_key, tmp_tuple.key);

			sst->tuples.push_back(tmp_tuple);		// 将tuple加入sst缓存
			sst->bloomfilter.insert(tmp_tuple.key); // 插入bloomfilter
			filesize += 20;							// 每个tuple占用20字节
		}

		// 将最后一个sst写入缓存
		h.tuple_num = sst->tuples.size();
		sst->header = h;
		sst->filesize = filesize;
		sstables[i + 1].push_back(sst); // 放入下一层

		if (sstables[i + 1].size() <= 1 << (i + 2)) // 如果i+1层的sstables数量小于等于2^(i+2) 则不需要合并
			break;

		// 选出时间戳最小、键最小的文件 向下一层合并 并且删除本层的这些文件
		std::sort(sstables[i + 1].begin(), sstables[i + 1].end(), [](SSTable *a, SSTable *b)
				  { return a->header.time < b->header.time || (a->header.time == b->header.time && a->header.min_key < b->header.min_key); });
		merge_ssts.clear();
		int n = sstables[i + 1].size() - (1 << (i + 2)); // 本层多余的文件数目
		for (int j = 0; j < n; j++)						 // 选出时间戳最小、键最小的文件和下一层的文件合并
			merge_ssts.push_back(sstables[i + 1][j]);
		sstables[i + 1].erase(sstables[i + 1].begin(), sstables[i + 1].begin() + n); // 删除本层的这些文件
	}

	// 把缓存里的sstables全部写入磁盘
	for (int i = 0; i <= lvl; i++)
	{
		// 删除所有sst旧文件
		std::string level_dir = dir + "/level-" + std::to_string(i);
		std::vector<std::string> sst_files;
		utils::scanDir(level_dir, sst_files);
		for (auto file : sst_files)
		{
			std::string fullpath = level_dir + "/" + file;
			remove(fullpath.c_str()); // 删除原文件
		}
		// 写入新文件
		for (auto sst : sstables[i])
		{
			// 创建新文件
			sst->filepath = level_dir + "/" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".sst";
			std::fstream sst_file(sst->filepath, std::ios::binary | std::ios::out); // 以二进制写方式打开文件
			// 写入sst文件头部信息 注意顺序
			sst_file.write((char *)&sst->header.time, sizeof(sst->header.time));
			sst_file.write((char *)&sst->header.tuple_num, sizeof(sst->header.tuple_num));
			sst_file.write((char *)&sst->header.min_key, sizeof(sst->header.min_key));
			sst_file.write((char *)&sst->header.max_key, sizeof(sst->header.max_key));
			// 写入bloomfilter
			sst_file.write((char *)&(sst->bloomfilter.bloom_filter_), sizeof(sst->bloomfilter.bloom_filter_));
			// 写入tuple
			for (int j = 0; j < sst->header.tuple_num; j++)
			{
				sst_file.write((char *)&sst->tuples[j].key, sizeof(sst->tuples[j].key));
				sst_file.write((char *)&sst->tuples[j].offset, sizeof(sst->tuples[j].offset));
				sst_file.write((char *)&sst->tuples[j].vlen, sizeof(sst->tuples[j].vlen));
			}
			sst_file.close();
		}
	}
}