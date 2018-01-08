#ifndef PURE_GAME_HEADER_H
#define PURE_GAME_HEADER_H

#include <memory>

class PureGame {
public:

									PureGame();
	virtual							~PureGame();

									PureGame(PureGame && other);
	PureGame &						operator=(PureGame && other);

									PureGame(const PureGame & other);
	PureGame &						operator=(const PureGame & other);

	// user calls Init which calls StartUp
	bool							Init();			// internal engine initialization
	virtual bool					StartUp();		// user-defined extra engine initialization

	void							SetName(const char * newName);
	const char *					GetName() const;

private:

	struct Impl;
	std::unique_ptr<Impl> pImpl;
};

#endif /* PURE_GAME_HEADER_H */