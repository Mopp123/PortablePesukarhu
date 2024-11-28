#include "Resource.h"
#include "core/Debug.h"


namespace pk
{
    Resource::Resource(ResourceType type, const std::string& filepath) :
        _type(type)
    {
        _resourceID = ID::generate();
        memset(_filepath, 0, MAX_FILEPATH_SIZE);
        if (filepath.size() > 0)
        {
            size_t pathSize = filepath.size();
            if (pathSize >= MAX_FILEPATH_SIZE)
            {
                Debug::log(
                    "@Resource::Resource Too long resource filepath: " + filepath + " max size is: " + std::to_string(MAX_FILEPATH_SIZE),
                    Debug::MessageType::PK_ERROR
                );
                pathSize = MAX_FILEPATH_SIZE;
            }
            memcpy(_filepath, filepath.data(), pathSize);
        }
        const std::string typeName = get_resource_type_name(_type);
        Debug::log("Created " + typeName + " resource with id: " + std::to_string(_resourceID));
    }

    Resource::~Resource()
    {
        ID::erase(_resourceID);
    }

    std::string Resource::get_resource_type_name(ResourceType type)
    {
        switch (type)
        {
            case RESOURCE_NONE :        return PK_RESOURCE_NAME_NONE;
            case RESOURCE_IMAGE :       return PK_RESOURCE_NAME_IMAGE;
            case RESOURCE_TEXTURE :     return PK_RESOURCE_NAME_TEXTURE;
            case RESOURCE_FONT :        return PK_RESOURCE_NAME_FONT;
            case RESOURCE_MATERIAL :    return PK_RESOURCE_NAME_MATERIAL;
            case RESOURCE_TERRAIN_MATERIAL : return PK_RESOURCE_NAME_TERRAIN_MATERIAL;
            case RESOURCE_MESH :            return PK_RESOURCE_NAME_MESH;
            case RESOURCE_TERRAIN_MESH :    return PK_RESOURCE_NAME_TERRAIN_MESH;
            case RESOURCE_MODEL :           return PK_RESOURCE_NAME_MODEL;
            case RESOURCE_ANIMATION :       return PK_RESOURCE_NAME_ANIMATION;
            default:
                Debug::log(
                    "@get_resource_type_name "
                    "No name specified for resource type: " + std::to_string(type),
                    Debug::MessageType::PK_ERROR
                );
                return "ERROR";
        }
    }
}
