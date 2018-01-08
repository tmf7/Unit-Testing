#include "PureGame.h"
#include <iostream>


class GameLocal : public PureGame {
public:

	virtual bool		StartUp() override;

};


int TestFunctionToPointTo(int param) {
	return param * 2;
}


bool GameLocal::StartUp() {
	std::cout << "GameLocal::StartUp called\n";
	SetName("IT WORKED!");
	return true;
}


int main() {

	GameLocal newGame;

	if (newGame.Init())
		std::cout << newGame.GetName() << '\n';

	system("PAUSE");
	return 0;

}