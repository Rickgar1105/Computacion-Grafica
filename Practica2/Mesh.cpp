#include "Mesh.h"


// ======================================================
// MESH
// ======================================================

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;

    indexCount = 0;
}


void Mesh::CreateMesh(
    GLfloat* vertices,
    unsigned int* indices,
    unsigned int numOfVertices,
    unsigned int numOfIndices
)
{
    indexCount = numOfIndices;


    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    // IBO
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * numOfIndices,
        indices,
        GL_STATIC_DRAW
    );


    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * numOfVertices,
        vertices,
        GL_STATIC_DRAW
    );


    // Posición
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        0
    );

    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Mesh::RenderMesh()
{
    glBindVertexArray(VAO);

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        IBO
    );

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}


void Mesh::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}


Mesh::~Mesh()
{
    ClearMesh();
}


// ======================================================
// MESH COLOR
// ======================================================

MeshColor::MeshColor()
{
    VAO = 0;
    VBO = 0;

    vertexCount = 0;
}


void MeshColor::CreateMeshColor(
    GLfloat* vertices,
    unsigned int numOfVertices
)
{
    // Cada vértice tiene:
    // X Y Z R G B
    vertexCount = numOfVertices / 6;


    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * numOfVertices,
        vertices,
        GL_STATIC_DRAW
    );


    // Posición
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(GLfloat),
        0
    );

    glEnableVertexAttribArray(0);


    // Color
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat))
    );

    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void MeshColor::RenderMeshColor()
{
    glBindVertexArray(VAO);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        vertexCount
    );

    glBindVertexArray(0);
}


void MeshColor::ClearMeshColor()
{
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    vertexCount = 0;
}


MeshColor::~MeshColor()
{
    ClearMeshColor();
}