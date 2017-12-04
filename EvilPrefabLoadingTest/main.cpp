#include "Game.h"
#include "PrefabFactoryUser.h"

int main() {

	game.Run(std::make_unique<PrefabFactoryUser>());

	return 0;
}