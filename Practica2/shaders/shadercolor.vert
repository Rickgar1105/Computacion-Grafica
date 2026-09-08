#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(pos, 1.0);
    vertexColor = color;
}