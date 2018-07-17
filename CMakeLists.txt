cmake_minimum_required(VERSION 3.11)
project(GLSL_EDITOR)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# build sre static library
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/SimpleRenderEngine/cmake")
SET(USE_SRE_EXAMPLES OFF CACHE BOOL "Disable examples for SRE" FORCE)
add_subdirectory(SimpleRenderEngine)
include_directories(${SRE_INCLUDE})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/submodules/imguiDock/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinyfd/")

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/submodules/imguiDock/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/submodules/imguiDock/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinyfd/tinyfiledialogs.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinyfd/*.h"
        )

add_executable(SRE_GLSL_EDITOR ${SOURCE_FILES})
target_link_libraries(SRE_GLSL_EDITOR ${SRE_LIBRARIES})