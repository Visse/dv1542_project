#include "DebugManager.h"
#include "SceneObject.h"
#include "DeferredEntity.h"
#include "LightObject.h"
#include "ComputeParticleSystem.h"
#include "Mesh.h"
#include "RandomMovingObjects.h"
#include "ComputeWater.h"

#include "imgui.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>



void DebugManager::showSceneObject( float dt, SceneObject *object )
{
    DebugDrawInfo &debugDrawInfo = mDebugDrawInfo[object];
    
    glm::vec3 position = object->getPosition();
    
    if( ImGui::InputFloat3("Position", glm::value_ptr(position)) ) {
        object->setPosition( position );
    }
    
    glm::vec3 deltaPos;
    
    if( ImGui::SliderFloat3("Move", glm::value_ptr(deltaPos),-1.f, 1.f) ) {
        position += deltaPos * dt;
        object->setPosition( position );
    }
    
    glm::quat orientation = object->getOrientation();
    glm::vec3 rotation;
    glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(orientation));
    
    if( ImGui::InputFloat3("Orientation", glm::value_ptr(eulerRotation)) ) {
        object->setOrientation( glm::quat(glm::radians(eulerRotation)) );
    }
    if( ImGui::SliderFloat3("Rotate", glm::value_ptr(rotation), -glm::half_pi<float>(), glm::half_pi<float>() ) ) {
        orientation *= glm::quat(rotation*dt);
        object->setOrientation( orientation );
    }
    ImGui::Checkbox( "Show BoundingSphere", &debugDrawInfo.bounds );
    ImGui::SameLine();
    ImGui::Checkbox( "Show Parent Nodes", &debugDrawInfo.parentSceneNodes );
    
    DeferredEntity *entity = dynamic_cast<DeferredEntity*>(object);
    if( entity ) {
        SharedPtr<Mesh> mesh = entity->getMesh();
        const std::string &meshName = mesh->getName();
        
        debugDrawInfo.mesh = mesh;
        
        if( !meshName.empty() ) {
            ImGui::LabelText( "Mesh", meshName.c_str() );
        }
        else {
            ImGui::LabelText( "Mesh", "Unknown" );
        }
        bool castShadow = entity->getCastShadow();
        if( ImGui::Checkbox("Cast Shadows", &castShadow) ) {
            entity->setCastShadow( castShadow );
        }
        
        ImGui::Checkbox( "WireFrame", &debugDrawInfo.wireFrame );
        ImGui::SameLine();
        ImGui::Checkbox( "Normals", &debugDrawInfo.normals );
    }
    
    ComputeParticleSystem *particleSys = dynamic_cast<ComputeParticleSystem*>(object);
    if( particleSys ) {
        float speed = particleSys->getSpeed();
        if( ImGui::SliderFloat("Speed", &speed, 0.f, 4.f) ) {
            particleSys->setSpeed( speed );
        }
        
        float distMod = particleSys->getDistMod();
        if( ImGui::SliderFloat("DistMod", &distMod, 0.f, 10.f) ) {
            particleSys->setDistMod( distMod );
        }
        
        float weightMod = particleSys->getWeightMod();
        if( ImGui::SliderFloat("WeightMod", &weightMod, 0.f, 20.f) ) {
            particleSys->setWeightMod( weightMod );
        }
        
        float lifeTime = particleSys->getLifeTime();
        if( ImGui::SliderFloat("LifeTime", &lifeTime, 0.f, 30.f) ) {
            particleSys->setLifeTime( lifeTime );
        }
        
        float damping = particleSys->getDamping();
        if( ImGui::SliderFloat("Damping", &damping, 0.0f, 2.0f) ) {
            particleSys->setDamping( damping );
        }
        
        float intensity = particleSys->getIntensity();
        if( ImGui::SliderFloat("Intensity", &intensity, 0.f, 5.f) ) {
            particleSys->setIntensity( intensity );
        }
        
        float pointSize = particleSys->getPointSize();
        if( ImGui::SliderFloat("PointSize", &pointSize, 0.f, 15.f) ) {
            particleSys->setPointSize( pointSize );
        }
        
        int groupCount = particleSys->getParticleGroupCount();
        int maxGroups = particleSys->getMaxParticleGroupCount();
        if( ImGui::SliderInt("Amount", &groupCount, 1, maxGroups) ) {
            particleSys->setParticleGroupCount( groupCount );
        }
        
        int attractorCount = particleSys->getAttractorCount();
        int maxAttractors = particleSys->getMaxAttractorCount();
        if( ImGui::SliderInt("Attractors", &attractorCount, 0, maxAttractors ) ) {
            particleSys->setAttractorCount( attractorCount );
        }
        
        bool showAttractors = particleSys->getShowAttractors();
        if( ImGui::Checkbox( "Show Attractors", &showAttractors) ) {
            particleSys->setShowAttractors( showAttractors );
        }
        
        unsigned int particleCount = particleSys->getParticleCount();
        ImGui::Value( "Particle Count", particleCount );
    }
    
    
    RandomMovingObjects *randomMovingObjects= dynamic_cast<RandomMovingObjects*>(object);
    if( randomMovingObjects ) {
        int objectCount = randomMovingObjects->getObjectCount();
        float timeMultiplier = randomMovingObjects->getTimeMultipler();
        float radius = randomMovingObjects->getRadius();
        
        if( ImGui::SliderInt( "ObjectCount", &objectCount, 0, 50) ) {
            randomMovingObjects->setObjectCount( objectCount );
        }
        if( ImGui::SliderFloat( "TimeMultiplier", &timeMultiplier, -1.f, 3.f) ) {
            randomMovingObjects->setTimeMultipler(timeMultiplier);
        }
        if( ImGui::SliderFloat( "Radius", &radius, 0.f, 10.f) ) {
            randomMovingObjects->setRadius( radius );
        }
        
        if( ImGui::Button("Recreate objects") ) {
            unsigned int count = randomMovingObjects->getObjectCount();
            // destroy old objects
            randomMovingObjects->setObjectCount( 0 );
            // recreate objects
            randomMovingObjects->setObjectCount( count );
        }
        
        SceneObject *templateObject = randomMovingObjects->getTemplate();
        if( templateObject ) {
            if( ImGui::TreeNode("Template") ) {
                showSceneObject( dt, templateObject );
                ImGui::TreePop();
            }
        }
    }
    
    LightObject *light = dynamic_cast<LightObject*>(object);
    if( light ) {
        glm::vec3 color = light->getColor();
        if( ImGui::ColorEdit3("Color", glm::value_ptr(color)) ) {
            light->setColor( color );
        }
        bool castShadows = light->getCastShadow();
        if( ImGui::Checkbox("Cast Shadows", &castShadows) ) {
            light->setCastShadow( castShadows );
        }
        
        if( PointLight *pointLight = dynamic_cast<PointLight*>(light) ) {
            float outerRadius = pointLight->getOuterRadius(),
                    innerRadius = pointLight->getInnerRadius(),
                    intesity = pointLight->getIntensity();
                
            if( ImGui::SliderFloat("OuterRadius", &outerRadius, 0.f, 20.f) ) {
                pointLight->setOuterRadius( outerRadius );
            }
            if( ImGui::SliderFloat("InnerRadius", &innerRadius, 0.f, outerRadius) ) {
                pointLight->setInnerRadius( innerRadius );
            }
            if( ImGui::SliderFloat("Intensity", &intesity, 0.f, 2.f) ) {
                pointLight->setIntensity( intesity );
            }
        }
        
        if( SpotLight *spotLight = dynamic_cast<SpotLight*>(light) ) {
            float outerAngle = spotLight->getOuterAngle(),
                    innerAngle = spotLight->getInnerAngle(),
                    outerDist  = spotLight->getOuterDistance(),
                    innerDist  = spotLight->getInnerDistance(),
                    intensity = spotLight->getIntensity();
                    
            if( ImGui::SliderAngle("OuterAngle", &outerAngle, 0.f, 90.f) ) {
                spotLight->setOuterAngle( outerAngle );
            }
            if( ImGui::SliderAngle("InnerAngle", &innerAngle, 0.f, glm::degrees(outerAngle)) ) {
                spotLight->setInnerAngle( innerAngle );
            }
            if( ImGui::SliderFloat("OuterDistance", &outerDist, 0.f, 20.f) ) {
                spotLight->setOuterDistance( outerDist );
            }
            if( ImGui::SliderFloat("InnerDistance", &innerDist, 0.f, outerDist) ) {
                spotLight->setInnerDistance( innerDist );
            }
            if( ImGui::SliderFloat("Intensity", &intensity, 0.f, 2.f) ) {
                spotLight->setIntensity( intensity );
            }
        }
    
        ImGui::Checkbox( "Light Volume", &debugDrawInfo.debugLight );
    }

    if( ComputeWater *water = dynamic_cast<ComputeWater*>(object) ) {
        ImGui::Checkbox( "Show SimTexture", &debugDrawInfo.showComputeWaterTexture );
        bool useWireFrame = water->getUseWireFrame();
        float heightScale = water->getHeightScale();
        float waterSizeScale = water->getWaterSizeScale();
        float lodScale = water->getLODScale();
        float frensel = water->getFrensel();
        float frenselFalloff = water->getFrenselFalloff();
        glm::vec3 color = water->getWaterColor();
        
        if( ImGui::Checkbox("Use Wireframe", &useWireFrame) ) {
            water->setUseWireFrame( useWireFrame );
        }
        if( ImGui::SliderFloat("Height Scale", &heightScale, 0.0, 2.0) ) {
            water->setHeightScale( heightScale );
        }
        if( ImGui::ColorEdit3("Color", glm::value_ptr(color)) ) {
            water->setWaterColor( color );
        }
        if( ImGui::SliderFloat("Size Scale", &waterSizeScale, 0.0, 10.0) ) {
            water->setWaterSizeScale( waterSizeScale );
        }
        if( ImGui::SliderFloat("LOD Scale", &lodScale, 0.0, 4.0) ) {
            water->setLODScale( lodScale );
        }
        if( ImGui::SliderFloat("Frensel", &frensel, 0.0, 3.0) ) {
            water->setFrensel( frensel );
        }
        if( ImGui::SliderFloat("Frensel Falloff", &frenselFalloff, 0.0, 20.0) ) {
            water->setFrenselFalloff( frenselFalloff );
        }
    }
    
}
