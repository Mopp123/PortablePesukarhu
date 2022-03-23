

#include "../pk/core/platform/web/WebWindow.h"
#include "../pk/core/input/platform/web/WebInputManager.h"
#include "../pk/core/Application.h"
#include "../pk/graphics/platform/web/WebContext.h"

#include "../pk/graphics/platform/web/WebMasterRenderer.h"
#include "../pk/graphics/platform/web/WebGUIRenderer.h"
#include "../pk/graphics/platform/web/WebFontRenderer.h"

#include "../pk/ecs/components/renderable/GUIRenderable.h"
#include "../pk/ecs/components/renderable/TextRenderable.h"

#include "../pk/core/Debug.h"

#include <string>
#include <vector>

using namespace pk;
using namespace web;

/*
	Current issues:
		* When rendering huge strings, it stops rendering characters at some point... no idea why..
*/


class TestScene : public Scene
{
public:

	std::vector<float> t1;

	TestScene() {}
	~TestScene() {}

	virtual void init()
	{
		/*GUIRenderable* r1 = new GUIRenderable;
		r1->pos = vec2(0,600);
		r1->scale = vec2(800,600);
		
		components[ComponentType::PK_RENDERABLE_GUI].push_back(r1);
		*/

		/*
		
			This is a test string thing. Here we test some...
			multiline strings.. amazing wow 123
			can u make any sense out of this?

			Fucking annoying dumbshitfuck...
			...No idea whats wrong! vittusaatana

			asdasdasdasdasdasdasdasdasdasdasdasdasdasdasd
			asdasdasdasdasdasdasdasdasdasdasdasdasdasdasd
			SPAMSPAMSPAMSPAMSPAMSPAMSPAMSPAMSPAMSPAMSPAM
		*/

		std::string testStr = R"(asdfghjklaconsectetur adipiscing elit. Nam bibendum felis quis metus vestibulum vulputate. Morbi vitae nunc luctus, dictum felis ut, consectetur est. Ut cursus justo est, eget egestas ex consequat a. Mauris vitae cursus arcu, sed euismod augue. Nam vel commodo elit, aliquet ullamcorper quam. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla id nisi eget metus ornare suscipit quis a mi. Etiam ac turpis dui. Suspendisse a massa lorem. Morbi sed tortor sollicitudin, pretium justo vitae, pharetra ex. Ut ac nisl nec enim vulputate porta. Ut congue, ante at elementum venenatis, metus risus pellentesque elit, non rhoncus nulla orci sit amet ipsum. Donec pellentesque rhoncus porta. Duis eu volutpat enim. Nunc tempor neque sollicitudin, dignissim nunc eget, tincidunt lacus. Vestibulum ac urna lobortis, rutrum elit quis, pretium tortor.
Sed et dui ac mi elementum pretium gravida sit amet turpis. Ut in luctus leo, vitae maximus lacus. Vestibulum ac nisl ultricies, dapibus justo a, accumsan nibh. Sed molestie ornare libero ac maximus. Etiam ut sodales nibh, at luctus lectus. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Sed felis diam, porttitor in nisl non, dapibus commodo eros. Nunc eleifend sit amet orci eu tincidunt. In sit amet tortor aliquet tortor mollis ultricies fermentum at augue. Duis mollis turpis nec nisi sodales, id interdum eros faucibus.
Sed a commodo tortor. Praesent eu elit aliquam, fringilla ligula eu, vulputate nunc. Sed ultricies urna nec libero laoreet dapibus. Proin non pharetra justo. Praesent vel egestas mauris. Sed a turpis quis arcu congue viverra in non ex. Morbi tempor ante quis augue maximus, in sagittis nibh lobortis.
Quisque fermentum ex orci, sed laoreet nunc pharetra eget. Aliquam erat volutpat. 
Sed tempor accumsan imperdiet. Aliquam tincidunt hendrerit magna, sit amet lobortis 
felis laoreet et. Morbi ut faucibus diam. Proin eget leo non quam scelerisque fermentum 
eu nec justo. Etiam efficitur, urna sit amet suscipit non.)";
		
		TextRenderable* r = new TextRenderable(testStr);
		r->pos = vec2(0, 600);
		components[ComponentType::PK_RENDERABLE_TEXT].push_back(r);

		TextRenderable* r2 = new TextRenderable(testStr);
		r2->pos = vec2(0, 300);
		components[ComponentType::PK_RENDERABLE_TEXT].push_back(r2);
		
		TextRenderable* r3 = new TextRenderable(testStr);
		r3->pos = vec2(0, 300 + 150);
		components[ComponentType::PK_RENDERABLE_TEXT].push_back(r3);

		TextRenderable* r4 = new TextRenderable(testStr);
		r4->pos = vec2(0, 300 - 150);
		components[ComponentType::PK_RENDERABLE_TEXT].push_back(r4);

	}

	virtual void update()
	{
	}
};


int main(int argc, const char** argv)
{
	bool initSuccess = true;

	WebWindow window(800, 600);
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