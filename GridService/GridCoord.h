#pragma once
#include "UserInfo.h"

struct GridCoord {
	int x, y;

	GridCoord(int x, int y) : x(x), y(y) {}
	/* Construct by Coordinate. has transformation formula to translate GPS coordinate into Grid Coordinate. */
	GridCoord(const UserInfo &c) :
		x(static_cast<int>(std::round((c.lat + 180) * 4000))),
		y(static_cast<int>(std::round((c.lon + 180) * 4000))) {};

	~GridCoord() {};

	friend std::hash<GridCoord>;
	friend bool operator==(const GridCoord& c1, const GridCoord& c2);
	friend bool operator!=(const GridCoord& c1, const GridCoord& c2);
};


namespace std {
	template<>
	class hash<GridCoord> {
	public:
		size_t operator() (const GridCoord &crd) const {
			using std::hash;
			// Our hash function for GridCoord. can be optimized? 
			return hash<int>()(crd.x) ^ hash<int>()(crd.y);  
		}
	};
}; 

inline bool operator==(const GridCoord& c1, const GridCoord& c2) {
	return (c1.x == c2.x) && (c1.y == c2.y);
};

inline bool operator!=(const GridCoord& c1, const GridCoord& c2) {
	return (c1.x != c2.x) || (c1.y != c2.y);
};