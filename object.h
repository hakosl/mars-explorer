#ifndef OBJECT_H
#define OBJECT_H

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

using namespace std;
using namespace glm;

#endif // OBJECT_H

unsigned int objectNumber = 1;
class Object {
private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int shaderId;
    unsigned int textureID;
    float shadeless;
    ModelOBJ::Material objMaterial;

public:
    mat4 mmodel;
    ModelOBJ model;
    unsigned int thisObjectNumber;
    Object(ModelOBJ &model, string textureName, unsigned int shaderId, mat4 mmodel, GLenum textureN, float shadeless) {
        this->model = model;
        this->mmodel = mmodel;
        this->shaderId = shaderId;
        this->thisObjectNumber = objectNumber;
        this->shadeless = shadeless;

        objMaterial = model.getMaterial(0);

        this->textureID = objectNumber;

        glGenTextures(1, &textureID);

        glActiveTexture(textureN);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int textureWidth, textureHeight, textureChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(textureName.c_str(), &textureWidth, &textureHeight, &textureChannels, 0);

        cout << "channels: " << textureChannels << "texture height " << textureHeight << endl;


        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        } else {
            cout << "failed to load image " << textureName << endl;
        }


        VBO = objectNumber;
        glGenBuffers(1, &VBO);

        VAO = objectNumber;
        glGenVertexArrays(1, &VAO);

        EBO = objectNumber;
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * model.getNumberOfIndices(), model.getIndexBuffer(), GL_STATIC_DRAW);



        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(ModelOBJ::Vertex) * model.getNumberOfVertices(), model.getVertexBuffer(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) offsetof(ModelOBJ::Vertex, texCoord));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) offsetof(ModelOBJ::Vertex, normal));
        glEnableVertexAttribArray(2);

        objectNumber++;
    }

    mat4 rotateAndScale(vec3 position, vec3 direction, vec3 up, vec3 front) {
        return translate(transpose(mat4(vec4(front, 0), vec4(up, 0), vec4(direction, 0), vec4(0, 0, 0, 1))), position);
    }

    mat4 rotateAndScale(vec3 position, float xRot, mat4 modelMatrix) {
        return rotate(translate(modelMatrix, position), radians(xRot + 90.0f), vec3(0.0f, 1.0f, 0.0f));
    }
    void bind(mat4 projectionMatrix, mat4 viewMatrix) {
        glUseProgram(shaderId);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * model.getNumberOfIndices(), model.getIndexBuffer(), GL_STATIC_DRAW);



        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(ModelOBJ::Vertex) * model.getNumberOfVertices(), model.getVertexBuffer(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) offsetof(ModelOBJ::Vertex, texCoord));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ModelOBJ::Vertex), (void*) offsetof(ModelOBJ::Vertex, normal));
        glEnableVertexAttribArray(2);


        glUniformMatrix4fv(glGetUniformLocation(shaderId, "pvm"), 1, GL_FALSE, &(projectionMatrix * viewMatrix * mmodel)[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "timodel"), 1, GL_FALSE, &(transpose(inverse(mmodel)))[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, &(mmodel)[0][0]);

        glUniform4fv(glGetUniformLocation(shaderId, "mat.ambient"), 1, &(objMaterial.ambient)[0]);
        glUniform4fv(glGetUniformLocation(shaderId, "mat.diffuse"), 1, &(objMaterial.diffuse)[0]);
        glUniform4fv(glGetUniformLocation(shaderId, "mat.specular"), 1, &(objMaterial.specular)[0]);
        glUniform1f(glGetUniformLocation(shaderId, "mat.shininess"), objMaterial.shininess * 1000.0f);
        glUniform1f(glGetUniformLocation(shaderId, "mat.alpha"), objMaterial.alpha);
        glUniform1f(glGetUniformLocation(shaderId, "mat.shadeless"), this->shadeless);
    }

    unsigned char *heightData;
    int heightMapWidth, heightMapHeight, heightMapTextureChannels;
    float radius = 0.0f;
    float length, width;

    void loadHeightMap(string mapName) {
        int textureWidth, textureHeight, textureChannels;
        heightData = stbi_load(mapName.c_str(), &textureWidth, &textureHeight, &textureChannels, 3);
        heightMapWidth = textureWidth;
        heightMapHeight = textureHeight;
        heightMapTextureChannels = textureChannels;

        width = model.getWidth();
        length = model.getLength();





//        cout << static_cast<int>(heightData[0]) << endl;
//        cout << static_cast<int>(heightData[1024*3 -1]) << endl;
//        cout << static_cast<int>(heightData[1024*1023*3     -1]) << endl;
//        cout << static_cast<int>(heightData[1024*1024*3 -1]) << endl;
//        accessPixel(0, 0, textureChannels, textureHeight);
//        accessPixel(1023, 0, textureChannels, textureHeight);
//        accessPixel(0, 1023, textureChannels, textureHeight);
//        accessPixel(1023, 1023, textureChannels, textureHeight);
        cout << "channels: " << textureChannels << " texture height " << textureHeight << " textureWidth " << textureWidth << endl;
        cout << "ok" << endl;
    }

    float getHeightAt(float x, float y) {

    }

    unsigned char* accessPixel(int i, int j, int channels, int y) {
        unsigned char* pixeloffset = heightData + (i + heightMapHeight*j) * channels;
        return pixeloffset;
    }

    void unbind() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
