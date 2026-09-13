//práctica 3: Modelado Geométrico y Cámara Sintética.
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
//clases para dar orden y limpieza al còdigo
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
static const char* vShaderColor = "shaders/shadercolor.vert";
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
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet);

}


//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,//frontal
		3,2,4,//izquierda
		2,1,4,//trasera
		1,0,4,//derecha
		0,1,2,//abajo1
		0,2,3//abajo2

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramidec = new Mesh();
	piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramidec);
}

//Triangulo rectangulo plano (con un poco de grosor), usado como pieza para armar
//los 4 triangulos de las esquinas de cada cara del cofre. El angulo recto queda
//en el origen local, con los catetos sobre +X y +Y (de longitud 1 cada uno);
//para usarlo en una esquina se escala con signo para reflejarlo hacia el lado que
//se necesite y se traslada a la posicion de esa esquina.
void CrearTrianguloRectangulo()
{
	unsigned int triangulo_indices[] = {
		0,1,2,//cara frontal
		3,5,4,//cara trasera
		0,4,1, 0,3,4,//lado del cateto sobre X (0-1)
		1,4,5, 1,5,2,//lado de la hipotenusa (1-2)
		2,5,3, 2,3,0//lado del cateto sobre Y (2-0)
	};
	GLfloat triangulo_vertices[] = {
		0.0f,0.0f, 0.05f,//0 frontal
		1.0f,0.0f, 0.05f,//1 frontal
		0.0f,1.0f, 0.05f,//2 frontal
		0.0f,0.0f,-0.05f,//3 trasera
		1.0f,0.0f,-0.05f,//4 trasera
		0.0f,1.0f,-0.05f //5 trasera
	};
	Mesh* triangulo = new Mesh();
	triangulo->CreateMesh(triangulo_vertices, triangulo_indices, 18, 24);
	meshList.push_back(triangulo);
}




/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
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

	//Se generan los indices de los vertices: cada "cuadro" lateral (entre el
	//angulo n y el n+1) se arma con 2 triangulos, usando los pares
	//(inferior,superior) generados arriba para esa pared. Solo se cubre la
	//pared lateral (las tapas no tienen un vertice central para armarse).
	for (n = 0; n < res; n++) {
		indices.push_back(2 * n);
		indices.push_back(2 * n + 1);
		indices.push_back(2 * n + 2);

		indices.push_back(2 * n + 1);
		indices.push_back(2 * n + 2);
		indices.push_back(2 * n + 3);
	}

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



//Una sola cara triangular de la piramide cuadrangular (coincide exactamente
//con la cara "frontal" de CrearPiramideCuadrangular). Girando esta pieza en
//incrementos de 90 grados sobre el eje Y se reconstruyen las 4 caras
//triangulares de la piramide, cada una con su propio color.
void CrearCaraTriangularPiramide()
{
	unsigned int indices[] = {
		0,1,2
	};
	GLfloat vertices[] = {
		0.5f,-0.5f,0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* cara = new Mesh();
	cara->CreateMesh(vertices, indices, 9, 3);
	meshList.push_back(cara);
}

//Cara cuadrada (base) de la piramide, en el plano y=-0.5, del mismo tamano
//que la base de CrearPiramideCuadrangular.
void CrearCaraCuadradaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		0,2,3
	};
	GLfloat vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
	};
	Mesh* cara = new Mesh();
	cara->CreateMesh(vertices, indices, 12, 6);
	meshList.push_back(cara);
}

