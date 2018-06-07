#pragma once
#include "basic_types.h"

struct coord {
	uid_t uid;
	double lat, lon;
	std::time_t timestamp;

	int x, y;
	

	coord(int x, int y) : x(x), y(y) {}
	/* Construct by uinfo_t. has transformation formula to translate GPS coordinate into Grid Coordinate. */
	coord(const uinfo_t &c) :
		x(static_cast<int>(std::round((c.lat + 180) * 1000))),
		y(static_cast<int>(std::round((c.lon + 180) * 1000))),
		uid(c.uid), lat(c.lat), lon(c.lon), timestamp(c.timestamp) {};
	coord(double lat, double lon) : 
		x(static_cast<int>(std::round((lat + 180) * 1000))),
		y(static_cast<int>(std::round((lon + 180) * 1000))) {};

	~coord() {};

	/* returns vector of coords that forms a sqaure sided 2r+1, with center to this coord. */
	coordvec_t radius_sqaure(const int &r) const {
		coordvec_t result;

		if (r > 0) {
			for (int i = 0; i <= r + r; i++) {
				result.push_back(coord(x - r, y - r + i));
				result.push_back(coord(x + r, y - r + i));
			}

			for (int i = 1; i < r + r; i++) {
				result.push_back(coord(x - r + i, y - r));
				result.push_back(coord(x - r + i, y + r));
			}
		}
		else if (r == 0) {
			result.push_back(*this);
		}

		return result;
	}

	friend std::hash<coord>;
	friend bool operator==(const coord& c1, const coord& c2);
	friend bool operator!=(const coord& c1, const coord& c2);
};

// custom hash function for coord. just xor of two hash<int>s.
namespace std {
	template<>
	class hash<coord> {
	public:
		size_t operator() (const coord &crd) const {
			using std::hash;
			// Our hash function for GridCoord. can be optimized? 
			return hash<int>()(crd.x) ^ hash<int>()(crd.y);  
		}
	};
}; 

// below are required operator for STL containers.
inline bool operator==(const coord& c1, const coord& c2) {
	return (c1.x == c2.x) && (c1.y == c2.y);
};

inline bool operator!=(const coord& c1, const coord& c2) {
	return (c1.x != c2.x) || (c1.y != c2.y);
};