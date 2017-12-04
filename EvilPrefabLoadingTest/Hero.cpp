#include "Hero.h"
#include "Game.h"

Hero::Hero() {
	SetBehavior(std::make_unique<DerivedBehavior2>());			// test that the Behavior subclass values are copied in Entity copy-ctor/assignment
	behavior->SetOwner(this);
	behavior->Start();
}

// makes a copy of itself (a prefab), then initializes it and add it to the game.entities
bool Hero::Spawn() {
	auto & newHero = std::make_unique<Hero>(*this);	// copy, don't move whatever the prefab Hero was constructed with

	newHero->spawnArgs = globalSpawnArgs;			// copy, don't move

	newHero->xPosition = newHero->spawnArgs.GetFloat("xposition", "0");
	newHero->yPosition = newHero->spawnArgs.GetFloat("yposition", "0");

	std::string behaviorName = newHero->spawnArgs.GetString("behavior", "behavior1");
	std::unique_ptr<Behavior> newBehavior = nullptr;
	if (behaviorName == "behavior1")
		newBehavior = std::make_unique<DerivedBehavior1>();
	else if (behaviorName == "behavior2")
		newBehavior = std::make_unique<DerivedBehavior2>();

	if (newBehavior != nullptr) {
		newHero->SetBehavior(std::move(newBehavior));
		newHero->behavior->SetOwner(newHero.get());
		newHero->behavior->Start();
	}

	game.AddEntity(std::move(newHero));
	std::cout << "Hero Spawned\n"; 
	return true;
}

void Hero::Think() {
	if (behavior != nullptr)
		behavior->Update();
}