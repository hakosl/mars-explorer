#ifndef CATMULLROM_H
#define CATMULLROM_H

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


using namespace std;
using namespace glm;




class Catmullrom {
public:
    mat4 catmullromCoefficients = mat4(-1, 3, -3, 1,
                                       2, -5, 4, -1,
                                       -1, 0, 1, 0,
                                       0, 2, 0, 0);

    float* coefficients;
    float duration;
    int splineParts;

    Catmullrom() {

    }

    Catmullrom(float cp[], int size, float d) {
        coefficients = new float[(size - 3)* 4];
        splineParts = size - 3;


        duration = d;


        for(int i = 0; i < size - 3; i++) {
            vec4 coeffvec = catmullrom(vec4(cp[i], cp[i + 1], cp[i + 2], cp[i + 3]));
            coefficients[i * 4] = coeffvec[0];
            coefficients[i * 4 + 1] = coeffvec[1];
            coefficients[i * 4 + 2] = coeffvec[2];
            coefficients[i * 4 + 3] = coeffvec[3];
        }
    }

    vec4 catmullrom(vec4 controlPoints) {
        return controlPoints * catmullromCoefficients / 2;
    }

    float evaluateSpline(float time) {
        float nt = fmod(time, duration * 2.0f);
        float ntime = 0.0f;

        if(nt > duration) {
            ntime = duration - abs(duration - nt);
        } else {
            ntime = nt;
        }

        float t = ntime / duration;
        int curSpline = t * static_cast<float>(splineParts);

        t = t * splineParts - curSpline;

        vec4 u = vec4(t * t * t, t * t, t, 1);


        float sum = 0;

        for(int i = 0; i < u.length(); i++) {
            sum += u[i] * coefficients[i + curSpline * 4];
        }

        return sum;
    }
};

#endif // CATMULLROM_H
