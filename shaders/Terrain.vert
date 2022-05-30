#version 450
#extension GL_ARB_separate_shader_objects : enable
const int TILE = 200;
const float TILESIZE = 1.0f;

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
	float height[TILE][TILE];//Used for the terrain, x,z
	vec2 translation; //Translation of the terrain on x,z
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;

void main() {
	int xTile = floor(inPosition.x);
	int zTile = floor(inPosition.z);
	//gl_Position = ubo.mvpMat * vec4(inPosition.x,ubo.height[xTile][zTile],inPosition.z, 1.0);
	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
	fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;
	fragNorm = mat3(ubo.nMat) * inNormal;
	fragTexCoord = inTexCoord;
}
