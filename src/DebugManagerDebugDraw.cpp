#include "DebugManager.h"
#include "SceneObject.h"
#include "LightObject.h"
#include "DebugDrawer.h"
#include "Root.h"
#include "Scene.h"
#include "SceneNode.h"
#include "SceneGraph.h"
#include "SceneManager.h"

#include "imgui.h"

static const glm::vec4 COLOR_WIREFRAME = glm::vec4(0.1f,1.f,0.5f,1.f),
                       COLOR_NORMAL = glm::vec4(0.5f,0.2f,0.2f,1.f),
                       COLOR_TANGENT = glm::vec4(0.2f,0.5f,0.2f,1.f),
                       COLOR_BITANGENT = glm::vec4(0.2f,0.2f,0.5f,1.f),
                       COLOR_BOUNDS = glm::vec4(0.5f,0.2f,0.2f,1.f),
                       COLOR_INNER_LIGHT_VOLUME = glm::vec4(0.2,0.2f,0.5f,1.f),
                       COLOR_OUTER_LIGHT_VOLUME = glm::vec4(0.2,0.5f,0.2f,1.f),
                       COLOR_PARENT_SCENENODES =  glm::vec4(0.5f,0.5f,0.2f,1.f),
                       COLOR_SAVED_FRUSTRUM =  glm::vec4(0.8f,0.3f,0.8f,1.f);

static const float NORMAL_LENGHT = 0.1f;

void DebugManager::submitDebugDraw()
{
    DebugDrawer *debugDrawer = mRoot->getDebugDrawer();
    
    for( const auto &entry : mDebugDrawInfo ) {
        const DebugDrawInfo &info = entry.second;
        SceneObject *object = entry.first;
        
        if( info.mesh && info.wireFrame ) {
            debugDrawer->drawWireFrame( info.mesh, object->getTransform(), COLOR_WIREFRAME );
        }
        if( info.mesh && info.normals ) {
            debugDrawer->drawVertexNormals( info.mesh, object->getTransform(), NORMAL_LENGHT, COLOR_NORMAL, COLOR_TANGENT, COLOR_BITANGENT );
        }
        if( info.bounds ) {
            showObjectBounds( object, COLOR_BOUNDS );
        }
        if( info.debugLight ) {
            LightObject *light = dynamic_cast<LightObject*>(object);
            if( light ) {
#pragma message "FIXME"
            }
        }
        if( info.parentSceneNodes ) {
            SceneNode *node = object->_getParent();
            while( node ) {
                showSceneNode( node, COLOR_PARENT_SCENENODES );
                node  = node->getParent();
            }
        }
    }

    if( mShowSceneBounds ) {
        SceneManager *sceneMgr = mRoot->getSceneManager();
        Scene *scene = sceneMgr->getScene();
        
        if( scene ) {
            int index = 0;
            scene->forEachObject(
                [&]( SceneObject *object ) {
                    glm::vec4 color( glm::fract(index*0.13f+0.19f), 0.8f, 0.8f, 0.5f );
                    ImGui::ColorConvertHSVtoRGB( color.r, color.g, color.b, color.r, color.g, color.b );
                    
                    showObjectBounds( object, color );
                    index++;
                }
            );
        }
    }
    if( mShowSceneGraph ) {
        SceneManager *sceneMgr = mRoot->getSceneManager();
        Scene *scene = sceneMgr->getScene();
        if( scene ) {
            SceneGraph *graph = scene->getSceneGraph();
            
            std::function<void(int,int,SceneNode*)> visitor;
            
            visitor = [&]( int level, int child, SceneNode *node ) {
                glm::vec4 color( glm::fract(level*0.16f+child*0.05f), 0.8f, 0.8f, 0.5f );
                ImGui::ColorConvertHSVtoRGB( color.r, color.g, color.b, color.r, color.g, color.b );
                
                showSceneNode( node, color );
                
                SceneNode *children = node->getChildren();
                if( children ) {
                    for( int i=0; i < 8; ++i ) {
                        visitor( level+1, i, children+i ); 
                    }
                }
            };
            
            visitor( 0, 0, graph->getRootNode() );
        }
    }
    if( mShowSavedFrustrum ) {
        glm::mat4 modelMatrix = glm::inverse( mSavedViewProjMatrix );
        debugDrawer->drawWireBox( glm::vec3(1.f), modelMatrix, COLOR_SAVED_FRUSTRUM );
    }
}