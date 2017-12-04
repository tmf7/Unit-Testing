#ifndef TEST_PREFAB_FACTORY_H
#define TEST_PREFAB_FACTORY_H

#include "Entity.h"

class Game;

class PrefabFactoryBase {
public:

	virtual bool		CreatePrefab(const std::string & prefabName) = 0;
	void				SpawnPrefabInstance(const std::string & prefabName);
	void				SetOwner(Game * newOwner) { owner = newOwner; }

protected:

	Game *														owner;
	std::unordered_map<std::string, std::unique_ptr<Entity>>	prefabEntities;

};

inline void PrefabFactoryBase::SpawnPrefabInstance(const std::string & prefabName) { 
	prefabEntities[prefabName]->Spawn(); 
}

#endif /* TEST_PREFAB_FACTORY_H */