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
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	//Estado que cambia con el teclado (ver ManejaTeclado); main solo lo lee
	bool getLinterna() { return linterna; }
	int getColorAvion() { return colorAvion; }
	bool getLuzHolocron() { return luzHolocron; }
	bool getLuzPinza() { return luzPinza; }
	bool getLucesNave() { return lucesNave; }
	bool getMotor(int m) { return motor[m]; }
	GLfloat getHolocronX() { return holocronX; }
	void reiniciarControles();
	//Para las capturas del reporte: fijan el estado sin usar el teclado
	void setLinterna(bool valor) { linterna = valor; }
	void setColorAvion(int valor) { colorAvion = valor; }
	void setLuzHolocron(bool valor) { luzHolocron = valor; }
	void setLuzPinza(bool valor) { luzPinza = valor; }
	void setLucesNave(bool valor) { lucesNave = valor; }
	void setMotor(int m, bool valor) { motor[m] = valor; }
	void setHolocronX(GLfloat valor) { holocronX = valor; }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	bool linterna;		//L
	int colorAvion;		//C: índice en COLORES_LUZ_AVION (0 rojo ... 5 magenta)
	bool luzHolocron;	//K
	bool luzPinza;		//P
	bool lucesNave;		//N
	bool motor[2];		//1 y 2
	GLfloat holocronX;	//flechas izquierda / derecha
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

