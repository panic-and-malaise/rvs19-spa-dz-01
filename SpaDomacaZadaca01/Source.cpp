#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "src/object.hpp"
#include "src/renderer.hpp"
#include "src/flower.hpp"

constexpr size_t WINDOW_WIDTH = 800;
constexpr size_t WINDOW_HEIGHT = 800;

constexpr size_t REFRESH_RATE = 60;

const std::string WINDOW_TITLE = "SPA-DZ-01";

#ifdef _WIN32 // Windows, tuned to Visual Studio using the solution directory as CWD
const std::string DEFAULT_MODELS_PATH    = "..\\models\\";
const std::string DEFAULT_TEXTURES_PATH  = "..\\textures\\";
#else // Linux
const std::string DEFAULT_MODELS_PATH   = "models/";
const std::string DEFAULT_TEXTURES_PATH = "textures/";
#endif

const std::string DEFAULT_MODEL   = "dandelion_lod0.obj";
const std::string DEFAULT_DIFFUSE = "dandelion_diffuse_alpha.png";

const sf::Color SKY_COLOR = sf::Color(130, 200, 229);

// Forward declared for readability of main
int handle_command_line_arguments(int argc, char* argv[], Renderer& renderer, MeshObject& cvijet);

int main(int argc, char* argv[]) {
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
	window.setFramerateLimit(REFRESH_RATE);

	Renderer renderer{};
	renderer.set_camera_pos(0.00179145f, 0.09254f, -0.218254f); // Magic numbers that I found work well for the flower mesh

	Cvijet cvijet(&window);
	cvijet.link_renderer(&renderer);

	if (handle_command_line_arguments(argc, argv, renderer, cvijet))
		return 0;

	// Capture, hide, and center cursor if mouse look is disabled
	if (renderer.get_mouse_look()) {
		window.setMouseCursorVisible(false);
		window.setMouseCursorGrabbed(true);

		sf::Vector2i center(window.getSize().x / 2, window.getSize().y / 2);
		sf::Mouse::setPosition(center, window);
	}

	sf::Clock delta_clock;
	sf::Clock fps_clock;

	sf::Time previous_time = delta_clock.getElapsedTime();
	sf::Time current_time;
	#define delta_time delta_clock.restart().asSeconds()

	float counter = 0.0f;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				// Recenter mouse on refocus to avoid snapping
				case sf::Event::GainedFocus: {
					// Just exit if mouse look is disabled
					if (!renderer.get_mouse_look())
						break;
					sf::Vector2i center(window.getSize().x / 2, window.getSize().y / 2);
					sf::Mouse::setPosition(center, window);
					break;
				}
				default:
					break;
			}
		}
		
		// renderer.physics_loop(delta_time);
		if (renderer.get_mouse_look())
			renderer.handle_mouse_look(window);

		window.clear(SKY_COLOR);

		cvijet.draw();

		window.display();

		current_time = fps_clock.getElapsedTime();
		float time_elapsed = current_time.asSeconds() - previous_time.asSeconds();

		float fps = 1.0f / time_elapsed;
		previous_time = current_time;

		counter += time_elapsed;

		// Update twice per second for readability
		if (counter >= 0.5f) {
			window.setTitle(WINDOW_TITLE + " | " + std::to_string((int)(fps + 0.5f)) + " FPS");
			counter = 0.0f;
		}
	}

	return 0;
}

