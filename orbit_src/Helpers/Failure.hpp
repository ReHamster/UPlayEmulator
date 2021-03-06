#pragma once

#include "pch.h"

namespace UbiorbitapiR2Loader
{
    using namespace std;

    inline void Fail(const string &message, const bool silent, const char *file = nullptr, const int line = 0)
    {
        if (silent)
        {
            LOGE << message;
        }
        else
        {
            if (file != nullptr && line > 0)
            {
                MessageBoxA(nullptr, fmt::format("Message: {}\nFile: {}\nLine: {}\n", message, file, line).c_str(),
                            "Error...", MB_ICONERROR | MB_ICONERROR);
            }
            else
            {
                MessageBoxA(nullptr, message.c_str(), "Error...", MB_ICONERROR | MB_ICONERROR);
            }
        }

        exit(EXIT_FAILURE);
    }
} // namespace UbiorbitapiR2Loader
