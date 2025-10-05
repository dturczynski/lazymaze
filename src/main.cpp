#include "extractParameter.hpp"
#include "result.hpp"
#include "windowProps.hpp"
#include "map.hpp"
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
	/* Map settings */
	constexpr unsigned mapWidth = 94;
	constexpr unsigned mapHeight = 50;
	constexpr unsigned blockSize = 20;

	namespace fs = std::filesystem;
	fs::current_path(fs::absolute(fs::path(argv[0]).parent_path()));

	lm::WindowSize winSize{};
	if (lm::initWindowSize(argc, argv, &winSize) != Result::Success) {
		std::cerr << "Failed to set proper window size!" << std::endl;
		return 1;
	}

	lm::Map map;
	if (auto ret = lm::CreateMap(&map, blockSize, blockSize, blockSize * 3); ret != Result::Success) {
		std::cerr << "Failed to create map" << std::endl;
		return 1;
	}
	std::unique_ptr<lm::MapT, void(*)(lm::Map)> uniqMap{map, lm::DestroyMap};

	if (auto ret = lm::ReadMap(map, mapWidth, mapHeight, "./data/level0.txt"); ret != Result::Success) {
		std::cerr << "Failed to read map!" << std::endl;
		return 1;
	}

	sf::RenderWindow window(sf::VideoMode({winSize.width, winSize.height}), "LazyMaze", sf::Style::None);
	window.setPosition({0,0});
	auto timeBegin = std::chrono::system_clock::now();

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>())
						window.close();
		}

		auto now = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeBegin) > std::chrono::milliseconds(40)) {
			lm::UpdateMap(map, &window);
			window.clear();
			lm::RenderMap(map, &window);
			window.display();
			timeBegin = now;	
		}
	}
}
