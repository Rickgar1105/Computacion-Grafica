#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

uniform vec3 color;
uniform vec3 eyePosition;

void main()
{
    vec3 lightDir = normalize(vec3(0.4, 1.0, 0.6));
    vec3 n = normalize(Normal);

    float ambient = 0.35;
    float diffuse = max(dot(n, lightDir), 0.0) * 0.65;

    vec3 viewDir = normalize(eyePosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, n);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0) * 0.25;

    vec3 result = color * (ambient + diffuse) + vec3(1.0, 1.0, 1.0) * spec;
    colour = vec4(result, 1.0);
}
