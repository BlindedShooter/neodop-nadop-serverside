#pragma once

#include "grid_types.h"
#include <iostream>

class UserDict {
private:
	udict_t dict;
	utimemap_t timemap;
public:
	void insert_user(const UserInfo &uinfo, const ugrid_t::iterator &c) {
		update_user(uinfo, c);
	}
	void update_user(const UserInfo &uinfo, const ugrid_t::iterator &c) {
		dict.insert_or_assign(uinfo.uid, c);
		timemap.insert_or_assign(uinfo.uid, uinfo.timestamp);
	}	

	bool contains_user(const UserInfo &uinfo) {
		return dict.find(uinfo.uid) != dict.end();
	}
	
	ugrid_t::iterator get_itor(const UserInfo &uinfo) {
		return dict[uinfo.uid];
	}
	grid_ulist_t clean_timeout_users() {
		std::time_t now;
		grid_ulist_t result;

		std::time(&now);

		for (auto it : timemap) {
			if (now - it.second > USER_LOCATION_INVALID_TIME) {
				auto tmp = dict.find(it.first);
				result.push_back(tmp->second);
				dict.erase(tmp);
			}
			else {
				break;
			}
		}

		return result;
	}

	UserDict() {};
};
