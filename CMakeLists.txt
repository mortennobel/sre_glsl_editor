cmake_minimum_required(VERSION 3.11)
project(SimpleRenderEngineTemplate CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# build sre static library
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/SimpleRenderEngine/cmake")
SET(USE_SRE_EXAMPLES OFF CACHE BOOL "Disable examples for SRE" FORCE)
add_subdirectory(SimpleRenderEngine)
include_directories(${SRE_INCLUDE})

add_executable(SimpleRenderEngineTemplate main.cpp)
target_link_libraries(SimpleRenderEngineTemplate ${SRE_LIBRARIES})