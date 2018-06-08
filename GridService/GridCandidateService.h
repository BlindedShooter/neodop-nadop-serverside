#pragma once

#include "GridCoord.h"
#include <iostream>

/* each bucket represents 1 cell of the grid*/
typedef std::unordered_multimap<coord, uinfo_t> usergrid_t;
typedef std::unordered_set<uid_t> userset_t;

/* iterates through pair<itor begin, itor end>. should manually increment iterator. */
#define for_range(range) for (auto it = range.first; it != range.second;)
#define for_cell(c) for_range(ugrid.equal_range(c))
/* returns true if the container has the key, or false. */
/* iterates through equal_range of given GridCoord 'c'. iterator name is 'it'.*/
#define is_contain(container, key) container.find(key) != container.end()
/* delete key from container.*/
#define delete_from(container, key) container.erase(container.find(key))
/* Quite weird but first uinfo is automatically tranformed into 'coord', and the second is to 'utime_t'. */
#define insert_to_grid(uinfo) ugrid.insert({ uinfo, uinfo })

class GridCandidateService{
private:
	usergrid_t ugrid;
	userset_t uset;
	std::time_t last_search_time, last_cleanup_time;
	std::size_t total_users = 0;
public:
	/* update the grid with given uinfo, insert if new and assign if already in. */
	void update_user(const uinfo_t &uinfo) {
		//std::cout << "[C++]: update_user request   ";
		/* If there is a user with the uid in the grid already, */
		if (is_contain(uset, uinfo.uid)) {
			for_cell(uinfo) {
				/* find the user,*/
				if (it->second.uid == uinfo.uid) {
					//std::cout << "uid found in the server, uid: " << uinfo.uid << "  ";
					/* and if the coord was changed, */
					if (it->first != uinfo) {
						//std::cout << "coord changed to: " << uinfo.lat << ", " << uinfo.lon << std::endl;
						ugrid.erase(it);  // erase previous info and
						insert_to_grid(uinfo); // insert new info to the grid. (since coord is the key)
					}
					/* but if the coord was not changed, */
					else {
						//std::cout << "coord not changed, updating timestamp from " << it->second.timestamp << " to: " << uinfo.timestamp << std::endl;
						/* just update its timestamp. */
						it->second.timestamp = uinfo.timestamp;
					}
					break; // since we found the user, no need to search anymore.
				}
				it++;
			}
		}
		/* or this is the first time user came to the grid, */
		else {
			/* newly insert the user. */
			uset.insert(uinfo.uid);
			insert_to_grid(uinfo);
			total_users++;
		}
	}

	/* searchs the grid from coord c, with extent to max_radius, until target_num helpers are found. */
	uservec_t search_grid(const coord &c, const int max_radius, const int target_num) {
		uservec_t result;
		/* update the search time. */
		std::time(&last_search_time);
		/* flag for nested for loop escape, when target_num is achieved.*/
		bool search_finish = false;

		/* code below is just obvious. study for yourself! */
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
	
	void cleanup_old_users() {
		std::time(&last_cleanup_time);
		for (auto it = ugrid.begin(); it != ugrid.end(); ) {
			if (it->second.is_user_location_invalid(last_cleanup_time)) {
				uset.erase(it->second.uid);
				it = ugrid.erase(it);
			}
			else {
				it++;
			}
		}
	}

	uinfovec_t get_all_users() {
		uinfovec_t result;
		for (auto const& i : ugrid) {
			result.push_back(i.second);
		}
		return result;
	}

	size_t get_user_num() {
		return total_users;
	}
	
	std::time_t get_last_search_time() {
		return last_search_time;
	}
	std::time_t get_last_cleanup_time() {
		return last_cleanup_time;
	}
};
