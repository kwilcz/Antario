#pragma once

struct Color
{
    int red, green, blue, alpha;

    Color()
    {
        this->red   = 0;
        this->green = 0;
        this->blue  = 0;
        this->alpha = 255;
    }

    Color(int r, int g, int b, int a = 255)
        : red{ r }, green{ g }, blue{ b }, alpha{ a }
    { }

    Color& operator /= (float div)
    {
        Color& color = *this;
        const float flDiv  = 1.f / div;
        color.red    = static_cast<int>(color.red * flDiv);
        color.green  = static_cast<int>(color.green * flDiv);
        color.blue   = static_cast<int>(color.blue * flDiv);
        return color;
    }

    Color& operator *= (float coeff)
    {
        Color& color = *this;
        color.red    = static_cast<int>(color.red * coeff);
        color.green  = static_cast<int>(color.green * coeff);
        color.blue   = static_cast<int>(color.blue * coeff);
        return color;
    }

    static Color Black()    { return Color(0, 0, 0); }
    static Color Grey()     { return Color(127, 127, 127); }
    static Color White()    { return Color(255, 255, 255); }
    static Color Red()      { return Color(255, 0, 0); }
    static Color Green()    { return Color(0, 255, 0); }
    static Color Blue()     { return Color(0, 0, 255); }
};