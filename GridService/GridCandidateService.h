#pragma once

#include "UserDict.h"
#include "UserGrid.h"

#define append_to_result(tempcoord) tmp_uid_list = usergrid.get_users_in_cell(tempcoord); helper_num += tmp_uid_list.size(); result.insert(result.end(), tmp_uid_list.begin(), tmp_uid_list.end());

class GridCandidateService {
private:
	UserGrid usergrid;
	UserDict userdict;
	std::time_t temp_time;
	std::time_t last_clean_time;

public:

	/* If the user was already in the grid, update the user in the grid. else, insert the user into the grid. */
	void update_user(const UserInfo& c) {
		if (userdict.contains_user(c)) {
			usergrid.erase_itor(userdict.get_itor(c));
			auto result = usergrid.insert_user(c);
			userdict.update_user(c, result);
		}
		else {
			auto result = usergrid.insert_user(c);
			userdict.update_user(c, result);
		} 
	}

	/* Searches 2d grid with enlarging square manner, returning vector of uid's.*/
	std::vector<uid_t> search_grid(const GridCoord& centercoord, const int max_radius, const int target_num) {
		std::time(&temp_time);

		if (temp_time - last_clean_time > USER_LOCATION_INVALID_TIME) {
			do_clean();
		}

		std::vector<uid_t> result;
		int helper_num = 0;

		GridCoord tempcoord = centercoord, tempcoord2 = centercoord;
		std::vector<uid_t> tmp_uid_list;

		append_to_result(centercoord);

		for (int r = 1; r <= max_radius; r++) {
			if (helper_num > target_num) break;
			tempcoord.x -= r;
			tempcoord.y -= r;
			tempcoord2.x += r;
			tempcoord2.y -= r;

			for (int i = 0; i <= r + r; i++) {
				append_to_result(tempcoord);
				append_to_result(tempcoord2);

				if (helper_num > target_num) break;

				tempcoord.y++;
				tempcoord2.y++;
			}
			if (helper_num > target_num) break;

			tempcoord = centercoord;
			tempcoord2 = centercoord;

			tempcoord.x -= r;
			tempcoord.y -= r;
			tempcoord2.x -= r;
			tempcoord2.y += r;

			for (int i = 1; i < r + r; i++) {
				tempcoord.x++;
				tempcoord2.x++;

				append_to_result(tempcoord);
				append_to_result(tempcoord2);

				if (helper_num > target_num) break;
			}
		}

		return result;
	}

	void do_clean() {
		std::time(&last_clean_time);  // update time
		auto grid_ulist = userdict.clean_timeout_users();
		for (auto it : grid_ulist) {
			usergrid.erase_itor(it);
		}
	}

	GridCandidateService() {
		std::time(&last_clean_time);
		std::time(&temp_time);
	}
};