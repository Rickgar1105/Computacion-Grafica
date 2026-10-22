#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
	reiniciarControles();
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	//sin estos valores iniciales el primer movimiento del ratón usaba basura y giraba la cámara
	mouseFirstMoved = true;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
	reiniciarControles();
}

//Estado inicial de las luces y del holocrón (también se usa al terminar cada captura)
void Window::reiniciarControles()
{
	linterna = true;
	colorAvion = 1;			//amarillo, el color que tenía en la práctica 7
	luzHolocron = true;
	luzPinza = true;
	lucesNave = true;
	motor[0] = motor[1] = true;
	holocronX = -6.0f;		//misma X que INICIO_HOLOCRON en ejercicio9.cpp
}

int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	//macOS solo ofrece el perfil Core hasta OpenGL 4.1; los shaders usan #version 330
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Ejercicio 9-2: Animacion compleja", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

	//Luces: cambian solo cuando se presiona la tecla (GLFW_PRESS), no mientras se mantiene
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_L) theWindow->linterna = !theWindow->linterna;
		if (key == GLFW_KEY_C) theWindow->colorAvion = (theWindow->colorAvion + 1) % 6;
		if (key == GLFW_KEY_K) theWindow->luzHolocron = !theWindow->luzHolocron;
		if (key == GLFW_KEY_P) theWindow->luzPinza = !theWindow->luzPinza;
		if (key == GLFW_KEY_N) theWindow->lucesNave = !theWindow->lucesNave;
		if (key == GLFW_KEY_1) theWindow->motor[0] = !theWindow->motor[0];
		if (key == GLFW_KEY_2) theWindow->motor[1] = !theWindow->motor[1];
	}
	//Holocrón: se mueve en X al presionar y mientras se mantiene la flecha (GLFW_REPEAT), entre -14 y 10
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_LEFT && theWindow->holocronX > -14.0f) theWindow->holocronX -= 0.3f;
		if (key == GLFW_KEY_RIGHT && theWindow->holocronX < 10.0f) theWindow->holocronX += 0.3f;
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
