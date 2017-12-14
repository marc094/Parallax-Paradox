#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;
	bool Count(float num);
	void Set(float num);
	void UpdateLoadedtime() const;
	uint32 Started() const;
	uint32  loaded_time = 0;

private:
	uint32	started_at;

};

#endif //__j1TIMER_H__