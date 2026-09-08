// Práctica 2: proyecciones, transformaciones geométricas
// Versión con pirámides y cubos, un shader por color

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Clases
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0f;

// Ventana
Window mainWindow;

// Listas
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Matrices globales
glm::mat4 projection;
glm::mat4 vista;

// Índices de meshList
const int PIRAMIDE = 0;
const int CUBO = 1;

// Índices de shaderList
const int ROJO = 0;
const int VERDE = 1;
const int AZUL = 2;
const int CAFE = 3;
const int MAGENTA = 4;
const int AMARILLO = 5;
const int NEGRO = 6;

// Fragment shader compartido por todos los colores
static const char* fShaderSolido = "shaders/shadersolido.frag";


// ======================================================
// CREAR PIRÁMIDE (base cuadrada)
// ======================================================
void CrearPiramide()
{
	unsigned int indices[] = {
		// Base
		0, 1, 2,
		0, 2, 3,
		// Caras laterales hacia el ápice (4)
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0
	};

	GLfloat vertices[] = {
		// X      Y      Z
		-0.5f, -0.5f, -0.5f,   // 0
		 0.5f, -0.5f, -0.5f,   // 1
		 0.5f, -0.5f,  0.5f,   // 2
		-0.5f, -0.5f,  0.5f,   // 3
		 0.0f,  0.5f,  0.0f    // 4 -> ápice
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(vertices, indices, 15, 18);
	meshList.push_back(piramide);
}


// ======================================================
// CREAR CUBO
// ======================================================
void CrearCubo()
{
	unsigned int indices[] = {
		// Frente
		4, 5, 6,
		4, 6, 7,
		// Atrás
		1, 0, 3,
		1, 3, 2,
		// Izquierda
		0, 4, 7,
		0, 7, 3,
		// Derecha
		5, 1, 2,
		5, 2, 6,
		// Arriba
		3, 7, 6,
		3, 6, 2,
		// Abajo
		0, 1, 5,
		0, 5, 4
	};

	GLfloat vertices[] = {
		// X      Y      Z
		-0.5f, -0.5f, -0.5f,   // 0
		 0.5f, -0.5f, -0.5f,   // 1
		 0.5f,  0.5f, -0.5f,   // 2
		-0.5f,  0.5f, -0.5f,   // 3
		-0.5f, -0.5f,  0.5f,   // 4
		 0.5f, -0.5f,  0.5f,   // 5
		 0.5f,  0.5f,  0.5f,   // 6
		-0.5f,  0.5f,  0.5f    // 7
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(vertices, indices, 24, 36);
	meshList.push_back(cubo);
}


// ======================================================
// CREAR SHADERS (uno por color)
// ======================================================
void AgregarShader(const char* archivoVertex)
{
	Shader* shader = new Shader();
	shader->CreateFromFiles(archivoVertex, fShaderSolido);
	shaderList.push_back(*shader);
}

void CreateShaders()
{

	shaderList.reserve(7);

	AgregarShader("shaders/shaderrojo.vert");       // 0 -> ROJO
	AgregarShader("shaders/shaderverde.vert");      // 1 -> VERDE
	AgregarShader("shaders/shaderazul.vert");       // 2 -> AZUL
	AgregarShader("shaders/shadercafe.vert");       // 3 -> CAFE
	AgregarShader("shaders/shadermagenta.vert");    // 4 -> MAGENTA
	AgregarShader("shaders/shaderamarillo.vert");   // 5 -> AMARILLO
	AgregarShader("shaders/shadernegro.vert");      // 6 -> NEGRO
}


// ======================================================
// DIBUJAR FIGURA
// ======================================================
void DibujarFigura(
	int indiceShader,
	int indiceMesh,
	glm::vec3 traslacion,
	glm::vec3 escala,
	float anguloZ = 0.0f
)
{
	shaderList[indiceShader].useShader();

	GLuint uniformModel = shaderList[indiceShader].getModelLocation();
	GLuint uniformProjection = shaderList[indiceShader].getProjectLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));


	glm::mat4 model = vista;

	model = glm::translate(model, traslacion);
	model = glm::rotate(model, glm::radians(anguloZ), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, escala);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	meshList[indiceMesh]->RenderMesh();
}


