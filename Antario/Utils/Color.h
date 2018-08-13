#pragma once

struct Color
{
    int red, green, blue, alpha;

    constexpr Color() : red(0), green(0), blue(0), alpha(255) { }

    constexpr Color(int r, int g, int b, int a = 255)
        : red(r), green(g), blue(b), alpha(a) { }

    constexpr Color& operator *=(const float coeff)
    {
        this->red   = int(this->red   * coeff);
        this->green = int(this->green * coeff);
        this->blue  = int(this->blue  * coeff);
        return *this;
    }

    constexpr Color& operator /=(const float div)
    {
        const float flDiv = 1.f / div;
        *this *= flDiv;
        return *this;
    }

    constexpr Color operator *(const float coeff) const
    {
        Color color = *this;
        return color *= coeff;
    }


    static constexpr Color Black(int a = 255) { return { 0, 0, 0, a }; }
    static constexpr Color Grey(int  a = 255) { return { 127, 127, 127, a }; }
    static constexpr Color White(int a = 255) { return { 255, 255, 255, a }; }
    static constexpr Color Red(int   a = 255) { return { 255, 0, 0, a }; }
    static constexpr Color Green(int a = 255) { return { 0, 255, 0, a }; }
    static constexpr Color Blue(int  a = 255) { return { 0, 0, 255, a }; }
};