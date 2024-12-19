cmake_minimum_required(VERSION 3.5)

# NOTE: IMPORTANT! Atm all web build stuff and src files have been removed from here!
# TODO: Some clever build system to also be able to build as lib for use with emscripten!

set(PROJ_NAME "pesukarhu")
set(TARGET_NAME "pesukarhu-engine")

project(${PROJ_NAME})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17 -DGLEW_NO_GLU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -DGLEW_NO_GLU -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/pesukarhu/glfw/include"
    "${PROJECT_SOURCE_DIR}/pesukarhu/glew/include"
    "${PROJECT_SOURCE_DIR}/pesukarhu/freetype/include"
)

file(
    GLOB SOURCE_FILES
    "pesukarhu/core/*.cpp"
    "pesukarhu/core/input/*.cpp"
    "pesukarhu/core/input/platform/web/*.cpp"
    "pesukarhu/core/input/platform/desktop/*.cpp"
    "pesukarhu/core/platform/web/*.cpp"
    "pesukarhu/core/platform/desktop/*.cpp"

    "pesukarhu/ecs/components/*.cpp"
    "pesukarhu/ecs/components/ui/*.cpp"
    "pesukarhu/ecs/components/renderable/*.cpp"
    "pesukarhu/ecs/components/lighting/*.cpp"
    "pesukarhu/ecs/systems/*.cpp"
    "pesukarhu/ecs/systems/ui/*.cpp"

    "pesukarhu/ui/*.cpp"

    "pesukarhu/graphics/*.cpp"
    "pesukarhu/graphics/shaders/*.cpp"
    "pesukarhu/graphics/renderers/*.cpp"
    "pesukarhu/graphics/platform/web/*.cpp"
    "pesukarhu/graphics/platform/web/shaders/*.cpp"
    "pesukarhu/graphics/platform/opengl/*.cpp"
    "pesukarhu/graphics/platform/opengl/shaders/*.cpp"

    "pesukarhu/resources/*.cpp"
    "pesukarhu/resources/platform/opengl/*.cpp"

    "pesukarhu/utils/*.cpp"
)


find_package(OpenGL REQUIRED)

add_library(${TARGET_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-g")

target_link_libraries(${TARGET_NAME} "${PROJECT_SOURCE_DIR}/pesukarhu/glfw/build/src/libglfw.so.3")
target_link_libraries(${TARGET_NAME} "${PROJECT_SOURCE_DIR}/pesukarhu/glew/lib/libGLEW.so.2.2")
target_link_libraries(${TARGET_NAME} "${PROJECT_SOURCE_DIR}/pesukarhu/freetype/objs/.libs/libfreetype.so.6.20.2")

target_link_libraries(${TARGET_NAME} ${OPENGL_LIBRARIES})
