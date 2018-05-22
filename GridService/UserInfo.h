#pragma once

#include "basic_types.h"

/* Container for 'GPS' coordinate input (prototype!!), Contains double lat, double lon, uid_t uid.*/
struct UserInfo {
	/* lat: latitude of GPS data. lon: longitude of GPS data. */
	double lat, lon;
	uid_t uid;
	std::time_t timestamp;

	UserInfo(double latitude, double longitude, uid_t uid_, std::time_t timestamp_)
		: lat(latitude), lon(longitude), uid(uid_), timestamp(timestamp_)
	{};
};
