#include "Context.h"
#include "../core/Debug.h"


namespace pk
{
    PK_byte Context::s_graphicsAPI = GRAPHICS_API_NONE;

    Context::Context(PK_byte graphicsAPIType)
    {
        s_graphicsAPI = graphicsAPIType;
    }

    Context::~Context()
    {
    }

    PK_byte Context::get_api_type()
    {
        return s_graphicsAPI;
    }
}
