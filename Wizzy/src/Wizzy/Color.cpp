#include "wzpch.h"

#include "Wizzy/Color.h"

namespace Wizzy {
	Color::Color()
		: r(1), g(1), b(1), a(1) {

	}
	Color::Color(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a) {

	}

    Color& Color::Add(const Color& other) {
        this->r += other.r;
        this->g += other.g;
        this->b += other.b;
        this->a += other.a;

        return *this;
    }
    Color& Color::Subtract(const Color& other) {
        this->r -= other.r;
        this->g -= other.g;
        this->b -= other.b;
        this->a -= other.a;

        return *this;
    }
    Color& Color::Multiply(const Color& other) {
        this->r *= other.r;
        this->g *= other.g;
        this->b *= other.b;
        this->a *= other.a;

        return *this;
    }
    Color& Color::Divide(const Color& other) {
        this->r /= other.r;
        this->g /= other.g;
        this->b /= other.b;
        this->a /= other.a;

        return *this;
    }

    Color operator+(Color left, const Color& right) {
        return left.Add(right);
    }
    Color operator-(Color left, const Color& right) {
        return left.Subtract(right);
    }
    Color operator*(Color left, const Color& right) {
        return left.Multiply(right);
    }
    Color operator/(Color left, const Color& right) {
        return left.Divide(right);
    }

	const Color Color::white =		Color(1.f, 1.f, 1.f, 1.f);
	const Color Color::red =		Color(.8f, .1f, .1f, 1.f);
	const Color Color::green =		Color(.1f, 1.f, .1f, 1.f);
	const Color Color::blue =		Color(.1f, .1f, 1.f, 1.f);
	const Color Color::cyan =		Color(.1f, 1.f, 1.f, 1.f);
	const Color Color::black =		Color(.0f, .0f, .0f, 1.f);
	const Color Color::darkGrey =	Color(.2f, .2f, .2f, 1.f);
	const Color Color::gray =		Color(.5f, .5f, .5f, 1.f);
	const Color Color::lightGrey =	Color(.75f, .75f, .75f, 1.f);
	const Color Color::pink =		Color(1.f, .25f, .35f, 1.f);
	const Color Color::magenta =	Color(1.f, .1f, .8f, 1.f);
	const Color Color::yellow =		Color(0.f, .8f, .8f, 1.f);
	const Color Color::violet =		Color(85.f, .1f, .7f, 1.f);
}
