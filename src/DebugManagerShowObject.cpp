#include "DebugManager.h"
#include "SceneObject.h"
#include "DeferredEntity.h"
#include "LightObject.h"
#include "ComputeParticleSystem.h"
#include "Mesh.h"


#include "imgui.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


#ifdef __GNUC__

#include <cxxabi.h>

std::string demangleName( const char *name )
{
    char *demangledName = __cxxabiv1::__cxa_demangle( name, NULL, NULL, NULL );
    std::string tmp(demangledName);
    
    free(demangledName);
    
    return tmp;
}

#else

std::string demangleName( const char *name )
{
    return std::string(name);
}

#endif


void DebugManager::showSceneObject( float dt, SceneObject *object )
{
    ImGui::PushID( object );
    
    DebugDrawInfo &debugDrawInfo = mDebugDrawInfo[object];
    
    std::string objectType = demangleName(typeid(*object).name());
    
    if( ImGui::TreeNode(objectType.c_str()) ) {
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
        
        LightObject *light = dynamic_cast<LightObject*>(object);
        if( light ) {
            glm::vec3 color = light->getColor();
            if( ImGui::ColorEdit3("Color", glm::value_ptr(color)) ) {
                light->setColor( color );
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
        ImGui::TreePop();
    }

    ImGui::PopID();
}
