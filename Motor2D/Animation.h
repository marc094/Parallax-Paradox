#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "p2Point.h"
#include "p2DynArray.h"

struct AnimationFrame {
	SDL_Rect rect;
	iPoint pivot;
};

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	p2DynArray<AnimationFrame> frames;
	const char* name;

private:
	float current_frame = 0.0f;
	int last_frame = 0;
	int loops = 0;

public:

	Animation()
	{}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), last_frame(anim.last_frame), frames(anim.frames)
	{
		//SDL_memcpy(&frames, anim.frames, sizeof(frames));
	}

	void PushBack(const SDL_Rect& rect, const iPoint& pivot = { 0, 0 })
	{
		frames[last_frame++] = { rect, pivot };
	}

	AnimationFrame& GetCurrentFrame()
	{
		current_frame += speed;
		if(current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0.0f;
		loops = 0;
		if (speed == 0.0f)
			speed = 0.15f;
	}

	int getFrameIndex() const
	{
		return (int)current_frame;
	}
};

#endif