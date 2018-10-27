#pragma once
#include "SPoint.h"

class SRect
{
public:
    int left, top, right, bottom;

    constexpr SRect() : left(0), top(0), right(0), bottom(0) { }
    constexpr SRect(int lft, int tp, int rght, int bttm)  : left(lft),   top(tp),    right(rght),  bottom(bttm)  { }
    constexpr SRect(const SPoint& pt1, const SPoint& pt2) : left(pt1.x), top(pt1.y), right(pt2.x), bottom(pt2.y) { }

    constexpr SRect& operator +=(const SPoint& pt)
    {
        this->top    += pt.y;
        this->bottom += pt.y;
        this->left   += pt.x;
        this->right  += pt.x;
        return *this;
    }
    constexpr SRect& operator -=(const SPoint& pt)
    {
        this->top    -= pt.y;
        this->bottom -= pt.y;
        this->left   -= pt.x;
        this->right  -= pt.x;
        return *this;
    }

    constexpr int    Height() const { return this->bottom - this->top; }
    constexpr int    Width()  const { return this->right - this->left; }
    constexpr SPoint Pos()    const { return SPoint(left, top);        }   
    constexpr SPoint Mid()    const { return SPoint((left + right) / 2, (top + bottom) / 2); }

    constexpr bool ContainsPoint(const SPoint& pt) const
    {
        const auto tmp = *this; /* Fixes some weird bux I had */
        if (tmp.top    > pt.y) return false;
        if (tmp.bottom < pt.y) return false;
        if (tmp.left   > pt.x) return false;
        if (tmp.right  < pt.x) return false;
        return true;
    }

    constexpr void Scissor(const SRect& rc) 
    {
        const auto tmp = this; /* same with the fix */
        if (tmp->top    < rc.top)    tmp->top    = rc.top;
        if (tmp->bottom > rc.bottom) tmp->bottom = rc.bottom;
        if (tmp->left   < rc.left)   tmp->left   = rc.left;
        if (tmp->right  > rc.right)  tmp->right  = rc.right;
    }
};