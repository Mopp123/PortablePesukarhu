#pragma once

#include "utils/ID.h"
#include <cstring>
#include <string>

#define MAX_FILEPATH_SIZE 64

// TODO: some better way of dealing with this..
#define PK_RESOURCE_NAME_NONE       "None"
#define PK_RESOURCE_NAME_IMAGE      "Image"
#define PK_RESOURCE_NAME_TEXTURE    "Texture"
#define PK_RESOURCE_NAME_FONT       "Font"
#define PK_RESOURCE_NAME_MATERIAL   "Material"
#define PK_RESOURCE_NAME_MESH       "Mesh"
#define PK_RESOURCE_NAME_MODEL      "Model"
#define PK_RESOURCE_NAME_ANIMATION  "Animation"


namespace pk
{
    enum ResourceType
    {
        RESOURCE_NONE = 0,
        RESOURCE_IMAGE,
        RESOURCE_TEXTURE,
        RESOURCE_FONT,
        RESOURCE_MATERIAL,
        RESOURCE_MESH,
        RESOURCE_MODEL,
        RESOURCE_ANIMATION
    };


    class Resource
    {
    protected:
        uint32_t _resourceID = 0;
        ResourceType _type;
        char _filepath[MAX_FILEPATH_SIZE];
        bool _loaded = false;

    public:
        Resource(ResourceType type, const std::string& filepath = "");
        Resource(const Resource&) = delete;
        virtual ~Resource();

        virtual void load() = 0;
        virtual void save() = 0;

        static std::string get_resource_type_name(ResourceType type);

        inline uint32_t getResourceID() const { return _resourceID; }
        inline ResourceType getType() const { return _type; }
        inline std::string getFilepathStr() const { return std::string(_filepath); }
        inline bool isLoaded() const { return _loaded; }
    };
}
