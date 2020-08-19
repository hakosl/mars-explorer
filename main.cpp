
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>


#include "stb_image.h"
#include "shader.h"
#include "model_obj.h"
#include "object.h"
#include "catmullrom.h"
#include "catmullrom3.h"

#define PI 3.14159265

using namespace std;
using namespace glm;

float eps = 0.0000001;

// taken from https://forums.ashesofthesingularity.com/482281/tutorial-skybox---environment-cube
string skysphere = "skysphere/SkyboxBackdrop3.obj";
string skysphereTexture = "skysphere/SkyboxBackdrop3.png";

string roverObj = "rover/rover15Body.obj";
string roverTexture = "rover/bodyTexture14.png";

string armObj = "rover/rover14Arm.obj";
string armTexture = "rover/armtexture14.png";

string spirit = "spirit/spirit.obj";
string spiritTexture = "spirit/spirit.png";
string spiritHeightMap = "spirit/heightmap.png";

float skyboxScale = 3.0f;


int windowHeight = 500, windowWidth = 1000;


float fov = 45.0f;

mat4 cView;
mat4 roverModelMatrix;
mat4 skyModelMatrix;
mat4 spiritModelMatrix;
mat4 armModelMatrix;
mat4 cProjection;

vec3 cameraPos;
vec3 cameraFront;
vec3 cameraUp;


vec3 roverPosition;
vec3 roverDirection;
vec3 roverUp;
float roverAngleDirection;

float deltaTime = 0.0f;
float lastTime = 0.0f;


float lastX = 500, lastY = 500;
float cYaw = 0.0f, cPitch = 0.0f;
float ambientLightIntensity = 0.1;
vec3 pointLightPosition(100.0f, 100.0f, 100.0f);
vec4 pointLightColor(0.3, 0.3, 0.3,0);
vec4 spotlightColor(1.0f, 1.0f, 1.0f, 0.0f);
vec3 floodLightPosition;
vec3 floodLightDirection;
float floodLightCutoffAngle = cos(radians(5.0f));

vec3 armPosition(0.0f, 0.46031f, 1.40189f);


struct PointLight{
    vec3 position;
    vec4 color;
};

struct DirectionalLight {
    vec3 position;
    vec3 direction;
    float angle;
    vec4 color;
};

PointLight plight = { pointLightPosition, pointLightColor };
DirectionalLight flight = {floodLightPosition, floodLightDirection, floodLightCutoffAngle, spotlightColor};


//string vertexSource =
//    "#version 330 core\n"
//    "layout (location = 0) in vec3 aPos;\n"
//    "out vec4 vertexColor;\n"
//    "out vec4 vsPosition;\n"
//    "void main()\n"
//    "{\n"
//    "   gl_Position = vec4(aPos, 1.0);\n"
//    "   vsPosition = vec4(aPos, 1.0);"
//        "vertexColor = vec4(0.5, 0.0, 0.0, 1.0);"
//    "}";

//string fragmentSource =
//    "#version 330 core\n"
//    "in vec4 vsPosition;\n"
//    "out vec4 FragColor;\n"
//    "uniform vec4 ourColor;\n"
//    "void main()\n"
//    "{\n"
//    "    FragColor = vec4(sin(vsPosition.x) * 2, ourColor.y, sin(vsPosition.y) * 10, 1.0f);\n"
//    "}";


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    cProjection = perspective(radians(45.0f), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
}


vec3 cp3[] = {vec3(-15.0f, -20.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f),
              vec3(-20.0f, 20.0f, 10.0f), vec3(-80.0f, 10.0f, -20.0f),
              vec3(-70.0f, -10.0f, -10.0f), vec3(-90.0f, 45.0f, 0.0f),
              vec3(0.0f, 0.0f, 0.0f), vec3(-130.0f, 20.0f, 20.0f)};

vec3 cameraCoords[] = {vec3(10.0f, 10.0f, 0.0f), vec3(-10.0f, 10.0f, 0.0f), vec3(0.0f, 10.0f, -10.0f),
                     vec3(10.0f, 10.0f, 0.0f), vec3(0.0f, 10.0f, 10.0f), vec3(-10.0f, 10.0f, 0.0f), vec3(0.0f, 10.0f, -10.0f)};

