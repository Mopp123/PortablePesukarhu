cmake_minimum_required(VERSION 3.5)


set(proj_name ${BUILD_TYPE})

project(${proj_name})

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17")
if(${BUILD_TYPE} MATCHES "PesukarhuWeb")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -fexceptions")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set(html_shell_location "${PROJECT_SOURCE_DIR}/shell.html")
    # NOTE: This gets converted to camel case for some reason...
    set(html_out "PesukarhuWebTest.html")
    add_compile_definitions(PK_PLATFORM_WEB=1)
else()
    message(FATAL_ERROR "ERROR: No BUILD_TYPE specified!")
    return()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PK_DEBUG_FULL=1)

# Need to have absolute dirs so we cannot use '../' to get the engine directory here!
get_filename_component(PARENT_DIR ../ ABSOLUTE)

include_directories (
    "${PROJECT_SOURCE_DIR}/src"
    "${PARENT_DIR}"
)

file (
    GLOB source_files
    # Engine files ---
    "${PARENT_DIR}/Pesukarhu/core/*.cpp"
    "${PARENT_DIR}/Pesukarhu/core/input/*.cpp"
    "${PARENT_DIR}/Pesukarhu/core/input/platform/web/*.cpp"
    "${PARENT_DIR}/Pesukarhu/core/platform/web/*.cpp"

    "${PARENT_DIR}/Pesukarhu/ecs/components/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/components/ui/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/components/renderable/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/components/lighting/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/systems/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/systems/ui/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/factories/ui/*.cpp"
    "${PARENT_DIR}/Pesukarhu/ecs/utils/ui/*.cpp"

    "${PARENT_DIR}/Pesukarhu/graphics/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/shaders/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/renderers/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/platform/web/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/platform/web/shaders/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/platform/opengl/*.cpp"
    "${PARENT_DIR}/Pesukarhu/graphics/platform/opengl/shaders/*.cpp"

    "${PARENT_DIR}/Pesukarhu/resources/*.cpp"
    "${PARENT_DIR}/Pesukarhu/resources/platform/opengl/*.cpp"

    "${PARENT_DIR}/Pesukarhu/utils/*.cpp"

    # Game client files ---
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${proj_name} ${source_files})

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    message(STATUS "Starting web build")
    set_target_properties(${proj_name} PROPERTIES LINK_FLAGS "-fexceptions -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 --shell-file ${html_shell_location} -o ${html_out} -sFORCE_FILESYSTEM --use-preload-plugins -sFETCH -sALLOW_MEMORY_GROWTH -l websocket.js")
endif()

