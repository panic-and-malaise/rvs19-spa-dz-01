#include "object.hpp"
#include "matrix.hpp"

#include <stdexcept>

// M_PI is known to be finicky, platform dependant, and version dependant, so I think this is unfortunately the best solution.
#define PI 3.14159265358979323846f

void MeshObject::load_mesh_from_obj(const std::string& obj_file, const std::string& diffuse_texture_file, const std::string& alpha_texture_file) {
	load_LOD_number(obj_file, 0);

	if (!diffuse_texture_file.empty()) {
		sf::Texture texture_;
		if (texture_.loadFromFile(diffuse_texture_file)) {
			diffuse_texture = texture_;
			texture_loaded = true;
		} else {
			throw std::runtime_error("Error opening diffuse texture!");
		}
	}

	if (!alpha_texture_file.empty()) {
		sf::Texture texture_;
		if (texture_.loadFromFile(alpha_texture_file)) {
			alpha_texture = texture_;
		} else {
			throw std::runtime_error("Error opening alpha texture!");
		}
	}
}

void MeshObject::load_LOD_number(const std::string& obj_file, const size_t LOD_number) {
	if (LOD_number > 3)
		throw std::invalid_argument("Maximum LOD number is 3!");

	Mesh lod;
	lod.load_from_obj_file(obj_file); // Unhandled exception; I would actually very much like to crash if this fails!
	correct_mesh_initial_rotation(lod);

	LODs[LOD_number] = lod;

	// Only increment if everything else went as expected
	num_LODs++;
}

void MeshObject::link_renderer(Renderer* renderer_) {
	renderer = renderer_;
}

void MeshObject::correct_mesh_initial_rotation(Mesh& mesh) {
	// Correct initial transform
	// (I have no clue why it's wrong in the first place, don't question it)
	Mat4 rotation_matrix = Mat4::rotation_matrix_y(PI/2);

	// That's right, it's O(n)! This is a deeply unserious project.
	for (Tri& tri : mesh.tris) {
		Tri temp = tri;
		Tri::multiply_with_matrix(tri, temp, rotation_matrix);
		tri = temp;
	}
}

void MeshObject::set_LODs_by_distance(const float distance) {
	if (num_LODs < 2) return;

	// These default distances are very short, and pretty much just tuned for
	// the built-in flower model. Tinkering is recommended for larger models.
	if (distance < 1.1f) {
		mesh = &LODs[0];   // high
	} else if (distance < 2.3f && num_LODs > 1) {
		mesh = &LODs[1];   // medium
	} else if (distance < 6.2f && num_LODs > 2) {
		mesh = &LODs[2];   // low
	} else if (num_LODs > 3) {
		mesh = &LODs[3];   // minimal
	}
}
