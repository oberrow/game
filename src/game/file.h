/*
 * game/file.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <fstream>
#include <sstream>
#include <string>

namespace utility
{
    inline static bool LoadFile(const char* path, std::string& str)
    {
        std::ifstream file { path };
        if (!file)
            return false;
        std::ostringstream data;
        data << file.rdbuf();
        str = data.str();
        return true;
    }
}