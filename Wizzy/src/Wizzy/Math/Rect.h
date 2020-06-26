#pragma once

#include "Wizzy/Math/Vec.h"

namespace Wizzy
{
	struct Rect
	{
		Rect() : vec(fvec4(0.f)) {}
		Rect(const fvec4& vec) : vec(vec) {}
		Rect(const fvec2& pos, const fvec2& size) : position(pos), size(size) {}
		Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

		union
		{
			fvec4 vec;
			struct { float x, y, w, h; };
			struct { fvec2 position, size; };
			float ptr[4];
		};

		operator fvec4& ()
		{
			return vec;
		}

		inline float Bottom() const { return y; }
		inline float Top() const { return y + h; }
		inline float Left() const { return x; }
		inline float Right() const { return x + w; }

		inline bool IsValid() const { return w != 0 && h != 0; }

		inline fvec2 Center() const { return fvec2(Left() + w / 2.f, Bottom() + h / 2.f); }

		inline bool Intersects(const Rect& other, Rect* intersection = NULL)
		{
			bool xIntersect = (this->Left() > other.Left() && this->Left() < other.Right()) ||
				(this->Right() > other.Left() && this->Left() < other.Right());

			bool yIntersect = (this->Bottom() > other.Bottom() && this->Bottom() < other.Top()) ||
				(this->Top() > other.Bottom() && this->Bottom() < other.Top());

			if (intersection != NULL && (xIntersect && yIntersect))
			{
				intersection->x = 
					other.Left() > this->Left()
					? other.Left()
					: this->Left();
				
				intersection->y = 
					other.Bottom() > this->Bottom()
					? other.Bottom()
					: this->Bottom();
				
				intersection->w = 
					other.Left() > this->Left()
					? this->Right() - intersection->Left()
					: other.Right() - intersection->Left();
				
				intersection->h = 
					other.Bottom() > this->Bottom()
					? this->Top() - intersection->Bottom()
					: other.Top() - intersection->Bottom();
			}

			return xIntersect && yIntersect;
		}
	};
}