// ======================================================
// MAIN
// ======================================================
int main()
{
	mainWindow = Window(1000, 700);
	mainWindow.Initialise();

	CrearPiramide();
	CrearCubo();
	CreateShaders();


	glEnable(GL_DEPTH_TEST);

	// PROYECCIÓN ORTOGONAL
	projection = glm::ortho(
		-10.0f, 10.0f,
		-5.0f, 5.0f,
		0.1f, 100.0f
	);

	
	vista = glm::mat4(1.0f);
	vista = glm::translate(vista, glm::vec3(0.0f, 0.0f, -5.0f));
	vista = glm::rotate(vista, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	vista = glm::rotate(vista, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	vista = glm::translate(vista, glm::vec3(0.0f, 0.0f, 5.0f));

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		// Fondo blanco
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ==================================================
		// INICIALES - "REGT" (PARTE SUPERIOR)
		// ==================================================

		// --- Letra R (rojo) ---
		// Poste izquierdo
		DibujarFigura(ROJO, CUBO, glm::vec3(-6.5f, 2.5f, -5.0f), glm::vec3(0.3f, 2.5f, 0.3f));
		// Barra superior
		DibujarFigura(ROJO, CUBO, glm::vec3(-5.9f, 3.6f, -5.0f), glm::vec3(1.0f, 0.3f, 0.3f));
		// Poste derecho (mitad superior)
		DibujarFigura(ROJO, CUBO, glm::vec3(-5.55f, 3.05f, -5.0f), glm::vec3(0.3f, 1.4f, 0.3f));
		// Barra media
		DibujarFigura(ROJO, CUBO, glm::vec3(-5.9f, 2.5f, -5.0f), glm::vec3(1.0f, 0.3f, 0.3f));
		// Pata diagonal
		DibujarFigura(ROJO, CUBO, glm::vec3(-5.5f, 1.905f, -5.0f), glm::vec3(0.3f, 1.6f, 0.3f), 35.0f);

		// --- Letra E (verde) ---
		DibujarFigura(VERDE, CUBO, glm::vec3(-2.5f, 2.5f, -5.0f), glm::vec3(0.3f, 2.5f, 0.3f));
		DibujarFigura(VERDE, CUBO, glm::vec3(-1.75f, 3.6f, -5.0f), glm::vec3(1.2f, 0.3f, 0.3f));
		DibujarFigura(VERDE, CUBO, glm::vec3(-1.75f, 2.5f, -5.0f), glm::vec3(1.2f, 0.3f, 0.3f));
		DibujarFigura(VERDE, CUBO, glm::vec3(-1.75f, 1.4f, -5.0f), glm::vec3(1.2f, 0.3f, 0.3f));

		// --- Letra G (azul) ---
		DibujarFigura(AZUL, CUBO, glm::vec3(1.5f, 2.5f, -5.0f), glm::vec3(0.3f, 2.5f, 0.3f));
		DibujarFigura(AZUL, CUBO, glm::vec3(2.25f, 3.6f, -5.0f), glm::vec3(1.2f, 0.3f, 0.3f));
		DibujarFigura(AZUL, CUBO, glm::vec3(2.25f, 1.4f, -5.0f), glm::vec3(1.2f, 0.3f, 0.3f));
		DibujarFigura(AZUL, CUBO, glm::vec3(2.85f, 1.95f, -5.0f), glm::vec3(0.3f, 0.8f, 0.3f));
		DibujarFigura(AZUL, CUBO, glm::vec3(2.5f, 2.5f, -5.0f), glm::vec3(0.7f, 0.3f, 0.3f));

		// --- Letra T (magenta) ---
		DibujarFigura(MAGENTA, CUBO, glm::vec3(6.5f, 3.6f, -5.0f), glm::vec3(2.0f, 0.3f, 0.3f));
		DibujarFigura(MAGENTA, CUBO, glm::vec3(6.5f, 2.35f, -5.0f), glm::vec3(0.3f, 2.2f, 0.3f));


		// ==================================================
		// BASE NEGRA INFERIOR
		// ==================================================
		DibujarFigura(
			NEGRO, CUBO,
			glm::vec3(0.0f, -2.2f, -5.0f),
			glm::vec3(20.0f, 0.4f, 2.0f)
		);

		// ==================================================
		// FIGURA 1 - IZQUIERDA
		// Los triángulos pasan a ser pirámides; los postes, cubos.
		// ==================================================
		DibujarFigura(CAFE, CUBO, glm::vec3(-7.85f, -0.4f, -5.0f), glm::vec3(0.2f, 3.2f, 0.4f));
		DibujarFigura(CAFE, CUBO, glm::vec3(-6.15f, -0.4f, -5.0f), glm::vec3(0.2f, 3.2f, 0.4f));
		DibujarFigura(AMARILLO, PIRAMIDE, glm::vec3(-7.0f, 0.5f, -5.0f), glm::vec3(1.5f, 1.0f, 1.5f), 180.0f);
		DibujarFigura(ROJO, PIRAMIDE, glm::vec3(-7.0f, -0.5f, -5.0f), glm::vec3(1.5f, 1.0f, 1.5f), 180.0f);
		DibujarFigura(VERDE, PIRAMIDE, glm::vec3(-7.0f, -1.5f, -5.0f), glm::vec3(1.5f, 1.0f, 1.5f), 180.0f);

		// ==================================================
		// FIGURA 2 - CENTRO
		// ==================================================
		DibujarFigura(AMARILLO, CUBO, glm::vec3(-0.75f, 0.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.0f));
		DibujarFigura(ROJO, CUBO, glm::vec3(0.75f, 0.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.0f));
		DibujarFigura(MAGENTA, CUBO, glm::vec3(-0.75f, -1.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.0f));
		DibujarFigura(VERDE, CUBO, glm::vec3(0.75f, -1.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.0f));
		// Cubo azul girado 45° (el rombo grande)
		DibujarFigura(AZUL, CUBO, glm::vec3(0.0f, -0.5f, -5.0f), glm::vec3(2.121f, 2.121f, 1.6f), 45.0f);
		// Cubo café girado 45° (el rombo pequeño del centro)
		DibujarFigura(CAFE, CUBO, glm::vec3(0.0f, -0.5f, -5.0f), glm::vec3(1.06f, 1.06f, 2.0f), 45.0f);

		// ==================================================
		// FIGURA 3 - DERECHA
		// ==================================================
		DibujarFigura(MAGENTA, PIRAMIDE, glm::vec3(7.0f, 0.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.5f));
		DibujarFigura(VERDE, PIRAMIDE, glm::vec3(6.25f, -1.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.5f));
		DibujarFigura(ROJO, PIRAMIDE, glm::vec3(7.75f, -1.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.5f));
		DibujarFigura(AMARILLO, PIRAMIDE, glm::vec3(7.0f, -1.25f, -5.0f), glm::vec3(1.5f, 1.5f, 1.5f), 180.0f);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}