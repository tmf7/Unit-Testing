#include "Game.h"

bool Entity::Spawn() {
	game.AddEntity(std::make_unique<Entity>());
	std::cout << "Entity Spawned\n"; 
	return true; 
}