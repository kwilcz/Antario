#pragma once
#include "Utils\DrawManager.h"
#include "SDK\Vector.h"

class MouseCursor
{
public:
    MouseCursor()
    {
        this->vecPosition = g_Render.GetScreenCenter();
    };
    void Draw()
    {
        float x = vecPosition.x;
        float y = vecPosition.y;

        // Draw inner fill color
        Vector2D vecPos1 = Vector2D(x + 1, y + 1);
        Vector2D vecPos2 = Vector2D(x + 25, y + 12);
        Vector2D vecPos3 = Vector2D(x + 12, y + 25);
        g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);
        
        // Draw second smaller inner fill color
        vecPos1 = Vector2D(x + 6, y + 6);
        vecPos2 = Vector2D(x + 19, y + 12);
        vecPos3 = Vector2D(x + 12, y + 19);
        g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);

        // Draw border
        g_Render.Line(x, y, x + 25, y + 12, Color(0, 0, 0, 200));
        g_Render.Line(x, y, x + 12, y + 25, Color(0, 0, 0, 200));
        g_Render.Line(x + 25, y + 12, x + 12, y + 25, Color(0, 0, 0, 200));
        
    };
    Vector2D    GetPosition()                { return vecPosition; };
    void        SetPosition(Vector2D vecPos) { this->vecPosition = vecPos; };

private:
    Vector2D vecPosition;
};

