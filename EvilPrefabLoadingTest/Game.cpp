#include <fstream>
#include "Game.h"

Dictionary globalSpawnArgs;
Game game;

void Game::Run(std::unique_ptr<PrefabFactoryBase> && newFactory) {
	prefabFactory = std::move(newFactory);										// user tells Game (eMap~) how it will create prefabs
	prefabFactory->SetOwner(this);

	std::ifstream read("MapWithPrefabsListed.map");
	if (!read.good()) {
		std::cout << "File read error\n";
		system("PAUSE");
		return;
	} else {
		std::cout << "File found\n";
	}

	char buffer[MAX_ESTRING_LENGTH];

	while (!read.eof()) {
		while (read.peek() == '#')
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// ignore comment lines between prefab definitions

		memset(buffer, 0, sizeof(buffer));
		read.getline(buffer, sizeof(buffer), '{');								// no spaces between prefabName and '{'
		std::string prefabName(buffer);
		read.ignore();															// ignore the newline after '{'
		
		prefabFactory->CreatePrefab(prefabName);								// add in instance of a default constructed prefab to the factory's list

		while (read.peek() != '}') {
			memset(buffer, 0, sizeof(buffer));
			read.getline(buffer, sizeof(buffer), ':');							// spawnArgs key
			std::string key(buffer);

			while (read.peek() == ' ' || read.peek() == '\t')					// skip whitespace b/t ':' and spawnArgs value
				read.ignore();

			memset(buffer, 0, sizeof(buffer));
			read.getline(buffer, sizeof(buffer), '\n');							// spawnArgs value
			std::string value(buffer);

			if (!key.empty() && !value.empty())
				globalSpawnArgs.Set(key.c_str(), value.c_str());
		}

		read.ignore(2, '\n');													// ignore "}\n"

		if (!globalSpawnArgs.IsEmpty()) {
			prefabFactory->SpawnPrefabInstance(prefabName);
			globalSpawnArgs.Clear();
		}
	}

	std::cout << "Load Complete\n";

	while (true) {																// deliberate infinite loop, use breakpoint to test output
		for (auto & entity : entities)
			entity->Think();
	}
}

int Game::AddEntity(std::unique_ptr<Entity> && newEntity) {
	int spawnID = 0;
	for (auto & entity : entities) {
		if (entity == nullptr) {
			newEntity->spawnID = spawnID;
			entities[spawnID] = std::move(newEntity);
			return spawnID;
		} else {
			++spawnID;
		}
	}

	if (spawnID == entities.size()) {
		newEntity->spawnID = spawnID;
		entities.emplace_back(std::move(newEntity));
		return spawnID;
	}
	return -1;
}