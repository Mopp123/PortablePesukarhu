# PortablePesukarhu
Some portable game engine tools things (initially for using with emscripten+webgl)

## Tests
### Web
On **Linux** activate Emscripten SDK from your Emscripten location `./emsdk activate latest` and `source ./emsdk_env.sh`
Go to "tests" directory and build with `./build-web-test.sh . web/build/`
Build packaged files for Emscripten's virtual file system using `./build-web-assets.sh`
Build docker image and container for test server to  run using script in tests/web `./build-docker.sh`
Start the docker container by navigating to tests/web and using `docker compose -f docker-compose.yml start`

