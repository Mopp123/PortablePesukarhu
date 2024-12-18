#include "Debug.h"
#include "pesukarhu/ecs/components/Component.h"
#include <iostream>
#include <cassert>

#ifdef PK_BUILD_WEB
#include <emscripten/console.h>
#endif

// NOTE: Not remembering anymore wtf "LS" stands for here...
#define LS_DEBUG_MESSAGE_TAG__MESSAGE		""
#define LS_DEBUG_MESSAGE_TAG__WARNING		"(Warning)"
#define LS_DEBUG_MESSAGE_TAG__ERROR			"[ERROR]"
#define LS_DEBUG_MESSAGE_TAG__FATAL_ERROR	"<!FATAL ERROR!>"


namespace pk
{

    void Debug::log(std::string message, MessageType t)
    {
        std::string fullMsg = std::string(LS_DEBUG_MESSAGE_TAG__ERROR) + " Invalid message type: " + std::to_string(t);
        switch (t)
        {
            case pk::Debug::PK_MESSAGE:		  fullMsg = std::string(LS_DEBUG_MESSAGE_TAG__MESSAGE) + " " + message; break;
            case pk::Debug::PK_WARNING:		  fullMsg = std::string(LS_DEBUG_MESSAGE_TAG__WARNING) +  " " + message; break;
            case pk::Debug::PK_ERROR:		    fullMsg = std::string(LS_DEBUG_MESSAGE_TAG__ERROR) +  " " + message; break;
            case pk::Debug::PK_FATAL_ERROR: fullMsg = std::string(LS_DEBUG_MESSAGE_TAG__FATAL_ERROR) +  " " + message; break;
            default:
                break;
        }

    #ifdef PK_BUILD_WEB
        emscripten_console_log(fullMsg.c_str());
    #else
        std::cout << fullMsg << std::endl;
    #endif
        if (t == MessageType::PK_FATAL_ERROR)
            assert(0 && "PK_FATAL_ERROR triggered");
    }

    /*
    void Debug::log_entity(Entity entity)
    {
        std::cout << "<Entity: " + std::to_string(entity.id) + "----------\n";
        std::cout << "\t[components]\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_GUI)
            std::cout << "\t\tPK_RENDERABLE_GUI\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_TEXT)
            std::cout << "\t\tPK_RENDERABLE_TEXT\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_SPRITE3D)
            std::cout << "\t\tPK_RENDERABLE_SPRITE3D\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_TERRAINTILE)
            std::cout << "\t\tPK_RENDERABLE_TERRAINTILE\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_STATIC3D)
            std::cout << "\t\tPK_RENDERABLE_STATIC3D\n";
        if (entity.componentMask & ComponentType::PK_UIELEM_STATE)
            std::cout << "\t\tPK_UIELEM_STATE\n";
        if (entity.componentMask & ComponentType::PK_LIGHT_DIRECTIONAL)
            std::cout << "\t\tPK_LIGHT_DIRECTIONAL\n";
        if (entity.componentMask & ComponentType::PK_TRANSFORM)
            std::cout << "\t\tPK_TRANSFORM\n";
        if (entity.componentMask & ComponentType::PK_UI_CONSTRAINT)
            std::cout << "\t\tPK_UI_CONSTRAINT\n";
        if (entity.componentMask & ComponentType::PK_CAMERA)
            std::cout << "\t\tPK_CAMERA\n";
        std::cout << "--------------------\n";
    }
    */

    void Debug::notify_unimplemented(const std::string& location)
    {
        log(
            "@" + location + " Unimplemented!",
            Debug::MessageType::PK_FATAL_ERROR
        );
    }
}