vec3 cameraAngles[] = {vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
                      vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)};


mat4 rotateAroundPoint(mat4 model, float pitch, float yaw, float roll, vec3 point) {
    mat4 nModel = translate(model, -1 * point);
    nModel = rotate(nModel, radians(pitch), vec3(1.0f, 0.0f, 0.0f));
    nModel = rotate(nModel, radians(yaw), vec3(0.0f, 1.0f, 0.0f));
    nModel = rotate(nModel, radians(roll), vec3(0.0f, 0.0f, 1.0f));

    nModel = translate(nModel, 1 * point);

    return nModel;
}

mat4 rotateAroundPoint(mat4 mode, vec3 angles, vec3 point) {
    return rotateAroundPoint(mode, angles.x, angles.y, angles.z, point);
}


bool rotating = false;
float rotationStartTime = 0.0f;
bool cameraAnimating = false;
float cameraAnimationStartTime = 0.0f;
float duration = 8.0f;

Catmullrom3 splineAngles(cp3, sizeof(cp3) / sizeof(*cp3), duration);
Catmullrom3 splineCameraPos(cameraCoords, sizeof(cameraCoords) / sizeof(*cameraCoords), duration);
Catmullrom3 splineCameraAngle(cp3, sizeof(cp3) / sizeof(*cp3), duration);




vec3* insert_into(vec3 array[], int size, int index, vec3 value) {
    vec3* narray = new vec3[size + 1];
    for(int i = 0; i < size + 1; i++) {
        if(i < index) {
            narray[i] = array[i];
        } else if(i == index) {
            narray[i] = value;
        } else {
            narray[i] = array[i];
        }
    }

    return narray;
}

void printArray(vec3 array[], int size) {
    for(int i = 0; i < size; i++) {
        cout << i << " " << glm::to_string(array[i]) << endl;
    }
}





void processInput(GLFWwindow *window) {
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;


    float cameraSpeed = 2.5f * deltaTime;
    float zoomSpeed = 2.5f * deltaTime;
    float moveSpeed = 2.5f * deltaTime;
    float angleMoveSpeed = 30.0f * deltaTime;


    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        cProjection = perspective(radians(45.0f), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        cProjection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotationStartTime = currentTime;
        rotating = true;
        splineAngles.reinitialize(cp3, sizeof(cp3)/sizeof(*cp3), duration);
    }

    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        roverPosition -= vec3(0.0f, 0.0f, 1.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        roverAngleDirection -= angleMoveSpeed;
        roverDirection = vec3(cos(radians(roverAngleDirection)), 0.0f,  -sin(radians(roverAngleDirection)));
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        roverAngleDirection += angleMoveSpeed;
        roverDirection = vec3(cos(radians(roverAngleDirection)), 0.0f,  -sin(radians(roverAngleDirection)));
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        roverPosition += roverDirection * moveSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        roverPosition -= roverDirection * moveSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        rotationStartTime = 0.0f;
        rotating = false;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cameraAnimating) {
        cameraAnimating = true;
        cameraAnimationStartTime = currentTime;

        vec3* ncameraCoords = insert_into(cameraCoords, sizeof(cameraCoords)/sizeof(*cameraCoords), 1, cameraPos);


        splineCameraPos.reinitialize(ncameraCoords, sizeof(cameraCoords)/sizeof(*cameraCoords) + 1, duration);
        splineCameraAngle.reinitialize(ncameraCoords, sizeof(cameraCoords)/sizeof(*cameraCoords) + 1, duration);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        cameraAnimating = false;
        cameraAnimationStartTime = 0.0f;
    }

}





void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(fov >= 1.0f && fov <= 45.0f) {
        fov -= yoffset;
    }
    if(fov <= 1.0f) {
        fov = 1.0f;
    }

    if(fov >= 45.0f) {
        fov = 45.0f;
    }
}

