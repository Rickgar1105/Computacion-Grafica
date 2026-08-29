#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

// ------------------------------------------------------
// Agrega un rectángulo formado por 2 triángulos
// ------------------------------------------------------
void agregarRectangulo(
    std::vector<float>& vertices,
    float x1, float y1,
    float x2, float y2)
{
    // Triángulo 1
    vertices.push_back(x1);
    vertices.push_back(y1);

    vertices.push_back(x2);
    vertices.push_back(y1);

    vertices.push_back(x2);
    vertices.push_back(y2);

    // Triángulo 2
    vertices.push_back(x1);
    vertices.push_back(y1);

    vertices.push_back(x2);
    vertices.push_back(y2);

    vertices.push_back(x1);
    vertices.push_back(y2);
}

// ------------------------------------------------------
// Agrega una línea gruesa usando 2 triángulos
// Sirve para la diagonal de la R
// ------------------------------------------------------
void agregarLineaGruesa(
    std::vector<float>& vertices,
    float x1, float y1,
    float x2, float y2,
    float grosor)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float longitud = std::sqrt(dx * dx + dy * dy);

    float px = -dy / longitud * grosor / 2.0f;
    float py =  dx / longitud * grosor / 2.0f;

    float ax = x1 + px;
    float ay = y1 + py;

    float bx = x1 - px;
    float by = y1 - py;

    float cx = x2 - px;
    float cy = y2 - py;

    float dx2 = x2 + px;
    float dy2 = y2 + py;

    // Triángulo 1
    vertices.push_back(ax);
    vertices.push_back(ay);

    vertices.push_back(bx);
    vertices.push_back(by);

    vertices.push_back(cx);
    vertices.push_back(cy);

    // Triángulo 2
    vertices.push_back(ax);
    vertices.push_back(ay);

    vertices.push_back(cx);
    vertices.push_back(cy);

    vertices.push_back(dx2);
    vertices.push_back(dy2);
}

// ------------------------------------------------------
// Compilar Shader
// ------------------------------------------------------
GLuint compilarShader(GLenum tipo, const char* codigo)
{
    GLuint shader = glCreateShader(tipo);

    glShaderSource(shader, 1, &codigo, nullptr);
    glCompileShader(shader);

    GLint correcto;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &correcto);

    if (!correcto)
    {
        char info[512];

        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            info
        );

        std::cout
            << "Error al compilar Shader:\n"
            << info
            << std::endl;
    }

    return shader;
}

