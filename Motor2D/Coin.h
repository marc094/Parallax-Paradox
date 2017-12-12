#pragma once
#include "Entity.h"
class Coin :
	public Entity
{
public:
	Coin();
	~Coin();
	bool Update(float dt);
};

