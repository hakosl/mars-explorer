#version 330 core

in vec3 vs_normal;

out vec4 color;

uniform vec4 customColor;
uniform vec4 lightColor;
uniform vec3 lightDirection;

void main(){

    // Fallback on default color if costum color is not set 244, 72, 66
    if(customColor == vec4(0.0f, 0.0f, 0.0f, 0.0f))
        color = lightColor * vec4(1.0f, 0.29f, 0.27f, 1.0f);
    else
        color = lightColor * customColor;

    // cartoon shading
    vec4 color2;
    float intensity = dot(normalize(-lightDirection), vs_normal);

    if (intensity > 0.45){
        color2 = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else if (intensity > 0.25){
        color2 = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        color2 = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // Return result
    color = color * color2;
}
