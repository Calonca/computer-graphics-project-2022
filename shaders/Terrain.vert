#version 450
#extension GL_ARB_separate_shader_objects : enable
const int TILE = 60;
const float TILESIZE = 1.0f;

layout(binding = 0) uniform UniformBufferObject {
	vec4 ti;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;//Transform the normals in world postion
} ubo;

layout(binding = 3) uniform TerrainUniformBufferObject {
	vec2 translation; //Translation of the terrain on x,z
    vec4 height[TILE+1][TILE+1];//Used for the terrain, x,z
} tubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;

void main() {
	int xTile = int(inPosition.x);
	int zTile = int(inPosition.z);
	//int xTileTranslation = int((translation.x/TILESIZE)/4);
	//int zTileTranslation = int((translation.x/TILESIZE)/4);

	vec2 translatedXZ = vec2(inPosition.x,inPosition.z)+tubo.translation;
	vec4 translatedPos = vec4(translatedXZ.x,tubo.height[xTile][zTile].x,translatedXZ.y, 1.0);
	//translatedPos = vec4(translatedXZ.x,inPosition.y,translatedXZ.y, 1.0);

	gl_Position = ubo.mvpMat * translatedPos;
	fragPos = (ubo.mMat * translatedPos).xyz;
	fragNorm = mat3(ubo.nMat) * inNormal;
	fragTexCoord = inTexCoord;
}
