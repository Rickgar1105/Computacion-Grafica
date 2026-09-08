#pragma once

#include <GL/glew.h>

// ======================================================
// MESH NORMAL
// ======================================================

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void CreateMesh(
        GLfloat* vertices,
        unsigned int* indices,
        unsigned int numOfVertices,
        unsigned int numOfIndices
    );

    void RenderMesh();
    void ClearMesh();

private:
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;

    GLsizei indexCount;
};


// ======================================================
// MESH CON COLOR
// ======================================================

class MeshColor
{
public:
    MeshColor();
    ~MeshColor();

    void CreateMeshColor(
        GLfloat* vertices,
        unsigned int numOfVertices
    );

    void RenderMeshColor();
    void ClearMeshColor();

private:
    GLuint VAO;
    GLuint VBO;

    GLsizei vertexCount;
};