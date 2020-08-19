#version 330 core
in vec2 ftexCoord;
in vec3 normal;
in vec3 fragposition;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float alpha;
    float shadeless;
};

struct PointLight {
    vec3 position;
    vec4 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec4 color;
    float cutoff;
};


uniform sampler2D texture1;

uniform vec3 viewPos;

uniform SpotLight spotlight;
uniform PointLight pointlight;

uniform Material mat;

vec3 norm;
vec3 viewDir;

out vec4 FragColor;

vec4 calculatePointLightIntensity(PointLight l) {
    vec3 lightDir = normalize(l.position - fragposition);

    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);



    vec4 pointLightIntensity = (diffuse * mat.diffuse + spec * mat.specular) * l.color;// + specularI;
    return pointLightIntensity;
}

vec4 calculateSpotLightIntensity(SpotLight l) {
    vec3 lightDir = normalize(l.position - fragposition);

    float theta = dot(lightDir, normalize(-l.direction));

    vec4 spotLightIntensity = vec4(0.0f);
    if(theta > l.cutoff) {
        float sDiffuse = max(dot(norm, lightDir), 0.0);
        vec3 sReflectDir = reflect(-lightDir, norm);
        float sSpec = pow(max(dot(viewDir, sReflectDir), 0.0), mat.shininess);

        spotLightIntensity = (sDiffuse * mat.diffuse + sSpec + mat.specular) * l.color * theta;
    }
    return spotLightIntensity;
}


void main()
{

    norm = normalize(normal);
    viewDir = normalize(viewPos - fragposition);


    vec4 plighti = calculatePointLightIntensity(pointlight);
    vec4 slighti = calculateSpotLightIntensity(spotlight);

    // scaling light intensity so it doesn't exceed 1.0
    vec4 lightIntensity = vec4(min(mat.ambient.r + plighti.r + slighti.r, 1.0), min(mat.ambient.g + plighti.g + slighti.g, 1.0 ), min(mat.ambient.b + plighti.b + slighti.b, 1.0), min((mat.ambient.a + plighti.a + slighti.a)/3, 1.0));

    if(mat.shadeless == 1.0f) {
        FragColor = (mat.ambient) * texture(texture1, ftexCoord);
    } else {
        FragColor = (mat.ambient + plighti + slighti) * texture(texture1, ftexCoord);
    }
}
