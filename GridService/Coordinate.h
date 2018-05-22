#pragma once

#include "basic_types.h"

/* Container for 'GPS' coordinate input (prototype!!), Contains double lat, double lon, uid_t uid.*/
struct Coordinate {
	/* lat: latitude of GPS data. lon: longitude of GPS data. */
	double lat, lon;
	uinfo_t uinfo;

	Coordinate(double latitude, double longitude, uinfo_t uinfo_)
		: lat(latitude), lon(longitude), uinfo(uinfo_) {};
};
