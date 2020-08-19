#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vtexCoord;
layout (location = 2) in vec3 aNorm;

out vec2 ftexCoord;
out vec4 vertexColor;
out vec3 fragposition;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 timodel;
uniform mat4 pvm;
void main()
{
   gl_Position = pvm * vec4(aPos, 1.0);
   ftexCoord = vtexCoord;
   fragposition = vec3(model * vec4(aPos, 1.0));
   normal = mat3(timodel) * aNorm;
}
