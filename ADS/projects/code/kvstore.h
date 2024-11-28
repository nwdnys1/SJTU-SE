#pragma once

#include "kvstore_api.h"
#include <cstdint>
#include <fstream>
#include "utils.h"
#include "memtable.h"
#include "sstable.h"
#include "vLog.h"

class KVStore : public KVStoreAPI
{
public:
	Memtable *memtable;
	std::string dir;
	std::string vlog_path;
	uint64_t timeStamp = 0;						  // 当前最新的时间戳
	std::vector<std::vector<SSTable *>> sstables; // sstables[i] 代表第i层的sstable
	VLog *vlog;									  // vlog 不含任何数据 只有头尾信息和magic
	std::fstream vlog_file;						  // vlog文件 方便读写 无需每次都打开关闭文件

	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) override;

	void gc(uint64_t chunk_size) override;

	void memtable2sst();

	void findinSSTables(uint64_t key, std::string &value);

	void compaction();
};
