#pragma once

#include "Pesukarhu/ecs/components/Component.h"
#include "Pesukarhu/ecs/Entity.h"
#include <unordered_map>


namespace pk
{
    class UIElemState : public Component
    {
    public:
        bool mouseOver = false;
        bool selectable = false;
        bool selected = false;
        int state = 0;
        // InputField specific
        bool clearOnSubmit = true;
        std::string content;

        static std::unordered_map<entityID_t, int> s_pickedLayers;

        UIElemState();
    	~UIElemState();

        static UIElemState* create(entityID_t target);
    };
}