bool firstMouse = true;
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if(!cameraAnimating) {
        if(firstMouse) // this bool variable is initially set to true
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float dX = xpos - lastX;
        float dY = ypos - lastY;

        lastX = xpos;
        lastY = ypos;

        float sens = 0.05f;

        dX *= sens;
        dY *= sens;

        cYaw += dX;
        cPitch += dY;

        if(cPitch > 89.0f) {
            cPitch = 89.0f;
        }

        if(cPitch < -89.0f) {
            cPitch = -89.0f;
        }

        vec3 front = vec3(0.0f);

        front.x = cos(radians(cPitch)) * cos(radians(cYaw));
        front.y = sin(radians(cPitch));
        front.z = cos(radians(cPitch)) * sin(radians(cYaw));

        cameraFront = normalize(front);
    }
}

ModelOBJ::Vertex *sphere(int r, int vertexes) {
    vector<ModelOBJ::Vertex> triangleStrip;

    triangleStrip.push_back(ModelOBJ::Vertex {{0, 0, -1}, {0, 0}, {0, 0, -1}, {1, 1, 0}});
    for(int t = 1; t < vertexes -1; t++) {
        float theta = t/vertexes * PI/2;
        for(int p = 0; p < vertexes; p++) {
            float phi = p / vertexes * PI;
            triangleStrip.push_back(ModelOBJ::Vertex {{
              sin(theta) * cos(phi),
              sin(theta) * sin(phi),
              cos(theta)
            }});
        }
    }
    triangleStrip.push_back(ModelOBJ::Vertex {{0, 0, 1}});
}



