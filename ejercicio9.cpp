/*
Ejercicio de Práctica 9: Animación Básica
Ricardo Emmanuel Galicia Tequianes - 118001740

Se parte del código de la práctica 8 (rover, avión, holocrón, nave y sus luces). El rover y el avión ya no se
mueven con el teclado, ahora se animan solos:
- El rover recorre el piso a lo largo de X en un ciclo infinito: avanza hasta que su frente llega a la orilla
  del plano, retrocede hasta que su parte trasera llega a la orilla contraria y vuelve a avanzar. Las seis
  llantas giran según la distancia recorrida, hacia un lado al avanzar y hacia el otro al retroceder.
- El avión vuela en un circuito cerrado: dos rectas unidas por dos medias vueltas. Siempre apunta en la
  dirección en la que avanza (nunca vuela de reversa) y se inclina hacia adentro en cada vuelta.
  Sus dos spotlight (la de abajo y el faro) van con él y el faro gira con su rumbo.

Las teclas se procesan en Window::ManejaTeclado (Window.cpp), que guarda el estado de cada luz, el color de la
luz del avión y la posición del holocrón; aquí solo se leen con los get de mainWindow. Las variables que se
usan en cada cuadro se declaran antes del while y dentro solo se les asigna valor.

Controles (Window.cpp):
	W A S D + ratón		cámara
	Flecha izq. / der.	el holocrón se mueve en X
	C					siguiente color de la luz inferior del avión
	K					enciende o apaga la luz del holocrón
	P					enciende o apaga la luz de la pinza del rover
	N					enciende o apaga las luces de la nave (puntual y spotlight)
	1 / 2				enciende o apaga el motor 1 (izquierdo) / motor 2 (derecho): llama y luz azul
	L					enciende o apaga la linterna de la cámara
	Esc					cerrar
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
// Textura del dado (práctica 6)
Texture dadoTexture;		//una sola imagen con las 6 caras (3 columnas x 2 filas)


Model Blackhawk_M;
//Rover de la práctica 5 separado en piezas para la jerarquía: cuerpo, brazo, seis patas y seis llantas
Model Cuerpo_M;
Model Brazo_M;
Model Pata_M[6];
Model Llanta_M[6];
//Holocrón Jedi (práctica 8): una esquina que se dibuja ocho veces y el núcleo; avión de la práctica 6
Model HolocronEsquina_M;
Model HolocronNucleo_M;
Model Avion_M;
//Lámpara de calle
Model Lampara_M;
//Nave espacial de la práctica 8 y textura de la llama de sus motores
Model Nave_M;
Texture fuegoTexture;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Puntos de unión de cada pata del rover con el cuerpo (calculados en Blender en la práctica 5)
//Orden: delantera der., delantera izq., media der., media izq., trasera der., trasera izq.
const glm::vec3 hingePata[6] = {
	glm::vec3(5.375442f, 1.945616f, -2.415981f),
	glm::vec3(5.711149f, 1.705651f, 2.801974f),
	glm::vec3(-0.945951f, 1.412757f, -3.890925f),
	glm::vec3(-0.945952f, 1.412758f, 3.890925f),
	glm::vec3(-5.112190f, 1.386433f, -3.890925f),
	glm::vec3(-5.131741f, 1.412758f, 3.890925f)
};

//Centro de cada llanta en las coordenadas del cuerpo del rover (origen que se le dio en Blender a cada llanta)
//Mismo orden que las patas
const glm::vec3 centroLlanta[6] = {
	glm::vec3(5.7099f, 1.7050f, -3.0700f),
	glm::vec3(5.7124f, 1.7050f, 3.0700f),
	glm::vec3(-0.9472f, 1.4121f, -4.1589f),
	glm::vec3(-0.9447f, 1.4121f, 4.1589f),
	glm::vec3(-5.1134f, 1.3858f, -4.1589f),
	glm::vec3(-5.1305f, 1.4121f, 4.1589f)
};
const float ESCALA_ROVER = 0.4f;
const float RADIO_LLANTA = 1.241f * ESCALA_ROVER;	//radio de la llanta en el OBJ por la escala del rover

//Posición del dado por código (flotando, como en el código base, para poder verlo desde abajo)
const glm::vec3 POS_DADO(-5.5f, 6.0f, 6.0f);

//Ejercicio 9: animación del rover. El piso va de -10 a 10 y se escala 30 en X y Z, así que su orilla está en ±300.
//En el OBJ el rover va de x = -6.604 (atrás) a x = 6.809 (frente); con la escala, su centro se detiene antes
//de la orilla para que sea el frente (o la parte trasera) el que la toque
const float INICIO_ROVER_X = 1.0f;		//posición del rover en el código base
const float ORILLA_PISO = 10.0f * 30.0f;
const float LIMITE_FRENTE_ROVER = ORILLA_PISO - 6.809f * ESCALA_ROVER;
const float LIMITE_ATRAS_ROVER = -ORILLA_PISO + 6.604f * ESCALA_ROVER;
const float VELOCIDAD_ROVER = 0.35f;
float posRoverX = INICIO_ROVER_X;
float sentidoRover = 1.0f;				//1: avanza hacia +X (su frente), -1: retrocede

//Ejercicio 9: circuito del avión. Visto desde arriba es un estadio: recta hacia +X en z = centro + radio,
//media vuelta, recta hacia -X en z = centro - radio y otra media vuelta. recorridoAvion es la distancia
//avanzada sobre el circuito; solo aumenta, por eso el avión nunca vuela de reversa
const glm::vec3 CENTRO_CIRCUITO(0.0f, 10.0f, -2.0f);
const float MEDIA_RECTA = 12.0f;		//cada recta va de x = -12 a x = 12 respecto al centro
const float RADIO_VUELTA = 8.0f;
const float LARGO_RECTA = 2.0f * MEDIA_RECTA;
const float LARGO_VUELTA = 3.14159265f * RADIO_VUELTA;
const float LARGO_CIRCUITO = 2.0f * LARGO_RECTA + 2.0f * LARGO_VUELTA;
const float INCLINACION_VUELTA = 25.0f;	//grados que se ladea a la mitad de cada vuelta
const float VELOCIDAD_AVION = 0.15f;
float recorridoAvion = 0.0f;

//Ejercicio de práctica 8
//Colores de la luz inferior del avión, en el orden en que se recorren con C
const glm::vec3 COLORES_LUZ_AVION[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),	//rojo
	glm::vec3(1.0f, 1.0f, 0.0f),	//amarillo
	glm::vec3(0.0f, 1.0f, 0.0f),	//verde
	glm::vec3(0.0f, 1.0f, 1.0f),	//cian
	glm::vec3(0.0f, 0.0f, 1.0f),	//azul
	glm::vec3(1.0f, 0.0f, 1.0f)		//magenta
};
//El índice del color actual, el encendido de cada luz y la posición en X del holocrón los guarda Window,
//porque cambian con el teclado (ver Window::ManejaTeclado)
//Holocrón Jedi: cubo de lado 2 con centro en su origen, armado con 8 esquinas y un núcleo.
//Las flechas izquierda / derecha lo mueven en X (Window); además gira, flota y sus esquinas se abren y se cierran
const glm::vec3 INICIO_HOLOCRON(-6.0f, 0.8f, -1.5f);
const float ALTURA_LUZ_HOLOCRON = 1.5f;	//la luz va arriba del centro, fuera del cubo
float anguloHolocron = 0.0f;			//grados, aumenta en cada cuadro
float aperturaHolocron = 0.0f;			//0: cerrado, 1: esquinas en su punto más alejado
const float SEPARACION_ESQUINAS = 0.4f;	//distancia que avanza cada esquina sobre su diagonal al abrirse
const float GIRO_ESQUINAS = 20.0f;		//grados que gira cada esquina sobre su diagonal al abrirse
glm::vec3 posHolocron = INICIO_HOLOCRON;
//Punto extremo de la pinza en coordenadas del rover (sin escala). La pinza viene en Cuerpo.obj,
//arriba del brazo; su vértice más alejado está en (2.38, 12.44, -0.94) y la luz se pone un poco afuera
const glm::vec3 PUNTA_PINZA(2.45f, 12.55f, -0.95f);

//Práctica 8: nave espacial. En el OBJ la nave mide 2 x 0.8 x 2.1, su frente es +Z y sus dos toberas están
//atrás en (±0.35, 0.25, -1.05). Motor 1 = izquierdo (+X del modelo), motor 2 = derecho (-X)
const glm::vec3 POS_NAVE(-2.0f, 4.0f, -9.0f);
const float ESCALA_NAVE = 2.0f;
const glm::vec3 TOBERA_NAVE[2] = { glm::vec3(0.35f, 0.25f, -1.05f), glm::vec3(-0.35f, 0.25f, -1.05f) };
const float LARGO_LLAMA = 0.9f;			//en unidades del modelo

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	//Las normales se calculan antes de crear las mallas; si se calculan después,
	//la GPU recibe normales (0,0,0) y el shader de iluminación no puede normalizarlas
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


//------------------------------------------------------------------
//Dado del ejercicio de clase de la práctica 6: una sola imagen con las seis caras
//acomodadas en 3 columnas x 2 filas. Cada cara tiene sus propios 4 vértices para poder
//tener sus propias coordenadas UV y su propia normal.
//
//NORMALES: con el shader del curso las normales van hacia el INTERIOR del objeto
//(así están el piso (0,-1,0) y los modelos de Model.cpp, que invierten la normal de Assimp).
//El shader calcula dot(normal, direccionDeLaLuz), donde la dirección va de la luz hacia el
//fragmento; con la normal hacia adentro ese producto es positivo cuando la cara mira a la luz.
//En el dado del código base la cara frontal tenía (0,0,1), al revés que las otras cinco,
//por eso se veía oscura desde fuera. Aquí todas se calculan igual.
//------------------------------------------------------------------
const float TEX_ANCHO = 1024.0f;
const float TEX_ALTO = 1024.0f;
const float COLUMNAS = 3.0f;
const float FILAS = 2.0f;
const float MARGEN_PX = 3.0f;	//se recorta cada celda para que el filtro lineal no tome píxeles de la cara vecina

//Las esquinas se dan en orden abajo-izq, abajo-der, arriba-der, arriba-izq vistas desde afuera.
//(col, fila) es la celda de la imagen; normalExterior es la normal hacia afuera de la cara.
void AgregarCara(std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normalExterior, int col, int fila)
{
	float u0 = col / COLUMNAS + MARGEN_PX / TEX_ANCHO;
	float u1 = (col + 1) / COLUMNAS - MARGEN_PX / TEX_ANCHO;
	float v1 = 1.0f - fila / FILAS - MARGEN_PX / TEX_ALTO;
	float v0 = 1.0f - (fila + 1) / FILAS + MARGEN_PX / TEX_ALTO;

	glm::vec3 n = -normalExterior;	//normal hacia el interior para el shader de iluminación
	unsigned int base = (unsigned int)(vertices.size() / 8);
	glm::vec3 p[4] = { p0, p1, p2, p3 };
	float uv[4][2] = { {u0, v0}, {u1, v0}, {u1, v1}, {u0, v1} };
	for (int i = 0; i < 4; i++)
		//	x, y, z,  u, v,  nx, ny, nz
		vertices.insert(vertices.end(), { p[i].x, p[i].y, p[i].z, uv[i][0], uv[i][1], n.x, n.y, n.z });

	indices.insert(indices.end(), { base, base + 1, base + 2,   base, base + 2, base + 3 });
}

void CrearDado() //dado de lado 2 centrado en el origen (24 vértices, 36 índices); caras opuestas suman 7
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;

	// Frente (+Z): cara 1
	AgregarCara(v, idx, { -1,-1, 1 }, { 1,-1, 1 }, { 1, 1, 1 }, { -1, 1, 1 }, { 0, 0, 1 }, 0, 0);
	// Derecha (+X): cara 2
	AgregarCara(v, idx, { 1,-1, 1 }, { 1,-1,-1 }, { 1, 1,-1 }, { 1, 1, 1 }, { 1, 0, 0 }, 1, 0);
	// Arriba (+Y): cara 3
	AgregarCara(v, idx, { -1, 1, 1 }, { 1, 1, 1 }, { 1, 1,-1 }, { -1, 1,-1 }, { 0, 1, 0 }, 2, 0);
	// Abajo (-Y): cara 4
	AgregarCara(v, idx, { -1,-1,-1 }, { 1,-1,-1 }, { 1,-1, 1 }, { -1,-1, 1 }, { 0,-1, 0 }, 0, 1);
	// Izquierda (-X): cara 5
	AgregarCara(v, idx, { -1,-1,-1 }, { -1,-1, 1 }, { -1, 1, 1 }, { -1, 1,-1 }, { -1, 0, 0 }, 1, 1);
	// Atrás (-Z): cara 6
	AgregarCara(v, idx, { 1,-1,-1 }, { -1,-1,-1 }, { -1, 1,-1 }, { 1, 1,-1 }, { 0, 0,-1 }, 2, 1);

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	meshListModel.push_back(dado);	//meshListModel[4]
}


//Llama de plasma de un motor: tres planos que se cruzan sobre el eje -Z (a 0°, 60° y 120°), de ancho 1 y largo 1.
//v = 0 en la tobera y v = 1 en la punta, igual que en la textura fuego_plasma.png. Al verse desde cualquier lado
//siempre hay un plano casi de frente, así la llama parece tener volumen
void CrearFuego()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	for (int k = 0; k < 3; k++)
	{
		float ang = k * 60.0f * toRadians;
		glm::vec3 lado(0.5f * cosf(ang), 0.5f * sinf(ang), 0.0f);	//mitad del ancho, perpendicular al eje
		glm::vec3 n(-sinf(ang), cosf(ang), 0.0f);					//normal del plano
		glm::vec3 p[4] = { -lado, lado, lado + glm::vec3(0, 0, -1), -lado + glm::vec3(0, 0, -1) };
		float uv[4][2] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
		unsigned int base = (unsigned int)(v.size() / 8);
		for (int i = 0; i < 4; i++)
			v.insert(v.end(), { p[i].x, p[i].y, p[i].z, uv[i][0], uv[i][1], n.x, n.y, n.z });
		idx.insert(idx.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });
	}
	MeshModel* fuego = new MeshModel();
	fuego->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	meshListModel.push_back(fuego);	//meshListModel[5]
}


//Ejercicio 9: punto del circuito del avión a una distancia s del inicio (s puede ser mayor que el largo del
//circuito, se toma el residuo). Regresa la posición, el rumbo (giro sobre Y para que su frente, +Z del modelo,
//apunte en la dirección del movimiento) y la inclinación lateral en grados.
//Tramos: recta hacia +X, vuelta del lado +X, recta hacia -X, vuelta del lado -X
void PuntoCircuitoAvion(float s, glm::vec3& pos, float& rumbo, float& inclinacion)
{
	s = fmod(s, LARGO_CIRCUITO);
	glm::vec3 dir;			//dirección del movimiento en el plano XZ
	inclinacion = 0.0f;
	if (s < LARGO_RECTA)
	{
		pos = CENTRO_CIRCUITO + glm::vec3(-MEDIA_RECTA + s, 0.0f, RADIO_VUELTA);
		dir = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	else if (s < LARGO_RECTA + LARGO_VUELTA)
	{
		float a = (s - LARGO_RECTA) / RADIO_VUELTA;	//ángulo recorrido en la vuelta, de 0 a pi
		pos = CENTRO_CIRCUITO + glm::vec3(MEDIA_RECTA + RADIO_VUELTA * sinf(a), 0.0f, RADIO_VUELTA * cosf(a));
		dir = glm::vec3(cosf(a), 0.0f, -sinf(a));
		inclinacion = INCLINACION_VUELTA * sinf(a);	//0 al entrar y al salir de la vuelta, máximo a la mitad
	}
	else if (s < 2.0f * LARGO_RECTA + LARGO_VUELTA)
	{
		float d = s - LARGO_RECTA - LARGO_VUELTA;
		pos = CENTRO_CIRCUITO + glm::vec3(MEDIA_RECTA - d, 0.0f, -RADIO_VUELTA);
		dir = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	else
	{
		float a = (s - 2.0f * LARGO_RECTA - LARGO_VUELTA) / RADIO_VUELTA;
		pos = CENTRO_CIRCUITO + glm::vec3(-MEDIA_RECTA - RADIO_VUELTA * sinf(a), 0.0f, -RADIO_VUELTA * cosf(a));
		dir = glm::vec3(-cosf(a), 0.0f, sinf(a));
		inclinacion = INCLINACION_VUELTA * sinf(a);
	}
	//girar sobre Y un ángulo r lleva +Z a (sen r, 0, cos r)
	rumbo = atan2f(dir.x, dir.z);
}


//------------------------------------------------------------------
//Capturas para el reporte. Con --capturas lista.txt el programa recorre la lista dentro de
//la misma ventana (una línea por captura: archivo.ppm y opciones) y luego sigue normal.
//Opciones: --camara x y z yaw pitch, --rover x, --sentido 1|-1 (rover), --recorrido s (avión), --linterna 0|1,
//          --puntual con lin exp (luz roja), --spot con lin exp (luz verde fija),
//          --color 0..5 (luz del avión), --holocron 0|1, --pinza 0|1, --holocronx x, --holocronang grados,
//          --apertura 0..1 (esquinas del holocrón), --nave 0|1, --motor1 0|1, --motor2 0|1
//------------------------------------------------------------------
struct Captura
{
	std::string archivo;
	std::vector<std::string> opciones;
};
Camera camaraInicial;
PointLight luzRojaInicial;
SpotLight luzVerdeInicial;

void RestaurarEscena()
{
	camera = camaraInicial;
	posRoverX = INICIO_ROVER_X;
	sentidoRover = 1.0f;
	recorridoAvion = 0.0f;
	anguloHolocron = 0.0f;
	aperturaHolocron = 0.0f;
	pointLights[0] = luzRojaInicial;
	spotLights[1] = luzVerdeInicial;
	mainWindow.reiniciarControles();
}

void AplicarOpciones(const std::vector<std::string>& op)
{
	for (size_t i = 0; i < op.size(); i++)
	{
		auto num = [&](size_t k) { return (float)atof(op[k].c_str()); };
		if (op[i] == "--camara" && i + 5 < op.size())
		{
			camera = Camera(glm::vec3(num(i + 1), num(i + 2), num(i + 3)), glm::vec3(0.0f, 1.0f, 0.0f), num(i + 4), num(i + 5), 0.3f, 0.5f);
			i += 5;
		}
		else if (op[i] == "--rover" && i + 1 < op.size()) posRoverX = num(++i);
		else if (op[i] == "--sentido" && i + 1 < op.size()) sentidoRover = num(++i);
		else if (op[i] == "--recorrido" && i + 1 < op.size()) recorridoAvion = num(++i);
		else if (op[i] == "--holocronx" && i + 1 < op.size()) mainWindow.setHolocronX(num(++i));
		else if (op[i] == "--holocronang" && i + 1 < op.size()) anguloHolocron = num(++i);
		else if (op[i] == "--apertura" && i + 1 < op.size()) aperturaHolocron = num(++i);
		else if (op[i] == "--linterna" && i + 1 < op.size()) mainWindow.setLinterna(num(++i) != 0.0f);
		else if (op[i] == "--color" && i + 1 < op.size()) mainWindow.setColorAvion((int)num(++i) % 6);
		else if (op[i] == "--holocron" && i + 1 < op.size()) mainWindow.setLuzHolocron(num(++i) != 0.0f);
		else if (op[i] == "--pinza" && i + 1 < op.size()) mainWindow.setLuzPinza(num(++i) != 0.0f);
		else if (op[i] == "--nave" && i + 1 < op.size()) mainWindow.setLucesNave(num(++i) != 0.0f);
		else if (op[i] == "--motor1" && i + 1 < op.size()) mainWindow.setMotor(0, num(++i) != 0.0f);
		else if (op[i] == "--motor2" && i + 1 < op.size()) mainWindow.setMotor(1, num(++i) != 0.0f);
		else if (op[i] == "--puntual" && i + 3 < op.size())
		{
			pointLights[0] = PointLight(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -6.0f, 1.5f, 1.5f, num(i + 1), num(i + 2), num(i + 3));
			i += 3;
		}
		else if (op[i] == "--spot" && i + 3 < op.size())
		{
			spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f, 1.0f, 2.0f, 5.0f, 10.0f, 0.0f, 0.0f, -5.0f, 0.0f,
				num(i + 1), num(i + 2), num(i + 3), 15.0f);
			i += 3;
		}
	}
}

std::vector<Captura> LeerListaCapturas(const char* archivo)
{
	std::vector<Captura> lista;
	std::ifstream entrada(archivo);
	std::string linea;
	while (std::getline(entrada, linea))
	{
		std::istringstream palabras(linea);
		Captura c;
		if (!(palabras >> c.archivo) || c.archivo[0] == '#') continue;
		std::string p;
		while (palabras >> p) c.opciones.push_back(p);
		lista.push_back(c);
	}
	printf("Capturas en la lista: %zu\n", lista.size());
	return lista;
}

//Guarda el cuadro actual en un archivo PPM
void GuardarCaptura(const char* archivo)
{
	int ancho = (int)mainWindow.getBufferWidth();
	int alto = (int)mainWindow.getBufferHeight();
	std::vector<unsigned char> pixeles(ancho * alto * 4);
	glReadPixels(0, 0, ancho, alto, GL_RGBA, GL_UNSIGNED_BYTE, pixeles.data());
	std::ofstream salida(archivo, std::ios::binary);
	salida << "P6\n" << ancho << " " << alto << "\n255\n";
	for (int y = alto - 1; y >= 0; y--)	//OpenGL entrega las filas de abajo hacia arriba
		for (int x = 0; x < ancho; x++)
			salida.write(reinterpret_cast<char*>(&pixeles[(y * ancho + x) * 4]), 3);
	printf("Captura %s %dx%d, glGetError = %u\n", archivo, ancho, alto, glGetError());
}


int main(int argc, char** argv)
{
#ifdef __APPLE__
	//Al abrir la .app desde Finder el directorio de trabajo no es el del programa:
	//se busca la carpeta donde están shaders, Models y Textures
	char ejecutable[4096]; uint32_t tam = sizeof(ejecutable);
	if (_NSGetExecutablePath(ejecutable, &tam) == 0 && !std::filesystem::exists(vShader))
	{
		auto base = std::filesystem::weakly_canonical(ejecutable).parent_path();
		if (std::filesystem::exists(base / "shaders")) std::filesystem::current_path(base);
		else if (std::filesystem::exists(base / "../Resources/shaders")) std::filesystem::current_path(base / "../Resources");
	}
#endif
	setvbuf(stdout, NULL, _IONBF, 0);	//los mensajes se escriben de inmediato en el registro
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	printf("OpenGL %s / GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	while (glGetError() != GL_NO_ERROR) {}	//GLEW deja un error de enumeración en perfil Core

	CreateObjects();
	CrearDado();
	CrearFuego();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 6.0f, 24.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -12.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	fuegoTexture = Texture("Textures/fuego_plasma.png");
	fuegoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado_starwars.png");
	dadoTexture.LoadTextureA();

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	//Rover propio con jerarquía (práctica 5)
	Cuerpo_M.LoadModel("Models/Cuerpo.obj");
	Brazo_M.LoadModel("Models/Brazo.obj");
	const char* nombresPiezas[6] = { "Delantera_Derecha", "Delantera_Izquierda", "Media_Derecha",
		"Media_Izquierda", "Trasera_Derecha", "Trasera_Izquierda" };
	for (int i = 0; i < 6; i++)
	{
		Pata_M[i].LoadModel(std::string("Models/Pata_") + nombresPiezas[i] + ".obj");
		Llanta_M[i].LoadModel(std::string("Models/Llanta_") + nombresPiezas[i] + ".obj");
	}

	//Holocrón Jedi: esquina (octante del cubo) y núcleo, 24 vértices y 12 triángulos cada uno, misma textura
	HolocronEsquina_M.LoadModel("Models/holocron_esquina.obj");
	HolocronNucleo_M.LoadModel("Models/holocron_nucleo.obj");
	Avion_M.LoadModel("Models/avion_verde.obj");

	Lampara_M.LoadModel("Models/lampara.obj");
	//Nave: craft_speederA de Kenney con coordenadas UV agregadas y cuatro texturas (ver _trabajo/nave_uv.py)
	Nave_M.LoadModel("Models/nave_speeder.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//Lámpara: se para junto al carril del rover, girada 180° para que su brazo quede sobre el carril.
	//La luz puntual blanca va justo debajo de la pantalla de la lámpara.
	glm::mat4 modeloLampara(1.0);
	modeloLampara = glm::translate(modeloLampara, glm::vec3(-16.0f, -1.0f, 2.6f));
	modeloLampara = glm::rotate(modeloLampara, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	modeloLampara = glm::scale(modeloLampara, glm::vec3(8.0f, 8.0f, 8.0f));
	glm::vec3 posFocoLampara = glm::vec3(modeloLampara * glm::vec4(0.0f, 0.62f, -0.175f, 1.0f));
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		posFocoLampara.x, posFocoLampara.y, posFocoLampara.z,
		1.0f, 0.09f, 0.02f);
	pointLightCount++;

	//luz del holocrón: azul claro, justo encima de su cara superior; se mueve con el holocrón en cada cuadro.
	//Dentro del cubo no iluminaría al holocrón, porque sus caras quedarían de espaldas a la luz
	pointLights[2] = PointLight(0.3f, 0.6f, 1.0f,
		0.0f, 1.0f,
		INICIO_HOLOCRON.x, INICIO_HOLOCRON.y + ALTURA_LUZ_HOLOCRON, INICIO_HOLOCRON.z,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//luz de la pinza del rover: verde; su posición se actualiza en cada cuadro con la del rover
	pointLights[3] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//luz puntual de la nave: baliza blanca cálida sobre la cabina; su posición se actualiza en cada cuadro
	pointLights[4] = PointLight(1.0f, 0.75f, 0.4f,
		0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//luces de los motores: azules, en el centro de cada llama. La intensidad ambiental (0.6) hace que la
	//llama se vea encendida aunque la luz quede en su mismo plano; la atenuación la limita a los alrededores
	for (int m = 0; m < 2; m++)
	{
		pointLights[5 + m] = PointLight(0.2f, 0.45f, 1.0f,
			0.6f, 2.0f,
			0.0f, 0.0f, 0.0f,
			1.0f, 0.22f, 0.2f);
		pointLightCount++;
	}

	unsigned int spotLightCount = 0;
	//linterna: el cono se abrió de 5° a 12° para que ilumine una cara completa del dado o del holocrón
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		12.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz del avión: amarilla, en la parte inferior del fuselaje y apuntando al piso;
	//su posición se actualiza en cada cuadro con la altura del avión
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,
		0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.1f, 0.01f,
		25.0f);
	spotLightCount++;

	//faro del rover: azul, en el frente del cuerpo y apuntando hacia adelante (+X), un poco al piso;
	//su posición se actualiza en cada cuadro con la posición del rover
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, -0.2f, 0.0f,
		1.0f, 0.05f, 0.005f,
		25.0f);
	spotLightCount++;

	//faro delantero del avión: naranja, delante de la hélice y apuntando hacia adelante (+Z),
	//un poco hacia el piso; en cada cuadro se coloca y se orienta con la matriz del avión
	spotLights[4] = SpotLight(1.0f, 0.5f, 0.0f,
		0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 1.0f,
		1.0f, 0.05f, 0.005f,
		20.0f);
	spotLightCount++;

	//faro de la nave: blanco, en la nariz y apuntando hacia adelante y hacia el piso (45°);
	//posición y dirección se calculan en cada cuadro con la matriz de la nave
	spotLights[5] = SpotLight(1.0f, 0.95f, 0.8f,
		0.0f, 4.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		22.0f);
	spotLightCount++;

	//Estado inicial para volver a él después de cada captura
	camaraInicial = camera;
	luzRojaInicial = pointLights[0];
	luzVerdeInicial = spotLights[1];
	std::vector<Captura> capturas;
	bool salirAlTerminar = false;
	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "--capturas" && i + 1 < argc) capturas = LeerListaCapturas(argv[++i]);
		else if (std::string(argv[i]) == "--salir") salirAlTerminar = true;
	}
	size_t capturaActual = 0;
	int cuadrosCaptura = 0;
	if (!capturas.empty())
	{
		//En macOS glfwSwapBuffers puede quedarse esperando la sincronía vertical si la ventana
		//queda tapada; mientras se toman las capturas no se espera la sincronía
		glfwSwapInterval(0);
		AplicarOpciones(capturas[0].opciones);
	}
	//Variables que se usan en cada cuadro: se declaran una sola vez aquí y dentro del while solo se les asigna valor
	GLfloat now = 0.0f;
	bool capturando = false;
	glm::vec3 lowerLight, posFaroRover, posLuzPinza;
	glm::vec3 posAvion, posLuzAvion, posFaroAvion, dirFaroAvion, dirFaroNave;
	float rumboAvion = 0.0f, inclinacionAvion = 0.0f, giroLlantas = 0.0f, parpadeo = 1.0f;
	glm::mat4 baseRover(1.0), baseAvion(1.0), baseNave(1.0), baseHolocron(1.0);
	glm::mat4 model(1.0), modelaux(1.0);
	glm::vec3 color(1.0f, 1.0f, 1.0f), signo, diagonal;
	PointLight puntualesEncendidas[MAX_POINT_LIGHTS];
	SpotLight spotEncendidas[MAX_SPOT_LIGHTS];
	unsigned int numPuntuales = 0, numSpot = 0;
	int i, e, m;	//contadores de los ciclos for del while

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		capturando = capturaActual < capturas.size();

		//Recibir eventos del usuario
		glfwPollEvents();
		if (!capturando)	//mientras se toman las capturas la vista depende solo de la lista
		{
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

			//Ejercicio 9, rover: avanza o retrocede según sentidoRover; al llegar a una orilla del piso
			//se deja justo en el límite y se invierte el sentido
			posRoverX += sentidoRover * VELOCIDAD_ROVER * deltaTime;
			if (posRoverX >= LIMITE_FRENTE_ROVER)
			{
				posRoverX = LIMITE_FRENTE_ROVER;
				sentidoRover = -1.0f;
			}
			else if (posRoverX <= LIMITE_ATRAS_ROVER)
			{
				posRoverX = LIMITE_ATRAS_ROVER;
				sentidoRover = 1.0f;
			}
			//Ejercicio 9, avión: solo avanza sobre el circuito; fmod evita que el número crezca sin límite
			recorridoAvion = fmod(recorridoAvion + VELOCIDAD_AVION * deltaTime, LARGO_CIRCUITO);
			//giro continuo sobre Y (en las capturas se deja fijo para que la vista no dependa del tiempo)
			anguloHolocron = fmod(anguloHolocron + 0.5f * deltaTime, 720.0f);
			//las esquinas se abren y se cierran solas (un ciclo cada ~8 s)
			aperturaHolocron = 0.5f - 0.5f * cosf(now * 0.8f);
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//Matrices base del rover y del avión: se usan para dibujarlos y para colocar sus luces,
		//así la luz siempre queda en el mismo punto del modelo aunque este se mueva
		baseRover = glm::mat4(1.0);
		baseRover = glm::translate(baseRover, glm::vec3(posRoverX, -1.0f, 6.0f));
		baseRover = glm::scale(baseRover, glm::vec3(ESCALA_ROVER, ESCALA_ROVER, ESCALA_ROVER));
		posFaroRover = glm::vec3(baseRover * glm::vec4(6.9f, 2.3f, 0.0f, 1.0f));	//frente del cuerpo
		spotLights[3].SetPos(posFaroRover);
		posLuzPinza = glm::vec3(baseRover * glm::vec4(PUNTA_PINZA, 1.0f));
		pointLights[3].SetPos(posLuzPinza);

		//Holocrón: posición del cuadro (X por teclado, Y flotando) y su luz encima
		posHolocron = glm::vec3(mainWindow.getHolocronX(), INICIO_HOLOCRON.y + (capturando ? 0.0f : 0.25f * sinf(now * 1.5f)), INICIO_HOLOCRON.z);
		//la luz sube un poco cuando el holocrón está abierto para no quedar dentro de las esquinas de arriba
		pointLights[2].SetPos(posHolocron + glm::vec3(0.0f, ALTURA_LUZ_HOLOCRON + 0.6f * aperturaHolocron, 0.0f));

		//Ejercicio 9: el avión se coloca en su punto del circuito, se gira sobre Y con el rumbo y se ladea sobre
		//su eje largo (Z del modelo). Las vueltas son a la izquierda y el ala izquierda es +X del modelo, por eso
		//el ángulo es negativo: esa ala baja, hacia el centro de la vuelta
		PuntoCircuitoAvion(recorridoAvion, posAvion, rumboAvion, inclinacionAvion);
		baseAvion = glm::mat4(1.0);
		baseAvion = glm::translate(baseAvion, posAvion);
		baseAvion = glm::rotate(baseAvion, rumboAvion, glm::vec3(0.0f, 1.0f, 0.0f));
		baseAvion = glm::rotate(baseAvion, -inclinacionAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		baseAvion = glm::scale(baseAvion, glm::vec3(0.75f, 0.75f, 0.75f));
		posLuzAvion = glm::vec3(baseAvion * glm::vec4(0.0f, -1.05f, 0.5f, 1.0f));	//debajo del fuselaje
		spotLights[2].SetPos(posLuzAvion);
		spotLights[2].SetColor(COLORES_LUZ_AVION[mainWindow.getColorAvion()]);
		//el faro apunta hacia el frente del avión y un poco hacia abajo; su dirección se gira con el avión
		posFaroAvion = glm::vec3(baseAvion * glm::vec4(0.0f, 0.0f, 3.4f, 1.0f));	//delante de la hélice
		dirFaroAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(0.0f, -0.5f, 1.0f, 0.0f)));
		spotLights[4].SetFlash(posFaroAvion, dirFaroAvion);

		//Nave: flota subiendo y bajando y se inclina un poco; está girada -90° en Y, así que su frente
		//apunta hacia -X y las llamas salen hacia +X. Las luces se colocan con su misma matriz
		baseNave = glm::mat4(1.0);
		baseNave = glm::translate(baseNave, POS_NAVE + glm::vec3(0.0f, capturando ? 0.0f : 0.3f * sinf(now * 1.2f), 0.0f));
		baseNave = glm::rotate(baseNave, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		baseNave = glm::rotate(baseNave, (capturando ? 0.0f : 4.0f * sinf(now * 0.9f)) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		baseNave = glm::scale(baseNave, glm::vec3(ESCALA_NAVE));
		pointLights[4].SetPos(glm::vec3(baseNave * glm::vec4(0.0f, 1.1f, 0.3f, 1.0f)));	//sobre la cabina
		dirFaroNave = glm::normalize(glm::vec3(baseNave * glm::vec4(0.0f, -1.0f, 1.0f, 0.0f)));
		spotLights[5].SetFlash(glm::vec3(baseNave * glm::vec4(0.0f, 0.3f, 1.15f, 1.0f)), dirFaroNave);	//nariz
		for (m = 0; m < 2; m++)	//luz azul a la mitad de cada llama
			pointLights[5 + m].SetPos(glm::vec3(baseNave * glm::vec4(TOBERA_NAVE[m] + glm::vec3(0.0f, 0.0f, -0.5f * LARGO_LLAMA), 1.0f)));

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//Solo se envían las luces puntuales encendidas: el shader recorre las primeras pointLightCount
		//del arreglo, así que se arma uno nuevo sin las que estén apagadas
		numPuntuales = 0;
		puntualesEncendidas[numPuntuales++] = pointLights[0];	//roja
		puntualesEncendidas[numPuntuales++] = pointLights[1];	//lámpara
		if (mainWindow.getLuzHolocron()) puntualesEncendidas[numPuntuales++] = pointLights[2];
		if (mainWindow.getLuzPinza()) puntualesEncendidas[numPuntuales++] = pointLights[3];
		if (mainWindow.getLucesNave()) puntualesEncendidas[numPuntuales++] = pointLights[4];
		if (mainWindow.getMotor(0)) puntualesEncendidas[numPuntuales++] = pointLights[5];
		if (mainWindow.getMotor(1)) puntualesEncendidas[numPuntuales++] = pointLights[6];
		shaderList[0].SetPointLights(puntualesEncendidas, numPuntuales);
		//Igual con las spotlight: la linterna (0) y el faro de la nave (5) solo se mandan si están encendidos
		numSpot = 0;
		if (mainWindow.getLinterna()) spotEncendidas[numSpot++] = spotLights[0];
		for (i = 1; i <= 4; i++) spotEncendidas[numSpot++] = spotLights[i];
		if (mainWindow.getLucesNave()) spotEncendidas[numSpot++] = spotLights[5];
		shaderList[0].SetSpotLights(spotEncendidas, numSpot);



		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();

		//BLACKHAWK
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel(uniformColor);

		//Rover de la práctica 5 con jerarquía: cuerpo -> pata -> llanta
		//Giro de las llantas = distancia recorrida / radio; negativo porque avanza hacia +X
		//y la llanta gira sobre su eje Z. Al retroceder posRoverX disminuye y las llantas giran al revés
		giroLlantas = -(posRoverX - INICIO_ROVER_X) / RADIO_LLANTA;
		model = baseRover;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cuerpo_M.RenderModel(uniformColor);
		Brazo_M.RenderModel(uniformColor);
		for (i = 0; i < 6; i++)
		{
			//pata: gira sobre su punto de unión con el cuerpo (en la práctica 5 se movía con teclado)
			modelaux = model;
			modelaux = glm::translate(modelaux, hingePata[i]);
			modelaux = glm::rotate(modelaux, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			modelaux = glm::translate(modelaux, -hingePata[i]);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
			Pata_M[i].RenderModel(uniformColor);
			//llanta: hija de la pata, se lleva a su centro y ahí gira
			modelaux = glm::translate(modelaux, centroLlanta[i]);
			modelaux = glm::rotate(modelaux, giroLlantas, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
			Llanta_M[i].RenderModel(uniformColor);
		}

		//Avión de la práctica 6: recorre su circuito con sus dos luces
		model = baseAvion;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion_M.RenderModel(uniformColor);

		//LÁMPARA
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modeloLampara));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara_M.RenderModel(uniformColor);

		//Holocrón Jedi: se traslada a su posición del cuadro y gira sobre Y y, más despacio, sobre X,
		//para que con el tiempo se vean sus seis caras
		baseHolocron = glm::mat4(1.0);
		baseHolocron = glm::translate(baseHolocron, posHolocron);
		baseHolocron = glm::rotate(baseHolocron, anguloHolocron * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		baseHolocron = glm::rotate(baseHolocron, 0.5f * anguloHolocron * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Núcleo: gira al revés que el holocrón; solo se ve cuando las esquinas se separan
		model = glm::rotate(baseHolocron, -2.0f * anguloHolocron * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HolocronNucleo_M.RenderModel(uniformColor);
		//Esquinas: la misma malla (octante +X +Y +Z) reflejada con escalas de -1 hacia los otros siete octantes.
		//Cada una se aleja del centro sobre su diagonal y gira sobre ella según la apertura
		for (e = 0; e < 8; e++)
		{
			signo = glm::vec3((e & 1) ? -1.0f : 1.0f, (e & 2) ? -1.0f : 1.0f, (e & 4) ? -1.0f : 1.0f);
			diagonal = glm::normalize(signo);
			model = glm::translate(baseHolocron, diagonal * SEPARACION_ESQUINAS * aperturaHolocron);
			model = glm::rotate(model, GIRO_ESQUINAS * aperturaHolocron * toRadians, diagonal);
			model = glm::scale(model, signo);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			HolocronEsquina_M.RenderModel(uniformColor);
		}

		//Dado por código
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, POS_DADO);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();



		//Agave ¿qué sucede si lo renderizan antes del helicóptero?
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);

		//Nave espacial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(baseNave));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel(uniformColor);

		//Llamas de los motores: se dibujan al final, con mezcla aditiva (se suman a lo que hay detrás, como una
		//luz) y sin escribir en el buffer de profundidad para que los tres planos no se tapen entre sí.
		//El largo varía un poco en cada cuadro para que parpadee
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		color = glm::vec3(1.0f, 1.0f, 1.0f);	//la textura ya trae el brillo; la luz azul del motor la aviva
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		fuegoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (m = 0; m < 2; m++)
		{
			if (!mainWindow.getMotor(m)) continue;
			parpadeo = capturando ? 1.0f : 1.0f + 0.12f * sinf(now * 23.0f + 2.0f * m);
			model = glm::translate(baseNave, TOBERA_NAVE[m]);
			model = glm::scale(model, glm::vec3(0.24f, 0.24f, LARGO_LLAMA * parpadeo));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[5]->RenderMeshModel();
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Capturas: se guarda el tercer cuadro de cada vista y se pasa a la siguiente
		if (capturando && ++cuadrosCaptura == 3)
		{
			GuardarCaptura(capturas[capturaActual].archivo.c_str());
			cuadrosCaptura = 0;
			capturaActual++;
			RestaurarEscena();
			if (capturaActual < capturas.size())
				AplicarOpciones(capturas[capturaActual].opciones);
			else if (salirAlTerminar)
				break;
			else
				glfwSwapInterval(1);	//termina la lista: se vuelve al modo normal
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
