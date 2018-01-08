#include <iostream>
#include "BaseTemplate.h"
#include <memory>
#include <string>

typedef struct Entity_s {
	std::string name	= "I_AM_ENTITIY";
	int	spawnID			= 0;

	Entity_s(std::string && name, int spawnID) : name(name), spawnID(spawnID) {}

} Entity;

int main() {

	IntManager intTest;
	intTest.Specialize();
	std::cout << intTest.GetClassType() << '\t' << intTest.DontSpecialize() << '\t' << intTest.ExtensionFn(1) << '\n';

//	ResourceManager<int> & rmTest = intTest;
//	rmTest.Specialize();		// compile error (ResourceManager<int>::Specialize undefined, as expected)
//	std::cout << rmTest.GetClassType() << '\t' << rmTest.DontSpecialize() << '\n' ;//<< rmTest->ExtensionFn(1) << '\n';	// ResourceManager<int> doesn't have ExtensionFn, as expected
	// lastly ResourceManager<int>::DontSpecialize is called through the base class pointer/reference because its not virtual, as expected


	FloatManager floatTest;
	floatTest.Specialize();
	std::cout << floatTest.GetClassType() << '\t' << floatTest.DontSpecialize() << '\t' << floatTest.ExtensionFn(1) << '\n';

	std::vector<std::unique_ptr<Entity>> entities;
	for (int i = 0; i < 10; ++i) {
		entities.emplace_back(std::make_unique<Entity>(std::string("Entity_") + std::to_string(i), i));
	}

	std::cout << "\n[Initial Vector]\n";
	for (auto & entity : entities) {
		std::cout << entity->name << '\n';
	}

	entities[0] = nullptr;
	entities[3] = nullptr;
	entities[9] = nullptr;

	std::cout << "\n[after delete]\n";
	for (auto & entity : entities) {
		if (entity != nullptr)
			std::cout << entity->name << '\n';
		else
			std::cout << "[deleted]\n";
	}

	int spawnID = 0;
	auto & toInsert = std::make_unique<Entity>(std::string("Entity_NEW"), -1);
	for (auto & entity : entities) {
		if (entity == nullptr) {
			std::cout << "\n[re-assign]\n";
			entity = std::move(toInsert);
			entities[spawnID]->spawnID = spawnID;
			break;
		} else {
			++spawnID;
		}	
	}
	if (spawnID == entities.size()) {
		std::cout << "\n[emplace back]\n";
		entities.emplace_back(std::move(toInsert));
		entities[spawnID]->spawnID = spawnID;
	}
		
	std::cout << "\n[after auto-replace]\n";
	for (auto & entity : entities) {
		if (entity != nullptr)
			std::cout << entity->name << '\t' << entity->spawnID << '\n';
		else
			std::cout << "[deleted]\n";
	}


	system("PAUSE");
	return 0;
}