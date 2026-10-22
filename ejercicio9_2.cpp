/*
Ejercicio de Práctica 9-2: Animación Compleja
Ricardo Emmanuel Galicia Tequianes - 118001740

Parte del código de la práctica 9 (nave en la pista de carreras y satélite en ocho). El avión ya no da vueltas en
el circuito del ejercicio 9: ahora ejecuta un programa de acrobacias. El programa es un arreglo de maniobras
(recta, rizo, tonel, medio rizo, medio tonel y tonel volado) que se recorre en orden; cada tipo de maniobra es una
función que mueve la posición del avión y gira su frente y su techo. Con medio rizo y medio tonel se arman un
Immelmann (sube y regresa en sentido contrario) y una Split-S (baja y regresa), así el programa termina donde
empezó y se repite sin saltos. Atrás del avión queda una estela de humo, que son sus posiciones anteriores.
El punto 2 del ejercicio (textura animada) se explicará la siguiente clase.

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
//Práctica 9: texturas de la pista y de los muros de su parte elevada (herramientas/texturas_pista.py)
Texture pistaTexture;
Texture muroTexture;


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

//Ejercicio 9-2: programa de acrobacias del avión. Cada maniobra parte de la posición y orientación con que
//terminó la anterior. largo es lo que avanza sobre su eje (recta, toneles y tonel volado) y radio el de los rizos
//y el del tonel volado. Todo el programa queda en el plano z = 16, delante de la escena
enum TipoManiobra { RECTA, RIZO, MEDIO_RIZO, TONEL, MEDIO_TONEL, TONEL_VOLADO };
struct Maniobra
{
	TipoManiobra tipo;
	float largo;
	float radio;
};
const Maniobra PROGRAMA_ACROBACIAS[] = {
	{ RECTA, 8.0f, 0.0f },
	{ RIZO, 0.0f, 8.0f },			//rizo completo hacia arriba
	{ RECTA, 6.0f, 0.0f },
	{ TONEL, 14.0f, 0.0f },			//una vuelta completa sobre su eje largo
	{ RECTA, 10.0f, 0.0f },
	{ MEDIO_RIZO, 0.0f, 9.0f },		//Immelmann: medio rizo hacia arriba (queda de cabeza y de regreso)...
	{ MEDIO_TONEL, 6.0f, 0.0f },	//...y medio tonel para quedar derecho
	{ RECTA, 3.0f, 0.0f },
	{ TONEL_VOLADO, 20.0f, 2.5f },	//vuelta alrededor de una línea que va por encima del avión
	{ RECTA, 3.0f, 0.0f },
	{ MEDIO_TONEL, 6.0f, 0.0f },	//Split-S: medio tonel para quedar de cabeza...
	{ MEDIO_RIZO, 0.0f, 9.0f }		//...y medio rizo hacia abajo, de regreso al inicio
};
const int NUM_MANIOBRAS = sizeof(PROGRAMA_ACROBACIAS) / sizeof(PROGRAMA_ACROBACIAS[0]);
const glm::vec3 INICIO_ACROBACIAS(-19.0f, 5.0f, 16.0f);	//el avión empieza derecho y hacia +X
float largoManiobra[NUM_MANIOBRAS];		//se calculan una vez al inicio (PrepararAcrobacias)
float largoPrograma = 0.0f;
const float VELOCIDAD_AVION = 0.25f;
float recorridoAvion = 0.0f;			//distancia avanzada sobre el programa; solo aumenta
//Estela de humo: esferas en las posiciones por donde pasó la cola del avión
const int PUNTOS_ESTELA = 70;
const float SEPARACION_ESTELA = 0.6f;	//distancia sobre el programa entre una esfera y la siguiente
const float TAMANO_ESTELA = 0.35f;		//radio de la primera esfera; las demás se hacen más chicas
const float COLA_AVION = 2.3f;			//en el OBJ la cola está en z = -3.1, por la escala 0.75

//Práctica 9: pista de carreras de la nave. Visto desde arriba es un óvalo: recta 1 hacia +X en z = centro + radio,
//curva 1 del lado +X, recta 2 hacia -X en z = centro - radio y curva 2. La recta 1 tiene, en este orden, un tramo
//plano, la rampa de subida, el tramo elevado, la rampa de bajada y otro tramo plano.
//recorridoNave es la distancia avanzada sobre la pista; solo aumenta, por eso la nave siempre va hacia adelante
const glm::vec3 CENTRO_PISTA(0.0f, -0.97f, -22.0f);	//un poco arriba del piso (y = -1) para que no parpadeen
const float MEDIA_RECTA_PISTA = 25.0f;		//cada recta va de x = -25 a x = 25
const float RADIO_CURVA_PISTA = 10.0f;
const float ANCHO_PISTA = 7.0f;
const float LARGO_RECTA_PISTA = 2.0f * MEDIA_RECTA_PISTA;
const float LARGO_CURVA_PISTA = 3.14159265f * RADIO_CURVA_PISTA;
const float LARGO_PISTA = 2.0f * LARGO_RECTA_PISTA + 2.0f * LARGO_CURVA_PISTA;
const float INICIO_SUBIDA = 7.0f;			//distancia desde el inicio de la recta 1
const float LARGO_RAMPA = 12.0f;
const float LARGO_ELEVADO = 12.0f;
const float ALTURA_RAMPA = 4.0f;
const float INICIO_BAJADA = INICIO_SUBIDA + LARGO_RAMPA + LARGO_ELEVADO;
const float INCLINACION_CURVA_NAVE = 20.0f;	//grados que se ladea la nave a la mitad de cada curva
const float ALTURA_NAVE_PISTA = 0.4f;		//la nave flota un poco arriba de la pista
const float VELOCIDAD_NAVE = 0.3f;
float recorridoNave = 0.0f;

//Práctica 9: satélite (la sonda espacial de la práctica 4). Vuela sobre la escena siguiendo un ocho:
//x = A·sen t, z = B·sen t·cos t, y además sube y baja con cos t. anguloSatelite es el parámetro t
const glm::vec3 CENTRO_SATELITE(0.0f, 16.0f, -10.0f);
const float ANCHO_OCHO = 26.0f;			//A: el ocho va de x = -26 a x = 26
const float FONDO_OCHO = 24.0f;			//B: en z va de -12 a 12 respecto al centro
const float ONDA_SATELITE = 1.5f;
const float VELOCIDAD_SATELITE = 0.004f;	//radianes de t por cuadro
const float ESCALA_SATELITE = 1.0f;		//en la práctica 4 se escalaba 2.2 para verla de cerca
float anguloSatelite = 0.0f;
//Extremidades del satélite: en la práctica 4 cada una giraba con su tecla; ahora oscilan solas ±30° alrededor de
//su posición original, cada una con 90° de desfase respecto a la anterior
const float AMPLITUD_EXTREMIDADES = 30.0f;		//grados
const float VELOCIDAD_EXTREMIDADES = 0.035f;	//radianes de fase por cuadro, un ciclo cada ~3 s a 60 fps
float faseExtremidades = 0.0f;

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
//atrás en (±0.35, 0.25, -1.05). Motor 1 = izquierdo (+X del modelo), motor 2 = derecho (-X).
//En la práctica 9 su posición ya no es fija: sale de la pista (ver PuntoPista)
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


//Ejercicio 9-2: desplazamiento del tonel volado respecto a la línea recta, en los ejes derecho y arriba del avión,
//para el ángulo a de 0 a 2pi. Sale y regresa a la línea con la dirección de la línea (derivada 0 en a = 0 y en
//a = 2pi), así el avión no cambia de rumbo de golpe al entrar o salir de la maniobra
glm::vec2 DesvioTonelVolado(float a, float radio)
{
	return radio * glm::vec2(sinf(a) * (1.0f - cosf(a)), 1.0f - cosf(a));
}

//Ejercicio 9-2: avanza d unidades dentro de la maniobra k del programa. p, f y u entran con la posición, el frente y el techo
//del avión al empezar la maniobra y salen con los del punto d. r es el lado derecho del avión
void AvanzarManiobra(int k, float d, glm::vec3& p, glm::vec3& f, glm::vec3& u)
{
	const float PI = 3.14159265f;
	const Maniobra& m = PROGRAMA_ACROBACIAS[k];
	glm::vec3 r = glm::cross(f, u), f0 = f, u0 = u;
	float a, x;
	switch (m.tipo)
	{
	case RECTA:
		p += f * d;
		break;
	case RIZO:
	case MEDIO_RIZO:
		//el frente gira hacia el techo: el avión describe un círculo de radio R en el plano de f y u
		a = d / m.radio;
		p += f0 * (m.radio * sinf(a)) + u0 * (m.radio * (1.0f - cosf(a)));
		f = f0 * cosf(a) + u0 * sinf(a);
		u = u0 * cosf(a) - f0 * sinf(a);
		break;
	case TONEL:
	case MEDIO_TONEL:
		//avanza en línea recta mientras el techo gira hacia la derecha: 2pi en el tonel y pi en el medio tonel
		a = (m.tipo == TONEL ? 2.0f : 1.0f) * PI * d / m.largo;
		p += f0 * d;
		u = u0 * cosf(a) + r * sinf(a);
		break;
	case TONEL_VOLADO:
	{
		//x es lo que avanza sobre la línea y a el ángulo de la vuelta alrededor de ella
		x = d * m.largo / largoManiobra[k];
		a = 2.0f * PI * x / m.largo;
		glm::vec2 o = DesvioTonelVolado(a, m.radio);
		p += f0 * x + r * o.x + u0 * o.y;
		//frente: la derivada de la trayectoria respecto a x
		glm::vec2 dO = m.radio * glm::vec2(cosf(a) - cosf(2.0f * a), sinf(a)) * (2.0f * PI / m.largo);
		f = glm::normalize(f0 + r * dO.x + u0 * dO.y);
		//techo: apunta hacia la línea central de la vuelta (arriba del punto de entrada, a la distancia del radio)
		glm::vec3 haciaEje = u0 * m.radio - (r * o.x + u0 * o.y);
		if (glm::length(haciaEje) < 0.001f) haciaEje = u0;
		u = glm::normalize(haciaEje - glm::dot(haciaEje, f) * f);
		break;
	}
	}
}

//Ejercicio 9-2: largo de cada maniobra sobre la trayectoria. El del tonel volado se mide sumando 400 tramos
//rectos de su curva, porque no tiene una fórmula sencilla
void PrepararAcrobacias()
{
	const float PI = 3.14159265f;
	largoPrograma = 0.0f;
	for (int k = 0; k < NUM_MANIOBRAS; k++)
	{
		const Maniobra& m = PROGRAMA_ACROBACIAS[k];
		if (m.tipo == RIZO) largoManiobra[k] = 2.0f * PI * m.radio;
		else if (m.tipo == MEDIO_RIZO) largoManiobra[k] = PI * m.radio;
		else if (m.tipo == TONEL_VOLADO)
		{
			largoManiobra[k] = 0.0f;
			glm::vec3 anterior(0.0f), actual;
			for (int i = 1; i <= 400; i++)
			{
				glm::vec2 o = DesvioTonelVolado(2.0f * PI * i / 400.0f, m.radio);
				actual = glm::vec3(m.largo * i / 400.0f, o.x, o.y);
				largoManiobra[k] += glm::length(actual - anterior);
				anterior = actual;
			}
		}
		else largoManiobra[k] = m.largo;
		largoPrograma += largoManiobra[k];
	}
}

//Ejercicio 9-2: posición, frente y techo del avión a una distancia s del inicio del programa. Se parte del inicio
//y se aplican completas las maniobras que ya terminaron; la última se aplica solo hasta donde va. Como siempre se
//calcula desde el inicio, los errores de redondeo no se acumulan de una vuelta a otra
void PoseAcrobacia(float s, glm::vec3& p, glm::vec3& f, glm::vec3& u)
{
	s = fmod(s, largoPrograma);
	if (s < 0.0f) s += largoPrograma;	//la estela pide puntos de antes del inicio
	p = INICIO_ACROBACIAS;
	f = glm::vec3(1.0f, 0.0f, 0.0f);
	u = glm::vec3(0.0f, 1.0f, 0.0f);
	for (int k = 0; k < NUM_MANIOBRAS; k++)
	{
		if (s < largoManiobra[k])
		{
			AvanzarManiobra(k, s, p, f, u);
			return;
		}
		AvanzarManiobra(k, largoManiobra[k], p, f, u);
		s -= largoManiobra[k];
	}
}


//Práctica 9: altura de la recta 1 a una distancia d de su inicio y su pendiente (lo que sube por unidad que avanza).
//Cada rampa sigue medio periodo de un coseno: empieza y termina con pendiente 0, así la nave no cambia de
//inclinación de golpe al entrar o al salir de ella
float AlturaRecta1(float d, float& pendiente)
{
	float u;
	pendiente = 0.0f;
	if (d < INICIO_SUBIDA || d >= INICIO_BAJADA + LARGO_RAMPA)
		return 0.0f;						//tramos planos
	if (d < INICIO_SUBIDA + LARGO_RAMPA)	//rampa de subida
	{
		u = 3.14159265f * (d - INICIO_SUBIDA) / LARGO_RAMPA;
		pendiente = 0.5f * ALTURA_RAMPA * sinf(u) * 3.14159265f / LARGO_RAMPA;
		return 0.5f * ALTURA_RAMPA * (1.0f - cosf(u));
	}
	if (d < INICIO_BAJADA)
		return ALTURA_RAMPA;				//tramo elevado
	u = 3.14159265f * (d - INICIO_BAJADA) / LARGO_RAMPA;	//rampa de bajada
	pendiente = -0.5f * ALTURA_RAMPA * sinf(u) * 3.14159265f / LARGO_RAMPA;
	return 0.5f * ALTURA_RAMPA * (1.0f + cosf(u));
}


//Práctica 9: punto de la pista a una distancia s del inicio (s puede ser mayor que el largo de la pista, se toma el
//residuo). Regresa la posición sobre la superficie, la dirección del movimiento (unitaria; en las rampas también
//sube o baja) y la inclinación lateral de la nave en grados. Mismos tramos que el circuito del avión
void PuntoPista(float s, glm::vec3& pos, glm::vec3& dir, float& inclinacion)
{
	float pendiente = 0.0f, a, d;
	s = fmod(s, LARGO_PISTA);
	inclinacion = 0.0f;
	if (s < LARGO_RECTA_PISTA)
	{
		pos = CENTRO_PISTA + glm::vec3(-MEDIA_RECTA_PISTA + s, AlturaRecta1(s, pendiente), RADIO_CURVA_PISTA);
		dir = glm::normalize(glm::vec3(1.0f, pendiente, 0.0f));
	}
	else if (s < LARGO_RECTA_PISTA + LARGO_CURVA_PISTA)
	{
		a = (s - LARGO_RECTA_PISTA) / RADIO_CURVA_PISTA;	//ángulo recorrido en la curva, de 0 a pi
		pos = CENTRO_PISTA + glm::vec3(MEDIA_RECTA_PISTA + RADIO_CURVA_PISTA * sinf(a), 0.0f, RADIO_CURVA_PISTA * cosf(a));
		dir = glm::vec3(cosf(a), 0.0f, -sinf(a));
		inclinacion = INCLINACION_CURVA_NAVE * sinf(a);
	}
	else if (s < 2.0f * LARGO_RECTA_PISTA + LARGO_CURVA_PISTA)
	{
		d = s - LARGO_RECTA_PISTA - LARGO_CURVA_PISTA;
		pos = CENTRO_PISTA + glm::vec3(MEDIA_RECTA_PISTA - d, 0.0f, -RADIO_CURVA_PISTA);
		dir = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	else
	{
		a = (s - 2.0f * LARGO_RECTA_PISTA - LARGO_CURVA_PISTA) / RADIO_CURVA_PISTA;
		pos = CENTRO_PISTA + glm::vec3(-MEDIA_RECTA_PISTA - RADIO_CURVA_PISTA * sinf(a), 0.0f, -RADIO_CURVA_PISTA * cosf(a));
		dir = glm::vec3(-cosf(a), 0.0f, sinf(a));
		inclinacion = INCLINACION_CURVA_NAVE * sinf(a);
	}
}


//Práctica 9: malla de la pista. Se recorre el circuito cada 0.25 unidades con PuntoPista y en cada punto se ponen
//dos vértices, uno en cada orilla, a ANCHO_PISTA/2 del centro y hacia los lados de la dirección del movimiento.
//u va de 0 a 1 de una orilla a la otra y v crece con la distancia. En la recta 1, desde que empieza la subida hasta
//que termina la bajada, también se arman los dos muros laterales, de la orilla de la pista hasta el piso
void CrearPista()
{
	std::vector<GLfloat> v, vm;
	std::vector<unsigned int> idx, idxm;
	const float PASO = 0.25f;
	const int n = (int)(LARGO_PISTA / PASO);
	//número entero de repeticiones de la textura (una cada ~4 unidades) para que no se corte donde cierra la pista
	const float REPETICIONES = roundf(LARGO_PISTA / 4.0f);
	glm::vec3 pos, dir, lado, normal, orilla[2];
	float incl, s, t, pendiente;
	unsigned int base;
	for (int i = 0; i <= n; i++)
	{
		s = (i == n) ? 0.0f : i * PASO;		//el último punto es el primero, así la pista queda cerrada
		PuntoPista(s, pos, dir, incl);
		lado = glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));	//hacia la derecha de la nave
		normal = glm::cross(lado, dir);			//normal exterior de la superficie (hacia arriba)
		t = REPETICIONES * (i == n ? 1.0f : s / LARGO_PISTA);
		orilla[0] = pos - lado * (0.5f * ANCHO_PISTA);
		orilla[1] = pos + lado * (0.5f * ANCHO_PISTA);
		//la normal se guarda hacia el interior, igual que en el piso y en el dado
		v.insert(v.end(), { orilla[0].x, orilla[0].y, orilla[0].z, 0.0f, t, -normal.x, -normal.y, -normal.z });
		v.insert(v.end(), { orilla[1].x, orilla[1].y, orilla[1].z, 1.0f, t, -normal.x, -normal.y, -normal.z });
		if (i > 0)
		{
			base = 2 * (i - 1);
			idx.insert(idx.end(), { base, base + 1, base + 3, base, base + 3, base + 2 });
		}
	}
	MeshModel* pista = new MeshModel();
	pista->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	meshListModel.push_back(pista);	//meshListModel[6]

	//Muros: por cada punto, arriba y abajo de cada orilla. u avanza con la distancia; v va de 0 en el piso
	//a 1 a la altura del tramo elevado (ahí la textura tiene una franja roja y blanca)
	int k = 0;
	for (s = INICIO_SUBIDA; s <= INICIO_BAJADA + LARGO_RAMPA + 0.001f; s += PASO, k++)
	{
		PuntoPista(s, pos, dir, incl);
		AlturaRecta1(s, pendiente);
		lado = glm::vec3(0.0f, 0.0f, 1.0f);	//en la recta 1 la nave avanza hacia +X, su derecha es +Z
		for (int m = 0; m < 2; m++)
		{
			glm::vec3 arriba = pos + lado * (m == 0 ? -0.5f : 0.5f) * ANCHO_PISTA;
			glm::vec3 nExt = lado * (m == 0 ? -1.0f : 1.0f);
			vm.insert(vm.end(), { arriba.x, arriba.y, arriba.z, s / 4.0f, (arriba.y - CENTRO_PISTA.y) / ALTURA_RAMPA, -nExt.x, -nExt.y, -nExt.z });
			vm.insert(vm.end(), { arriba.x, CENTRO_PISTA.y, arriba.z, s / 4.0f, 0.0f, -nExt.x, -nExt.y, -nExt.z });
		}
		if (k > 0)
		{
			base = 4 * (k - 1);	//vértices del punto anterior: base..base+3; del actual: base+4..base+7
			for (unsigned int m = 0; m < 2; m++)
				idxm.insert(idxm.end(), { base + 2 * m, base + 2 * m + 1, base + 2 * m + 5, base + 2 * m, base + 2 * m + 5, base + 2 * m + 4 });
		}
	}
	MeshModel* muros = new MeshModel();
	muros->CreateMeshModel(vm.data(), idxm.data(), (unsigned int)vm.size(), (unsigned int)idxm.size());
	meshListModel.push_back(muros);	//meshListModel[7]
}


//Práctica 9: piezas de la sonda espacial de la práctica 4 (el satélite). Son las mismas figuras y medidas de esa
//práctica: cubo de lado 1, cilindro de radio 1 y alto 1, cono de radio 2 y alto 1, pirámide cuadrangular de lado 1 y
//esfera de radio 1, todas centradas en el origen. Allá solo tenían posición; aquí llevan UV y normales para el
//shader de iluminación, y como en AgregarCara la normal se guarda hacia el interior
void AgregarVerticeSonda(std::vector<GLfloat>& v, glm::vec3 p, float u, float t, glm::vec3 normalExterior)
{
	v.insert(v.end(), { p.x, p.y, p.z, u, t, -normalExterior.x, -normalExterior.y, -normalExterior.z });
}

void GuardarPiezaSonda(std::vector<GLfloat>& v, std::vector<unsigned int>& idx)
{
	MeshModel* pieza = new MeshModel();
	pieza->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	meshListModel.push_back(pieza);
	v.clear();
	idx.clear();
}

void CrearPiezasSonda()
{
	const float PI = 3.14159265f;
	const int RES = 24;		//lados del cilindro y del cono (en la práctica 4 el cilindro tenía 5)
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	unsigned int b;
	float ang;

	//Cubo: 4 vértices por cara para que cada cara tenga su propia normal
	const glm::vec3 normalesCubo[6] = { {0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0} };
	for (const glm::vec3& n : normalesCubo)
	{
		glm::vec3 e1 = (fabsf(n.y) > 0.5f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);	//dos ejes sobre la cara
		glm::vec3 e2 = glm::cross(n, e1);
		b = (unsigned int)(v.size() / 8);
		AgregarVerticeSonda(v, 0.5f * (n - e1 - e2), 0, 0, n);
		AgregarVerticeSonda(v, 0.5f * (n + e1 - e2), 1, 0, n);
		AgregarVerticeSonda(v, 0.5f * (n + e1 + e2), 1, 1, n);
		AgregarVerticeSonda(v, 0.5f * (n - e1 + e2), 0, 1, n);
		idx.insert(idx.end(), { b, b + 1, b + 2, b, b + 2, b + 3 });
	}
	GuardarPiezaSonda(v, idx);	//meshListModel[8]

	//Cilindro: pared con normales hacia afuera del eje y dos tapas
	for (int k = 0; k <= RES; k++)
	{
		ang = 2.0f * PI * k / RES;
		glm::vec3 n(cosf(ang), 0.0f, sinf(ang));
		AgregarVerticeSonda(v, glm::vec3(n.x, -0.5f, n.z), (float)k / RES, 0, n);
		AgregarVerticeSonda(v, glm::vec3(n.x, 0.5f, n.z), (float)k / RES, 1, n);
		if (k > 0)
		{
			b = 2 * (k - 1);
			idx.insert(idx.end(), { b, b + 1, b + 3, b, b + 3, b + 2 });
		}
	}
	for (float y : { -0.5f, 0.5f })
	{
		b = (unsigned int)(v.size() / 8);	//centro de la tapa
		AgregarVerticeSonda(v, glm::vec3(0.0f, y, 0.0f), 0.5f, 0.5f, glm::vec3(0.0f, 2.0f * y, 0.0f));
		for (int k = 0; k <= RES; k++)
		{
			ang = 2.0f * PI * k / RES;
			AgregarVerticeSonda(v, glm::vec3(cosf(ang), y, sinf(ang)), 0.5f + 0.5f * cosf(ang), 0.5f + 0.5f * sinf(ang), glm::vec3(0.0f, 2.0f * y, 0.0f));
			if (k > 0) idx.insert(idx.end(), { b, b + k, b + k + 1 });
		}
	}
	GuardarPiezaSonda(v, idx);	//meshListModel[9]

	//Cono de radio 2 (como CrearCono(25, 2.0) de la práctica 4): la normal de la pared es perpendicular a la
	//generatriz, (cos, R/alto, sen) normalizada; la punta se repite en cada lado para llevar su propia normal
	for (int k = 0; k < RES; k++)
	{
		float a0 = 2.0f * PI * k / RES, a1 = 2.0f * PI * (k + 1) / RES, am = 0.5f * (a0 + a1);
		b = (unsigned int)(v.size() / 8);
		AgregarVerticeSonda(v, glm::vec3(2.0f * cosf(a0), -0.5f, 2.0f * sinf(a0)), (float)k / RES, 0, glm::normalize(glm::vec3(cosf(a0), 2.0f, sinf(a0))));
		AgregarVerticeSonda(v, glm::vec3(2.0f * cosf(a1), -0.5f, 2.0f * sinf(a1)), (float)(k + 1) / RES, 0, glm::normalize(glm::vec3(cosf(a1), 2.0f, sinf(a1))));
		AgregarVerticeSonda(v, glm::vec3(0.0f, 0.5f, 0.0f), (k + 0.5f) / RES, 1, glm::normalize(glm::vec3(cosf(am), 2.0f, sinf(am))));
		idx.insert(idx.end(), { b, b + 1, b + 2 });
	}
	b = (unsigned int)(v.size() / 8);		//base del cono
	AgregarVerticeSonda(v, glm::vec3(0.0f, -0.5f, 0.0f), 0.5f, 0.5f, glm::vec3(0.0f, -1.0f, 0.0f));
	for (int k = 0; k <= RES; k++)
	{
		ang = 2.0f * PI * k / RES;
		AgregarVerticeSonda(v, glm::vec3(2.0f * cosf(ang), -0.5f, 2.0f * sinf(ang)), 0.5f + 0.5f * cosf(ang), 0.5f + 0.5f * sinf(ang), glm::vec3(0.0f, -1.0f, 0.0f));
		if (k > 0) idx.insert(idx.end(), { b, b + k, b + k + 1 });
	}
	GuardarPiezaSonda(v, idx);	//meshListModel[10]

	//Pirámide cuadrangular: base de lado 1 en y = -0.5 y punta en (0, 0.5, 0). Cada cara lateral mira hacia su lado
	//y hacia arriba; su normal es (lado + (0, 0.5, 0)) normalizada
	const glm::vec3 lados[4] = { {0, 0, 1}, {1, 0, 0}, {0, 0, -1}, {-1, 0, 0} };
	for (const glm::vec3& d : lados)
	{
		glm::vec3 e = glm::cross(glm::vec3(0, 1, 0), d);	//a lo largo de la orilla de la base
		glm::vec3 n = glm::normalize(d + glm::vec3(0.0f, 0.5f, 0.0f));
		b = (unsigned int)(v.size() / 8);
		AgregarVerticeSonda(v, 0.5f * d - 0.5f * e + glm::vec3(0, -0.5f, 0), 0, 0, n);
		AgregarVerticeSonda(v, 0.5f * d + 0.5f * e + glm::vec3(0, -0.5f, 0), 1, 0, n);
		AgregarVerticeSonda(v, glm::vec3(0.0f, 0.5f, 0.0f), 0.5f, 1, n);
		idx.insert(idx.end(), { b, b + 1, b + 2 });
	}
	b = (unsigned int)(v.size() / 8);
	AgregarVerticeSonda(v, glm::vec3(-0.5f, -0.5f, -0.5f), 0, 0, glm::vec3(0, -1, 0));
	AgregarVerticeSonda(v, glm::vec3(0.5f, -0.5f, -0.5f), 1, 0, glm::vec3(0, -1, 0));
	AgregarVerticeSonda(v, glm::vec3(0.5f, -0.5f, 0.5f), 1, 1, glm::vec3(0, -1, 0));
	AgregarVerticeSonda(v, glm::vec3(-0.5f, -0.5f, 0.5f), 0, 1, glm::vec3(0, -1, 0));
	idx.insert(idx.end(), { b, b + 1, b + 2, b, b + 2, b + 3 });
	GuardarPiezaSonda(v, idx);	//meshListModel[11]

	//Esfera de radio 1 (las uniones de la práctica 4): en una esfera la normal es la misma posición
	const int PISOS = 12;
	for (int i = 0; i <= PISOS; i++)
	{
		float fi = PI * i / PISOS;				//de 0 (polo de arriba) a pi
		for (int k = 0; k <= RES; k++)
		{
			ang = 2.0f * PI * k / RES;
			glm::vec3 p(sinf(fi) * cosf(ang), cosf(fi), sinf(fi) * sinf(ang));
			AgregarVerticeSonda(v, p, (float)k / RES, 1.0f - (float)i / PISOS, p);
			if (i > 0 && k > 0)
			{
				unsigned int actual = i * (RES + 1) + k, arriba = actual - (RES + 1);
				idx.insert(idx.end(), { arriba - 1, arriba, actual, arriba - 1, actual, actual - 1 });
			}
		}
	}
	GuardarPiezaSonda(v, idx);	//meshListModel[12]
}


//Práctica 9: punto del satélite sobre el ocho para el parámetro t y su rumbo. El rumbo sale de la derivada de x y z
//respecto a t (dx = A·cos t, dz = B·cos 2t), así el frente del satélite (+Z) apunta hacia donde avanza
void PuntoSatelite(float t, glm::vec3& pos, float& rumbo)
{
	pos = CENTRO_SATELITE + glm::vec3(ANCHO_OCHO * sinf(t), ONDA_SATELITE * cosf(t), 0.5f * FONDO_OCHO * sinf(2.0f * t));
	rumbo = atan2f(ANCHO_OCHO * cosf(t), FONDO_OCHO * cosf(2.0f * t));
}


//Práctica 9: una extremidad del satélite, igual que en la práctica 4: CUERPO -> EXTREMIDAD -> PIERNA -> PIE.
//esquina es el punto de unión con el cuerpo; aperturaZ y aperturaX son los giros fijos que abren la pata hacia su
//esquina y articulacion es el giro que antes daba la tecla y ahora sale de la animación
void DibujarExtremidad(glm::mat4 cuerpo, glm::vec3 esquina, float aperturaZ, float aperturaX, float articulacion,
	GLuint uniformModel, GLuint uniformColor)
{
	glm::mat4 extremidad, model;
	glm::vec3 color;
	extremidad = glm::translate(cuerpo, esquina);
	extremidad = glm::rotate(extremidad, glm::radians(aperturaZ), glm::vec3(0.0f, 0.0f, 1.0f));
	extremidad = glm::rotate(extremidad, glm::radians(aperturaX), glm::vec3(1.0f, 0.0f, 0.0f));
	extremidad = glm::rotate(extremidad, glm::radians(articulacion), glm::vec3(1.0f, 0.0f, 0.0f));
	//unión visible en el eje (esfera azul)
	model = glm::scale(extremidad, glm::vec3(0.5f));
	color = glm::vec3(0.2f, 0.4f, 1.0f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshListModel[12]->RenderMeshModel();
	//PIERNA
	model = glm::translate(extremidad, glm::vec3(0.0f, -0.9f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
	color = glm::vec3(0.55f, 0.55f, 0.55f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshListModel[9]->RenderMeshModel();
	//PIE: en el extremo de la pierna, sin heredar su escala
	model = glm::translate(extremidad, glm::vec3(0.0f, -1.8f, 0.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.35f, 0.6f));
	color = glm::vec3(0.3f, 0.3f, 0.3f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshListModel[11]->RenderMeshModel();
}


//------------------------------------------------------------------
//Capturas para el reporte. Con --capturas lista.txt el programa recorre la lista dentro de
//la misma ventana (una línea por captura: archivo.ppm y opciones) y luego sigue normal.
//Opciones: --camara x y z yaw pitch, --rover x, --sentido 1|-1 (rover), --recorrido s (avión), --linterna 0|1,
//          --puntual con lin exp (luz roja), --spot con lin exp (luz verde fija),
//          --color 0..5 (luz del avión), --holocron 0|1, --pinza 0|1, --holocronx x, --holocronang grados,
//          --apertura 0..1 (esquinas del holocrón), --nave 0|1, --motor1 0|1, --motor2 0|1,
//          --recorrido s también es la distancia del avión sobre el programa de acrobacias (ejercicio 9-2),
//          --pista s (nave sobre la pista), --satelite t (parámetro del ocho), --fase f (extremidades, radianes)
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
	recorridoNave = 0.0f;
	anguloSatelite = 0.0f;
	faseExtremidades = 0.0f;
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
		else if (op[i] == "--pista" && i + 1 < op.size()) recorridoNave = num(++i);
		else if (op[i] == "--satelite" && i + 1 < op.size()) anguloSatelite = num(++i);
		else if (op[i] == "--fase" && i + 1 < op.size()) faseExtremidades = num(++i);
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
	CrearPista();
	CrearPiezasSonda();
	PrepararAcrobacias();
	CreateShaders();

	//Ejercicio 9-2: la cámara empieza más atrás que en la práctica 9 para que se vean completas las acrobacias,
	//que van de y = 5 a y = 28 en el plano z = 16
	camera = Camera(glm::vec3(0.0f, 14.0f, 62.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -4.0f, 0.3f, 0.5f);

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
	pistaTexture = Texture("Textures/pista.png");
	pistaTexture.LoadTextureA();
	muroTexture = Texture("Textures/pista_muro.png");
	muroTexture.LoadTextureA();

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
	float giroLlantas = 0.0f, parpadeo = 1.0f;
	glm::vec3 frenteAvion, techoAvion, posEstela, frenteEstela, techoEstela;
	float tamanoEstela = 0.0f;
	glm::vec3 posNave, dirNave, posSatelite;
	float rumboNave = 0.0f, cabeceoNave = 0.0f, inclinacionNave = 0.0f, rumboSatelite = 0.0f;
	glm::mat4 baseRover(1.0), baseAvion(1.0), baseNave(1.0), baseHolocron(1.0), baseSatelite(1.0), cuerpoSatelite(1.0);
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
			//Ejercicio 9-2, avión: solo avanza sobre el programa de acrobacias; fmod lo regresa al inicio
			recorridoAvion = fmod(recorridoAvion + VELOCIDAD_AVION * deltaTime, largoPrograma);
			//Práctica 9: la nave solo avanza sobre la pista; el satélite avanza sobre el ocho y sus extremidades oscilan
			recorridoNave = fmod(recorridoNave + VELOCIDAD_NAVE * deltaTime, LARGO_PISTA);
			anguloSatelite = fmod(anguloSatelite + VELOCIDAD_SATELITE * deltaTime, 2.0f * 3.14159265f);
			faseExtremidades = fmod(faseExtremidades + VELOCIDAD_EXTREMIDADES * deltaTime, 2.0f * 3.14159265f);
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

		//Ejercicio 9-2: el avión toma su pose del programa de acrobacias. Su orientación se arma directamente con los
		//vectores: el ala izquierda (+X del modelo) es techo x frente, el techo es +Y y el frente (la hélice) es +Z
		PoseAcrobacia(recorridoAvion, posAvion, frenteAvion, techoAvion);
		baseAvion = glm::mat4(1.0);
		baseAvion = glm::translate(baseAvion, posAvion);
		baseAvion = baseAvion * glm::mat4(glm::vec4(glm::cross(techoAvion, frenteAvion), 0.0f), glm::vec4(techoAvion, 0.0f),
			glm::vec4(frenteAvion, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		baseAvion = glm::scale(baseAvion, glm::vec3(0.75f, 0.75f, 0.75f));
		posLuzAvion = glm::vec3(baseAvion * glm::vec4(0.0f, -1.05f, 0.5f, 1.0f));	//debajo del fuselaje
		spotLights[2].SetPos(posLuzAvion);
		spotLights[2].SetColor(COLORES_LUZ_AVION[mainWindow.getColorAvion()]);
		//el faro apunta hacia el frente del avión y un poco hacia abajo; su dirección se gira con el avión
		posFaroAvion = glm::vec3(baseAvion * glm::vec4(0.0f, 0.0f, 3.4f, 1.0f));	//delante de la hélice
		dirFaroAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(0.0f, -0.5f, 1.0f, 0.0f)));
		spotLights[4].SetFlash(posFaroAvion, dirFaroAvion);

		//Práctica 9: la nave se coloca en su punto de la pista, un poco arriba de la superficie. Se gira sobre Y con el
		//rumbo, sobre X con el cabeceo (levanta la nariz en la subida y la baja en la bajada) y sobre Z con la
		//inclinación de las curvas. Las luces y las llamas se colocan con esta misma matriz
		PuntoPista(recorridoNave, posNave, dirNave, inclinacionNave);
		rumboNave = atan2f(dirNave.x, dirNave.z);
		cabeceoNave = asinf(dirNave.y);
		baseNave = glm::mat4(1.0);
		baseNave = glm::translate(baseNave, posNave + glm::vec3(0.0f, ALTURA_NAVE_PISTA, 0.0f));
		baseNave = glm::rotate(baseNave, rumboNave, glm::vec3(0.0f, 1.0f, 0.0f));
		baseNave = glm::rotate(baseNave, -cabeceoNave, glm::vec3(1.0f, 0.0f, 0.0f));	//con ángulo negativo +Z sube
		baseNave = glm::rotate(baseNave, -inclinacionNave * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
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

		//Práctica 9: pista de carreras y muros de su parte elevada (los vértices ya están en coordenadas del mundo)
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pistaTexture.UseTexture();
		meshListModel[6]->RenderMeshModel();
		muroTexture.UseTexture();
		meshListModel[7]->RenderMeshModel();

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

		//Avión de la práctica 6: hace sus acrobacias con sus dos luces
		model = baseAvion;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Avion_M.RenderModel(uniformColor);

		//Ejercicio 9-2: estela de humo. Cada esfera es la cola del avión en un punto anterior del programa, calculado
		//con la misma función; mientras más atrás, más chica, hasta desaparecer
		plainTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.95f, 0.95f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		for (i = 1; i <= PUNTOS_ESTELA; i++)
		{
			PoseAcrobacia(recorridoAvion - i * SEPARACION_ESTELA, posEstela, frenteEstela, techoEstela);
			tamanoEstela = TAMANO_ESTELA * (1.0f - (float)i / (PUNTOS_ESTELA + 1));
			model = glm::translate(glm::mat4(1.0), posEstela - frenteEstela * COLA_AVION);
			model = glm::scale(model, glm::vec3(tamanoEstela));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[12]->RenderMeshModel();
		}

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

		//Práctica 9: satélite (sonda espacial de la práctica 4), con la misma jerarquía:
		//CUERPO -> ANTENA -> PUNTA y CUERPO -> 4 EXTREMIDADES -> PIERNA -> PIE.
		//baseSatelite lo lleva a su punto del ocho y lo gira con su rumbo; cuerpoSatelite es el pivote de sus hijos
		PuntoSatelite(anguloSatelite, posSatelite, rumboSatelite);
		baseSatelite = glm::mat4(1.0);
		baseSatelite = glm::translate(baseSatelite, posSatelite);
		baseSatelite = glm::rotate(baseSatelite, rumboSatelite, glm::vec3(0.0f, 1.0f, 0.0f));
		cuerpoSatelite = glm::scale(baseSatelite, glm::vec3(ESCALA_SATELITE));
		plainTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//CUERPO
		model = glm::scale(cuerpoSatelite, glm::vec3(3.0f, 2.0f, 3.0f));
		color = glm::vec3(0.75f, 0.75f, 0.8f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[8]->RenderMeshModel();
		//EJE ANTENA: en el techo del cuerpo, con su unión azul
		modelaux = glm::translate(cuerpoSatelite, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(modelaux, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.4f, 1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[12]->RenderMeshModel();
		//ANTENA
		model = glm::translate(modelaux, glm::vec3(0.0f, 0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.8f, 0.25f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[9]->RenderMeshModel();
		//PUNTA: sensor en el extremo de la antena, sin heredar la escala de la antena
		model = glm::translate(modelaux, glm::vec3(0.0f, 1.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.4f));
		color = glm::vec3(1.0f, 0.3f, 0.1f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[10]->RenderMeshModel();
		//EXTREMIDADES: mismas esquinas y aperturas fijas que en la práctica 4. La articulación (antes teclas Q, Z, X
		//y C) es un seno de la fase; cada extremidad va 90° detrás de la anterior, así el movimiento recorre las
		//cuatro esquinas: frontal derecha, trasera derecha, trasera izquierda y frontal izquierda
		DibujarExtremidad(cuerpoSatelite, glm::vec3(1.3f, -0.7f, 1.3f), -35.0f, 35.0f,
			AMPLITUD_EXTREMIDADES * sinf(faseExtremidades), uniformModel, uniformColor);
		DibujarExtremidad(cuerpoSatelite, glm::vec3(1.3f, -0.7f, -1.3f), -35.0f, -35.0f,
			AMPLITUD_EXTREMIDADES * sinf(faseExtremidades - 0.5f * 3.14159265f), uniformModel, uniformColor);
		DibujarExtremidad(cuerpoSatelite, glm::vec3(-1.3f, -0.7f, -1.3f), 35.0f, -35.0f,
			AMPLITUD_EXTREMIDADES * sinf(faseExtremidades - 3.14159265f), uniformModel, uniformColor);
		DibujarExtremidad(cuerpoSatelite, glm::vec3(-1.3f, -0.7f, 1.3f), 35.0f, 35.0f,
			AMPLITUD_EXTREMIDADES * sinf(faseExtremidades - 1.5f * 3.14159265f), uniformModel, uniformColor);



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
