#pragma once

namespace Wizzy {

	struct Color {
		Color();
		Color(float r, float g, float b, float a);

		union {
			float rgba[4];
			struct { float r, g, b, a; };
			glm::vec4 asVec4;
		};


		glm::vec4 ToVec4() const {
			return glm::vec4(r, g, b, a);
		}

        Color& Add(const Color& other);
        Color& Subtract(const Color& other);
        Color& Multiply(const Color& other);
        Color& Divide(const Color& other);

        friend Color operator+(Color left, const Color& right);
        friend Color operator-(Color left, const Color& right);
        friend Color operator*(Color left, const Color& right);
        friend Color operator/(Color left, const Color& right);

		static const Color white;
		static const Color red;
		static const Color green;
		static const Color blue;
		static const Color cyan;
		static const Color black;
		static const Color darkGray;
		static const Color gray;
		static const Color lightGray;
		static const Color pink;
		static const Color magenta;

        inline static Color Lerp(const Color& a, const Color& b, float f) {
            return Color(a.r + f, a.g + f, a.b + f, a.a + f) * (b - a);
        }
	};
}
