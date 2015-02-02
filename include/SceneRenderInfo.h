#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


struct SceneRenderUniforms {
    glm::mat4 viewMatrix,
              projectionMatrix,
              viewProjMatrix;
};
