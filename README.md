# PortablePesukarhu
Some old game engine tools things (initially for using with emscripten+webgl)<br/>
*Not continuing this project.<br/>
  ->Started rewriting everything in https://github.com/Mopp123/Platypus<br/>

## Build
### Linux
Install some build tools `apt-get install build-essential` or something...<br/>
Install third party lib dependancies:<br/>
GLFW: `apt-get install libxmu-dev libxi-dev libgl-dev libwayland-dev libxkbcommon-dev xorg-dev`<br/>
Freetype and harfbuzz: `apt-get install automake libtool autoconf libfreetype6-dev libglib2.0-dev libcairo2-dev meson pkg-config gtk-doc-tools`<br/>
Build all dependancies `./build-dependancies.sh`<br/>


## Tests
### Web
On **Linux** activate Emscripten SDK from your Emscripten location `./emsdk activate latest` and `source ./emsdk_env.sh`<br/>
Go to "tests" directory and build with `./build-web-test.sh . web/build/`<br/>
Build packaged files for Emscripten's virtual file system using `./build-web-assets.sh`<br/>
Build docker image and container for test server to  run using script in tests/web `./build-docker.sh`<br/>
Start the docker container by navigating to tests/web and using `docker compose -f docker-compose.yml start`<br/>

