#include "ID.h"
#include <time.h>
#include <cstdlib>
#include "core/Debug.h"
#include <cstring>

namespace pk
{
    std::set<uint32_t> ID::s_usedIDs;
    bool ID::s_initialized = false;

    // Atm just quick ugly way to do this..
    uint32_t ID::generate()
    {
        if (!s_initialized)
        {
            time_t time;
            std::srand((unsigned int)&time);
            s_initialized = true;
        }

        uint32_t id = 0;
        while ((s_usedIDs.find(id) != s_usedIDs.end()) || id == 0)
        {
            // randomize each byte individually..
            for (size_t i = 0; i < sizeof(uint32_t); ++i)
            {
                uint8_t bVal = (uint8_t)(std::rand() % 255);
                memset(((uint8_t*)&id) + i, bVal, 1);
            }
        }
        s_usedIDs.insert(id);

        return id;
    }

    void ID::erase(uint32_t idToErase)
    {
        std::set<uint32_t>::iterator it = s_usedIDs.find(idToErase);
        if (it == s_usedIDs.end())
        {
            Debug::log(
                "@ID::erase ID: " + std::to_string(idToErase) + " wasn't found from used IDs",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
        else
        {
            s_usedIDs.erase(it);
            Debug::log(
                "ID: " + std::to_string(idToErase) + " deleted!"
            );
        }
    }
}
