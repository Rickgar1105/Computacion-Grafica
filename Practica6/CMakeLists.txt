cmake_minimum_required(VERSION 3.16)
project(practica6_texturizado CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)

# assimp: carga el cubo exportado de Blender (Model.h/Model.cpp)
pkg_check_modules(ASSIMP IMPORTED_TARGET assimp)
if(NOT ASSIMP_FOUND)
	find_path(ASSIMP_INCLUDE_DIR NAMES assimp/scene.h
		PATHS /opt/homebrew/include /usr/local/include)
	find_library(ASSIMP_LIBRARY NAMES assimp
		PATHS /opt/homebrew/lib /usr/local/lib)
	if(NOT ASSIMP_INCLUDE_DIR OR NOT ASSIMP_LIBRARY)
		message(FATAL_ERROR "No se encontro assimp. Instala con: brew install assimp")
	endif()
endif()

# glm es solo headers; Homebrew no siempre instala un .pc, por eso se busca la carpeta
find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp
	PATHS /opt/homebrew/include /usr/local/include)
if(NOT GLM_INCLUDE_DIR)
	message(FATAL_ERROR "No se encontro glm. Instala con: brew install glm")
endif()

add_executable(practica6
	practica6.cpp
	Camera.cpp
	Window.cpp
	Shader_m.cpp
	Mesh_tn.cpp
	Model.cpp
	Texture.cpp
)

target_include_directories(practica6 PRIVATE
	${GLFW_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}/GLFW
	${GLEW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}/GL
	${GLM_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}/glm
)

if(ASSIMP_FOUND)
	target_include_directories(practica6 PRIVATE ${ASSIMP_INCLUDE_DIRS})
	target_link_libraries(practica6 PRIVATE PkgConfig::ASSIMP)
else()
	target_include_directories(practica6 PRIVATE ${ASSIMP_INCLUDE_DIR})
	target_link_libraries(practica6 PRIVATE ${ASSIMP_LIBRARY})
endif()

target_link_libraries(practica6 PRIVATE
	PkgConfig::GLFW
	PkgConfig::GLEW
	OpenGL::GL
)

# Copiar shaders y modelos junto al ejecutable para que las rutas relativas
# ("shaders/...", "Models/...", "Textures/...") funcionen al correr
# el programa desde VS Code / terminal.
add_custom_command(TARGET practica6 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/shaders
		$<TARGET_FILE_DIR:practica6>/shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/Models
		$<TARGET_FILE_DIR:practica6>/Models
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/Textures
		$<TARGET_FILE_DIR:practica6>/Textures
)
