#include "renderer.hpp"
#include "object.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cmath>

#define PI 3.14159265358979323846f
#define radians(degrees) ((degrees) * (PI / 180.0f))

// Offset a bit backwards so the mesh is in view by default
Renderer::Renderer() : camera({ 0.0f, 0.0f, -5.0f }) {}

Mat4 Renderer::create_screen_projection_matrix(const float screen_width, const float screen_height) {
	// Curse you, STL devs, and your tangent function not being inline...
	const float FOV_SCALING_FACTOR = 1.0f / std::tanf(radians(FOV / 2.0f));

	return Mat4{{
		{ (screen_width / screen_height) * FOV_SCALING_FACTOR, 0.0f, 0.0f, 0.0f },
		{ 0.0f, FOV_SCALING_FACTOR, 0.0f, 0.0f },
		{ 0.0f, 0.0f,  Z_PROJECT, 		  1.0f },
		{ 0.0f, 0.0f, -Z_FAR * Z_PROJECT, 0.0f }
	}};
}

// Movement and turn speed are constants defined in the header file
void Renderer::handle_camera_movement(const float delta, const Vec3D& look_direction, const Vec3D& east) {
	// Remove the y component so pitch doesn't influence the movement direction
	Vec3D move_forward = look_direction;
	move_forward.y = 0;
	move_forward.normalize();

	// Z-Axis movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		camera.position = camera.position + (move_forward * (movement_speed * -delta)); // Why negative delta? Why indeed...
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		camera.position = camera.position - (move_forward * (movement_speed * -delta));
	}
	//
	//Strafing
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		camera.position = camera.position + (east * (movement_speed * delta));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		camera.position = camera.position + (east * -(movement_speed * delta));
	}

	// Vertical movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		camera.position.y += movement_speed * delta;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		camera.position.y -= movement_speed * delta;
	}

	// Camera controls
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		camera.set_yaw(camera.get_yaw() - turn_speed * delta);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		camera.set_yaw(camera.get_yaw() + turn_speed * delta);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		camera.set_pitch(camera.get_pitch() - turn_speed * delta);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		camera.set_pitch(camera.get_pitch() + turn_speed * delta);
	}
}

void Renderer::handle_mouse_look(sf::RenderWindow& window) {
    sf::Vector2u size = window.getSize();
    sf::Vector2i center(size.x / 2, size.y / 2);

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    sf::Vector2i mouse_delta = mouse_pos - center;

    const float sensitivity = 0.0025f;

    camera.set_yaw(camera.get_yaw() 	- mouse_delta.x *  sensitivity);
    camera.set_pitch(camera.get_pitch() - mouse_delta.y * -sensitivity);

    sf::Mouse::setPosition(center, window);
}

void Renderer::draw_tri_wireframe(sf::RenderWindow* window, const Tri& tri, const sf::Color& color) {
	sf::VertexArray triangle(sf::LineStrip, 4);

	triangle[0].position = sf::Vector2f(tri.get_vertices()[0].x, tri.get_vertices()[0].y);
	triangle[1].position = sf::Vector2f(tri.get_vertices()[1].x, tri.get_vertices()[1].y);
	triangle[2].position = sf::Vector2f(tri.get_vertices()[2].x, tri.get_vertices()[2].y);
	triangle[3].position = sf::Vector2f(tri.get_vertices()[0].x, tri.get_vertices()[0].y);

	triangle[0].color = color;
	triangle[1].color = color;
	triangle[2].color = color;
	triangle[3].color = color;

	window->draw(triangle);
}

void Renderer::draw_triangle(sf::RenderWindow* window, const Tri& tri, const sf::Color& color) {
	sf::VertexArray triangle(sf::Triangles, 3);

	triangle[0].position = sf::Vector2f(tri.get_vertices()[0].x, tri.get_vertices()[0].y);
	triangle[1].position = sf::Vector2f(tri.get_vertices()[1].x, tri.get_vertices()[1].y);
	triangle[2].position = sf::Vector2f(tri.get_vertices()[2].x, tri.get_vertices()[2].y);

	triangle[0].color = color;
	triangle[1].color = color;
	triangle[2].color = color;

	window->draw(triangle);
}

