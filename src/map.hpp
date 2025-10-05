#pragma once

#include "entity.hpp"
#include <string>

namespace sf {
class RenderWindow;
}

namespace lm {
struct MapT;
using Map = MapT*;

using MapSize = unsigned;

struct EntityDesc {
	MapSize left, width;
	MapSize top, height;
	bool changeDirection{false};
};

int CreateMap(Map*, unsigned blockSize, unsigned offsetX = 0, unsigned offsetY = 0);
void DestroyMap(Map);

int ReadMap(Map, MapSize width, MapSize height, const std::string& src);
int RenderMap(Map, sf::RenderWindow*);
int UpdateMap(Map, sf::RenderWindow*);
}
