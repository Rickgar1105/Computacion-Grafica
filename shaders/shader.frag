#version 330 core

out vec4 colour;

uniform vec3 color;

void main()
{
	colour = vec4(color, 1.0);
}
