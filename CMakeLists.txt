cmake_minimum_required(VERSION 3.16)
project(practica4_modelado_jerarquico CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)

# glm es solo headers; Homebrew no siempre instala un .pc, por eso se busca la carpeta
find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp
	PATHS /opt/homebrew/include /usr/local/include)
if(NOT GLM_INCLUDE_DIR)
	message(FATAL_ERROR "No se encontro glm. Instala con: brew install glm")
endif()

add_executable(practica4
	practica4.cpp
	Camera.cpp
	Window.cpp
	Shader.cpp
	Mesh.cpp
	Sphere.cpp
)

# Se agregan las carpetas internas (GLFW/, GL/, glm/) para que los includes
# del proyecto (ej. <glfw3.h>, <glew.h>, <glm.hpp>, <gtc/matrix_transform.hpp>)
# sigan funcionando tal como estan, sin tener que reescribirlos con el prefijo
# de cada libreria.
target_include_directories(practica4 PRIVATE
	${GLFW_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}/GLFW
	${GLEW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}/GL
	${GLM_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}/glm
)

target_link_libraries(practica4 PRIVATE
	PkgConfig::GLFW
	PkgConfig::GLEW
	OpenGL::GL
)

# Copiar la carpeta shaders junto al ejecutable para que la ruta relativa
# "shaders/shader.vert" funcione al correr el programa desde VS Code.
add_custom_command(TARGET practica4 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/shaders
		$<TARGET_FILE_DIR:practica4>/shaders
)
