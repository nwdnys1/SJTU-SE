#pragma once

#include "utils.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <algorithm>

using key_type = uint64_t;
using value_t = std::string;
using scan_result_t = std::list<std::pair<uint64_t, std::string>>;

#define delete_tag "~DELETED~"

#define sst_level 12

#define sst_header_size 32

#define sst_bf_size 1024 * 8 // 8KB

#define sst_max_size 1024 * 16 // 16KB

#define cache_policy 3 // 1.无缓存 2.只缓存了索引信息 3.缓存了索引和bf