int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ca2-Håkon-Osland", nullptr, nullptr);

    if(window == nullptr) {
        cout << "failed to open window " << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK) {
        cout << "error failed to initialize GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    glEnable(GL_DEPTH_TEST);


    Shader shader("res/vert.shader", "res/frag.shader");
    unsigned int shaderId = shader.ID;


    cameraPos   = vec3(0.0f, 5.0f,  -5.0f);
    cameraFront = vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = vec3(0.0f, 1.0f,  0.0f);

    cProjection = mat4(1.0f);

    roverAngleDirection = 0.0f;

    roverPosition = vec3(0.0f, 0.0f, 0.0f);
    roverDirection = vec3(cos(radians(roverAngleDirection)), 0.0f,  -sin(radians(roverAngleDirection)));
    roverUp = vec3(0.0f, 1.0f, 0.0f);


    roverModelMatrix = mat4(1.0f);
    roverModelMatrix = translate(lookAt(roverPosition, roverPosition - roverDirection, roverUp), vec3(0.0f, 1.0f, 0.0f) * 0.5f);


    armPosition = vec3(armPosition.x, -1 * armPosition.z, armPosition.y);

    armModelMatrix = translate(roverModelMatrix, vec3(0.0f));



    skyModelMatrix = mat4(1.0f);
    spiritModelMatrix = mat4(1.0f);
    cView = mat4(1.0f);

    cProjection = perspective(radians(fov), (float)windowWidth/(float)windowHeight, 1.0f, 51.0f);
    cout << glm::to_string(cProjection) << endl;

    flight.direction = cameraFront;
    flight.position = cameraPos;


    ModelOBJ roverModel;

    bool succImport = roverModel.import(roverObj.c_str(), false);

    ModelOBJ armModel;

    succImport = armModel.import(armObj.c_str(), false);

    ModelOBJ skySphereModel;
    succImport = skySphereModel.import(skysphere.c_str(), false);

    ModelOBJ spiritLandingSiteModel;
    succImport = spiritLandingSiteModel.import(spirit.c_str(), true);

    if(!succImport) {
        cout << "could not import model " << endl;
        glfwTerminate();
        return -1;
    }


    Object rover = Object(roverModel, roverTexture, shaderId, roverModelMatrix, GL_TEXTURE0, 0.0f);


    Object arm = Object(armModel, armTexture, shaderId, armModelMatrix, GL_TEXTURE3, 0.0f);

    Object skyObject = Object(skySphereModel, skysphereTexture, shaderId, scale(skyModelMatrix, vec3(skyboxScale, skyboxScale, skyboxScale)), GL_TEXTURE1, 1.0f);
    Object spiritObject = Object(spiritLandingSiteModel, spiritTexture, shaderId, translate(spiritModelMatrix, vec3(0.0f, -0.5f, 0.0f)), GL_TEXTURE2, 0.0f);

    spiritObject.loadHeightMap(spiritHeightMap);

    int frames = 0;

    float itime = glfwGetTime();


    bool firstloop = true;
    while(!glfwWindowShouldClose(window)) {
        // event updates

        frames++;

        if(frames % 600 == 0) {
            float deltaTime =  glfwGetTime() - itime;

            cout << "fps: " << frames / deltaTime << endl;

            itime = glfwGetTime();
            frames = 0;
        }

        processInput(window);

        rover.mmodel = rover.rotateAndScale(roverPosition, roverAngleDirection, mat4(1.0f));
        flight.position = roverPosition;
        flight.direction = roverDirection;

        if(rotating) {
            float deltaTime = glfwGetTime() - rotationStartTime;

            vec3 ra = splineAngles.evaluateSpline(deltaTime);
            arm.mmodel = rotateAroundPoint(mat4(1.0f), ra, armPosition);
            arm.mmodel = rover.mmodel * arm.mmodel;
        } else {
            arm.mmodel = rover.mmodel;
        }
        if(cameraAnimating) {
            float deltaTime = glfwGetTime() - cameraAnimationStartTime;

            cameraPos = splineCameraPos.evaluateSpline(deltaTime);

            cameraPos = vec4(cameraPos, 0) * rover.mmodel;

            cameraFront = normalize(roverPosition - cameraPos);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
        }
        skyObject.mmodel = rotate(scale(mat4(1.0f), vec3(skyboxScale)), -cameraPos.z / 500.0f ,vec3(1.0f, 0.0f, 0.0f));
        skyObject.mmodel = rotate(skyObject.mmodel, cameraPos.x / 500.0f, vec3(0.0f, 0.0f, 1.0f));


        //flight.direction = cameraFront;
        //flight.position = cameraPos;
        cView = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // calculate new position of rover
        //rover.mmodel = translate(lookAt(roverPosition, roverPosition - roverDirection, roverUp), vec3(0.0f, 1.0f, 0.0f) * 0.5f);



        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //vertex and shader initialization





        //setting pointlight properties
        glUniform3fv(glGetUniformLocation(shaderId, "pointlight.position"), 1, &plight.position[0]);
        glUniform4fv(glGetUniformLocation(shaderId, "pointlight.color"), 1, &plight.color[0]);

        //setting spotlight properties
        glUniform3fv(glGetUniformLocation(shaderId, "spotlight.position"), 1, &flight.position[0]);
        glUniform3fv(glGetUniformLocation(shaderId, "spotlight.direction"), 1, &flight.direction[0]);
        glUniform4fv(glGetUniformLocation(shaderId, "spotlight.color"), 1, &flight.color[0]);
        glUniform1f(glGetUniformLocation(shaderId, "spotlight.cutoff"), flight.angle );


        glUniform3fv(glGetUniformLocation(shaderId, "viewPos"), 1, &cameraPos[0]);

        glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, &cProjection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, &cView[0][0]);


        rover.bind(cProjection, cView);
        glDrawElements(GL_TRIANGLES, rover.model.getNumberOfIndices(), GL_UNSIGNED_INT, nullptr);


        arm.bind(cProjection, cView);
        glDrawElements(GL_TRIANGLES, arm.model.getNumberOfIndices(), GL_UNSIGNED_INT, nullptr);



        skyObject.bind(cProjection, cView);
        glDrawElements(GL_TRIANGLES, skyObject.model.getNumberOfIndices(), GL_UNSIGNED_INT, nullptr);


        spiritObject.bind(cProjection, cView);
        glDrawElements(GL_TRIANGLES, spiritObject.model.getNumberOfIndices(), GL_UNSIGNED_INT, nullptr);



        glfwSwapBuffers(window);
        glfwPollEvents();


        if(firstloop) {
            // macbook pro 2016 specific code
            glfwSetWindowSize(window, 1000, 500);
            glfwSetWindowSize(window, windowWidth, windowHeight);
            firstloop = false;
        }

    }

    rover.unbind();
    skyObject.unbind();

    glfwTerminate();
    return 0;

}


