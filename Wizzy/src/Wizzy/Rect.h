#pragma once

namespace Wizzy
{
	struct Rect
	{
		Rect() : vec({ 0.f, 0.f, 0.f, 0.f }) {}
		Rect(const glm::vec4& vec) : vec(vec) {}
		Rect(const glm::vec2& pos, const glm::vec2& size) : position(pos), size(size) {}
		Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

		union
		{
			glm::vec4 vec;
			struct { float x, y, w, h; };
			struct { glm::vec2 position, size; };
		};

		operator glm::vec4& ()
		{
			return vec;
		}

		inline float Bottom() const { return y; }
		inline float Top() const { return y + h; }
		inline float Left() const { return x; }
		inline float Right() const { return x + w; }

		inline bool IsValid() const { return w != 0 && h != 0; }

		inline vec2 Center() const { return vec2(Left() + w / 2.f, Bottom() + h / 2.f); }

		inline bool Intersects(const Rect& other, Rect* intersection = NULL)
		{
			bool xIntersect = (this->Left() > other.Left() && this->Left() < other.Right()) ||
				(this->Right() > other.Left() && this->Right() < other.Right());

			bool yIntersect = (this->Bottom() > other.Bottom() && this->Bottom() < other.Top()) ||
				(this->Top() > other.Bottom() && this->Top() < other.Top());

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

