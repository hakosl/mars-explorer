#ifndef CATMULLROM3_H
#define CATMULLROM3_H

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

#include "catmullrom.h"


using namespace std;
using namespace glm;



class Catmullrom3 {
public:
    float* x;
    float* y;
    float* z;

    Catmullrom xSpline;
    Catmullrom ySpline;
    Catmullrom zSpline;

    Catmullrom3() {}
    Catmullrom3(vec3 cp[], int size, float duration) {
        x = new float[size];
        y = new float[size];
        z = new float[size];

        for(int i = 0; i < size; i++) {
            x[i] = cp[i].x;
            y[i] = cp[i].y;
            z[i] = cp[i].z;
        }


        xSpline = Catmullrom(x, size, duration);
        ySpline = Catmullrom(y, size, duration);
        zSpline = Catmullrom(z, size, duration);
    }

    void reinitialize(vec3 cp[], int size, float duration) {
        x = new float[size];
        y = new float[size];
        z = new float[size];

        for(int i = 0; i < size; i++) {
            x[i] = cp[i].x;
            y[i] = cp[i].y;
            z[i] = cp[i].z;
        }


        xSpline = Catmullrom(x, size, duration);
        ySpline = Catmullrom(y, size, duration);
        zSpline = Catmullrom(z, size, duration);
    }

    vec3 evaluateSpline(float t) {
        return vec3(xSpline.evaluateSpline(t), ySpline.evaluateSpline(t), zSpline.evaluateSpline(t));
    }
};

#endif // CATMULLROM3_H
