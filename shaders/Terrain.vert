#version 450
#extension GL_ARB_separate_shader_objects : enable
const int TILE = 200;
const float TILESIZE = 1.0f;

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;//Transform the normals in world postion
	float height[TILE][TILE];//Used for the terrain, x,z
	vec2 translation; //Translation of the terrain on x,z
	bool updateTerrain;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;

void main() {
	int xTile = floor(inPosition.x)/4;
	int zTile = floor(inPosition.z)/4;
	//int xTileTranslation = int((translation.x/TILESIZE)/4);
	//int zTileTranslation = int((translation.x/TILESIZE)/4);
	vec2 translatedXZ = vec2(inPosition.x,inPosition.z)+ubo.translation;
	vec4 translatedPos = vec4(translatedXZ.x,ubo.height[xTile][zTile],translatedXZ.y, 1.0);

	gl_Position = ubo.mvpMat * translatedPos;
	//gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
	fragPos = (ubo.mMat * translatedPos).xyz;
	fragNorm = mat3(ubo.nMat) * inNormal;
	fragTexCoord = inTexCoord;
}
