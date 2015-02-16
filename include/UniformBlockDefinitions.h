#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class UniformBlockLayout;

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
    
    glm::vec3 cameraPosition;
    float _dummy1[1];
    
    static const UniformBlockLayout& GetUniformBlockLayout();
};


struct EntityUniforms {
    glm::mat4 modelMatrix;
    
    static const UniformBlockLayout& GetUniformBlockLayout();
};


struct PointLightUniforms {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    glm::vec2 radius;
    float dummy0[2];

    static const UniformBlockLayout& GetUniformBlockLayout();
};

struct SpotLightUniforms {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    glm::vec2 angle, distance;

    static const UniformBlockLayout& GetUniformBlockLayout();
};



