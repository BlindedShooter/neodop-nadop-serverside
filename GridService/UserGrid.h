#pragma once

#include "grid_types.h"

/* iterates through pair<itor begin, itor end>. */
#define for_range(range) for (auto it = range.first; it != range.second; it++)
/* iterates through equal_range of given GridCoord 'c'. iterator name is 'it'.*/
#define for_cell(c) for_range(grid.equal_range(c))

class UserGrid {
private:
	ugrid_t grid;
public:
	inline auto insert_user(const GridCoord &c, const uid_t &uid) {
		return grid.insert({ c, uid });
	}
	inline auto insert_user(const UserInfo &c) {
		return insert_user(GridCoord(c), c.uid);
	}

	void delete_user(const GridCoord& c, const uid_t &uid) {
		for_cell(c) {
			if (it->second == uid) {
				grid.erase(it);
				break;
			}
		}
	}

	inline auto update_user(const GridCoord& c, const uid_t &uid) {
		delete_user(c, uid);
		auto result = insert_user(c, uid);
		return insert_user(c, uid);
	}
	inline auto update_user(const UserInfo& c) {
		return update_user(GridCoord(c), c.uid);
	}

	std::vector<uid_t> get_users_in_cell(const GridCoord& c) {
		std::vector<uid_t> result;

		for_cell(c) {
			result.push_back(it->second);
		}

		return result;
	}

	inline void erase_itor(ugrid_t::iterator it) {
		grid.erase(it);
	}

	UserGrid() {
		grid.rehash(GRID_HASH_TABLE_RESERVE_BUCKET_SIZE);
	}
};