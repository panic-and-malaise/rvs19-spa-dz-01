#include "mesh.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

// Anonymous namespace for some helper structs and functions
namespace {
	struct FaceIndex {
		int v = 0;
		int vt = 0;
		int vn = 0;
	};

	FaceIndex parse_face(const char*& ptr) {
		FaceIndex idx;

		// Technically UB, but I can't be bothered
		idx.v = std::strtol(ptr, (char**)&ptr, 10);

		if (*ptr == '/') {
			ptr++;

			if (*ptr != '/')
				idx.vt = std::strtol(ptr, (char**)&ptr, 10);

			if (*ptr == '/') {
				ptr++;
				idx.vn = std::strtol(ptr, (char**)&ptr, 10);
			}
		}
		return idx;
	}
}

void Mesh::load_from_obj_file(const std::string& filename) {
	std::ifstream file(filename);

	if (!file) {
		throw std::runtime_error("Error opening .obj file \"" + filename + "\"!");
		return;
	}

	size_t line_count = 0;
	std::string line;

	// Count the numbere of lines in the file, important later for optimization
	while (std::getline(file, line))
		line_count++;

	file.clear();
	file.seekg(0);

	std::vector<Vec3D> points;
	std::vector<Vec3D> vertex_colors;
	std::vector<Vec3D> normals;
	std::vector<Vec3D> texture_coordinates;

	// Reserve n-lines amount of space in each vector. This is always more than is necessary, but is much better than dynamically resizing.
	points.reserve(line_count);
	vertex_colors.reserve(line_count);
	normals.reserve(line_count);
	texture_coordinates.reserve(line_count);

	std::string temp;
	while (std::getline(file, temp)) {
		Vec3D point{};
		Vec3D color{-1, -1, -1};

		const char* ptr = temp.c_str() + 2;

		switch (temp[0]) {
			case 'v': {
				switch (temp[1]) {
					// Lines starting with 'v' are vertices, and they indicate the coordinates of individual vertices
					case ' ': {
						// Using the fact that strt functions advance the char pointer
						// This cast is *technically* safe, since the pointer data isn't mutated, but I concur that it is still undefined behaviour.
						point.x = std::strtof(ptr, (char**)&ptr);
						point.y = std::strtof(ptr, (char**)&ptr);
						point.z = std::strtof(ptr, (char**)&ptr);

						points.push_back(point);

						color.x = std::strtof(ptr, (char**)&ptr);
						color.y = std::strtof(ptr, (char**)&ptr);
						color.z = std::strtof(ptr, (char**)&ptr);

						vertex_colors.push_back(color);

						break;
					}
					case 'n': {
						point.x = std::strtof(ptr, (char**)&ptr);
						point.y = std::strtof(ptr, (char**)&ptr);
						point.z = std::strtof(ptr, (char**)&ptr);
						normals.push_back(point);
						break;
					}
					case 't': {
						point.x = std::strtof(ptr, (char**)&ptr);
						point.y = std::strtof(ptr, (char**)&ptr);

						point.z = 1.0f;
						texture_coordinates.push_back(point);
						break;
					}
					default:
						break;
				}
				break;
			}
			// Lines starting with 'f' are "faces" and point to the prior defined vertices by index (starting at 1)
			case 'f': {
				int v1{}, v2{}, v3{};
				int vt1{}, vt2{}, vt3{};
				int vn1{}, vn2{}, vn3{};

				if (points.empty()) break;

				// Literally just a struct that stores v, vt, and vn
				std::vector<FaceIndex> face;

				while (*ptr) {
					// Advance the pointer on spaces
					while (*ptr == ' ') ptr++;
					// Break on null terminator, cstring style
					if (*ptr == '\0') break;

					FaceIndex idx = parse_face(ptr);

					// .obj technically supports negative (from back) indexes, so handle that
					if (idx.v < 0)
						idx.v = points.size() + idx.v + 1;
					if (idx.vt < 0)
						idx.vt = texture_coordinates.size() + idx.vt + 1;
					if (idx.vn < 0)
						idx.vn = normals.size() + idx.vn + 1;

					face.push_back(idx);
				}

				// Quick triangulization of n-gons
				for (size_t i = 1; i + 1 < face.size(); i++) {
					Tri tri{};

					FaceIndex a = face[0];
					FaceIndex b = face[i];
					FaceIndex c = face[i+1];

					v1 = a.v - 1;
					v2 = b.v - 1;
					v3 = c.v - 1;

					if (texture_coordinates.empty() && normals.empty()) {
						tri = {
							points[v1],
							points[v2],
							points[v3]
						};
					} else if (normals.empty()) {
						vt1 = a.vt - 1;
						vt2 = b.vt - 1;
						vt3 = c.vt - 1;

						tri = {
							points[v1],
							points[v2],
							points[v3],

							texture_coordinates[vt1],
							texture_coordinates[vt2],
							texture_coordinates[vt3],

							{},
							{},
							{}
						};
					} else if (texture_coordinates.empty()) {
						vn1 = a.vn - 1;
						vn2 = b.vn - 1;
						vn3 = c.vn - 1;

						tri = {
							points[v1],
							points[v2],
							points[v3],

							normals[vn1],
							normals[vn2],
							normals[vn3]
						};
					} else {
						vt1 = a.vt - 1;
						vt2 = b.vt - 1;
						vt3 = c.vt - 1;

						vn1 = a.vn - 1;
						vn2 = b.vn - 1;
						vn3 = c.vn - 1;

						tri = {
							points[v1],
							points[v2],
							points[v3],

							texture_coordinates[vt1],
							texture_coordinates[vt2],
							texture_coordinates[vt3],

							normals[vn1],
							normals[vn2],
							normals[vn3]
						};
					}
					tri.set_vertex_color(vertex_colors[v1], vertex_colors[v2], vertex_colors[v3]);

					tris.push_back(tri);
				}
				break;
			}
			default:
				break;
		}
	}

	file.close();
}
