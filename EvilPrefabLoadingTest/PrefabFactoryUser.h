#ifndef TEST_PREFAB_FACTORY_USER_H
#define TEST_PREFAB_FACTORY_USER_H

#include "PrefabFactoryBase.h"
#include "Hero.h"

class PrefabFactoryUser : public PrefabFactoryBase {
public:

	virtual bool		CreatePrefab(const std::string & prefabName) override;

};

// no-op if the prefab already exists
inline bool PrefabFactoryUser::CreatePrefab(const std::string & prefabName) {
	if (prefabName == "Entity") {

		prefabEntities.emplace(prefabName,std::make_unique<Entity>());
	
	} else if (prefabName == "Hero") {

		prefabEntities.emplace(prefabName, std::make_unique<Hero>());		
	}
	return false;
}

#endif /* TEST_PREFAB_FACTORY_USER_H */