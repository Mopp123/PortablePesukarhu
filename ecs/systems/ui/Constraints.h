#pragma once

#include "../System.h"
#include "../../../utils/pkmath.h"
#include "../../../core/Application.h"
#include "../../../core/Window.h"
#include "../../components/Transform.h"


namespace pk
{
    namespace ui
    {
        enum ConstraintType
        {
            PIXEL_LEFT = 1,
            PIXEL_RIGHT = 2,
            PIXEL_TOP = 3,
            PIXEL_BOTTOM = 4,
            PIXEL_CENTER_HORIZONTAL = 5,
            PIXEL_CENTER_VERTICAL = 6
        };

        class Constraint : public Updateable
        {
        private:
            Transform* _pTransform = nullptr;
            ConstraintType _type = PIXEL_LEFT;
            float _value = 0.0f;

        public:
            Constraint(Transform* pTransform, ConstraintType type, float val) : 
                _pTransform(pTransform),
                _type(type),
                _value(val)
            {}

            Constraint(const Constraint& other) : 
                _pTransform(other._pTransform),
                _type(other._type),
                _value(other._value)
            {}

            void update()
            {
                const Window* const win = Application::get()->getWindow();
                const float windowWidth = win->getWidth();
                const float windowHeight = win->getHeight();

                mat4& tMat = _pTransform->accessTransformationMatrix();

                const float& transformWidth = tMat[0 + 0 * 4];
                const float& transformHeight = tMat[1 + 1 * 4];

                switch (_type)
                {
                    case pk::ui::PIXEL_LEFT:		        tMat[0 + 3 * 4] = _value;                                                   break;
                    case pk::ui::PIXEL_RIGHT:		        tMat[0 + 3 * 4] = windowWidth - _value - transformWidth;	            break;
                    case pk::ui::PIXEL_BOTTOM:		        tMat[1 + 3 * 4] = _value + transformHeight;		                    break;
                    case pk::ui::PIXEL_TOP:			tMat[1 + 3 * 4] = windowHeight - _value;		                    break;
                    case pk::ui::PIXEL_CENTER_HORIZONTAL:	tMat[0 + 3 * 4] = windowWidth * 0.5f + _value - transformWidth * 0.5f;	    break;
                    case pk::ui::PIXEL_CENTER_VERTICAL:		tMat[1 + 3 * 4] = windowHeight * 0.5f + _value - transformHeight * 0.5f;    break;
                    default:
                        break;
                }
            }
        };
    }
}