int handle_command_line_arguments(int argc, char* argv[], Renderer& renderer, MeshObject& object) {
	std::stringstream help_menu{};

	help_menu << "Usage: spa-dz-01 [OPTION] [ARGUMENT] ...\n\n";
	help_menu << "  -h, --help\t\t\t show this help menu and exit\n\n";

	help_menu << "  -o, --obj \t\t\t specify .obj file to load\n";
	help_menu << "  -t, --texture \t\t specify texture file to load\n\n";

	help_menu << "  -bc, --backface-culling \t whether the renderer should do backface culling\t(default: true)\n";
	help_menu << "  -lt, --lighting \t\t whether the renderer should simulate lighting\t\t(default: true)\n";
	help_menu << "  -wf, --wireframe \t\t whether to draw a wireframe around the object\t\t(default: false)\n\n";

	help_menu << "  -cs, --camera-start\t\t the starting position of the camera\t\t\t(default: {0, 0, -5})\n";
	help_menu << "  -ms, --move-speed\t\t the movement speed of the camera\t\t\t(default: 0.5f)\n";
	help_menu << "  -ts, --turn-speed\t\t the turning speed of the camera\t\t\t(default: 2.0f)\n";
	help_menu << "  -ml, --mouse-look\t\t toggle mouse look!\t\t\t\t\t(default: off)\n";
	help_menu << "\n";

	help_menu << "Controls:\n\n";
	help_menu << "  WASD \t\t\t\t move horizontally\n";
	help_menu << "  Shift & Space \t\t move vertically\n";
	help_menu << "  Arrow keys    \t\t tilt camera";
	help_menu << "\n";

	std::string model   = DEFAULT_MODELS_PATH   + DEFAULT_MODEL;
	std::string texture = DEFAULT_TEXTURES_PATH + DEFAULT_DIFFUSE;

	// Load the models and textures, either from arguments or a default model
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		// Show help menu and exit
		if (arg == "--help" || arg == "-h") {
			std::cout << help_menu.str() << std::endl; // endl conveniently flushes the buffer here
			return 1;

		// Load .obj file, with optional LOD support
		} else if (arg == "--obj" || arg == "-o") {
			if (i + 1 < argc) {
				i++;
				model = argv[i];
			} else {
				std::cerr << "Error: --obj requires a filename as an argument!\n";
				return 1;
			}

		// Load texture from any image file
		} else if (arg == "--texture" || arg == "-t") {
			if (i + 1 < argc) {
				i++;
				texture = argv[i];
			}
		} else if (arg == "--backface-culling" || arg == "-bc") {
			renderer.toggle_backface_culling(false);
		} else if (arg == "--lighting" || arg == "-lt") {
			renderer.toggle_lighting(false);
		} else if (arg == "--wireframe" || arg == "-wf") {
			renderer.toggle_wireframe(true);
		} else if (arg == "--camera-start" || arg == "-cs") {
			i++;

			std::string value = argv[i];
			size_t brace_start = value.find('{');

			value.erase(std::remove(value.begin(), value.end(), '{'), value.end());
			value.erase(std::remove(value.begin(), value.end(), '}'), value.end());

			if (brace_start == std::string::npos) {
				std::cerr << "Invalid syntax! Should be: -cs \"{x, y, z}\"!";
				return 1;
			}

			std::stringstream ss(value);
			std::string temp;
			std::vector<float> coords;

			while (std::getline(ss, temp, ',')) {
				coords.push_back(std::stof(temp));
			}

			renderer.set_camera_pos(coords[0], coords[1], coords[2]);
		} else if (arg == "--move-speed" || arg == "-ms") {
			i++;
			float speed = std::stof(argv[i]);

			renderer.set_camera_move_speed(speed);
		} else if (arg == "--turn-speed" || arg == "-ts") {
			i++;
			float speed = std::stof(argv[i]);

			renderer.set_camera_turn_speed(speed);
		} else if (arg == "--mouse-look" || arg == "-ml") {
			renderer.toggle_mouse_look(true);

		// Exit on unknown argument
		} else {
			std::cerr << "Unknown argument: \"" << arg << "\"!\n";
			std::cout << help_menu.str() << std::endl;
			return 1;
		}
	}
	
	// Show the help menu if user didn't input any arguments
	if (argc < 2)
		std::cout << help_menu.str() << std::endl;

	// Attempt to load the model and its LODs
	try {
		object.load_mesh_from_obj(model, texture); // Load model

		// Assumes filenames of models with LODs end with "_lod[0123456789].obj"
		std::size_t lod_extension = model.find("_lod");
		bool is_lod = lod_extension != std::string::npos && lod_extension + 4 < model.size();

		if (is_lod) {
			std::cout << "Loading LODs...\n";
			try {
				// Attempt to load LODs 1, 2, and 3
				for (int i = 1; i < 4; ++i) {
					object.load_LOD_number(model.replace(lod_extension + 4, 1, std::to_string(i)), i);
					std::cout << "Loaded LOD " << i << "!\n";
				}
			} catch(std::exception& e) {
				std::cout << e.what() << '\n';
			}
		}

	} catch (std::exception& e) {
		std::cout << e.what() << '\n';
		std::cout << "Loading fallback model..." << std::endl;
	}

	return 0;
}
