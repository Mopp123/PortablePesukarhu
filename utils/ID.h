#pragma once

#include <cstdint>
#include <set>

namespace pk
{
    class ID
    {
    private:
        static std::set<uint32_t> s_usedIDs;
        static bool s_initialized;

    public:
        static uint32_t generate();
        static void erase(uint32_t idToErase);
    };
}
