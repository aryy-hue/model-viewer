# 3D Model Viewer

Custom-built 3D model viewer developed from the ground up using C++ and modern OpenGL (Core Profile). This project demonstrates a foundational understanding of the graphics pipeline, 3D transformation matrices, complex mesh data extraction, and fundamental lighting models.

## Tech Stack & Dependencies

This project leverages industry-standard computer graphics libraries:
* **C++17** 
* **OpenGL 3.3**
* **GLFW** 
* **GLAD** 
* **GLM** 
* **Assimp** 
* **CMake** 

## Prerequisites 

Ensure you have a C++ compiler (`gcc-c++`), `cmake`, and `make` installed. For Fedora Linux users, you can install the required dependencies using the package manager:

```bash
sudo dnf install gcc-c++ cmake make glfw-devel glm-devel assimp-devel

```

## How to Build
Run the following commands in the root directory of the project
```bash
# 1. Create a build directory
mkdir build
cd build

# 2. Configure the project
cmake ..

# 3. Compile the executable
make
```
## Usage

```bash
./ModelViewer
```