//Dibuja una piramide completa a partir de sus 5 caras por separado (las 4
//triangulares mas la cuadrada), cada una con su propio color, usando "base"
//como la transformacion (posicion + rotacion + escala) de esa piramide.
//Colores fijos por cara: roja, verde, amarilla y magenta en las 4 caras
//triangulares (se obtienen rotando la misma pieza 90 grados cada vez) y
//azul en la cara cuadrada, sin importar desde que angulo se vea la figura.
void DibujarPiramideColores(const glm::mat4& base, GLuint uniformModel, GLuint uniformColor)
{
	glm::vec3 coloresCara[4] = {
		glm::vec3(1.0f, 0.0f, 0.0f), //roja
		glm::vec3(0.0f, 1.0f, 0.0f), //verde
		glm::vec3(1.0f, 1.0f, 0.0f), //amarilla
		glm::vec3(1.0f, 0.0f, 1.0f)  //magenta
	};

	glm::mat4 model;
	for (int i = 0; i < 4; i++)
	{
		model = glm::rotate(base, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(coloresCara[i]));
		meshList[6]->RenderMesh(); //cara triangular de la piramide
	}

	glm::vec3 azul = glm::vec3(0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	glUniform3fv(uniformColor, 1, glm::value_ptr(azul));
	meshList[7]->RenderMesh(); //cara cuadrada (base) de la piramide
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(16, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CrearTrianguloRectangulo();//índice 5 en MeshList
	CrearCaraTriangularPiramide();//índice 6 en MeshList
	CrearCaraCuadradaPiramide();//índice 7 en MeshList
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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	
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
		
		//Transformacion comun: ubica toda la escena frente a la camara y permite
		//seguir rotando todo el conjunto con las teclas E (X), R (Y), T (Z)
		glm::mat4 escena = glm::mat4(1.0);
		escena = glm::translate(escena, glm::vec3(0.0f, 0.0f, -6.0f));
		escena = glm::rotate(escena, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		escena = glm::rotate(escena, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		escena = glm::rotate(escena, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//ejercicio: Instanciar primitivas geometricas para recrear la figura central
		//de la practica pasada en 3D (no importa que internamente se encimen los cubos).
		//Cada piramide de cada esquina lleva el color de una de las 4 piramides de la
		//practica pasada. Existe un plano de piso negro.

		//Piso negro (cubo aplanado en Y)
		model = glm::translate(escena, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 0.05f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); //cubo usado como piso

		//Figura central: un cofre (cubo grande) que reproduce en sus 6 caras el mismo
		//dibujo de la practica pasada: 4 triangulos de colores en las esquinas y un rombo
		//azul con uno olivo adentro, cubriendo cada cara de borde a borde (sin piramides
		//sueltas aparte), para que se vea igual sin importar desde donde se rote el cubo.
		float ladoCofre = 2.2f;
		float mitadCofre = ladoCofre * 0.5f;
		glm::mat4 cofre = glm::translate(escena, glm::vec3(0.0f, 0.15f, 0.0f));

		//Cuerpo del cofre (solo se alcanza a ver en los bordes; el resto de cada cara
		//visible queda cubierto de borde a borde por el patron de triangulos y rombos)
		model = glm::scale(cofre, glm::vec3(ladoCofre));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.55f, 0.2f); //dorado
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//Base de cada una de las 6 caras del cofre: ya trae la traslacion hasta la cara y
		//la rotacion para que el plano local XY quede pegado sobre esa cara; el patron
		//(triangulos y rombos) se define igual para las 6 caras, en su propio plano local.
		glm::mat4 baseCara[6];
		baseCara[0] = glm::translate(cofre, glm::vec3(0.0f, 0.0f, mitadCofre + 0.02f)); //frontal
		baseCara[1] = glm::translate(cofre, glm::vec3(mitadCofre + 0.02f, 0.0f, 0.0f));
		baseCara[1] = glm::rotate(baseCara[1], glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //derecha
		baseCara[2] = glm::translate(cofre, glm::vec3(0.0f, mitadCofre + 0.02f, 0.0f));
		baseCara[2] = glm::rotate(baseCara[2], glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //superior
		baseCara[3] = glm::translate(cofre, glm::vec3(0.0f, 0.0f, -(mitadCofre + 0.02f)));
		baseCara[3] = glm::rotate(baseCara[3], glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //trasera
		baseCara[4] = glm::translate(cofre, glm::vec3(-(mitadCofre + 0.02f), 0.0f, 0.0f));
		baseCara[4] = glm::rotate(baseCara[4], glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //izquierda
		baseCara[5] = glm::translate(cofre, glm::vec3(0.0f, -(mitadCofre + 0.02f), 0.0f));
		baseCara[5] = glm::rotate(baseCara[5], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //inferior

		//El patron cubre la cara completa de borde a borde (H = mitad del lado del cofre)
		float H = mitadCofre;

		//Los 4 triangulos de esquina: posicion de la esquina, signo con el que se escala
		//el triangulo (para que sus catetos apunten hacia el centro) y color, usando los
		//mismos colores de las 4 piramides de la practica pasada
		glm::vec3 posEsquina[4] = {
			glm::vec3(-H,  H, 0.0f), //superior izquierda
			glm::vec3( H,  H, 0.0f), //superior derecha
			glm::vec3(-H, -H, 0.0f), //inferior izquierda
			glm::vec3( H, -H, 0.0f)  //inferior derecha
		};
		glm::vec2 signoEsquina[4] = {
			glm::vec2( 1.0f, -1.0f),
			glm::vec2(-1.0f, -1.0f),
			glm::vec2( 1.0f,  1.0f),
			glm::vec2(-1.0f,  1.0f)
		};
		glm::vec3 colorEsquina[4] = {
			glm::vec3(1.0f, 1.0f, 0.0f), //amarillo
			glm::vec3(1.0f, 0.0f, 0.0f), //rojo
			glm::vec3(0.6f, 0.0f, 0.8f), //morado
			glm::vec3(0.0f, 1.0f, 0.0f)  //verde
		};

		for (int cara = 0; cara < 6; cara++)
		{
			//4 triangulos de esquina de esta cara
			for (int i = 0; i < 4; i++)
			{
				model = glm::translate(baseCara[cara], posEsquina[i]);
				model = glm::scale(model, glm::vec3(signoEsquina[i].x * H, signoEsquina[i].y * H, 0.1f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glUniform3fv(uniformColor, 1, glm::value_ptr(colorEsquina[i]));
				meshList[5]->RenderMesh(); //triangulo rectangulo
			}

			//Rombo exterior azul (toca los puntos medios de los 4 lados de la cara)
			model = glm::rotate(baseCara[cara], glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(H * 1.4142f, H * 1.4142f, 0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.0f, 0.0f, 1.0f); //azul
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshList[0]->RenderMesh();

			//Rombo interior olivo, un poco mas afuera para que no se pierda dentro del azul
			model = glm::translate(baseCara[cara], glm::vec3(0.0f, 0.0f, 0.03f));
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(H * 0.6f, H * 0.6f, 0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.5f, 0.5f, 0.0f); //verde olivo
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshList[0]->RenderMesh();
		}


		//==================================================
		//EJERCICIO: Cohete espacial (instancias de cilindro,
		//cono, piramide, cubo y esfera)
		//==================================================
		glm::mat4 cohete = glm::translate(escena, glm::vec3(-3.3f, -0.3f, 0.0f));

		//Cuerpo (cilindro)
		model = glm::scale(cohete, glm::vec3(0.5f, 3.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.85f, 0.85f, 0.85f); //gris claro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh(); //cilindro

		//Nariz (cono): su base (radio 2 ya incluido en la geometria del cono)
		//queda pegada justo en la punta superior del cilindro
		model = glm::translate(cohete, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.1f, 0.1f); //rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry(); //cono (se dibuja como abanico de triangulos)

		//3 aletas (piramide cuadrangular achatada) alrededor de la base del cuerpo
		color = glm::vec3(0.1f, 0.3f, 0.8f); //azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		for (int i = 0; i < 3; i++)
		{
			float angulo = 90.0f + i * 120.0f;
			model = glm::rotate(cohete, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.55f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.9f, 0.15f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[4]->RenderMesh(); //piramide cuadrangular usada como aleta
		}

		//Plataforma (cubo achatado) debajo del cohete
		model = glm::translate(cohete, glm::vec3(0.0f, -1.85f, 0.0f));
		model = glm::scale(model, glm::vec3(1.6f, 0.1f, 1.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.3f, 0.3f, 0.3f); //gris oscuro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); //cubo

		//Ventana (esfera): usa su propio shader, con color por vertice
		shaderList[1].useShader();
		GLuint uniformModelEsfera = shaderList[1].getModelLocation();
		GLuint uniformProjectionEsfera = shaderList[1].getProjectLocation();
		GLuint uniformViewEsfera = shaderList[1].getViewLocation();
		glUniformMatrix4fv(uniformProjectionEsfera, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformViewEsfera, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		model = glm::translate(cohete, glm::vec3(0.0f, 0.3f, 0.52f));
		model = glm::scale(model, glm::vec3(0.3f));
		glUniformMatrix4fv(uniformModelEsfera, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		//Se regresa al shader de color uniforme para seguir dibujando
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//==================================================
		//EJERCICIO: union de 8 piramides. Cada piramide tiene sus 4 caras
		//triangulares de colores distintos (roja, verde, amarilla, magenta)
		//y su cara cuadrada azul, sin importar desde que angulo se vea.
		//Las 8 piramides apuntan hacia las 8 direcciones diagonales de un
		//cubo (esquinas +-X,+-Y,+-Z), compartiendo un mismo centro.
		//==================================================
		glm::mat4 estrella = glm::translate(escena, glm::vec3(3.3f, 0.0f, 0.0f));

		const float anguloMagico = 35.264f; //asin(1/raiz(3)) en grados, para que
		                                    //las 8 direcciones queden exactamente
		                                    //sobre las diagonales de un cubo
		float signosX[2] = { 1.0f, -1.0f };
		float betas[4] = { 45.0f, -45.0f, 135.0f, -135.0f };

		for (int sx = 0; sx < 2; sx++)
		{
			float thetaZ = (signosX[sx] > 0.0f) ? -anguloMagico : anguloMagico;

			for (int j = 0; j < 4; j++)
			{
				glm::mat4 piramide = glm::rotate(estrella, glm::radians(betas[j]), glm::vec3(1.0f, 0.0f, 0.0f));
				piramide = glm::rotate(piramide, glm::radians(thetaZ), glm::vec3(0.0f, 0.0f, 1.0f));
				//La referencia (Ender's Game, Battle Room) muestra picos largos y
				//delgados, no piramides "gordas". Por eso la escala NO es uniforme:
				//se alarga en Y (altura del pico) y se angosta en X/Z (base chica).
				//La formula de alineacion de base sigue funcionando igual con
				//escala no uniforme (ver deduccion abajo), asi que no hay que
				//tocar nada mas.
				glm::vec3 escalaPico(0.35f, 2.4f, 0.35f);
				piramide = glm::scale(piramide, escalaPico);
				//Se recorre +0.5 en el eje Y local (ya escalado) para que la BASE de
				//la piramide (que esta en y=-0.5 en su geometria original) quede
				//exactamente en el centro compartido; el apice queda apuntando
				//hacia afuera. Esto funciona sin importar la escala en Y porque:
				//   y_base_final = escalaPico.y*(-0.5) + escalaPico.y*(0.5) = 0
				//Asi, cada par de piramides opuestas (mismo eje, direcciones
				//contrarias) comparte el mismo centro, y con los picos delgados
				//ya no se ve un amasijo: se ven 8 puntas finas saliendo de un
				//nucleo pequeno, como la figura de referencia.
				piramide = glm::translate(piramide, glm::vec3(0.0f, 0.5f, 0.0f));

				DibujarPiramideColores(piramide, uniformModel, uniformColor);
			}
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

	
		