#pragma once

#include "GridCoord.h"

/* each bucket represents 1 cell of the grid*/
typedef std::unordered_multimap<coord, utime_t> usergrid_t;
typedef std::unordered_set<uid_t> userset_t;

/* iterates through pair<itor begin, itor end>. should manually increment iterator. */
#define for_range(range) for (auto it = range.first; it != range.second;)
/* iterates through equal_range of given GridCoord 'c'. iterator name is 'it'.*/
#define for_cell(c) for_range(ugrid.equal_range(c))

#define is_contain(container, key) container.find(key) != container.end()
#define delete_from(container, key) container.erase(container.find(key))
/* Quite weird but first uinfo is automatically tranformed into 'coord', and the second is to 'utime_t'. */
#define insert_to_grid(uinfo) ugrid.insert({ uinfo, uinfo })

class GridCandidateService{
private:
	usergrid_t ugrid;
	userset_t uset;
	std::time_t last_search_time;
	std::size_t total_users = 0;
public:
	void update_user(const uinfo_t &uinfo) {
		if (is_contain(uset, uinfo.uid)) {
			for_cell(uinfo) {
				if (it->second.uid == uinfo.uid) {
					if (it->first != uinfo) {
						ugrid.erase(it);
						insert_to_grid(uinfo);
					}
					else {
						it->second.timestamp = uinfo.timestamp;
					}
					break;
				}
				it++;
			}
		}
		else {
			uset.insert(uinfo.uid);
			insert_to_grid(uinfo);
			total_users++;
		}
	}

	uservec_t search_grid(const coord &c, const int max_radius, const int target_num) {
		uservec_t result;
		std::time(&last_search_time);
		bool search_finish = false;

		for (int r = 0; r < max_radius; r++) {
			for (coord t : c.radius_sqaure(r)) {
				for_cell(t) {
					if (it->second.is_user_location_invalid(last_search_time)) {
						uset.erase(it->second.uid);
						it = ugrid.erase(it);
						total_users--;
					}
					else {
						result.push_back(it->second.uid);
						if (result.size() > target_num) {
							search_finish = true;
							break;
						}
						it++;
					}
				}
				if (search_finish = true) break;
			}
			if (search_finish = true) break;
		}

		return result;
	}

	size_t get_user_num() {
		return total_users;
	}
};