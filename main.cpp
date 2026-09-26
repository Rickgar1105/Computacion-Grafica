/*
Practica 6: Jerarquia y rotaciones - Holocron, Satelite y Rover en la misma escena

Controles:
  Camara:      W A S D (mover), mouse (mirar)

  Rover (ya resuelto en la Practica 5):
    1-6        seleccionar pata (Del.Der, Del.Izq, Med.Der, Med.Izq, Tra.Der, Tra.Izq)
    Flecha Arriba/Abajo   rotar la pata seleccionada (+-45 grados)

  Holocron (esquinas independientes alrededor del centro del Holocron):
    F1-F8      seleccionar esquina 1-8
    [ / ]      rotar la esquina seleccionada alrededor del centro del Holocron (sin limite)

  Satelite - traslacion de todo el satelite sobre los 3 ejes:
    J / L      mover en X (-/+)
    I / K      mover en Y (+/-)
    U / O      mover en Z (-/+)

  Satelite - rotacion de 5 partes, cada una sobre su bisagra real de union con el cuerpo:
    Z, X, C, V, B   seleccionar parte (Panel Izq, Panel Der, Antena Ppal A, Antena Ppal B, Antena Secundaria)
    , / .           rotar la parte seleccionada (+-60 grados)
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Model.h"

Window mainWindow;
std::vector<MeshModel*> meshListModel; // solo se usa para el piso (xyz uv nx ny nz)
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

// ------------------------------------------------------------------
// ROVER (igual que en la Practica 5, ya corregido: las patas se separan
// del cuerpo por su bisagra real, no por el centro de la rueda)
// ------------------------------------------------------------------
Model Cuerpo_M;
Model Brazo_M;
Model PataRueda_DelanteraDerecha_M;
Model PataRueda_DelanteraIzquierda_M;
Model PataRueda_MediaDerecha_M;
Model PataRueda_MediaIzquierda_M;
Model PataRueda_TraseraDerecha_M;
Model PataRueda_TraseraIzquierda_M;

const glm::vec3 hingeDelanteraDerecha(5.375442f, 1.945616f, -2.415981f);
const glm::vec3 hingeDelanteraIzquierda(5.711149f, 1.705651f, 2.801974f);
const glm::vec3 hingeMediaDerecha(-0.945951f, 1.412757f, -3.890925f);
const glm::vec3 hingeMediaIzquierda(-0.945952f, 1.412758f, 3.890925f);
const glm::vec3 hingeTraseraDerecha(-5.112190f, 1.386433f, -3.890925f);
const glm::vec3 hingeTraseraIzquierda(-5.131741f, 1.412758f, 3.890925f);

float anguloPata[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
int pataSeleccionada = 0;
const float LIMITE_PATA = glm::radians(45.0f);
const float VELOCIDAD_PATA = glm::radians(45.0f);

// ------------------------------------------------------------------
// HOLOCRON: un cuerpo estatico (Cristal + Cuerpo_Inside + Cuerpo_Main)
// y 8 esquinas que giran, cada una, alrededor del CENTRO del Holocron
// (no alrededor de su propio centro). El punto de pivote es el mismo
// para las 8 esquinas: el centro del Holocron (Holocron_Centro en Blender).
// ------------------------------------------------------------------
Model Holocron_Cuerpo_M;
Model Holocron_Esquina_M[8];

// Punto de pivote (centro del Holocron), convertido de coordenadas de Blender
// (Xb,Yb,Zb) al espacio de exportacion OBJ (forward=-Z, up=Y): (Xo,Yo,Zo)=(Xb,Zb,-Yb)
const glm::vec3 holocronCentro(20.0f, 5.0f, 0.0f);

float anguloEsquina[8] = { 0,0,0,0,0,0,0,0 };
int esquinaSeleccionada = 0;
const float VELOCIDAD_ESQUINA = glm::radians(60.0f); // grados por segundo, sin limite de angulo

// ------------------------------------------------------------------
// SATELITE: un cuerpo estatico (Satelite_Cuerpo + Antena_Whip) que se
// traslada como conjunto sobre X, Y, Z; y 5 partes que ademas giran
// cada una sobre su propia bisagra real de union con el cuerpo.
// ------------------------------------------------------------------
Model Satelite_Cuerpo_M;
Model Panel_Solar_Izquierdo_M;
Model Panel_Solar_Derecho_M;
Model Antena_Principal_A_M;
Model Antena_Principal_B_M;
Model Antena_Secundaria_M;

// Bisagras reales (Satelite_Cuerpo -> parte), convertidas igual que arriba
const glm::vec3 hingePanelIzquierdo(-1.177934f, 7.536116f, -25.000431f);
const glm::vec3 hingePanelDerecho(-1.177934f, 7.536116f, -25.000431f);
const glm::vec3 hingeAntenaPrincipalA(0.629043f, 7.232842f, -23.519133f);
const glm::vec3 hingeAntenaPrincipalB(-0.626132f, 7.232842f, -23.519133f);
const glm::vec3 hingeAntenaSecundaria(0.636649f, 7.562293f, -25.737486f);

// angulo de cada una de las 5 partes rotables del satelite, en el mismo orden
// que las teclas de seleccion Z,X,C,V,B: 0=PanelIzq 1=PanelDer 2=AntenaA 3=AntenaB 4=AntenaSec
float anguloSatParte[5] = { 0,0,0,0,0 };
int satParteSeleccionada = 0;
const float LIMITE_SAT = glm::radians(60.0f);
const float VELOCIDAD_SAT = glm::radians(45.0f);

// Traslacion (offset) de todo el satelite, controlada con teclas J/L, I/K, U/O
glm::vec3 satOffset(0.0f, 0.0f, 0.0f);
const float VELOCIDAD_SAT_TRASLACION = 4.0f; // unidades por segundo


void CreateFloor()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		//	x       y      z			u	   v			nx	  ny    nz
			-40.0f, 0.0f, -40.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
			40.0f, 0.0f, -40.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
			-40.0f, 0.0f, 40.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
			40.0f, 0.0f, 40.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	MeshModel* piso = new MeshModel();
	piso->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(piso);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateFloor();

	// macOS exige que haya un VAO enlazado al momento de validar el programa
	// de shaders (glValidateProgram dentro de CreateShaders); sin esto, la
	// validacion falla y las ubicaciones de los uniforms (model/view/
	// projection/color) nunca se consultan, quedando todas en 0.
	GLuint dummyVAO;
	glGenVertexArrays(1, &dummyVAO);
	glBindVertexArray(dummyVAO);

	CreateShaders();

	// Camara alejada y elevada para poder ver los 3 conjuntos (Rover cerca del
	// origen, Holocron en X=20, Satelite en Z=-25) al mismo tiempo: se coloco en
	// (35,20,25) apuntando hacia el centro aproximado del conjunto (7,3,-9),
	// con yaw/pitch calculados a partir de esa direccion (no adivinados).
	camera = Camera(glm::vec3(35.0f, 20.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f), -129.5f, -21.1f, 10.0f, 0.3f);

	// --- Cargar Rover (igual que en la Practica 5) ---
	Cuerpo_M.LoadModel("Models/Cuerpo.obj");
	Brazo_M.LoadModel("Models/Brazo.obj");
	PataRueda_DelanteraDerecha_M.LoadModel("Models/PataRueda_Delantera_Derecha.obj");
	PataRueda_DelanteraIzquierda_M.LoadModel("Models/PataRueda_Delantera_Izquierda.obj");
	PataRueda_MediaDerecha_M.LoadModel("Models/PataRueda_Media_Derecha.obj");
	PataRueda_MediaIzquierda_M.LoadModel("Models/PataRueda_Media_Izquierda.obj");
	PataRueda_TraseraDerecha_M.LoadModel("Models/PataRueda_Trasera_Derecha.obj");
	PataRueda_TraseraIzquierda_M.LoadModel("Models/PataRueda_Trasera_Izquierda.obj");

	// --- Cargar Holocron ---
	Holocron_Cuerpo_M.LoadModel("Models/Holocron_Cuerpo.obj");
	Holocron_Esquina_M[0].LoadModel("Models/Holocron_Esquina_1.obj");
	Holocron_Esquina_M[1].LoadModel("Models/Holocron_Esquina_2.obj");
	Holocron_Esquina_M[2].LoadModel("Models/Holocron_Esquina_3.obj");
	Holocron_Esquina_M[3].LoadModel("Models/Holocron_Esquina_4.obj");
	Holocron_Esquina_M[4].LoadModel("Models/Holocron_Esquina_5.obj");
	Holocron_Esquina_M[5].LoadModel("Models/Holocron_Esquina_6.obj");
	Holocron_Esquina_M[6].LoadModel("Models/Holocron_Esquina_7.obj");
	Holocron_Esquina_M[7].LoadModel("Models/Holocron_Esquina_8.obj");

	// --- Cargar Satelite ---
	Satelite_Cuerpo_M.LoadModel("Models/Satelite_Cuerpo.obj");
	Panel_Solar_Izquierdo_M.LoadModel("Models/Panel_Solar_Izquierdo.obj");
	Panel_Solar_Derecho_M.LoadModel("Models/Panel_Solar_Derecho.obj");
	Antena_Principal_A_M.LoadModel("Models/Antena_Principal_A.obj");
	Antena_Principal_B_M.LoadModel("Models/Antena_Principal_B.obj");
	Antena_Secundaria_M.LoadModel("Models/Antena_Secundaria.obj");

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		auto keys = mainWindow.getsKeys();

		// ---------------- Rover: seleccion y rotacion de patas ----------------
		if (keys[GLFW_KEY_1]) pataSeleccionada = 0;
		if (keys[GLFW_KEY_2]) pataSeleccionada = 1;
		if (keys[GLFW_KEY_3]) pataSeleccionada = 2;
		if (keys[GLFW_KEY_4]) pataSeleccionada = 3;
		if (keys[GLFW_KEY_5]) pataSeleccionada = 4;
		if (keys[GLFW_KEY_6]) pataSeleccionada = 5;

		if (keys[GLFW_KEY_UP])
		{
			anguloPata[pataSeleccionada] += VELOCIDAD_PATA * deltaTime;
			if (anguloPata[pataSeleccionada] > LIMITE_PATA) anguloPata[pataSeleccionada] = LIMITE_PATA;
		}
		if (keys[GLFW_KEY_DOWN])
		{
			anguloPata[pataSeleccionada] -= VELOCIDAD_PATA * deltaTime;
			if (anguloPata[pataSeleccionada] < -LIMITE_PATA) anguloPata[pataSeleccionada] = -LIMITE_PATA;
		}

		// ---------------- Holocron: seleccion y rotacion de esquinas ----------------
		if (keys[GLFW_KEY_F1]) esquinaSeleccionada = 0;
		if (keys[GLFW_KEY_F2]) esquinaSeleccionada = 1;
		if (keys[GLFW_KEY_F3]) esquinaSeleccionada = 2;
		if (keys[GLFW_KEY_F4]) esquinaSeleccionada = 3;
		if (keys[GLFW_KEY_F5]) esquinaSeleccionada = 4;
		if (keys[GLFW_KEY_F6]) esquinaSeleccionada = 5;
		if (keys[GLFW_KEY_F7]) esquinaSeleccionada = 6;
		if (keys[GLFW_KEY_F8]) esquinaSeleccionada = 7;

		if (keys[GLFW_KEY_LEFT_BRACKET])  anguloEsquina[esquinaSeleccionada] -= VELOCIDAD_ESQUINA * deltaTime;
		if (keys[GLFW_KEY_RIGHT_BRACKET]) anguloEsquina[esquinaSeleccionada] += VELOCIDAD_ESQUINA * deltaTime;

		// ---------------- Satelite: traslacion de todo el conjunto ----------------
		if (keys[GLFW_KEY_L]) satOffset.x += VELOCIDAD_SAT_TRASLACION * deltaTime;
		if (keys[GLFW_KEY_J]) satOffset.x -= VELOCIDAD_SAT_TRASLACION * deltaTime;
		if (keys[GLFW_KEY_I]) satOffset.y += VELOCIDAD_SAT_TRASLACION * deltaTime;
		if (keys[GLFW_KEY_K]) satOffset.y -= VELOCIDAD_SAT_TRASLACION * deltaTime;
		if (keys[GLFW_KEY_O]) satOffset.z += VELOCIDAD_SAT_TRASLACION * deltaTime;
		if (keys[GLFW_KEY_U]) satOffset.z -= VELOCIDAD_SAT_TRASLACION * deltaTime;

		// ---------------- Satelite: seleccion y rotacion de partes ----------------
		if (keys[GLFW_KEY_Z]) satParteSeleccionada = 0; // Panel Solar Izquierdo
		if (keys[GLFW_KEY_X]) satParteSeleccionada = 1; // Panel Solar Derecho
		if (keys[GLFW_KEY_C]) satParteSeleccionada = 2; // Antena Principal A
		if (keys[GLFW_KEY_V]) satParteSeleccionada = 3; // Antena Principal B
		if (keys[GLFW_KEY_B]) satParteSeleccionada = 4; // Antena Secundaria

		if (keys[GLFW_KEY_COMMA])
		{
			anguloSatParte[satParteSeleccionada] -= VELOCIDAD_SAT * deltaTime;
			if (anguloSatParte[satParteSeleccionada] < -LIMITE_SAT) anguloSatParte[satParteSeleccionada] = -LIMITE_SAT;
		}
		if (keys[GLFW_KEY_PERIOD])
		{
			anguloSatParte[satParteSeleccionada] += VELOCIDAD_SAT * deltaTime;
			if (anguloSatParte[satParteSeleccionada] > LIMITE_SAT) anguloSatParte[satParteSeleccionada] = LIMITE_SAT;
		}

		// Clear
		glClearColor(0.04f, 0.05f, 0.09f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glm::vec3 eyePos = camera.getPosition();
		glUniform3f(uniformEyePosition, eyePos.x, eyePos.y, eyePos.z);

		// ---------------- Piso ----------------
		color = glm::vec3(0.45f, 0.45f, 0.48f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[0]->RenderMeshModel();

		// ================= ROVER (sin cambios respecto a la Practica 5) =================
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -1.5f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Brazo_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeDelanteraDerecha);
		modelaux = glm::rotate(modelaux, anguloPata[0], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeDelanteraDerecha);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_DelanteraDerecha_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeDelanteraIzquierda);
		modelaux = glm::rotate(modelaux, anguloPata[1], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeDelanteraIzquierda);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_DelanteraIzquierda_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeMediaDerecha);
		modelaux = glm::rotate(modelaux, anguloPata[2], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeMediaDerecha);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_MediaDerecha_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeMediaIzquierda);
		modelaux = glm::rotate(modelaux, anguloPata[3], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeMediaIzquierda);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_MediaIzquierda_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeTraseraDerecha);
		modelaux = glm::rotate(modelaux, anguloPata[4], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeTraseraDerecha);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_TraseraDerecha_M.RenderModel();

		modelaux = model; modelaux = glm::translate(modelaux, hingeTraseraIzquierda);
		modelaux = glm::rotate(modelaux, anguloPata[5], glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -hingeTraseraIzquierda);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		PataRueda_TraseraIzquierda_M.RenderModel();

		// ================= HOLOCRON =================
		// Cuerpo estatico del Holocron (cristal + carcasa), sin transformacion extra:
		// sus coordenadas ya vienen en su posicion absoluta dentro de la escena.
		color = glm::vec3(0.55f, 0.85f, 1.0f);
		model = glm::mat4(1.0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Holocron_Cuerpo_M.RenderModel();

		// Las 8 esquinas: cada una gira sobre el CENTRO del Holocron (mismo pivote
		// para las 8), usando la tecla F1-F8 para seleccionar y [ / ] para rotar.
		color = glm::vec3(0.75f, 0.6f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		for (int i = 0; i < 8; i++)
		{
			modelaux = glm::mat4(1.0);
			modelaux = glm::translate(modelaux, holocronCentro);
			modelaux = glm::rotate(modelaux, anguloEsquina[i], glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = glm::translate(modelaux, -holocronCentro);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
			Holocron_Esquina_M[i].RenderModel();
		}

		// ================= SATELITE =================
		// Todo el satelite (cuerpo + partes) se traslada junto con satOffset,
		// controlado con teclas J/L (X), I/K (Y), U/O (Z).
		glm::mat4 satBase = glm::translate(glm::mat4(1.0), satOffset);

		color = glm::vec3(0.85f, 0.85f, 0.85f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = satBase;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Satelite_Cuerpo_M.RenderModel();

		// Paneles solares: giran sobre su bisagra real con el cuerpo (eje X local)
		color = glm::vec3(0.15f, 0.25f, 0.65f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		modelaux = satBase; modelaux = glm::translate(modelaux, hingePanelIzquierdo);
		modelaux = glm::rotate(modelaux, anguloSatParte[0], glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -hingePanelIzquierdo);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Panel_Solar_Izquierdo_M.RenderModel();

		modelaux = satBase; modelaux = glm::translate(modelaux, hingePanelDerecho);
		modelaux = glm::rotate(modelaux, anguloSatParte[1], glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -hingePanelDerecho);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Panel_Solar_Derecho_M.RenderModel();

		// Antena principal (2 mitades): giran sobre su bisagra con el cuerpo (eje Y, tipo abanico)
		color = glm::vec3(0.9f, 0.9f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		modelaux = satBase; modelaux = glm::translate(modelaux, hingeAntenaPrincipalA);
		modelaux = glm::rotate(modelaux, anguloSatParte[2], glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = glm::translate(modelaux, -hingeAntenaPrincipalA);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Antena_Principal_A_M.RenderModel();

		modelaux = satBase; modelaux = glm::translate(modelaux, hingeAntenaPrincipalB);
		modelaux = glm::rotate(modelaux, anguloSatParte[3], glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = glm::translate(modelaux, -hingeAntenaPrincipalB);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Antena_Principal_B_M.RenderModel();

		// Antena secundaria: gira sobre su bisagra con el cuerpo (eje X)
		color = glm::vec3(0.8f, 0.4f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		modelaux = satBase; modelaux = glm::translate(modelaux, hingeAntenaSecundaria);
		modelaux = glm::rotate(modelaux, anguloSatParte[4], glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -hingeAntenaSecundaria);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Antena_Secundaria_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
