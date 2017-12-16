#include "j1Transition.h"



j1Transition::j1Transition()
{
}


j1Transition::~j1Transition()
{
}

j1Transition::Transition j1Transition::TransitionType(j1Transition::Transition t)
{
	if (t == NONE) return type;	
	type = t;
	return NONE;
}

bool j1Transition::Awake(pugi::xml_node &)
{
	return true;
}

bool j1Transition::Start()
{
	return true;
}

bool j1Transition::PreUpdate()
{
	return true;
}

bool j1Transition::Update(float dt)
{
	return true;
}

bool j1Transition::PostUpdate()
{
	return true;
}

bool j1Transition::CleanUp()
{
	return true;
}

bool j1Transition::Load(pugi::xml_node &)
{
	return true;
}

bool j1Transition::Save(pugi::xml_node &) const
{
	return true;
}
