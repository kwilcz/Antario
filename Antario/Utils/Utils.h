#pragma once
#include <Psapi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include "..\SDK\IVEngineClient.h"

#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

class Utils
{
public:
    template<unsigned int IIdx, typename TRet, typename ... TArgs>
    static auto CallVFunc(void* thisptr, TArgs ... argList) -> TRet
    {
        using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
    }


    static uintptr_t FindSignature(const char* szModule, const char* szSignature)
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

            if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GET_BYTE(pat))
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
        return 0u;
    }

    // basefunct
    static std::string SetupStringParams(std::string szBasicString)
    {
        return szBasicString;
    }

    // Replace % with a desired string / value represented after semicolons. Works kinda like printf.
    template <typename T, typename... Targs>
    static std::string SetupStringParams(std::string szBasicString, T arg, Targs&& ...args)
    {
        const auto found = szBasicString.find_first_of('%');
        if (found != std::string::npos)
        {
            std::stringstream tmp;
            tmp << arg;
            szBasicString.replace(found, 1, tmp.str());
            szBasicString = SetupStringParams(szBasicString, std::forward<Targs>(args)...);
        }
        return szBasicString;
    }


    template <typename ... Args>
    static void Log(const std::string& str, Args ...arguments)
    {
        Utils::Log(Utils::SetupStringParams(str.c_str(), arguments...));
    }


    /**
    *   GetCurrentSystemTime - Gets actual system time
    *   @timeInfo: Reference to your own tm variable, gets modified.
    */
    static void GetCurrentSystemTime(tm& timeInfo)
    {
        const std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(systemNow);
        localtime_s(&timeInfo, &now_c); // using localtime_s as std::localtime is not thread-safe.
    };


    /**
    *   Log - Logs specified message to debug console if in debug mode.
    *   Format: [HH:MM:SS] str
    *   @str: Debug message to be shown.
    */
    static void Log(const std::string& str)
    {
#ifdef _DEBUG
        tm timeInfo { };
        Utils::GetCurrentSystemTime(timeInfo);

        std::stringstream ssTime; // Temp stringstream to keep things clean
        ssTime << "[" << std::put_time(&timeInfo, "%T") << "] " << str << std::endl;

        std::cout << ssTime.str();
#endif // _DEBUG
    };



    /**
    *   Log(HRESULT) - Sets up an error message when you pass HRESULT
    *   Message contains error code only, no specific information
    */
    static void Log(const HRESULT hr)
    {
#ifdef _DEBUG
        std::stringstream strFormatted;
        strFormatted << "Operation failed, error code = 0x" << std::hex << hr;

        Utils::Log(strFormatted.str());
#endif // _DEBUG
    };



    /**
    *   WorldToScreen - Converts game coordinates to screen
    *   @origin: 3D coordinates to be converted
    *   @screen: Viewport coordinates to which we will convert
    */
    static bool WorldToScreen(const Vector &origin, Vector &screen)
    {
        const auto screenTransform = [&origin, &screen]() -> bool
        {
            static std::uintptr_t pViewMatrix;
            if (!pViewMatrix)
            {
                pViewMatrix = static_cast<std::uintptr_t>(Utils::FindSignature("client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"));
                pViewMatrix += 3;
                pViewMatrix = *reinterpret_cast<std::uintptr_t*>(pViewMatrix);
                pViewMatrix += 176;
            }

            const VMatrix& w2sMatrix = *reinterpret_cast<VMatrix*>(pViewMatrix);
            screen.x = w2sMatrix.m[0][0] * origin.x + w2sMatrix.m[0][1] * origin.y + w2sMatrix.m[0][2] * origin.z + w2sMatrix.m[0][3];
            screen.y = w2sMatrix.m[1][0] * origin.x + w2sMatrix.m[1][1] * origin.y + w2sMatrix.m[1][2] * origin.z + w2sMatrix.m[1][3];
            screen.z = 0.0f;

            float w = w2sMatrix.m[3][0] * origin.x + w2sMatrix.m[3][1] * origin.y + w2sMatrix.m[3][2] * origin.z + w2sMatrix.m[3][3];

            if (w < 0.001f)
            {
                screen.x *= 100000;
                screen.y *= 100000;
                return true;
            }

            float invw = 1.f / w;
            screen.x *= invw;
            screen.y *= invw;

            return false;
        };

        if (!screenTransform())
        {
            int iScreenWidth, iScreenHeight;
            g_pEngine->GetScreenSize(iScreenWidth, iScreenHeight);

            screen.x = (iScreenWidth * 0.5f) + (screen.x * iScreenWidth) * 0.5f;
            screen.y = (iScreenHeight * 0.5f) - (screen.y * iScreenHeight) * 0.5f;

            return true;
        }
        return false;
    }
};
