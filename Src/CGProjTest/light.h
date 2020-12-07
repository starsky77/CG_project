#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

using namespace std;

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

class Light
{
public:
    Shader lightingShader;
    Shader lightCubeShader;

    Light(glm::vec3 LightPositions[], int num);
    void SetShaderValue(Camera camera, unsigned int lightCubeVAO, glm::mat4& model);


private:
    glm::vec3* pointLightPositions;
    int LightNum;
};

#endif