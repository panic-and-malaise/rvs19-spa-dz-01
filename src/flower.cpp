#include "renderer.hpp"
#include "flower.hpp"
#include "matrix.hpp"
#include "object.hpp"
#include "renderer.hpp"
#include "vec3d.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

Cvijet::Cvijet(sf::RenderWindow* window_) : MeshObject(window_) {}

void Cvijet::draw() {
	static sf::Clock delta_clock;
	sf::Time delta = delta_clock.restart();
	#define delta_time delta.asSeconds()

	// static so there's only one instance. I don't want to make more member variables.
	static float elapsed_time{};
	elapsed_time += delta_time;
	
	rotation.y = elapsed_time;

	renderer->render(*this);
}
