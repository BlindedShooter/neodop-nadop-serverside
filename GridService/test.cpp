#include <iostream>
#include "GridCandidateService.h"

int main() {
	GridCandidateService gms;

	double lat = 0, lon = 0;
	uinfo_t uinfo;
	
	for (int i = 0; i < 10; i++) {
		lat += 0.0001;

		lon = 0;

		for (int j = 0; j < 10; j++) {
			lon += 0.0001;
			uinfo.second = std::to_string(i) + ',' + std::to_string(j);
			gms.update_user(Coordinate(lat, lon, uinfo));
		}
	}

	gms.print_all_users();

	uinfo.second = "123";
	lat -= 0.0005;
	lon -= 0.0005;

	std::cout << "target coordinate:  lat: " << lat << "  lon: " << lon << std::endl << std::endl;
	GridCoord target = GridCoord(Coordinate(lat, lon, uinfo));
	std::cout << "target GridCoord:  x: " << target.x << "  y: " << target.y << std::endl << std::endl << "candidate uids:" << std::endl;

	std::vector<uid_t> list = gms.search_grid(Coordinate(lat, lon, uinfo), 0, 10);

	for (auto it : list) {
		std::cout << it << std::endl;
	}

	int a;
	std::cin >> a;
	return 0;
}