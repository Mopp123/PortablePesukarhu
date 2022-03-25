

#include "../pk/core/platform/web/WebWindow.h"
#include "../pk/core/input/platform/web/WebInputManager.h"
#include "../pk/core/Application.h"
#include "../pk/graphics/platform/web/WebContext.h"

#include "../pk/graphics/platform/web/WebMasterRenderer.h"
#include "../pk/graphics/platform/web/WebGUIRenderer.h"
#include "../pk/graphics/platform/web/WebFontRenderer.h"

#include "../pk/ecs/systems/ui/GUIImage.h"
#include "../pk/ecs/systems/ui/Text.h"
#include "../pk/ecs/systems/ui/combinedFunctional/Button.h"


#include "../pk/ecs/systems/ui/Constraints.h"

#include "../pk/core/Debug.h"

#include <string>
#include <vector>

using namespace pk;
using namespace web;
using namespace ui;

/*
	Current issues:
		* When rendering huge strings, it stops rendering characters at some point... no idea why..
*/


class TestKeyEvent : public KeyEvent
{
public:

	virtual void func(InputKeyName key, int scancode, int action, int mods)
	{
		if (key == PK_INPUT_KEY_4)
			Debug::log("(custom) 4");

		if (key == PK_INPUT_KEY_W)
			Debug::log("(custom) w");

		if (key == PK_INPUT_KEY_J)
			Debug::log("(custom) J");

		if (key == PK_INPUT_KEY_C)
			Debug::log("(custom) C");

	}
	
};

class TestButtonEvent : public MouseButtonEvent
{
public:

	virtual void func(InputMouseButtonName button, InputAction action, int mods)
	{
		if (button == PK_INPUT_MOUSE_LEFT)
		{

			if (action == PK_INPUT_PRESS)
				Debug::log("(custom) LEFT PRESS");
			else if (action == PK_INPUT_RELEASE)
				Debug::log("(custom) LEFT RELEASE");
		}

		if (button == PK_INPUT_MOUSE_RIGHT)
		{

			if (action == PK_INPUT_PRESS)
				Debug::log("(custom) RIGHT PRESS");
			else if (action == PK_INPUT_RELEASE)
				Debug::log("(custom) RIGHT RELEASE");
		}
	}
};

class TestCursorPosEvent : public CursorPosEvent
{
public:

	virtual void func(int x, int y)
	{
		Debug::log("custom cPos event: " + std::to_string(x) + ", " + std::to_string(y)); 
	}
};

class TestCharEvent : public CharInputEvent
{
public:

	virtual void func(unsigned int codepoint)
	{

		Debug::log(std::to_string(codepoint));
	}
};

//perse...
class TestScene : public Scene
{
public:

	
	Text* _text = nullptr;
	Text* _boldText = nullptr;
	Button* _button = nullptr;

	TestScene() {}
	~TestScene() {}

	virtual void init()
	{

		std::string testStr = R"(asdfghjklaconsectetur adipiscing elit. Nam bibendum felis quis metus vestibulum vulputate. Morbi vitae nunc luctus, dictum felis ut, consectetur est. Ut cursus justo est, eget egestas ex consequat a. Mauris vitae cursus arcu, sed euismod augue. Nam vel commodo elit, aliquet ullamcorper quam. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla id nisi eget metus ornare suscipit quis a mi. Etiam ac turpis dui. Suspendisse a massa lorem. Morbi sed tortor sollicitudin, pretium justo vitae, pharetra ex. Ut ac nisl nec enim vulputate porta. Ut congue, ante at elementum venenatis, metus risus pellentesque elit, non rhoncus nulla orci sit amet ipsum. Donec pellentesque rhoncus porta. Duis eu volutpat enim. Nunc tempor neque sollicitudin, dignissim nunc eget, tincidunt lacus. Vestibulum ac urna lobortis, rutrum elit quis, pretium tortor.
Sed et dui ac mi elementum pretium gravida sit amet turpis. Ut in luctus leo, vitae maximus lacus. Vestibulum ac nisl ultricies, dapibus justo a, accumsan nibh. Sed molestie ornare libero ac maximus. Etiam ut sodales nibh, at luctus lectus. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Sed felis diam, porttitor in nisl non, dapibus commodo eros. Nunc eleifend sit amet orci eu tincidunt. In sit amet tortor aliquet tortor mollis ultricies fermentum at augue. Duis mollis turpis nec nisi sodales, id interdum eros faucibus.
Sed a commodo tortor. Praesent eu elit aliquam, fringilla ligula eu, vulputate nunc. Sed ultricies urna nec libero laoreet dapibus. Proin non pharetra justo. Praesent vel egestas mauris. Sed a turpis quis arcu congue viverra in non ex. Morbi tempor ante quis augue maximus, in sagittis nibh lobortis.
Quisque fermentum ex orci, sed laoreet nunc pharetra eget. Aliquam erat volutpat. 
Sed tempor accumsan imperdiet. Aliquam tincidunt hendrerit magna, sit amet lobortis 
felis laoreet et. Morbi ut faucibus diam. Proin eget leo non quam scelerisque fermentum 
eu nec justo. Etiam efficitur, urna sit amet suscipit non.)";
		

		//Application::get()->accessInputManager()->addKeyEvent(new TestKeyEvent);
		Application::get()->accessInputManager()->addMouseButtonEvent(new TestButtonEvent);
		Application::get()->accessInputManager()->addCursorPosEvent(new TestCursorPosEvent);
		Application::get()->accessInputManager()->addCharInputEvent(new TestCharEvent);


		_button = new Button(
			"Test Button asd",
			{
				{ConstraintType::PIXEL_LEFT, 0},
				{ConstraintType::PIXEL_TOP, 400}
			},
			160, 24,0,0
		);

		_text = new Text(
			testStr,
			{
				{ ConstraintType::PIXEL_LEFT, 5 },
				{ ConstraintType::PIXEL_TOP, 5 }
			}
		);

		_boldText = new Text(
			"This is some bold text over here",
			{
				{ ConstraintType::PIXEL_LEFT, 0 },
				{ ConstraintType::PIXEL_BOTTOM, 0 }
			},
			true
		);

	}

	virtual void update()
	{
		/*
		for (GUIImage* i : images)
			i->applyConstraints();

		for (Text* t : texts)
			t->applyConstraints();
			*/

	}
};


int main(int argc, const char** argv)
{
	bool initSuccess = true;

	WebWindow window;
	WebContext graphicsContext;
	WebInputManager inputManager;

	WebMasterRenderer masterRenderer;
	Renderer* pGuiRenderer = (Renderer*)(new WebGUIRenderer);
	Renderer* pFontRenderer = (Renderer*)(new WebFontRenderer);

	Application application(
		"Emscripten testing..", 
		&window, &graphicsContext, &inputManager, 
		&masterRenderer, 
		{ 
			{ ComponentType::PK_RENDERABLE_GUI, pGuiRenderer },
			{ ComponentType::PK_RENDERABLE_TEXT, pFontRenderer }
		});

	application.switchScene(new TestScene);

	application.run();

	// set "main loop"
	//emscripten_set_main_loop(update, 0, 0);

	return 0;
}