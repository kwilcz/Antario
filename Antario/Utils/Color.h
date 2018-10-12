#pragma once

struct Color
{
    int red, green, blue, alpha;

    constexpr Color() : red(0), green(0), blue(0), alpha(255) { }

    constexpr Color(int r, int g, int b, int a = 255)
        : red{ r }, green{ g }, blue{ b }, alpha{ a } { }

    constexpr Color& operator *=(const float coeff)
    {
        this->red   = static_cast<int>(this->red * coeff);
        this->green = static_cast<int>(this->green * coeff);
        this->blue  = static_cast<int>(this->blue * coeff);
        return *this;
    }

    constexpr Color operator ()(const int a) const
    {
        return Color(red, green, blue, a);
    }

    constexpr Color& operator /=(const float div)
    {
        const auto flDiv = 1.f / div;
        *this *= flDiv;
        return *this;
    }

    constexpr Color& operator *(const float coeff) const
    {
        auto color    = *this;
        return color *= coeff;
    }

    constexpr DWORD GetARGB() const
    {
        return 0;
    }

    constexpr Color& FromHSV(float h, float s, float v)
    {
        float colOut[3]{ };
        if (s == 0.0f)
        {
            red = green = blue = static_cast<int>(v * 255);
            return *this;
        }

        h = std::fmodf(h, 1.0f) / (60.0f / 360.0f);
        int   i = static_cast<int>(h);
        float f = h - static_cast<float>(i);
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i)
        {
        case 0:
            colOut[0] = v;
            colOut[1] = t;
            colOut[2] = p;
            break;
        case 1:
            colOut[0] = q;
            colOut[1] = v;
            colOut[2] = p;
            break;
        case 2:
            colOut[0] = p;
            colOut[1] = v;
            colOut[2] = t;
            break;
        case 3:
            colOut[0] = p;
            colOut[1] = q;
            colOut[2] = v;
            break;
        case 4:
            colOut[0] = t;
            colOut[1] = p;
            colOut[2] = v;
            break;
        case 5: default:
            colOut[0] = v;
            colOut[1] = p;
            colOut[2] = q;
            break;
        }

        red   = static_cast<int>(colOut[0] * 255);
        green = static_cast<int>(colOut[1] * 255);
        blue  = static_cast<int>(colOut[2] * 255);
        return *this;
    }

    constexpr auto ToHSV(float& h, float& s, float& v)
    {
        float col[3]    = { red / 255.f, green / 255.f, blue / 255.f };

        float K = 0.f;
        if (col[1] < col[2])
        {
            swap(col[1], col[2]);
            K = -1.f;
        }
        if (col[0] < col[1])
        {
            swap(col[0], col[1]);
            K = -2.f / 6.f - K;
        }

        const float chroma = col[0] - (col[1] < col[2] ? col[1] : col[2]);
        h = colfabs(K + (col[1] - col[2]) / (6.f * chroma + 1e-20f));
        s = chroma / (col[0] + 1e-20f);
        v = col[1];
    }


    static constexpr Color Black(int a = 255) { return { 0, 0, 0, a }; }
    static constexpr Color Grey(int  a = 255) { return { 127, 127, 127, a }; }
    static constexpr Color White(int a = 255) { return { 255, 255, 255, a }; }
    static constexpr Color Red(int   a = 255) { return { 255, 0, 0, a }; }
    static constexpr Color Green(int a = 255) { return { 0, 255, 0, a }; }
    static constexpr Color Blue(int  a = 255) { return { 0, 0, 255, a }; }

private:
    constexpr void  swap(float& a, float& b) { float tmp = a; a = b; b = tmp; }
    constexpr float colfabs(const float& x)  { return x < 0 ? x * -1 : x; }

};