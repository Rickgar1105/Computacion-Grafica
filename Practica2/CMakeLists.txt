cmake_minimum_required(VERSION 3.16)

project(Practica2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(Practica2
    practica2.cpp
    Mesh.cpp
    Shader.cpp
    Window.cpp
)

target_link_libraries(Practica2
    PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
)
