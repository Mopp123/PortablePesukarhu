#include "Resource.h"
#include "Debug.h"


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
    }

    Resource::~Resource()
    {
        ID::erase(_resourceID);
    }
}
