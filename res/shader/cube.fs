#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float ambient;

void main()
{    
    FragColor = texture(skybox, TexCoords) * vec4(ambient, ambient, ambient, 1.0);
}