#pragma once

#include "mesh.hpp"
#include "vec3d.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Renderer;

class MeshObject {
public:
	MeshObject(sf::RenderWindow* window_) : window(window_), position{}, rotation{}, mesh(&LODs[0]) {}
	virtual ~MeshObject() {}
	
	void load_mesh_from_obj(const std::string& obj_file, const std::string& diffuse_texture_file = "", const std::string& alpha_texture_file = "");
	void load_LOD_number(const std::string& obj_file, const size_t LOD_number);

	void link_renderer(Renderer* renderer_);

	virtual void draw() = 0;
protected:
	sf::RenderWindow* window;
	Renderer* renderer; // No destructor, these are externally managed

	sf::Texture diffuse_texture;
	sf::Texture alpha_texture;

	Vec3D position;
	Vec3D rotation; // x -> pitch, y -> yaw, z -> roll

	Mesh* mesh; // No destructor, this only ever points to a value in LODs, which is owned by this class
	Mesh LODs[4];

	bool texture_loaded = false;

	int num_LODs = 0;

	void set_LODs_by_distance(const float distance);

	// Best solution I've found to keeping encapsulation while exposing important members to crucial code
	friend class Renderer;
private:
	void correct_mesh_initial_rotation(Mesh& mesh);
};
