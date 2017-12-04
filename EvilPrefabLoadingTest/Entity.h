#ifndef TEST_ENTITY_H
#define TEST_ENTITY_H

#include <memory>
#include "Dictionary.h"
#include "Behavior.h"

class Entity {
public:

	friend class Game;

public:

	virtual					   ~Entity() = default;
								Entity() = default;
								Entity(const Entity & other);
								Entity(Entity && other) = default;
	Entity &					operator=(const Entity & other);
	Entity &					operator=(Entity && other) = default;

	virtual bool				Spawn();
	virtual void				Think() {}
	void						SetBehavior(std::unique_ptr<Behavior> && newBehavior) { behavior = std::move(newBehavior); }

	float &						GetX() { return xPosition; }
	float &						GetY() { return yPosition; }

	// TODO: basic attached components updating, then let user create new algorithms
//	void						UpdateComponents() { if (behavior != nullptr) behavior->Update(); }	

protected:

	std::unique_ptr<Behavior>	behavior	= nullptr;
	Dictionary					spawnArgs;
	int							spawnID		= -1;
	float						xPosition	= 0.0f;
	float						yPosition	= 0.0f;

};


inline Entity::Entity(const Entity & other) 
	: spawnArgs(other.spawnArgs),
	  spawnID(other.spawnID),
	  xPosition(other.xPosition),
	  yPosition(other.yPosition) {
	if (other.behavior != nullptr) {
		behavior = other.behavior->GetCopy();		// maintain the runtime type of the Behavior
		std::cout << "Behavior Copied via CCTOR: " << behavior->Name() << '\n';
	}
}

inline Entity & Entity::operator=(const Entity & other) {
	spawnArgs = other.spawnArgs;
	spawnID = other.spawnID;
	xPosition = other.xPosition;
	yPosition = other.yPosition;
	if (other.behavior != nullptr) {
		behavior = other.behavior->GetCopy();		// maintain the runtime type of the Behavior
		std::cout << "Behavior Copied via operator=: " << behavior->Name() << '\n';
	}
	return *this;
}

#endif /* TEST_ENTITY_H */