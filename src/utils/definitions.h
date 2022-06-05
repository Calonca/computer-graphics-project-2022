#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

enum PipelineType { Flat, Terrain };
struct RigidBody;

struct VertexDescriptor {
    bool hasPos;
    bool hasNormal;
    bool hasTexCoord;
    bool hasColor;
    bool hasTangent;

    int deltaPos;
    int deltaNormal;
    int deltaTexCoord;
    int deltaColor;
    int deltaTangent;

    int locPos;
    int locNormal;
    int locTexCoord;
    int locColor;
    int locTangent;

    int size;//For example 8. 3 for pos, 3 norm and 2 textCoords
    int loc;

    VertexDescriptor(bool hPos, bool hNormal, bool hTexCoord, bool hColor, bool hTangent) {
        size = 0;
        loc = 0;

        hasPos = hPos;
        hasNormal = hNormal;
        hasTexCoord = hTexCoord;
        hasColor = hColor;
        hasTangent = hTangent;

        if(hasPos) {deltaPos = size; size += 3; locPos = loc; loc++;} else {deltaPos = -1; locPos = -1;}
        if(hasNormal) {deltaNormal = size; size += 3; locNormal = loc; loc++;} else {deltaNormal = -1; locNormal = -1;}
        if(hasTexCoord) {deltaTexCoord = size; size += 2; locTexCoord = loc; loc++;} else {deltaTexCoord = -1; locTexCoord = -1;}
        if(hasColor) {deltaColor = size; size += 4; locColor = loc; loc++;} else {deltaColor = -1; locColor = -1;}
        if(hasTangent) {deltaTangent = size; size += 4; locTangent = loc; loc++;} else {deltaTangent = -1; locTangent = -1;}
    }


    VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = size * sizeof(float);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions(int binding = 0) {
        std::vector<VkVertexInputAttributeDescription>
                attributeDescriptions{};
        attributeDescriptions.resize(loc);
        if(hasPos) {
            attributeDescriptions[locPos].binding = binding;
            attributeDescriptions[locPos].location = locPos;
            attributeDescriptions[locPos].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[locPos].offset = deltaPos * sizeof(float);
        }

        if(hasNormal) {
            attributeDescriptions[locNormal].binding = binding;
            attributeDescriptions[locNormal].location = locNormal;
            attributeDescriptions[locNormal].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[locNormal].offset = deltaNormal * sizeof(float);
        }

        if(hasTexCoord) {
            attributeDescriptions[locTexCoord].binding = binding;
            attributeDescriptions[locTexCoord].location = locTexCoord;
            attributeDescriptions[locTexCoord].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[locTexCoord].offset = deltaTexCoord * sizeof(float);
        }

        if(hasColor) {
            attributeDescriptions[locColor].binding = binding;
            attributeDescriptions[locColor].location = locColor;
            attributeDescriptions[locColor].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[locColor].offset = deltaColor * sizeof(float);
        }

        if(hasTangent) {
            attributeDescriptions[locTangent].binding = binding;
            attributeDescriptions[locTangent].location = locTangent;
            attributeDescriptions[locTangent].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[locTangent].offset = deltaTangent * sizeof(float);
        }

        return attributeDescriptions;
    }

    glm::vec3 getPos(float *data, int i) {
        if(hasPos) {
            return glm::vec3(data[i * size + deltaPos], data[i * size + deltaPos + 1], data[i * size + deltaPos + 2]);
        } else {
            return glm::vec3(0.0f);
            std::cerr << "Vertex has no position \n";
        }
    }

    void setPos(float *data, int i, glm::vec3 pos) {
        if(hasPos) {
            data[i * size + deltaPos] = pos.x;
            data[i * size + deltaPos + 1] = pos.y;
            data[i * size + deltaPos + 2] = pos.z;
        } else {
            std::cerr << "Vertex has no position \n";
        }
    }

    glm::vec3 getNormal(float *data, int i) {
        if(hasPos) {
            return glm::vec3(data[i * size + deltaNormal], data[i * size + deltaNormal + 1], data[i * size + deltaNormal + 2]);
        } else {
            return glm::vec3(0.0f);
            std::cerr << "Vertex has no normal \n";
        }
    }

