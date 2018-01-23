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
        : red{ r }, green{ g }, blue{ b }, alpha{ a } { }

    Color& operator *=(const float coeff)
    {
        this->red   = static_cast<int>(this->red   * coeff);
        this->green = static_cast<int>(this->green * coeff);
        this->blue  = static_cast<int>(this->blue  * coeff);
        return *this;
    }

    Color& operator /=(const float div)
    {
        const auto flDiv = 1.f / div;
        *this *= flDiv;
        return *this;
    }

    Color& operator *(const float coeff) const
    {
        auto color    = *this;
        return color *= coeff;
    }


    static Color Black()    { return {0, 0, 0};       }
    static Color Grey()     { return {127, 127, 127}; }
    static Color White()    { return {255, 255, 255}; }
    static Color Red()      { return {255, 0, 0};     }
    static Color Green()    { return { 0, 255, 0 };   }
    static Color Blue()     { return { 0, 0, 255 };   }
};