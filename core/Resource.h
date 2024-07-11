#pragma once

#include "utils/ID.h"
#include <cstring>
#include <string>

#define MAX_FILEPATH_SIZE 32

namespace pk
{
    enum ResourceType
    {
        RESOURCE_NONE = 0,
        RESOURCE_IMAGE,
        RESOURCE_TEXTURE,
        RESOURCE_FONT
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

        inline uint32_t getResourceID() const { return _resourceID; }
        inline ResourceType getType() const { return _type; }
        inline std::string getFilepathStr() const { return std::string(_filepath); }
        inline bool isLoaded() const { return _loaded; }
    };
}
