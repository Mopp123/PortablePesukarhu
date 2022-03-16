#pragma once

#include <string>
#include "../graphics/MasterRenderer.h"
#include <memory>

namespace pk
{
	
	class Application
	{
	private:

		std::string _name;
		bool _running = true;

		static Application* s_application;

		MasterRenderer* _pMasterRenderer = nullptr;

	public:

		Application(std::string name, MasterRenderer* mRenderer);
		~Application();

		void run();

		inline bool isRunning() const { return _running; }

	private:

		friend void update();
	};

}