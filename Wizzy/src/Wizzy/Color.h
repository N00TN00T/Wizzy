#pragma once

#include <imgui.h>

namespace Wizzy {

	struct Color {
		Color();
		Color(float r, float g, float b, float a);
		Color(const glm::vec4& vec) : asVec4(vec) {}
		Color(const ImVec4& vec) : imguiVec4(vec) {}

		union {
			float rgba[4];
			struct { float r, g, b, a; };
			glm::vec4 asVec4;
			ImVec4 imguiVec4;
		};


		glm::vec4 ToVec4() const {
			return glm::vec4(r, g, b, a);
		}

		operator glm::vec4& ()
		{
			return asVec4;
		}

		operator const glm::vec4& () const
		{
			return asVec4;
		}

		operator ImVec4& ()
		{
			return imguiVec4;
		}

		operator const ImVec4& () const
		{
			return imguiVec4;
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
		static const Color darkGrey;
		static const Color gray;
		static const Color lightGrey;
		static const Color pink;
		static const Color magenta;
		static const Color yellow;
		static const Color violet;

        inline static Color Lerp(const Color& a, const Color& b, float f) {
            return Color(a.r + f, a.g + f, a.b + f, a.a + f) * (b - a);
        }
	};
}
