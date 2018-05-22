#pragma once

#include "grid_types.h"
#include <iostream>

class UserDict {
private:
	udict_t dict;
public:
	void print_all_users() {
		for (auto it : dict) {
			std::cout << "uid: " << it.first.second << "\tx: " << it.second->first.x << "\ty: " << it.second->first.y << std::endl;
		}
	}
	void insert_user(const uinfo_t &uinfo, const ugrid_t::iterator &c) {
		update_user(uinfo, c);
	}
	void update_user(const uinfo_t &uinfo, const ugrid_t::iterator &c) {
		dict.insert_or_assign(uinfo, c);
	}	

	bool contains_user(const uinfo_t &uinfo) {
		return dict.find(uinfo) != dict.end();
	}

	grid_ulist_t clean_timeout_users() {
		std::time_t now;
		grid_ulist_t result;

		std::time(&now);

		auto temp = dict.begin();

		for (auto it = dict.begin(); it != dict.end(); ++it) {
			if (now - it->first.first > USER_LOCATION_INVALID_TIME) {
				temp = it++;
				result.push_back(temp->second);
				dict.erase(temp);
			}
			else {
				break;
			}
		}

		return result;
	}

	UserDict() {};
};
