#pragma once

#include "pesukarhu/ecs/components/Component.h"
#include "pesukarhu/ecs/Entity.h"
#include <unordered_map>


namespace pk
{
    // TODO: GET RID OF THIS WHOLE PIECE OF SHIT!
    // -> Move to handling this stuff exclusively in the different ui classes!
    // UPDATE TO ABOVE:
    //  *When having some kind of editor thing it would actually be nice to see what is going on
    //  in ui through component like this.
    //  -> Needs to clean up and maybe have more specific kind of UI element components for different
    //  kinds of UI elements?
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
