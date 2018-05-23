#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

struct coord;

typedef std::string uid_t;
typedef std::vector<uid_t> uservec_t;
typedef std::vector<coord> coordvec_t;

#define GRID_HASH_TABLE_RESERVE_BUCKET_SIZE 1000000
#define USER_HASH_TABLE_RESERVE_SIZE 10000000
#define USER_LOCATION_INVALID_TIME 600  // 10 mins = 600


/* Container for 'GPS' coordinate input (prototype!!), Contains double lat, double lon, uid_t uid.*/
struct uinfo_t {
	/* lat: latitude of GPS data. lon: longitude of GPS data. */
	double lat, lon;
	uid_t uid;
	std::time_t timestamp;

	uinfo_t(double latitude, double longitude, uid_t uid_, std::time_t timestamp_)
		: lat(latitude), lon(longitude), uid(uid_), timestamp(timestamp_)
	{};
};

struct utime_t {
	uid_t uid;
	std::time_t timestamp;

	utime_t(uid_t uid, std::time_t timestamp) : uid(uid), timestamp(timestamp) {};
	utime_t(const uinfo_t& uinfo) : uid(uinfo.uid), timestamp(uinfo.timestamp) {};

	inline bool is_user_location_invalid(const std::time_t current_time) {
		return current_time - timestamp > USER_LOCATION_INVALID_TIME;
	}
};