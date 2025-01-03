#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"
#include <unordered_map>


namespace pk
{
    // TODO: GET RID OF THIS WHOLE PIECE OF SHIT!
    // -> Move to handling this stuff exclusively in the different ui classes!
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
        // Checkbox specific
        bool checked = false;

        static std::unordered_map<entityID_t, int> s_pickedLayers;

        UIElemState();
    	~UIElemState();
        UIElemState& operator=(UIElemState&& other);

        static UIElemState* create(entityID_t target);
    };
}
