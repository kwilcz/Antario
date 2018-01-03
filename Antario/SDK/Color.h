#pragma once

struct Color
{
    int r, g, b, a;

    Color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 255;
    }

    Color(int r, int g, int b, int a = 255)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color& operator /= (float div)
    {
        Color& color = *this;
        float flDiv = 1.f / div;
        color.r = static_cast<int>(color.r * flDiv);
        color.g = static_cast<int>(color.g * flDiv);
        color.b = static_cast<int>(color.b * flDiv);
        return color;
    }

    Color& operator *= (float coeff)
    {
        Color& color = *this;
        color.r = static_cast<int>(color.r * coeff);
        color.g = static_cast<int>(color.g * coeff);
        color.b = static_cast<int>(color.b * coeff);
        return color;
    }

	static Color Black() {
		return Color(0, 0, 0);
	}
	static Color White() {
		return Color(255, 255, 255);
	}
	static Color Red() {
		return Color(255, 0, 0);
	}
	static Color Green() {
		return Color(0, 255, 0);
	}
	static Color Blue() {
		return Color(0, 0, 255);
	}
};