cmake_minimum_required(VERSION 3.10)
project(model-viewer)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_executable(ModelViewer src/main.cpp src/glad.c)

target_include_directories(ModelViewer PRIVATE include)
target_link_libraries(ModelViewer PRIVATE OpenGL::GL glfw assimp)