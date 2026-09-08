#include "Shader.h"


// ======================================================
// CONSTRUCTOR
// ======================================================

Shader::Shader()
{
    shaderID = 0;

    uniformModel = 0;
    uniformProjection = 0;
}


// ======================================================
// LEER ARCHIVO
// ======================================================

std::string Shader::ReadFile(
    const char* fileLocation
)
{
    std::string content;

    std::ifstream fileStream(
        fileLocation,
        std::ios::in
    );


    if (!fileStream.is_open())
    {
        std::cerr
            << "Error al leer el archivo: "
            << fileLocation
            << std::endl;

        return "";
    }


    std::string line = "";


    while (std::getline(fileStream, line))
    {
        content.append(line + "\n");
    }


    fileStream.close();


    return content;
}


// ======================================================
// CREAR SHADER DESDE TEXTO
// ======================================================

void Shader::CreateFromString(
    const char* vertexCode,
    const char* fragmentCode
)
{
    CompileShader(
        vertexCode,
        fragmentCode
    );
}


// ======================================================
// CREAR SHADER DESDE ARCHIVOS
// ======================================================

void Shader::CreateFromFiles(
    const char* vertexLocation,
    const char* fragmentLocation
)
{
    std::string vertexString =
        ReadFile(vertexLocation);


    std::string fragmentString =
        ReadFile(fragmentLocation);


    const char* vertexCode =
        vertexString.c_str();


    const char* fragmentCode =
        fragmentString.c_str();


    CompileShader(
        vertexCode,
        fragmentCode
    );
}


// ======================================================
// COMPILAR PROGRAMA DE SHADER
// ======================================================

void Shader::CompileShader(
    const char* vertexCode,
    const char* fragmentCode
)
{
    shaderID = glCreateProgram();


    if (!shaderID)
    {
        std::cerr
            << "Error creando el programa de shader."
            << std::endl;

        return;
    }


    // Agregar vertex shader
    AddShader(
        shaderID,
        vertexCode,
        GL_VERTEX_SHADER
    );


    // Agregar fragment shader
    AddShader(
        shaderID,
        fragmentCode,
        GL_FRAGMENT_SHADER
    );


    GLint result = 0;

    GLchar eLog[1024] = { 0 };


    // ==================================================
    // LINK
    // ==================================================

    glLinkProgram(shaderID);


    glGetProgramiv(
        shaderID,
        GL_LINK_STATUS,
        &result
    );


    if (!result)
    {
        glGetProgramInfoLog(
            shaderID,
            sizeof(eLog),
            nullptr,
            eLog
        );


        std::cerr
            << "Error enlazando programa: "
            << eLog
            << std::endl;


        return;
    }


    /*
        IMPORTANTE PARA macOS:

        NO usamos glValidateProgram() aquí.

        En macOS con OpenGL Core Profile puede aparecer:

        Validation Failed:
        No vertex array object bound.

        Esto sucede porque en este momento todavía
        no hay un VAO enlazado.
    */


    // ==================================================
    // OBTENER VARIABLES UNIFORM
    // ==================================================

    uniformModel =
        glGetUniformLocation(
            shaderID,
            "model"
        );


    uniformProjection =
        glGetUniformLocation(
            shaderID,
            "projection"
        );


    // Información útil en Terminal
    std::cout
        << "Shader creado correctamente."
        << std::endl;


    std::cout
        << "uniformModel = "
        << uniformModel
        << std::endl;


    std::cout
        << "uniformProjection = "
        << uniformProjection
        << std::endl;
}


// ======================================================
// AGREGAR SHADER AL PROGRAMA
// ======================================================

void Shader::AddShader(
    GLuint theProgram,
    const char* shaderCode,
    GLenum shaderType
)
{
    GLuint theShader =
        glCreateShader(shaderType);


    const GLchar* theCode[1];

    theCode[0] =
        shaderCode;


    GLint codeLength[1];

    codeLength[0] =
        static_cast<GLint>(
            std::strlen(shaderCode)
        );


    glShaderSource(
        theShader,
        1,
        theCode,
        codeLength
    );


    glCompileShader(theShader);


    GLint result = 0;

    GLchar eLog[1024] = { 0 };


    glGetShaderiv(
        theShader,
        GL_COMPILE_STATUS,
        &result
    );


    if (!result)
    {
        glGetShaderInfoLog(
            theShader,
            sizeof(eLog),
            nullptr,
            eLog
        );


        std::cerr
            << "Error compilando shader: "
            << eLog
            << std::endl;


        glDeleteShader(theShader);

        return;
    }


    glAttachShader(
        theProgram,
        theShader
    );


    // Ya quedó adjunto al programa
    glDeleteShader(theShader);
}


// ======================================================
// USAR SHADER
// ======================================================

void Shader::useShader()
{
    glUseProgram(shaderID);
}


// ======================================================
// OBTENER MODEL
// ======================================================

GLuint Shader::getModelLocation()
{
    return uniformModel;
}


// ======================================================
// OBTENER PROJECTION
// ======================================================

GLuint Shader::getProjectLocation()
{
    return uniformProjection;
}


// ======================================================
// LIMPIAR SHADER
// ======================================================

void Shader::ClearShader()
{
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);

        shaderID = 0;
    }


    uniformModel = 0;

    uniformProjection = 0;
}


// ======================================================
// DESTRUCTOR
// ======================================================

Shader::~Shader()
{
    ClearShader();
}