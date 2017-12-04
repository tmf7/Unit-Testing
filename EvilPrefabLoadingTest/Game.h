#ifndef TEST_GAME_H
#define TEST_GAME_H

#include <vector>
#include "PrefabFactoryBase.h"

class Game {
public:

	void										Run(std::unique_ptr<PrefabFactoryBase> && newFactory);
	int											AddEntity(std::unique_ptr<Entity> && newEntity);
	void										RemoveEntity(int spawnID) { entities[spawnID] = nullptr; }

private:

	std::vector<std::unique_ptr<Entity>>		entities;
	std::unique_ptr<PrefabFactoryBase>			prefabFactory;

};

extern Dictionary globalSpawnArgs;
extern Game game;

#endif /* TEST_GAME_H */