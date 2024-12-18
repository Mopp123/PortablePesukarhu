cmake_minimum_required(VERSION 3.5)


set(proj_name "build")
project(${proj_name})


# Need to have absolute dirs so we cannot use '../' to get the engine directory here!
get_filename_component(PARENT_DIR ../ ABSOLUTE)
# Default include dirs for all build types
set(proj_includes ${PROJECT_SOURCE_DIR}/src ${PARENT_DIR})

if(${BUILD_TYPE} MATCHES "web")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -fexceptions")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set(html_shell_location "${PROJECT_SOURCE_DIR}/shell.html")
    # NOTE: This gets converted to camel case for some reason...
    # NOTE: html_out becomes the proj_name for some reason... was too afraid to delete it from here since
    # Emscripten recommends using the -o out.html
    set(html_out "PesukarhuWebTest.html")
    add_compile_definitions(PK_BUILD_WEB=1)
elseif(${BUILD_TYPE} MATCHES "linux")


    add_compile_definitions(PK_BUILD_LINUX=1)
else()
    message(FATAL_ERROR "ERROR: No BUILD_TYPE specified!")
    return()
endif()

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PK_DEBUG_FULL=1)


include_directories(${proj_includes})

file (
    GLOB source_files
    # Engine files ---
    "${PARENT_DIR}/pesukarhu/core/*.cpp"
    "${PARENT_DIR}/pesukarhu/core/input/*.cpp"
    "${PARENT_DIR}/pesukarhu/core/input/platform/web/*.cpp"
    "${PARENT_DIR}/pesukarhu/core/platform/web/*.cpp"

    "${PARENT_DIR}/pesukarhu/ecs/components/*.cpp"
    "${PARENT_DIR}/pesukarhu/ecs/components/ui/*.cpp"
    "${PARENT_DIR}/pesukarhu/ecs/components/renderable/*.cpp"
    "${PARENT_DIR}/pesukarhu/ecs/components/lighting/*.cpp"
    "${PARENT_DIR}/pesukarhu/ecs/systems/*.cpp"
    "${PARENT_DIR}/pesukarhu/ecs/systems/ui/*.cpp"

    "${PARENT_DIR}/pesukarhu/ui/*.cpp"

    "${PARENT_DIR}/pesukarhu/graphics/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/shaders/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/renderers/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/platform/web/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/platform/web/shaders/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/platform/opengl/*.cpp"
    "${PARENT_DIR}/pesukarhu/graphics/platform/opengl/shaders/*.cpp"

    "${PARENT_DIR}/pesukarhu/resources/*.cpp"
    "${PARENT_DIR}/pesukarhu/resources/platform/opengl/*.cpp"

    "${PARENT_DIR}/pesukarhu/utils/*.cpp"

    # Game client files ---
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${proj_name} ${source_files})

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    message(STATUS "Starting web build")
    set_target_properties(${proj_name} PROPERTIES LINK_FLAGS "-fexceptions -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -o ${html_out} --shell-file ${html_shell_location} -sFORCE_FILESYSTEM --use-preload-plugins -sFETCH -sALLOW_MEMORY_GROWTH -l websocket.js")
endif()

