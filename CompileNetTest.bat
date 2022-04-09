em++ ^
../src_gameClient/Main.cpp ^
../src_gameClient/scenes/LoginMenu.cpp ^
../src_gameClient/scenes/CreateFactionMenu.cpp ^
../src_gameClient/scenes/InGame.cpp ^
../src_gameClient/scenes/ChatTest.cpp ^
../src_gameClient/world/World.cpp ^
../src_gameClient/world/Tile.cpp ^
../src_gameClient/net/Client.cpp ^
../src_gameClient/net/ByteBuffer.cpp ^
../src_gameClient/net/requests/Request.cpp ^
../src_gameClient/net/requests/Commands.cpp ^
../src_gameClient/net/requests/platform/web/WebRequest.cpp ^
../pk/core/Debug.cpp ^
../pk/core/Timing.cpp ^
../pk/core/Window.cpp ^
../pk/core/platform/web/WebWindow.cpp ^
../pk/graphics/Context.cpp ^
../pk/graphics/platform/web/WebContext.cpp ^
../pk/core/input/InputManager.cpp ^
../pk/core/input/platform/web/WebInputManager.cpp ^
../pk/core/Application.cpp ^
../pk/core/SceneManager.cpp ^
../pk/graphics/platform/web/WebMasterRenderer.cpp ^
../pk/graphics/platform/web/WebTexture.cpp ^
../pk/graphics/platform/web/WebGUIRenderer.cpp ^
../pk/graphics/platform/web/WebFontRenderer.cpp ^
../pk/graphics/platform/web/WebTerrainRenderer.cpp ^
../pk/graphics/platform/web/WebSpriteRenderer.cpp ^
../pk/graphics/platform/web/WebBuffers.cpp ^
../pk/graphics/platform/web/shaders/WebShader.cpp ^
../pk/ecs/systems/ui/Text.cpp ^
../pk/ecs/systems/ui/GUIImage.cpp ^
../pk/ecs/systems/ui/combinedFunctional/Button.cpp ^
../pk/ecs/systems/ui/combinedFunctional/InputField.cpp ^
../pk/ecs/systems/CameraUtils.cpp ^
-s FULL_ES2=1 -s WASM=1 --memory-init-file 0 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_FREETYPE=1 --shell-file HTMLShell.html -o HelloEmscripten.html --preload-file assets --use-preload-plugins -sFETCH