    void setNormal(float *data, int i, glm::vec3 norm) {
        if(hasNormal) {
            data[i * size + deltaNormal] = norm.x;
            data[i * size + deltaNormal + 1] = norm.y;
            data[i * size + deltaNormal + 2] = norm.z;
        } else {
            std::cerr << "Vertex has no normal \n";
        }
    }

    glm::vec2 getTexCoord(float *data, int i) {
        if(hasPos) {
            return glm::vec2(data[i * size + deltaTexCoord], data[i * size + deltaTexCoord + 1]);
        } else {
            return glm::vec2(0.0f);
            std::cerr << "Vertex has no UV \n";
        }
    }

    void setTexCoord(float *data, int i, glm::vec3 uv) {
        if(hasNormal) {
            data[i * size + deltaTexCoord] = uv.x;
            data[i * size + deltaTexCoord + 1] = uv.y;
        } else {
            std::cerr << "Vertex has no UV \n";
        }
    }
};



struct TextureData {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;
};

struct ModelData {
    VertexDescriptor *vertDesc;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct SceneModel {
    // Model data
    ModelData MD;

    // Texture data
    TextureData TD;

};

struct Model {
	const char* ObjFile;
	const char* TextureFile;
	float scale;
	PipelineType pt;

    SceneModel* pSceneModel;
};

//Collections of points which collision will be tested
struct CollisionObject{
private:
    std::vector<vec3> points;//For now contains only one point
    mat4 t;
public:
    vec3 forceAfterCollision;
    bool isColliding =false;//default
    vec3 normal;
    CollisionObject(std::vector<vec3> points, const mat4 &t, const vec3 &forceAfterCollision, bool isColliding);

    vec3 getGlobalPoint (unsigned int i){
        if (i<0 || i>=points.size())
        {
            std::cout<<"Collision points do not contain the following index: "
            <<i<<"\nThe maximum allowed value is :"<<points.size()-1<<std::endl;
        }
        return t*vec4(points[i],1);
    }

    vec3 getLocalPoint (unsigned int i){
        if (i<0 || i>=points.size())
        {
            std::cout<<"Collision points do not contain the following index: "
                     <<i<<"\nThe maximum allowed value is :"<<points.size()-1<<std::endl;
        }
        return points[i];
    }


    int pointsLen (){
        return points.size();
    }
    void setTransform(RigidBody* r);
};

struct Moment {
    vec3 force;
    vec3 point;
    bool isGlobal;
};

/// <summary>
/// Component used ot hold physics values for entities
/// </summary>
struct RigidBody {
    mat4 transform;
    //float lookYaw;
    //float lookPitch;
    //float lookRoll;
	std::vector<Moment> moments;
	vec3 velocity;
	float mass;//Mass in kg

	vec3 fGravity;//Gravitational force in multiples of g
	bool hasGravity;//True if gravitational force is applied

	float staticFriction;
	float dynamicFriction;
	float bounciness;

    std::vector<CollisionObject> co;
    vec3 angularVelocity;

    //Adds a force to the object at the application point in the Rigidbody reference system.
    //The application point is a point in the local reference system
    void addLocalMoment(vec3 force, vec3 applyPoint){
        moments.push_back({force, applyPoint, false});
    }

    //Adds a force to the object at the application point in the global reference system.
    //Useful for forces like gravity that point always in the same direction regardless of the Rigidbody orientation
    //The application point is a point in the local reference system
    void addGlobalMoment(vec3 force, vec3 applyPoint){
        moments.push_back({force, applyPoint,true});
    }
};



//A volume that when point are in it generates collisions
struct Collider {
    //Function tha when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co) {
        co->isColliding= false;
    }
};


struct PlaneCollider : Collider {
    float planeY{};
    //Function that when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co) {
        float pointY = co->getGlobalPoint(0).y;
        //float planeHeight = planeY+2*po
        if (pointY < planeY){
            co->isColliding= true;
            co->forceAfterCollision=vec3(0,planeY - pointY,0);
            //std::cout<<"Is colliding: "<<co->forceAfterCollision.y;
        }else{
            co->isColliding= false;
            //std::cout<<"Not colliding";
        }
    }
};