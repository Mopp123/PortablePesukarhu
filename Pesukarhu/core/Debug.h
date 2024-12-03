#pragma once

#include "ecs/Entity.h"
#include <string>


namespace pk
{
    class Debug
    {
    public:
        enum MessageType
        {
            PK_MESSAGE = 0x0,
            PK_WARNING,
            PK_ERROR,
            PK_FATAL_ERROR
        };

        static void log(std::string message, MessageType t = PK_MESSAGE);
        //static void log_entity(Entity entity);
        static void notify_unimplemented(const std::string& location);
    };
}