void Renderer::draw_triangle_vertex_colors(sf::RenderWindow* window, const Tri& tri, const float light) {
	sf::VertexArray triangle(sf::Triangles, 3);

	triangle[0].position = sf::Vector2f(tri.get_vertices()[0].x, tri.get_vertices()[0].y);
	triangle[1].position = sf::Vector2f(tri.get_vertices()[1].x, tri.get_vertices()[1].y);
	triangle[2].position = sf::Vector2f(tri.get_vertices()[2].x, tri.get_vertices()[2].y);

	Vec3D vert_color1 = tri.get_vertex_colors()[0] * light;

	// If vertex colors are invalid, defaults to white using the light value
	if (vert_color1.x <= 0.0f) {
		sf::Color blank(light, light, light);
		triangle[0].color = blank;
		triangle[1].color = blank;
		triangle[2].color = blank;
	} else {
		Vec3D vert_color2 = tri.get_vertex_colors()[1] * light;
		Vec3D vert_color3 = tri.get_vertex_colors()[2] * light;

		triangle[0].color = sf::Color(vert_color1.x, vert_color1.y, vert_color1.z);
		triangle[1].color = sf::Color(vert_color2.x, vert_color2.y, vert_color2.z);
		triangle[2].color = sf::Color(vert_color3.x, vert_color3.y, vert_color3.z);
	}


	window->draw(triangle);
}

void Renderer::draw_triangle_textured(sf::RenderWindow* window, const Tri& tri, const sf::Texture& texture, const float light) {
	sf::VertexArray triangle(sf::Triangles, 3);

	triangle[0].position = sf::Vector2f(tri.get_vertices()[0].x, tri.get_vertices()[0].y);
	triangle[1].position = sf::Vector2f(tri.get_vertices()[1].x, tri.get_vertices()[1].y);
	triangle[2].position = sf::Vector2f(tri.get_vertices()[2].x, tri.get_vertices()[2].y);

	float uv_x1 = tri.get_uv()[0].x * texture.getSize().x;
	float uv_y1 = (1.0f - tri.get_uv()[0].y) * texture.getSize().y;

	float uv_x2 = tri.get_uv()[1].x * texture.getSize().x;
	float uv_y2 = (1.0f - tri.get_uv()[1].y) * texture.getSize().y;

	float uv_x3 = tri.get_uv()[2].x * texture.getSize().x;
	float uv_y3 = (1.0f - tri.get_uv()[2].y) * texture.getSize().y;

	triangle[0].texCoords = sf::Vector2f(uv_x1, uv_y1);
	triangle[1].texCoords = sf::Vector2f(uv_x2, uv_y2);
	triangle[2].texCoords = sf::Vector2f(uv_x3, uv_y3);

	sf::Color light_color = sf::Color(light, light, light);
	triangle[0].color = light_color;
	triangle[1].color = light_color;
	triangle[2].color = light_color;

	sf::RenderStates state;
	state.texture = &texture;

	window->draw(triangle, state);
}

void Renderer::toggle_backface_culling(const bool value) {
	backface_culling = value;
}

void Renderer::toggle_lighting(const bool value) {
	should_use_lighting = value;
}

void Renderer::toggle_wireframe(const bool value) {
	should_draw_wireframe = value;
}

void Renderer::toggle_mouse_look(const bool value) {
	mouse_look_enabled = value;
}

bool Renderer::get_mouse_look() const {
	return mouse_look_enabled;
}

void Renderer::set_camera_pos(const Vec3D pos) {
	camera.position = pos;
}

void Renderer::set_camera_pos(const float x, const float y, const float z) {
	camera.position = { x, y, z };
}

void Renderer::set_camera_move_speed(const float speed) {
	movement_speed = speed;
}

void Renderer::set_camera_turn_speed(const float speed) {
	turn_speed = speed;
}

void Renderer::physics_loop(const float delta_time) {
	Vec3D look_direction = { 0, 0, 1 };
	Vec3D up = { 0, 1, 0 };
	Vec3D target = { 0, 0, -1 }; // I genuinely don't know why this is like this. Positive values just flip the entire viewport vertically for some reason...

	Mat4 camera_pitch_rotation_matrix = Mat4::rotation_matrix_x(camera.get_pitch());
	Mat4 camera_yaw_rotation_matrix = Mat4::rotation_matrix_y(camera.get_yaw());
	Mat4 camera_roll_rotation_matrix = Mat4::identity_matrix();
	if (camera.get_roll() != 0.0f)
		camera_roll_rotation_matrix = Mat4::rotation_matrix_z(camera.get_roll());

	// Order is very important here. Do not change!!!
	Mat4::multiply_matrix_vector(target, look_direction, camera_pitch_rotation_matrix * camera_yaw_rotation_matrix * camera_roll_rotation_matrix);

	Vec3D east = Vec3D::cross_product(look_direction, up);
	east.normalize();

	handle_camera_movement(delta_time, look_direction, east);
}

