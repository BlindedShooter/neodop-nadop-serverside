#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

typedef std::string uid_t;
/* Container for updated time and uid_t, first == updated time, second == uid. */
typedef std::pair<std::time_t, uid_t> uinfo_t;  // there was some mistake... uinfo_t should perform as 'Coordiate' class.

#define GRID_HASH_TABLE_RESERVE_BUCKET_SIZE 10000
#define USER_HASH_TABLE_RESERVE_SIZE 100000
#define USER_LOCATION_INVALID_TIME 600  // 10 mins
#define GRID_CLEANING_INTERVAL 300	// 5 mins