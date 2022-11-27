#pragma once

#include "Component.h"

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

        UIElemState() : Component(PK_UIELEM_STATE) {}
    	~UIElemState() {}
    };
}
