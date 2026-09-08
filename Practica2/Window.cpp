#include "Window.h"


Window::Window()
{
    width = 800;
    height = 600;

    bufferWidth = 0;
    bufferHeight = 0;

    mainWindow = nullptr;
}


Window::Window(GLint windowWidth, GLint windowHeight)
{
    width = windowWidth;
    height = windowHeight;

    bufferWidth = 0;
    bufferHeight = 0;

    mainWindow = nullptr;
}


int Window::Initialise()
{
    // Inicializar GLFW
    if (!glfwInit())
    {
        printf("Error al inicializar GLFW\n");
        glfwTerminate();

        return 1;
    }


    // ============================================
    // CONFIGURACIÓN ESPECIAL PARA macOS
    // ============================================

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        4
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        1
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );


    // Crear ventana
    mainWindow = glfwCreateWindow(
        width,
        height,
        "Practica 2: Proyecciones, transformaciones",
        nullptr,
        nullptr
    );


    if (!mainWindow)
    {
        printf("Error al crear la ventana GLFW\n");

        glfwTerminate();

        return 1;
    }


    // Obtener tamaño real del framebuffer.
    // Esto es importante en pantallas Retina de Mac.
    glfwGetFramebufferSize(
        mainWindow,
        &bufferWidth,
        &bufferHeight
    );


    // Usar el contexto OpenGL de esta ventana
    glfwMakeContextCurrent(mainWindow);


    // ============================================
    // INICIALIZAR GLEW
    // ============================================

    glewExperimental = GL_TRUE;

    GLenum error = glewInit();


    if (error != GLEW_OK)
    {
        printf(
            "Error al inicializar GLEW: %s\n",
            glewGetErrorString(error)
        );

        glfwDestroyWindow(mainWindow);
        glfwTerminate();

        return 1;
    }


    // Limpiar posible error generado por GLEW
    glGetError();


    // Habilitar profundidad
    glEnable(GL_DEPTH_TEST);


    // Definir viewport
    glViewport(
        0,
        0,
        bufferWidth,
        bufferHeight
    );


    printf("OpenGL: %s\n", glGetString(GL_VERSION));
    printf("GPU: %s\n", glGetString(GL_RENDERER));


    return 0;
}


GLfloat Window::getBufferWidth()
{
    return static_cast<GLfloat>(bufferWidth);
}


GLfloat Window::getBufferHeight()
{
    return static_cast<GLfloat>(bufferHeight);
}


bool Window::getShouldClose()
{
    return glfwWindowShouldClose(mainWindow);
}


void Window::swapBuffers()
{
    glfwSwapBuffers(mainWindow);
}


Window::~Window()
{
    if (mainWindow)
    {
        glfwDestroyWindow(mainWindow);
        mainWindow = nullptr;
    }

    glfwTerminate();
}