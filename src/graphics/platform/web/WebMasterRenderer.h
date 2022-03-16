#pragma once

#include "../../MasterRenderer.h"

namespace pk
{
	namespace web
	{

		class WebMasterRenderer : public MasterRenderer
		{
		public:

			WebMasterRenderer();
			~WebMasterRenderer();

			virtual void render();

		};
	}
}