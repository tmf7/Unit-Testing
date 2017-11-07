#include <iostream>
#include <array>
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>
#include <typeindex>

class eComponent { 
public: 
	
	virtual ~eComponent() = default; 
};

class eCollider : public eComponent { 
public:

	eCollider() = default;
	eCollider(int uniqueNumber) : intComponent(uniqueNumber) {}

	int GetInt() const { return intComponent; }

private:
	
	int intComponent = 1;
};

class eBoxCollider : public eCollider {
public:

	eBoxCollider() = default;
	eBoxCollider(std::string & uniqueName) : name(uniqueName) {}

	const std::string & GetName() const { return name; }

private:
	
	std::string name = "defaultBoxCollider";
};


class eSprite : public eComponent { 
public:

	eSprite() = default;
	eSprite(double uniqueNumber) : doubleComponent(uniqueNumber) {}

	double	GetDouble() const { return doubleComponent; }

private:

	double doubleComponent = 1.0f;
};

#define REGISTER_ENUM(x) x,
enum {
	REGISTER_ENUM(GameObject)
	REGISTER_ENUM(Entity)
	REGISTER_ENUM(Tile)
	Invalid
};
#undef REGISTER_ENUM

/*
#define REGISTER_ENUM(x) #x,
static const char * names[] {
	REGISTER_ENUM(GameObject)
	REGISTER_ENUM(Entity)
	"invalid"
};
#undef REGISTER_ENUM
*/

class eGameObject {
public:

	virtual			~eGameObject() = default;

	virtual	int		GetGameObjectType() { return GameObject; }

	template < typename ReturnType >
	ReturnType &	GetComponent() {
		return *static_cast<ReturnType *>(components[std::type_index(typeid(ReturnType))].get());
	}

protected:

	std::unordered_map<std::type_index, std::unique_ptr<eComponent>> components;
	
};

// NOTE: eGameObject can only have one component of each type (not currently counting base class vs. derived class conflicts)
// NOTE: works with derived classes, but a component cannot be gotten by its base class
class eEntity : public eGameObject {
public:

	eEntity () = default;
	eEntity(int justCollider, int id) : entityID(id) {
		components[std::type_index(typeid(eCollider))] = std::make_unique<eCollider>(justCollider);  
	}
	eEntity(double justSprite, int id) : entityID(id) {
		components[std::type_index(typeid(eSprite))] = std::make_unique<eSprite>(justSprite);
	}
	eEntity(std::string && collider, double sprite, int id) : entityID(id) {
		components[std::type_index(typeid(eBoxCollider))] = std::make_unique<eBoxCollider>(collider);  
		components[std::type_index(typeid(eSprite))] = std::make_unique<eSprite>(sprite);
	}

	virtual int GetGameObjectType() override { return Entity; }

	int GetID() const { return entityID; }


private:
	
	int entityID = 1;

};

class eTile : public eGameObject {
public:

				eTile() = default;
				eTile(std::string && uniqueName) : name(uniqueName) {}

	virtual int GetGameObjectType() override { return Tile; }

	const std::string & GetName() const { return name; }

private:

	std::string name = "eTile";
};

int main() {
	// NOTE: copying them into an array (instead of their pointers) casts them and negates virtual use w/o another cast to the derived class
	eEntity one, two(2,2), three(3.0f,3), four("four",4.0f,4);
	eTile monkey("monkey");
	std::array<eGameObject *, 5> entities = { &one, &two, &monkey, &three, &four };
	
	for (auto & ent : entities) {
		switch (ent->GetGameObjectType()) {
			case Entity:
				std::cout << "eEntity (" << static_cast<eEntity *>(ent)->GetID() << "):\n";
				break;
			case Tile:
				std::cout << "eTile (" << static_cast<eTile *>(ent)->GetName() << "):\n";
				break;
		}

		auto & pC = ent->GetComponent<eCollider>();
		if (&pC != nullptr)
			std::cout << "eCollider::int = (" << pC.GetInt() << ")\n";
	
		auto & pS = ent->GetComponent<eSprite>();
		if (&pS != nullptr)
			std::cout << "eSprite::double = (" << pS.GetDouble() << ")\n";

		auto & pB = ent->GetComponent<eBoxCollider>();
		if (&pB != nullptr) {
			std::cout << "eBoxCollider::int = (" << pB.GetInt() << ")\n";
			std::cout << "eBoxCollider::name = (" << pB.GetName() << ")\n";
		}

		std::cout << '\n';
	}

	return 0;
}
