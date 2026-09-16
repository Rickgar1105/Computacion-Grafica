#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getrotay() { return rotay; }
	GLfloat getrotax() { return rotax; }
	GLfloat getrotaz() { return rotaz; }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getarticulacion6() { return articulacion6; }
	GLfloat getarticulacion1i() { return articulacion1i; }
	GLfloat getarticulacion2i() { return articulacion2i; }
	GLfloat getarticulacion3i() { return articulacion3i; }
	bool getEscenaSonda() { return escenaSonda; }
	GLfloat getarticulacionE1() { return articulacionE1; }
	GLfloat getarticulacionE2() { return articulacionE2; }
	GLfloat getarticulacionE3() { return articulacionE3; }
	GLfloat getarticulacionE4() { return articulacionE4; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	GLfloat rotax,rotay,rotaz, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, articulacion6;
	GLfloat articulacion1i, articulacion2i, articulacion3i; //llantas del lado izquierdo (espejo), independientes de las del lado derecho
	bool escenaSonda; //false = rover (Actividad 1), true = sonda espacial (Actividad 2)
	GLfloat articulacionE1, articulacionE2, articulacionE3, articulacionE4; //las 4 extremidades de la sonda
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;
	void createCallbacks();
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};

