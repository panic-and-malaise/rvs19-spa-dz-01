#pragma once

#include "camera.hpp"
#include "matrix.hpp"
#include "object.hpp"
#include "tri.hpp"
#include "vec3d.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Renderer {
public:
	Renderer();

	static Mat4 create_screen_projection_matrix(const float screen_width, const float screen_height);
	void handle_camera_movement(const float delta, const Vec3D& look_direction, const Vec3D& east);
	void handle_mouse_look(sf::RenderWindow& window);

	static void draw_tri_wireframe(sf::RenderWindow* window, const Tri& tri, const sf::Color& color = sf::Color(255, 165, 0));
	static void draw_triangle(sf::RenderWindow* window, const Tri& tri, const sf::Color& color = sf::Color(255, 255, 255));

	static void draw_triangle_vertex_colors(sf::RenderWindow* window, const Tri& tri, const float light = 255.0f);
	static void draw_triangle_textured(sf::RenderWindow* window, const Tri& tri, const sf::Texture& texture, const float light = 255.0f);

	void toggle_backface_culling(const bool value);
	void toggle_lighting(const bool value);
	void toggle_wireframe(const bool value);

	void toggle_mouse_look(const bool value);
	bool get_mouse_look() const;

	void set_camera_pos(const Vec3D pos);
	void set_camera_pos(const float x, const float y, const float z);
	void set_camera_move_speed(const float speed);
	void set_camera_turn_speed(const float speed);

	void physics_loop(const float delta_time);
	void render(MeshObject& object);
private:
	static constexpr float FOV = 90.0f;

	static constexpr float Z_NEAR = 0.1f;
	static constexpr float Z_FAR  = 1000.0f;

	static constexpr float Z_PROJECT = Z_FAR / (Z_FAR - Z_NEAR);

	Camera camera;
	float movement_speed = 0.25f;
	float turn_speed = 2.0f;
	bool mouse_look_enabled = false;

	bool backface_culling = true;
	bool should_use_lighting = true;
	bool should_draw_wireframe = false;
};
