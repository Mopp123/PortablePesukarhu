#pragma once

#include "Constraints.h"
#include "../../components/Transform.h"
#include <memory>
#include <vector>

namespace pk
{
	namespace ui
	{

		class UIElement
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
		};
	}
}