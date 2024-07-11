/*
 * game/file.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <cstdint>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace utility
{
    inline static bool LoadFile(const char* path, std::string& str)
    {
        std::ifstream file { path, std::ios_base::binary };
        if (!file)
            return false;
        std::ostringstream data;
        data << file.rdbuf();
        str = data.str();
        file.close();
        return true;
    }
    inline static bool LoadFile(const char* path, std::vector<uint8_t>& buf)
    {
        std::ifstream file { path, std::ios_base::binary };
        if (!file)
            return false;
        file.seekg(0, std::ios_base::end);
        size_t filesize = file.tellg();
        file.seekg(0);
        buf.resize(filesize);
        file.read((char*)buf.data(), filesize);
        file.close();
        return true;
    }
}