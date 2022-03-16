em++ ^
src_gameClient/Main.cpp ^
pk/core/Debug.cpp ^
pk/core/Timing.cpp ^
pk/core/Window.cpp ^
pk/core/platform/web/WebWindow.cpp ^
pk/graphics/Context.cpp ^
pk/graphics/platform/web/WebContext.cpp ^
pk/core/input/InputManager.cpp ^
pk/core/input/platform/web/WebInputManager.cpp ^
pk/core/Application.cpp ^
pk/graphics/MasterRenderer.cpp ^
pk/graphics/platform/web/WebMasterRenderer.cpp ^
-s FULL_ES2=1 -s WASM=1 --memory-init-file 0 -s USE_SDL=2 -o WEB_BUILD/HelloEmscripten.html