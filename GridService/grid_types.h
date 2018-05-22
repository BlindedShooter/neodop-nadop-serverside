#pragma once
#include "GridCoord.h"

/* a bucket in multimap represents one cell in the grid. */
typedef std::unordered_multimap<GridCoord, uid_t> ugrid_t;
typedef std::map<uinfo_t, ugrid_t::iterator> udict_t;
typedef std::vector<ugrid_t::iterator> grid_ulist_t;
