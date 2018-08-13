#pragma once




class SPoint
{
public:
    int x, y;
    constexpr SPoint() : x(0), y(0) {}
    constexpr SPoint(int posX, int posY) : x(posX), y(posY) {}
    constexpr SPoint(const SPoint &pt) = default;


    constexpr bool operator==(const SPoint& rhs) const
    {
        if (this->x != rhs.x) return false;
        if (this->y != rhs.y) return false;
        return true;
    }

    constexpr bool operator!=(const SPoint& rhs) const
    {
        return !(rhs == *this);
    }

    constexpr SPoint& operator +=(const SPoint& p2)
    {
        this->x += p2.x;
        this->y += p2.y;
        return *this;
    }

    constexpr SPoint& operator -=(const SPoint& p2)
    {
        this->x -= p2.x;
        this->y -= p2.y;
        return *this;
    }

    constexpr SPoint operator +(const SPoint& point) const
    {
        auto tmp = *this;
        return tmp += point;
    }

    constexpr SPoint operator -(const SPoint& point) const
    {
        auto tmp = *this;
        return tmp -= point;
    }

    constexpr SPoint operator +(const int& val) const
    {
        SPoint tmp = *this;
        tmp.x += val;
        tmp.y += val;
        return tmp;
    }

    constexpr SPoint operator -(const int& val) const
    {
        SPoint tmp = *this;
        tmp.x -= val;
        tmp.y -= val;
        return tmp;
    }

    constexpr SPoint operator *(const int& val) const
    {
        SPoint tmp = *this;
        tmp.x *= val;
        tmp.y *= val;
        return tmp;
    }

    constexpr SPoint operator *(const float& val) const
    {
        SPoint tmp = *this;
        auto x = float(tmp.x),
             y = float(tmp.y);
        x *= val;
        y *= val;
        tmp.x = roundFloat(x);
        tmp.y = roundFloat(y);
        return tmp;
    }
private:

    /* No constexpr in cmath yet */
    constexpr int roundFloat(const float x) const { return int(x >= 0 ? x + 0.5f : x - 0.5f); }
};