void Renderer::render(MeshObject& object) {
	static sf::Clock delta_clock;
	sf::Time delta = delta_clock.restart();
	#define delta_time delta.asSeconds()

	Mat4 screen_projection_matrix = Renderer::create_screen_projection_matrix(object.window->getView().getSize().x, object.window->getView().getSize().y);

	Mat4 mat_rot_x = Mat4::rotation_matrix_x(object.rotation.x);
	Mat4 mat_rot_y = Mat4::rotation_matrix_y(object.rotation.y);
	Mat4 mat_rot_z = Mat4::rotation_matrix_z(object.rotation.z);

	Mat4 rotation_matrix = mat_rot_x * mat_rot_y * mat_rot_z;

	Vec3D look_direction = { 0, 0, 1 };
	Vec3D up = { 0, 1, 0 };
	Vec3D target = { 0, 0, -1 };

	// Windows specific fix until I refactor the entire window system
	if (object.window->hasFocus())
		physics_loop(delta_time);

	Mat4 camera_pitch_rotation_matrix = Mat4::rotation_matrix_x(camera.get_pitch());
	Mat4 camera_yaw_rotation_matrix = Mat4::rotation_matrix_y(camera.get_yaw());

	Mat4::multiply_matrix_vector(target, look_direction, camera_pitch_rotation_matrix * camera_yaw_rotation_matrix);

	target = camera.position + look_direction;
	Mat4 camera_matrix = Mat4::point_at(camera.position, target, up);
	Mat4 view_matrix = Mat4::quick_inverse(camera_matrix);

	// Use squared distance here to avoid an expensive sqrt operation
	float distance_squared = (camera.position - object.position).length_squared();
	object.set_LODs_by_distance(distance_squared);

	std::vector<Tri> visible_tris;
	visible_tris.reserve(object.mesh->tris.size()); // Who needs memory optimization

	for (const Tri& tri : object.mesh->tris) {
		Tri current = tri;

		Tri rot = current;
		Tri::multiply_with_matrix(tri, rot, rotation_matrix);

		current = rot;
		current.translate(object.position);

		Tri view = current;
		Tri::multiply_with_matrix(current, view, view_matrix);

		// Cull tris that are outside the view frustrum
		Vec3D camera_ray = current.get_vertices()[0] - camera.position;

		if (backface_culling)
			if (Vec3D::dot_product(current.normal(), camera_ray) >= 0.0f) continue;

		int clipped_triangles = 0;
		Tri clipped[2];

		clipped_triangles = Tri::triangles_to_clip({0, 0, 0.1f}, {0, 0, -1.0f}, view, clipped[0], clipped[1]);

		if (clipped_triangles == 1) // Very hacky way of reducing visual artifacts and eliminating the dreaded "seeing behind your back" issue
			visible_tris.push_back(current);
	}

	// A Painter's algorithm. As slow as it is simple. A depth buffer would go a long way, but alas, I only have so much time.
	std::sort(visible_tris.begin(), visible_tris.end(), [](const Tri& a, const Tri& b){
		float z1 = (a.get_vertices()[0].z + a.get_vertices()[1].z + a.get_vertices()[2].z) / 3.0f;
		float z2 = (b.get_vertices()[0].z + b.get_vertices()[1].z + b.get_vertices()[2].z) / 3.0f;
		return z1 > z2;
	});

	for (Tri& tri : visible_tris) {
		// Convert world space to local (view) space
		Tri view = tri;
		Tri::multiply_with_matrix(tri, view, view_matrix);

		// Project triangles from 3D to 2D
		Tri projected = view;
		Tri::multiply_with_matrix(view, projected, screen_projection_matrix);
		projected.normalize_to_viewport(object.window->getView().getSize().x, object.window->getView().getSize().y);

		static Vec3D light_direction{0.0f, 0.0f, -1.0f};
		float light_dot = std::max(0.0f, Vec3D::dot_product(tri.normal(), light_direction) * 255.0f);

		sf::Color color = sf::Color(light_dot, light_dot, light_dot);

		// Hacky fix against artifacting... It also seems that my world might be flipped for some reason...
		if (view.get_vertices()[0].z > 0 || view.get_vertices()[1].z > 0 || view.get_vertices()[2].z > 0) continue;

		if (object.texture_loaded)
			Renderer::draw_triangle_textured(object.window, projected, object.diffuse_texture, should_use_lighting ? light_dot : 255.0f);
		else
			Renderer::draw_triangle_vertex_colors(object.window, projected, should_use_lighting ? light_dot : 255.0f);

		// Fair warning: enabling this can and will reduce performance drastically;
		// It quite literally doubles the render cost per frame!
		if (should_draw_wireframe)
			Renderer::draw_tri_wireframe(object.window, projected);
	}
}
