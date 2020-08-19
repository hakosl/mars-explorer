#shader vertex
#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
        vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
        vs_color = vec3(1.0f, 0.0f, 0.0f);
        vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
        vs_normal = mat3(ModelMatrix) * vertex_normal;

        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}

#shader fragment
#version 330 core

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform vec4 lightPosition;
uniform vec4 customColor;
uniform vec4 lightColor;
uniform vec3 lightPos;

void main(){

    // Default color
    /*if (customColor == vec4(0.0, 0.0, 0.0, 0.0)){
        color = lightColor * vec4(0.5, 0.5, 1.0, 1.0);
    }
    else {
        color = lightColor * customColor;
    }*/

    fs_color = vec4(vs_color, 1.f);

    //color = lightColor * vec4(1.0, 1.0, 1.0, 1.0);

    // Setting up ambient lighting
    //float ambientStrength = 0.15;
    //color = ambientStrength * color;


}

//#shader vertex
//#version 330 core

//layout(location = 0) in vec4 position;
//layout (location = 1) in vec3 normal_in;
//out vec4 fragmentPosition;
//out vec3 normal;

//uniform mat4 model_matrix;
//uniform mat4 view_matrix;
//uniform mat4 proj_matrix;


//void main(){
    //gl_Position = position;
    //gl_Normal = normal_in;

  //  gl_Position = proj_matrix * view_matrix * model_matrix * position;
  //  fragmentPosition = model_matrix * position;
  //  normal = normal_in;
//}


//#shader fragment
//#version 330 core

//in vec3 normal;
//in vec4 fragmentPosition;
//layout(location = 0) out vec4 color;

//uniform vec4 lightPosition;
//uniform vec4 customColor;
//uniform vec3 lightColor;
//uniform vec4 lightDir;

//void main(){

    // Default color
  //  if (customColor == vec4(0.0, 0.0, 0.0, 0.0)){
    //    color = lightColor * vec4(0.5, 0.5, 1.0, 1.0);
    //}
    //else {
      //  color = lightColor * customColor;
    //}

    //color = lightColor * vec4(1.0, 1.0, 1.0, 1.0);

    // Setting up ambient lighting
    //float ambientStrength = 0.15;
    //color = ambientStrength * color;


    //vec4 norm = normalize(normal, 1.0);
    //vec4 lightDir = vec4(normalize(-lightDir));

    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 result = (ambient + diffuse) * customColor;
    //FragColor = vec4(result, 1.0);

//}
