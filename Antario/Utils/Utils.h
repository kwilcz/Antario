#pragma once
#include <Psapi.h>
#include "..\SDK\IVEngineClient.h"

#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

extern HANDLE g_hConsoleHandle;

class Utils
{
public:
    uintptr_t FindSignature(const char* szModule, const char* szSignature)
    {
        const char* pat = szSignature;
        DWORD firstMatch = 0;
        DWORD rangeStart = (DWORD)GetModuleHandleA(szModule);
        MODULEINFO miModInfo;
        GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
        DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
        for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
        {
            if (!*pat)
                return firstMatch;

            if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
            {
                if (!firstMatch)
                    firstMatch = pCur;

                if (!pat[2])
                    return firstMatch;

                if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
                    pat += 3;

                else
                    pat += 2;
            }
            else
            {
                pat = szSignature;
                firstMatch = 0;
            }
        }
        return NULL;
    }

    bool WorldToScreen(const Vector &origin, Vector &screen)
    {
        if (!ScreenTransform(origin, screen))
        {
            int iScreenWidth, iScreenHeight;
            g_pEngine->GetScreenSize(iScreenWidth, iScreenHeight);

            screen.x = (iScreenWidth * 0.5f) + (screen.x * iScreenWidth) * 0.5f;
            screen.y = (iScreenHeight * 0.5f) - (screen.y * iScreenHeight) * 0.5f;

            return true;
        }
        return false;
    }

    void LogToConsole(std::wstring str, ...)
    {
        WriteConsole(hConsoleHandle, str.c_str(), str.length(), NULL, NULL);
    }

    template <typename T>
    T CallVFunc(void* ppClass, int iIndex)
    {
        return (*(T**)ppClass)[iIndex];
    }

    void SetConsoleHandle(HANDLE hHandle)
    {
        hConsoleHandle = hHandle;
    }

private:
    HANDLE hConsoleHandle;

    bool ScreenTransform(const Vector& point, Vector& screen)
    {
        static ptrdiff_t viewMatrixPrt;
        if (!viewMatrixPrt)
        {
            viewMatrixPrt = static_cast<ptrdiff_t>(Utils::FindSignature("client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"));
            viewMatrixPrt += 0x3;
            viewMatrixPrt = *reinterpret_cast<uintptr_t*>(viewMatrixPrt);
            viewMatrixPrt += 176;
        }

        const VMatrix& w2sMatrix = *(VMatrix*)viewMatrixPrt;
        screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
        screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
        screen.z = 0.0f;

        float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

        if (w < 0.001f) {
            screen.x *= 100000;
            screen.y *= 100000;
            return true;
        }

        float invw = 1.0f / w;
        screen.x *= invw;
        screen.y *= invw;

        return false;
    }
};
extern Utils g_Utils;