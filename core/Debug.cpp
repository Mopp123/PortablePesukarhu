#include "Debug.h"
#include "ecs/components/Component.h"
#include <iostream>

#define LS_DEBUG_MESSAGE_TAG__MESSAGE		""
#define LS_DEBUG_MESSAGE_TAG__WARNING		"(Warning)"
#define LS_DEBUG_MESSAGE_TAG__ERROR			"[ERROR]"
#define LS_DEBUG_MESSAGE_TAG__FATAL_ERROR	"<!FATAL ERROR!>"


namespace pk
{
    void Debug::log(std::string message, MessageType t)
    {
        switch (t)
        {
            case pk::Debug::PK_MESSAGE:		std::cout << LS_DEBUG_MESSAGE_TAG__MESSAGE << " " << message << std::endl; break;
            case pk::Debug::PK_WARNING:		std::cout << LS_DEBUG_MESSAGE_TAG__WARNING << " " << message << std::endl; break;
            case pk::Debug::PK_ERROR:		std::cout << LS_DEBUG_MESSAGE_TAG__ERROR << " " << message << std::endl; break;
            case pk::Debug::PK_FATAL_ERROR: std::cout << LS_DEBUG_MESSAGE_TAG__FATAL_ERROR << " " << message << std::endl; break;
            default:
                break;
        }
    }

    void Debug::log_entity(Entity entity)
    {
        std::cout << "<Entity: " + std::to_string(entity.id) + "----------\n";
        std::cout << "\t[components]\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_GUI)
            std::cout << "\tPK_RENDERABLE_GUI\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_TEXT)
            std::cout << "\tPK_RENDERABLE_TEXT\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_SPRITE3D)
            std::cout << "\tPK_RENDERABLE_SPRITE3D\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_TERRAINTILE)
            std::cout << "\tPK_RENDERABLE_TERRAINTILE\n";
        if (entity.componentMask & ComponentType::PK_RENDERABLE_STATIC3D)
            std::cout << "\tPK_RENDERABLE_STATIC3D\n";
        if (entity.componentMask & ComponentType::PK_UIELEM_STATE)
            std::cout << "\tPK_UIELEM_STATE\n";
        if (entity.componentMask & ComponentType::PK_LIGHT_DIRECTIONAL)
            std::cout << "\tPK_LIGHT_DIRECTIONAL\n";
        if (entity.componentMask & ComponentType::PK_TRANSFORM)
            std::cout << "\tPK_TRANSFORM\n";
        if (entity.componentMask & ComponentType::PK_UI_CONSTRAINT)
            std::cout << "\tPK_UI_CONSTRAINT\n";
        if (entity.componentMask & ComponentType::PK_CAMERA)
            std::cout << "\tPK_CAMERA\n";
        std::cout << "--------------------\n";
    }

    void Debug::notify_unimplemented(const std::string& location)
    {
        log(
            "@" + location + " Unimplemented!",
            Debug::MessageType::PK_FATAL_ERROR
        );
    }
}
