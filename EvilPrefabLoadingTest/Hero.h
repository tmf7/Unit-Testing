#ifndef TEST_HERO_H
#define TEST_HERO_H

#include "Entity.h"

class Hero : public Entity {
public:

								Hero();

	virtual bool				Spawn() override;
	virtual void				Think() override;

private:

	int	unusedData = 999;

};

#endif /* TEST_HERO_H */