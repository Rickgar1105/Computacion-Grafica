/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas para rotaciones de articulaciones del rover (Actividad 1):
	F: EJE FRONTAL derecho   (llanta delantera derecha)
	G: EJE LL2 derecho       (llanta central derecha)
	H: EJE LL3 derecho       (llanta trasera derecha)
	V: EJE FRONTAL izquierdo (llanta delantera izquierda)
	B: EJE LL2 izquierdo     (llanta central izquierda)
	N: EJE LL3 izquierdo     (llanta trasera izquierda)
	(las 6 llantas se controlan por separado, cada una con su propia tecla)
	J: EJE BB-BR1        (hombro del brazo)
	K: EJE BR1-BR2       (codo del brazo)
	L: EJE BR2-PINZA     (muñeca del brazo)

Tecla M: alterna entre la escena del rover (Actividad 1) y la escena de la sonda espacial
	construida jerárquicamente (Actividad 2). Solo una escena se dibuja a la vez.

Teclas para las articulaciones de la sonda espacial (Actividad 2), una por extremidad:
	Q: EXTREMIDAD frontal derecha
	Z: EXTREMIDAD trasera derecha
	X: EXTREMIDAD frontal izquierda
	C: EXTREMIDAD trasera izquierda
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include <gtc/random.hpp>
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh *cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res,float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res+2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh *cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh *piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(5, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CrearCilindro(20, 1.0f);//índice 5 en MeshList - cilindro de alta resolución usado para las llantas
	CreateShaders();
	

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(20.0f, 14.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0f), -135.0f, -15.0f, 0.3f, 0.3f); //posicion inicial ajustada para que el rover completo entre en cuadro al arrancar

	
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux2;
	//Matrices auxiliares para las 4 ramas que cuelgan de BASE (eje frontal, brazo, eje LL2 y eje LL3).
	//Se reutilizan de una rama a otra porque cada rama se dibuja completa antes de pasar a la siguiente.
	glm::mat4 modelauxA, modelauxB, modelauxC, modelauxD, modelauxE, modelauxF;

	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();


		//articulacion1 hasta articulación6 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen
		//La proyección y la vista de la cámara se mandan una sola vez por cuadro, antes de decidir
		//qué escena se dibuja, para que la cámara responda igual en el rover y en la sonda.
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		if (!mainWindow.getEscenaSonda())
		{ //===================== ACTIVIDAD 1: ROVER =====================
		model = glm::mat4(1.0);
	
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -4.0f)); //NOS POSICIONAMOS EN EL CENTRO DEL OBJETO X AMARILLA 
		modelaux = model; //guardamos la matriz de modelo para que la base se mueva con el objeto
		// Creando la cabina del rover ¿CUÁNTAS UNIDADES MEDIRÁ EN Z?
		model = glm::translate(model, glm::vec3(1.0f, 2.0f, 0.0f));//PARA LLEGAR AL CENTRO DE LA CABINA a PARTIR DEL ORIGEN
		model = glm::scale(model, glm::vec3(8.0f, 4.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular
		//meshList[3]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro, cono, pirámide base cuadrangular
		//sp.render(); //dibuja esfera


		// BASE
		model = glm::mat4(1.0); //Si dejamos está linea, la base no estará unida al centro del objeto. se debe comentar para que la base se mueva con el objeto.
		/*En su lugar usamos la matriz auxiliar modelaux para que la base se mueva con el objeto.
		Lo que debemos de saber es: de las transformaciones geométricas que se aplican a la cabina y 
		cuales queremos que se apliquen a la base. En este caso, sólo queremos que se aplique la traslación del origen, no la rotación ni el escalado de la cabina. 
		Por lo tanto, debemos de guardar en modelaux sólo la traslación del origen y luego aplicarla a la base.
		*/
		model = modelaux;
		//NOS POSICIONAMOS EN EL CENTRO DE LA BASE
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		modelaux = model; //guardamos la matriz de modelo para conectar los siguientes objetos a la base
		model = glm::scale(model, glm::vec3(10.0f, 1.5f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh();
		
		model = modelaux;
		modelaux2 = model; //guardamos la matriz de modelo para conectar los siguientes objetos a la base
		/*
		A PARTIR DE ESTE PUNTO HAY QUE ACOMODAR VARIOS ELEMENTOS:
		LA BASE PARA LA LLANTA FRONTAL
		LA BASE PARA EL BRAZO
		LA BASE PARA LA LLANTA DE EN MEDIO
		LA BASE PARA LA LLANTA DE ATRÁS
		.........¿FALTA ALGO MÁS?
		CÓMO ESTAREMOS REGRESANDO A ESTE PUNTO EN ESPECÍFICO VARIAS VECES, DEBEMOS DE USAR UNA NUEVA MATRIZ AUXILIAR PARA CADA UNO DE LOS ELEMENTOS 
		QUE SE QUIERAN CONECTAR A LA BASE, YA QUE SI USAMOS LA MISMA MATRIZ AUXILIAR, SE SOBRESCRIBIRÁN LAS TRANSFORMACIONES GEOMÉTRICAS DE CADA ELEMENTO 
		Y NO SE PODRÁN CONECTAR CORRECTAMENTE.

		Las dimensiones y distancias usadas abajo son un punto de partida razonable a la escala de la cabina/base
		ya construidas; ajústalas si el profesor dio medidas exactas o si al correrlo alguna pieza se ve fuera de lugar.
		*/

		//===== EJE FRONTAL: llanta delantera derecha (tecla F -> articulacion1) =====
		//Diagrama: EJE FRONTAL -> BARRA -> EJE B-PL -> PATA L -> LLANTA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 3.5f)); //punto donde gira el eje frontal
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model; //pivote EJE FRONTAL, aquí se conectan sus hijos
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//BARRA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 1.5f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE B-PL: pivote al final de la barra (sin escalado, para que la pata no herede el tamaño de la barra)
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 3.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxB;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL: pivote al final de la pata (eje de la llanta)
		model = modelauxB;
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		modelauxC = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA (cilindro acostado, girado 90 grados sobre Z para que su eje quede lateral, como el eje de una rueda)
		model = modelauxC;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.2f, 1.4f, 2.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//===== EJE FRONTAL (lado opuesto): llanta delantera izquierda (tecla V -> articulacion1i) =====
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 3.5f)); //mismo punto que el eje frontal, espejado en X
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1i()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//BARRA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 1.5f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE B-PL
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 3.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxB;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL
		model = modelauxB;
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		modelauxC = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA
		model = modelauxC;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.2f, 1.4f, 2.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();


		//===== EJE LL2: llanta central derecha (tecla G -> articulacion2) =====
		//Diagrama: EJE LL2 -> PATA L -> EJE PL-LL -> LLANTA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(5.0f, -0.3f, 0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model; //pivote EJE LL2
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -3.6f, 0.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA
		model = modelauxB;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.6f, 1.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//===== EJE LL2 (lado opuesto): llanta central izquierda (tecla B -> articulacion2i) =====
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.0f, -0.3f, 0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2i()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -3.6f, 0.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA
		model = modelauxB;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.6f, 1.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();


		//===== EJE LL3: llanta trasera derecha (tecla H -> articulacion3) =====
		//Diagrama: EJE LL3 -> PATA L -> EJE PL-LL -> LLANTA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(5.0f, -0.3f, -2.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model; //pivote EJE LL3
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -3.6f, 0.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA
		model = modelauxB;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.6f, 1.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//===== EJE LL3 (lado opuesto): llanta trasera izquierda (tecla N -> articulacion3i) =====
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.0f, -0.3f, -2.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3i()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelauxA = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PATA L
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE PL-LL
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -3.6f, 0.0f));
		modelauxB = model;
		{ //union/junta visible en este eje (esfera pequena, marca el punto de articulacion)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.7f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//LLANTA
		model = modelauxB;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.6f, 1.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();


		//===== EJE BRAZO: brazo con pinza (hombro = tecla J, codo = tecla K, muñeca = tecla L) =====
		//Diagrama: EJE BRAZO -> BASE BRAZO -> EJE BB-BR1 -> BRAZO 1 -> EJE BR1-BR2 -> BRAZO 2 -> EJE BR2-PINZA -> PINZA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.5f, 0.75f, 3.6f)); //punto fijo donde se monta el brazo sobre la base (movido para que no atraviese la cabina)
		modelauxA = model; //pivote EJE BRAZO (fijo, no tiene tecla asignada)
		{ //union/junta visible en este eje del brazo (esfera un poco mas grande)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//BASE BRAZO: pedestal fijo
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(1.4f, 1.2f, 1.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f)); //arriba del pedestal
		modelauxB = model; //referencia BASE BRAZO

		//EJE BB-BR1: hombro (tecla J -> articulacion4)
		model = modelauxB;
		model = glm::rotate(model, glm::radians(20.0f + mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f)); //20 grados fijos de flexion en el hombro (para que el brazo no salga recto), mas lo que sume la tecla J
		modelauxC = model; //pivote EJE BB-BR1
		{ //union/junta visible en este eje del brazo (esfera un poco mas grande)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//BRAZO 1
		model = modelauxC;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.7f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelauxC;
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f)); //extremo de BRAZO 1
		modelauxD = model;

		//EJE BR1-BR2: codo (tecla K -> articulacion5)
		model = modelauxD;
		model = glm::rotate(model, glm::radians(-50.0f + mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f)); //-50 grados fijos en el codo (dobla el brazo hacia el otro lado), mas lo que sume la tecla K
		modelauxE = model; //pivote EJE BR1-BR2
		{ //union/junta visible en este eje del brazo (esfera un poco mas grande)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//BRAZO 2
		model = modelauxE;
		model = glm::translate(model, glm::vec3(0.0f, 1.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 3.2f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.7f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelauxE;
		model = glm::translate(model, glm::vec3(0.0f, 3.2f, 0.0f)); //extremo de BRAZO 2
		modelauxF = model;

		//EJE BR2-PINZA: muñeca (tecla L -> articulacion6)
		model = modelauxF;
		model = glm::rotate(model, glm::radians(15.0f + mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f)); //15 grados fijos en la muneca, mas lo que sume la tecla L
		{ //union/junta visible en este eje del brazo (esfera un poco mas grande)
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PINZA
		model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.4f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		} //===================== fin ACTIVIDAD 1: ROVER =====================
		else
		{ //===================== ACTIVIDAD 2: SONDA ESPACIAL (jerarquica) =====================
		/*Diagrama: CUERPO (nodo padre) -> ANTENA -> PUNTA
		                              -> EXTREMIDAD x4 (una por esquina) -> PIERNA -> EJE PIERNA-PIE -> PIE
		Igual que en el rover, se guarda un pivote antes de escalar cada pieza para que la escala de una
		pieza no se herede a sus hijos. Las 4 extremidades se escriben como ramas explicitas (misma idea
		de "espejo" que las llantas del rover, ahora combinando X y Z para las 4 esquinas del cuerpo).*/

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -4.0f)); //mismo punto de referencia que usa el rover
		modelaux = model; //pivote CUERPO, aqui se conectan todos los hijos de la sonda
		//La sonda se veia muy pequena y lejana con la camara pensada para el rover (mucho mas grande),
		//por lo que las articulaciones Q/Z/X/C se notaban muy poco aunque si estaban rotando. Se agranda
		//aqui, de forma uniforme y alrededor del mismo punto de anclaje, para que el movimiento se vea claro.
		modelaux = glm::scale(modelaux, glm::vec3(2.2f));

		//CUERPO
		model = modelaux;
		model = glm::scale(model, glm::vec3(3.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//EJE ANTENA: pivote arriba del cuerpo (sin escalado, para que la antena no herede el tamano del cuerpo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); //techo del cuerpo (cuerpo mide 2.0 en Y, mitad = 1.0)
		modelauxF = model;
		{ //union/junta visible en este eje
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//ANTENA
		model = modelauxF;
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//EJE PUNTA: extremo de la antena (sin escalado, para que la punta no herede el tamano de la antena)
		model = modelauxF;
		model = glm::translate(model, glm::vec3(0.0f, 1.8f, 0.0f));
		modelauxE = model;

		//PUNTA (sensor en la punta de la antena)
		model = modelauxE;
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.3f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		//===== EXTREMIDAD: frontal derecha (esquina +X +Z) =====
		//Diagrama: CUERPO -> EXTREMIDAD -> PIERNA -> EJE PIERNA-PIE -> PIE
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.3f, -0.7f, 1.3f)); //esquina inferior del cuerpo
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //abre la pata en X
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //abre la pata en Z
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionE1()), glm::vec3(1.0f, 0.0f, 0.0f)); //articulacion interactiva (tecla Q), se suma a la apertura fija
		modelauxA = model; //pivote EXTREMIDAD
		{ //union/junta visible en este eje
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PIERNA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//EJE PIERNA-PIE: pivote al final de la pierna (sin escalado, para que el pie no herede el tamano de la pierna)
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		modelauxB = model;

		//PIE
		model = modelauxB;
		model = glm::scale(model, glm::vec3(0.6f, 0.35f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		//===== EXTREMIDAD: trasera derecha (esquina +X -Z) =====
		//Diagrama: CUERPO -> EXTREMIDAD -> PIERNA -> EJE PIERNA-PIE -> PIE
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.3f, -0.7f, -1.3f)); //esquina inferior del cuerpo
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //abre la pata en X
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //abre la pata en Z
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionE2()), glm::vec3(1.0f, 0.0f, 0.0f)); //articulacion interactiva (tecla Z), se suma a la apertura fija
		modelauxA = model; //pivote EXTREMIDAD
		{ //union/junta visible en este eje
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PIERNA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//EJE PIERNA-PIE: pivote al final de la pierna (sin escalado, para que el pie no herede el tamano de la pierna)
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		modelauxB = model;

		//PIE
		model = modelauxB;
		model = glm::scale(model, glm::vec3(0.6f, 0.35f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		//===== EXTREMIDAD: frontal izquierda (esquina -X +Z) =====
		//Diagrama: CUERPO -> EXTREMIDAD -> PIERNA -> EJE PIERNA-PIE -> PIE
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.3f, -0.7f, 1.3f)); //esquina inferior del cuerpo
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //abre la pata en X
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //abre la pata en Z
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionE3()), glm::vec3(1.0f, 0.0f, 0.0f)); //articulacion interactiva (tecla X), se suma a la apertura fija
		modelauxA = model; //pivote EXTREMIDAD
		{ //union/junta visible en este eje
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PIERNA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//EJE PIERNA-PIE: pivote al final de la pierna (sin escalado, para que el pie no herede el tamano de la pierna)
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		modelauxB = model;

		//PIE
		model = modelauxB;
		model = glm::scale(model, glm::vec3(0.6f, 0.35f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		//===== EXTREMIDAD: trasera izquierda (esquina -X -Z) =====
		//Diagrama: CUERPO -> EXTREMIDAD -> PIERNA -> EJE PIERNA-PIE -> PIE
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.3f, -0.7f, -1.3f)); //esquina inferior del cuerpo
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //abre la pata en X
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //abre la pata en Z
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionE4()), glm::vec3(1.0f, 0.0f, 0.0f)); //articulacion interactiva (tecla C), se suma a la apertura fija
		modelauxA = model; //pivote EXTREMIDAD
		{ //union/junta visible en este eje
			glm::mat4 modelUnion = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelUnion));
			color = glm::vec3(0.2f, 0.4f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			sp.render();
		}

		//PIERNA
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.55f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//EJE PIERNA-PIE: pivote al final de la pierna (sin escalado, para que el pie no herede el tamano de la pierna)
		model = modelauxA;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		modelauxB = model;

		//PIE
		model = modelauxB;
		model = glm::scale(model, glm::vec3(0.6f, 0.35f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		} //===================== fin ACTIVIDAD 2: SONDA ESPACIAL =====================


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
