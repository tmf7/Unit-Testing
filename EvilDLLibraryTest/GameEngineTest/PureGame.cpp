#include "PureGame.h"
#include <iostream>
#include <string>


struct PureGame::Impl {
	const char * name;
};


PureGame::PureGame()
	: pImpl(std::make_unique<Impl>()) {

}

PureGame::~PureGame() = default;

PureGame::PureGame(PureGame && other) = default;
PureGame & PureGame::operator=(PureGame && other) = default;

PureGame::PureGame(const PureGame & other)
	: pImpl(std::make_unique<Impl>(*other.pImpl)) {
}

PureGame & PureGame::operator=(const PureGame & other) {
	*pImpl = *(other.pImpl);
	return *this;
}



void PureGame::SetName(const char * newName) {
	pImpl->name = newName;
}

const char * PureGame::GetName() const {
	return pImpl->name;
}

bool PureGame::StartUp() {
	// left blank for user to override in a derived GameLocal type class
	std::cout << "PureGame::Startup called\n";
	return true;
}

bool PureGame::Init() {
	std::cout << "PureGame::Init called\n";
	return StartUp();
}

