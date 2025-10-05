#include "map.hpp"
#include "entity.hpp"
#include "result.hpp"
#include <list>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <iostream>

namespace lm {
struct MapT {
	std::list<EntityDesc> emptyLayout;
	std::list<EntityDesc> wallLayout;
	std::list<EntityDesc> playerLayout;
	std::list<EntityDesc> playableAreaLayout;
	std::list<EntityDesc> finishAreaLayout;
	std::list<EntityDesc> cokeLayout;
	std::list<EntityDesc> fentanylLayout;
	std::list<EntityDesc> hitmanLayout;
	std::list<EntityDesc> enemyLayout;
	std::list<EntityDesc> enemyBossLayout;

	unsigned blockOffsetX;
	unsigned blockOffsetY;
	unsigned blockSize;

	sf::Keyboard::Key primaryLeft, primaryRight, primaryUp, primaryDown;

	MapSize width, height;

	std::unique_ptr<sf::Font> mainFont;
	std::unique_ptr<sf::Text> bannerText;
};

std::list<EntityDesc>* GetLayout(Map map, const std::string& input) {
	if (input == "Wall")
		return &map->wallLayout;
	else if (input == "Player")
		return &map->playerLayout;
	else if (input == "PlayableArea")
		return &map->playableAreaLayout;
	else if (input == "FinishArea")
		return &map->finishAreaLayout;
	else if (input == "Coke")
		return &map->cokeLayout;
	else if (input == "Fentanyl")
		return &map->fentanylLayout;
	else if (input == "Hitman")
		return &map->hitmanLayout;
	else if (input == "Enemy")
		return &map->enemyLayout;
	else if (input == "EnemyBoss")
		return &map->enemyLayout;
	else return &map->emptyLayout;
}

int CreateMap(Map* map, unsigned blkSz, unsigned offX, unsigned offY) {
	*map = new MapT;
	(*map)->blockSize = blkSz;
	(*map)->blockOffsetX = offX;
	(*map)->blockOffsetY = offY;
	(*map)->primaryUp = sf::Keyboard::Key::Up;
	(*map)->primaryDown = sf::Keyboard::Key::Down;
	(*map)->primaryLeft = sf::Keyboard::Key::Left;
	(*map)->primaryRight = sf::Keyboard::Key::Right;
	(*map)->mainFont.reset(new sf::Font{"./data/DancingScript-Regular.ttf"});
	(*map)->bannerText.reset(new sf::Text{*(*map)->mainFont});
	return Result::Success;
}

void DestroyMap(Map map) {
	delete map;
}

int ValidateMapFile(MapSize width, MapSize height, const std::string& src) {
	std::ifstream stream{src};
	if (!stream.is_open())
		return Result::NotFound;	

	std::string element{};
	std::size_t count{};

	while (stream >> element)
		++count;

	if (width * height != count)
		return Result::DomainError;
	return Result::Success;
}

int ReadMap(Map map, MapSize width, MapSize height, const std::string& src) {
	map->width = width;
	map->height = height;

	if (auto ret = ValidateMapFile(width, height, src) != Result::Success)
		return ret;

	std::ifstream stream{src};
	if (!stream.is_open())
		return Result::NotFound;	

	for (MapSize row = 0; row < height; ++row)
		for (MapSize col = 0; col < width; ++col) {
			std::string field{};
			stream >> field;
			auto dst = GetLayout(map, field);
			dst->push_back({col, 1, row, 1});
		}

	return Result::Success;
}

int RenderMap(Map map, sf::RenderWindow* win) {

	auto renderLayout = [blkSz=map->blockSize, offX=map->blockOffsetX, offY=map->blockOffsetY, win](
			const auto& layout, sf::Color col) {
		sf::RectangleShape block(sf::Vector2f(blkSz, blkSz));
		for (const auto& e : layout) {
			block.setPosition(sf::Vector2f(offX + e.left * blkSz, offY + e.top * blkSz));
			block.setFillColor(col);
			win->draw(block);
		}
	};

	renderLayout(map->emptyLayout, sf::Color(28, 72, 107, 255));
	renderLayout(map->wallLayout, sf::Color(77, 52, 43, 255));
	renderLayout(map->enemyLayout, sf::Color::Red);
	renderLayout(map->enemyBossLayout, sf::Color::Red);
	renderLayout(map->playerLayout, sf::Color(40, 163, 82, 255));

	win->draw(*map->bannerText);	
	return Result::Success;
}

bool emptyExist(Map map, const EntityDesc& query) {
	for (const auto& e : map->emptyLayout)
		if (e.left == query.left && e.top == query.top)
			return true;
	return false;
}

void updateOnKeyPress(Map map, sf::Keyboard::Key key, EntityDesc& object,
		bool increment, bool sense) {
		if (sf::Keyboard::isKeyPressed(key)) {
			if (!emptyExist(map, object))
				map->emptyLayout.push_back({object.left, map->blockSize, object.top,
						map->blockSize});
			if (increment) {
				if (sense)
					++object.left;
				else ++object.top;
			}
			else {
				if (sense)
					--object.left;
				else --object.top;
			}
		}
}

bool collide(Map map, const EntityDesc& object) {
	for (const auto& e : map->wallLayout) {
		if (object.left == e.left && object.top == e.top)
			return true;
	}
	return false;
}

bool killCheck(Map map, const EntityDesc& object) {
	for (const auto& e : map->enemyLayout) {
		if (object.left == e.left && object.top == e.top)
			return true;
	}
	for (const auto& e : map->enemyBossLayout) {
		if (object.left == e.left && object.top == e.top)
			return true;
	}
	return false;
}

int UpdateMap(Map map, sf::RenderWindow* win) {
	static bool update = true;
	static bool won = false;
	if (update) {
		auto& primaryPlayer = map->playerLayout.front();
		if (primaryPlayer.left > map->blockOffsetX / map->blockSize) {
			updateOnKeyPress(map, map->primaryLeft, primaryPlayer, false, true);
			if (collide(map, primaryPlayer))
				++primaryPlayer.left;
		}

		if (primaryPlayer.left < map->width - map->blockOffsetX / map->blockSize - 1) {
			updateOnKeyPress(map, map->primaryRight, primaryPlayer, true, true);
			if (collide(map, primaryPlayer))
				--primaryPlayer.left;
		}

		if (primaryPlayer.top > map->blockOffsetX / map->blockSize) {
			updateOnKeyPress(map, map->primaryUp, primaryPlayer, false, false);
			if (collide(map, primaryPlayer))
				++primaryPlayer.top;
		}

		if (primaryPlayer.top < map->height - map->blockOffsetY / map->blockSize + 1) {
			updateOnKeyPress(map, map->primaryDown, primaryPlayer, true, false);
			if (collide(map, primaryPlayer))
				--primaryPlayer.top;
		}

		for (auto& enemy : map->enemyBossLayout) {
			if (enemy.top > map->blockOffsetX / map->blockSize) {
				++enemy.top;
			}

			if (enemy.top < map->height - map->blockOffsetY / map->blockSize + 1) {
				--enemy.top;
			}
		}

		if (killCheck(map, primaryPlayer)) {
			std::cerr << "Hit enemy" << std::endl;
			update = false;
		}
	}
	else {
		sf::RectangleShape banner{};
		banner.setSize(sf::Vector2f(map->width / 2, map->height / 2));
		auto& text = *map->bannerText;
		text.setCharacterSize(60);
		if (won) {
			text.setString("You Win");
			text.setFillColor(sf::Color::Green);
		}
		else {
			text.setString("You Loose");
			text.setFillColor(sf::Color::Red);
		}
	}

	return Result::Success;
}
}
