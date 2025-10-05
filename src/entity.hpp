#pragma once

namespace lm {
using EntityType = unsigned;
enum class Entity : EntityType {
	Empty,
	Wall,
	Player,
	PlayableArea,
	FinishArea,
	Coke,
	Fentanyl,
	Hitman,
	Enemy,
	EnemyBoss
};
}
