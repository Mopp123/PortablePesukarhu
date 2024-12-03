#pragma once

#include "Pesukarhu/core/input/InputManager.h"
#include <emscripten.h>
#include <emscripten/html5.h>

#include <string>
#include <unordered_map>


namespace pk
{
	namespace web
	{

		class WebInputManager : public InputManager
		{
		private:


			// ...quite fucking disqusting, i know.... xDd
			// (This is because emscripten doesnt give these for us the way we'd like with our font rendering)
			const std::unordered_map<unsigned int, unsigned int> _mapping_specialKeys
			{
				{	164, 228 },//	ä
				{	165, 229 },//	å
				{	182, 246 },//	ö

				{	132, 196 },//	Ä
				{	133, 197 },//	Å
				{	150, 214 }, //	Ö

			};

			const std::unordered_map<std::string, InputKeyName> _mapping_keyboard_emscToPk
			{
				{"0", PK_INPUT_KEY_0},
				{"1", PK_INPUT_KEY_1},
				{"2", PK_INPUT_KEY_2},
				{"3", PK_INPUT_KEY_3},
				{"4", PK_INPUT_KEY_4},
				{"5", PK_INPUT_KEY_5},
				{"6", PK_INPUT_KEY_6},
				{"7", PK_INPUT_KEY_7},
				{"8", PK_INPUT_KEY_8},
				{"9", PK_INPUT_KEY_9},

				{"F1", PK_INPUT_KEY_F1},
				{"F2", PK_INPUT_KEY_F2},
				{"F3", PK_INPUT_KEY_F3},
				{"F4", PK_INPUT_KEY_F4},
				{"F5", PK_INPUT_KEY_F5},
				{"F6", PK_INPUT_KEY_F6},
				{"F7", PK_INPUT_KEY_F7},
				{"F8", PK_INPUT_KEY_F8},
				{"F9", PK_INPUT_KEY_F9},
				{"F10", PK_INPUT_KEY_F10},
				{"F11", PK_INPUT_KEY_F11},
				{"F12", PK_INPUT_KEY_F12},

				{"q", PK_INPUT_KEY_Q},
				{"w", PK_INPUT_KEY_W},
				{"e", PK_INPUT_KEY_E},
				{"r", PK_INPUT_KEY_R},
				{"t", PK_INPUT_KEY_T},
				{"y", PK_INPUT_KEY_Y},
				{"u", PK_INPUT_KEY_U},
				{"i", PK_INPUT_KEY_I},
				{"o", PK_INPUT_KEY_O},
				{"p", PK_INPUT_KEY_P},
				{"a", PK_INPUT_KEY_A},
				{"s", PK_INPUT_KEY_S},
				{"d", PK_INPUT_KEY_D},
				{"f", PK_INPUT_KEY_F},
				{"g", PK_INPUT_KEY_G},
				{"h", PK_INPUT_KEY_H},
				{"j", PK_INPUT_KEY_J},
				{"k", PK_INPUT_KEY_K},
				{"l", PK_INPUT_KEY_L},
				{"z", PK_INPUT_KEY_Z},
				{"x", PK_INPUT_KEY_X},
				{"c", PK_INPUT_KEY_C},
				{"v", PK_INPUT_KEY_V},
				{"b", PK_INPUT_KEY_B},
				{"n", PK_INPUT_KEY_N},
				{"m", PK_INPUT_KEY_M},

				{"ArrowUp",		PK_INPUT_KEY_UP},
				{"ArrowDown",	PK_INPUT_KEY_DOWN},
				{"ArrowLeft",	PK_INPUT_KEY_LEFT},
				{"ArrowRight",	PK_INPUT_KEY_RIGHT},

				{" ", PK_INPUT_KEY_SPACE},
				{"Backspace", PK_INPUT_KEY_BACKSPACE},
				{"Enter", PK_INPUT_KEY_ENTER},
				{"Control", PK_INPUT_KEY_LCTRL},
				{"Shift", PK_INPUT_KEY_SHIFT},
				{"Tab", PK_INPUT_KEY_TAB}
			};

			const std::unordered_map<unsigned short, InputMouseButtonName> _mapping_mouse_emscToPk
			{
				{ 0, PK_INPUT_MOUSE_LEFT},
				{ 1, PK_INPUT_MOUSE_MIDDLE},
				{ 2, PK_INPUT_MOUSE_RIGHT}
			};


		public:

			WebInputManager();
			~WebInputManager();


			static void query_window_size(int* outWidth, int* outHeight);
			static void query_window_surface_size(int* outWidth, int* outHeight);


			unsigned int parseSpecialCharCodepoint(unsigned int val) const;

			bool isCharacter(const char* keyname) const;

			inline InputKeyName convert_to_keyname(const std::string& key)
			{
				return _mapping_keyboard_emscToPk.find(key)->second;
			}

			inline InputMouseButtonName convert_to_buttonname(unsigned short button)
			{
				return _mapping_mouse_emscToPk.find(button)->second;
			}
		};
	}
}