// ------------------------------------------------------
// MAIN
// ------------------------------------------------------
int main()
{
    // ==================================================
    // INICIAR GLFW
    // ==================================================

    if (!glfwInit())
    {
        std::cout << "Error al iniciar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Necesario para macOS
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );

    GLFWwindow* ventana =
        glfwCreateWindow(
            1000,
            600,
            "Practica 1 - REGT",
            nullptr,
            nullptr
        );

    if (!ventana)
    {
        std::cout
            << "Error al crear la ventana"
            << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(ventana);

    // ==================================================
    // INICIAR GLEW
    // ==================================================

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout
            << "Error al iniciar GLEW"
            << std::endl;

        return -1;
    }

    // ==================================================
    // DATOS DE OPENGL
    // ==================================================

    std::cout
        << "OpenGL: "
        << glGetString(GL_VERSION)
        << std::endl;

    std::cout
        << "GPU: "
        << glGetString(GL_RENDERER)
        << std::endl;

    // ==================================================
    // VERTEX SHADER
    // ==================================================

    const char* vertexShaderCodigo = R"(

        #version 330 core

        layout (location = 0) in vec2 posicion;

        void main()
        {
            gl_Position =
                vec4(
                    posicion.x,
                    posicion.y,
                    0.0,
                    1.0
                );
        }

    )";

    // ==================================================
    // FRAGMENT SHADER
    // Todas las letras serán del mismo color
    // ==================================================

    const char* fragmentShaderCodigo = R"(

        #version 330 core

        out vec4 FragColor;

        uniform vec3 colorLetras;

        void main()
        {
            FragColor =
                vec4(
                    colorLetras,
                    1.0
                );
        }

    )";

    GLuint vertexShader =
        compilarShader(
            GL_VERTEX_SHADER,
            vertexShaderCodigo
        );

    GLuint fragmentShader =
        compilarShader(
            GL_FRAGMENT_SHADER,
            fragmentShaderCodigo
        );

    // ==================================================
    // CREAR PROGRAMA DE SHADERS
    // ==================================================

    GLuint programa =
        glCreateProgram();

    glAttachShader(
        programa,
        vertexShader
    );

    glAttachShader(
        programa,
        fragmentShader
    );

    glLinkProgram(programa);

    GLint enlaceCorrecto;

    glGetProgramiv(
        programa,
        GL_LINK_STATUS,
        &enlaceCorrecto
    );

    if (!enlaceCorrecto)
    {
        char info[512];

        glGetProgramInfoLog(
            programa,
            512,
            nullptr,
            info
        );

        std::cout
            << "Error al enlazar programa:\n"
            << info
            << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ==================================================
    // CREAR LAS LETRAS REGT
    // ==================================================

    std::vector<float> vertices;

    const float superior = 0.55f;
    const float inferior = -0.55f;
    const float medio = 0.00f;

    const float grosor = 0.07f;

    // ==================================================
    // LETRA R
    // ==================================================

    float Rx1 = -0.92f;
    float Rx2 = -0.56f;

    // Barra izquierda
    agregarRectangulo(
        vertices,
        Rx1,
        inferior,
        Rx1 + grosor,
        superior
    );

    // Barra superior
    agregarRectangulo(
        vertices,
        Rx1,
        superior - grosor,
        Rx2,
        superior
    );

    // Barra central
    agregarRectangulo(
        vertices,
        Rx1,
        medio - grosor / 2.0f,
        Rx2,
        medio + grosor / 2.0f
    );

    // Barra derecha superior
    agregarRectangulo(
        vertices,
        Rx2 - grosor,
        medio,
        Rx2,
        superior
    );

    // Pierna diagonal de R
    agregarLineaGruesa(
        vertices,
        Rx1 + grosor,
        medio,
        Rx2,
        inferior,
        grosor
    );

    // ==================================================
    // LETRA E
    // ==================================================

    float Ex1 = -0.43f;
    float Ex2 = -0.10f;

    // Vertical
    agregarRectangulo(
        vertices,
        Ex1,
        inferior,
        Ex1 + grosor,
        superior
    );

    // Superior
    agregarRectangulo(
        vertices,
        Ex1,
        superior - grosor,
        Ex2,
        superior
    );

    // Central
    agregarRectangulo(
        vertices,
        Ex1,
        medio - grosor / 2.0f,
        Ex2 - 0.05f,
        medio + grosor / 2.0f
    );

    // Inferior
    agregarRectangulo(
        vertices,
        Ex1,
        inferior,
        Ex2,
        inferior + grosor
    );

    // ==================================================
    // LETRA G
    // ==================================================

    float Gx1 = 0.05f;
    float Gx2 = 0.39f;

    // Izquierda
    agregarRectangulo(
        vertices,
        Gx1,
        inferior,
        Gx1 + grosor,
        superior
    );

    // Superior
    agregarRectangulo(
        vertices,
        Gx1,
        superior - grosor,
        Gx2,
        superior
    );

    // Inferior
    agregarRectangulo(
        vertices,
        Gx1,
        inferior,
        Gx2,
        inferior + grosor
    );

    // Lado derecho inferior
    agregarRectangulo(
        vertices,
        Gx2 - grosor,
        inferior,
        Gx2,
        0.05f
    );

    // Parte interna de la G
    agregarRectangulo(
        vertices,
        0.23f,
        -0.02f,
        Gx2,
        0.05f
    );

    // ==================================================
    // LETRA T
    // ==================================================

    float Tx1 = 0.52f;
    float Tx2 = 0.92f;

    // Barra superior
    agregarRectangulo(
        vertices,
        Tx1,
        superior - grosor,
        Tx2,
        superior
    );

    // Barra vertical
    float centroT =
        (Tx1 + Tx2) / 2.0f;

    agregarRectangulo(
        vertices,
        centroT - grosor / 2.0f,
        inferior,
        centroT + grosor / 2.0f,
        superior
    );

    // ==================================================
    // CREAR VAO Y VBO
    // ==================================================

    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(
        1,
        &VAO
    );

    glGenBuffers(
        1,
        &VBO
    );

    glBindVertexArray(VAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // ==================================================
    // CONFIGURAR ATRIBUTO DE VÉRTICES
    // ==================================================

    glVertexAttribPointer(
        0,                  // location
        2,                  // X, Y
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(0);

    // ==================================================
    // COLOR DE LAS LETRAS
    // Todas usarán el mismo color
    // ==================================================

    glUseProgram(programa);

    GLint ubicacionColor =
        glGetUniformLocation(
            programa,
            "colorLetras"
        );

    // Amarillo
    glUniform3f(
        ubicacionColor,
        1.0f,
        1.0f,
        0.0f
    );

    // ==================================================
    // GENERADOR DE NÚMEROS ALEATORIOS
    // ==================================================

    std::random_device rd;

    std::mt19937 generador(
        rd()
    );

    std::uniform_real_distribution<float>
        colorRandom(
            0.0f,
            1.0f
        );

    // Primer color completamente aleatorio
    float rojo =
        colorRandom(generador);

    float verde =
        colorRandom(generador);

    float azul =
        colorRandom(generador);

    double ultimoCambio = 0.0;

    std::cout
        << "Fondo inicial RGB: "
        << rojo << ", "
        << verde << ", "
        << azul
        << std::endl;

    // ==================================================
    // CICLO PRINCIPAL
    // ==================================================

    while (!glfwWindowShouldClose(ventana))
    {
        // ----------------------------------------------
        // Tamaño real del framebuffer
        // Importante en pantallas Retina
        // ----------------------------------------------

        int ancho;
        int alto;

        glfwGetFramebufferSize(
            ventana,
            &ancho,
            &alto
        );

        glViewport(
            0,
            0,
            ancho,
            alto
        );

        // ----------------------------------------------
        // CAMBIO DE COLOR CADA 2 SEGUNDOS
        // ----------------------------------------------

        double tiempoActual =
            glfwGetTime();

        if (
            tiempoActual - ultimoCambio
            >= 2.0
        )
        {
            rojo =
                colorRandom(generador);

            verde =
                colorRandom(generador);

            azul =
                colorRandom(generador);

            ultimoCambio =
                tiempoActual;

            std::cout
                << "Nuevo fondo RGB: "
                << rojo << ", "
                << verde << ", "
                << azul
                << std::endl;
        }

        // ----------------------------------------------
        // COLOR ALEATORIO DEL FONDO
        // ----------------------------------------------

        glClearColor(
            rojo,
            verde,
            azul,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT
        );

        // ----------------------------------------------
        // DIBUJAR REGT
        // ----------------------------------------------

        glUseProgram(programa);

        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(
                vertices.size() / 2
            )
        );

        glBindVertexArray(0);

        glfwSwapBuffers(ventana);
        glfwPollEvents();
    }

    // ==================================================
    // LIBERAR RECURSOS
    // ==================================================

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glDeleteProgram(programa);

    glfwDestroyWindow(ventana);
    glfwTerminate();

    return 0;
}