# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(GLFW C CXX)
set(GLFW_LIB_NAME glfw)
set(GLFW_VERSION_MAJOR "3")
set(GLFW_VERSION_MINOR "2")
set(GLFW_VERSION "${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_subdirectory(src)
add_subdirectory(json)
add_subdirectory(imgui)
#add_subdirectory(msg_host)
#add_subdirectory(graphics_app)