#pragma once

#include "../System.h"
#include "Constraints.h"
#include "../../components/Transform.h"
#include <memory>
#include <vector>


namespace pk
{
    namespace ui
    {
        class UIElement : public Updateable
        {
        protected:
            Transform* _transform = nullptr;
            std::vector<Constraint> _constraints;

        public:
            UIElement(std::vector<Constraint> constraints) :
                _constraints(constraints)
            {}

            virtual ~UIElement() {}

            void applyConstraints()
            {
                for (Constraint& c : _constraints)
                    c.apply(*_transform);
            }

            Transform* accessTransform() { return _transform; }
            std::vector<Constraint>& accessConstraints() { return _constraints; }

            virtual void update()
            {
                applyConstraints();
            }
        };
    }
}
