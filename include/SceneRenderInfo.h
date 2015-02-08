#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


struct SceneRenderUniforms {
    glm::mat4 viewMatrix,
              projectionMatrix,
              viewProjMatrix,
              
              inverseViewMatrix,
              inverseProjectionMatrix,
              inverseViewProjMatrix;
    
    glm::vec2 clippingPlanes; // x = near plane, y = far plane
    // aligment, see opengl layout std140
    float _dummy0[2];
    
    glm::vec3 cameraPos;
    float _dummy1[1];
};
