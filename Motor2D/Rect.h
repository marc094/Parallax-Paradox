#ifndef __RECT_H__
#define __RECT_H__

template<class TYPE>
struct Rect {
	TYPE x, y, w, h;

	SDL_Rect toSDL_Rect() {
		return { (int)x, (int)y, (int)w, (int)h };
	}

	Rect(const Rect &rect) {
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	}

	Rect(const SDL_Rect &rect) {
		x = rect.x;
		y = rect.y;
		w = rect.w;
		h = rect.h;
	}

	Rect operator *(TYPE n) const {
		Rect<TYPE> r(x * n, y * n, w * n, h * n);
		return r;
	}

	Rect() {}
	Rect(TYPE x, TYPE y, TYPE w, TYPE h) : x(x), y(y), w(w), h(h) {}
	~Rect() {}
};
typedef Rect<int> iRect;

#endif