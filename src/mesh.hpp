#pragma once

#include "tri.hpp"

#include <string>
#include <vector>

struct Mesh {
	std::vector<Tri> tris;

	void load_from_obj_file(const std::string& filename);
};
