cmake_minimum_required(VERSION 3.16)
project(practica8_iluminacion CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)

# assimp: carga de modelos OBJ (Model.h/Model.cpp)
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

add_executable(practica8
	practica8.cpp
	Camera.cpp
	Window.cpp
	Shader_light.cpp
	Mesh_tn.cpp
	Model.cpp
	Texture.cpp
	Skybox.cpp
	Light.cpp
	DirectionalLight.cpp
	PointLight.cpp
	SpotLight.cpp
	Material.cpp
)

# Los fuentes del curso incluyen <glew.h>, <glfw3.h> y <glm.hpp> sin carpeta
target_include_directories(practica8 PRIVATE
	${GLFW_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}/GLFW
	${GLEW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}/GL
	${GLM_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}/glm
)

if(ASSIMP_FOUND)
	target_include_directories(practica8 PRIVATE ${ASSIMP_INCLUDE_DIRS})
	target_link_libraries(practica8 PRIVATE PkgConfig::ASSIMP)
else()
	target_include_directories(practica8 PRIVATE ${ASSIMP_INCLUDE_DIR})
	target_link_libraries(practica8 PRIVATE ${ASSIMP_LIBRARY})
endif()

target_link_libraries(practica8 PRIVATE
	PkgConfig::GLFW
	PkgConfig::GLEW
	OpenGL::GL
)

# Copiar shaders, modelos y texturas junto al ejecutable para que funcionen
# las rutas relativas ("shaders/...", "Models/...", "Textures/...").
add_custom_command(TARGET practica8 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/shaders
		$<TARGET_FILE_DIR:practica8>/shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/Models
		$<TARGET_FILE_DIR:practica8>/Models
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/Textures
		$<TARGET_FILE_DIR:practica8>/Textures
)
