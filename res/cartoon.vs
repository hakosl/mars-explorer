#version 330 core

layout(location = 0) in vec3 vertex_position;
layout (location = 2) in vec3 vertex_normal;

out vec3 vs_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
    gl_Position = proj * view * model * vec4(vertex_position, 1.0f);
    vs_normal = mat3(view * model) * normalize(vertex_normal);
}
