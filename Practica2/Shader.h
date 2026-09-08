#pragma once

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include <GL/glew.h>

class Shader
{
public:
    Shader();
    ~Shader();

    void CreateFromString(
        const char* vertexCode,
        const char* fragmentCode
    );

    void CreateFromFiles(
        const char* vertexLocation,
        const char* fragmentLocation
    );

    std::string ReadFile(
        const char* fileLocation
    );

    void useShader();
    void ClearShader();

    GLuint getModelLocation();
    GLuint getProjectLocation();

private:

    GLuint shaderID;

    GLuint uniformModel;
    GLuint uniformProjection;

    void CompileShader(
        const char* vertexCode,
        const char* fragmentCode
    );

    void AddShader(
        GLuint theProgram,
        const char* shaderCode,
        GLenum shaderType
    );
};