#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "object.hpp"

class Cvijet : public MeshObject {
public:
	Cvijet(sf::RenderWindow* window_);

	void draw() override;
};
