#pragma once

#include <string>
#include "Window.h"
#include "../graphics/MasterRenderer.h"
#include "Timing.h"

#include <memory>

namespace pk
{
	
	class Application
	{
	private:

		std::string _name;
		bool _running = true;

		Timing _timing;

		static Application* s_pApplication;

		Window* _pWindow = nullptr;
		MasterRenderer* _pMasterRenderer = nullptr;

	public:

		Application(std::string name, Window* window, MasterRenderer* mRenderer);
		~Application();

		void run();

		void resizeWindow(int w, int h);

		static Application* get();

		inline bool isRunning() const { return _running; }

	private:

		friend void update();
	};

}