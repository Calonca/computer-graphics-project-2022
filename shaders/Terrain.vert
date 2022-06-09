#version 450
#extension GL_ARB_separate_shader_objects : enable
const int TILE = 120	;
const float TILESIZE = 1.0f;

layout(binding = 0) uniform UniformBufferObject {
	vec4 ti;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;//Transform the normals in world postion
} ubo;

layout(binding = 3) uniform TerrainUniformBufferObject {
	vec2 translation; //Translation of the terrain on x,z
    vec4 height[(TILE+1)*(TILE+1)];//Used for the terrain, x,z
} tubo;

layout(location = 0) in vec3 inPosition;//The y coordinate stores the index to access the height vector
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;


vec3 normal(vec3 p1, vec3 p2, vec3 p3){
	vec3 v1=p3-p1;
	vec3 v2=p3-p2;
	return cross(v1,v2);
}

float getHeight(int heightId){
	//return tubo.height[heightId/4].x;
	int tile4 = int(floor(heightId/4));
	vec4 hvec = tubo.height[tile4];
	int vecIndex = heightId- tile4*4;

	switch (vecIndex) {
		case 0:
		return hvec.x;
		case 1:
		return hvec.y;
		case 2:
		return hvec.z;
		case 3:
		return hvec.w;
	}
	return 0;
}

float clamp(float p, int min,int max){
	if (p<min)
		return min;
	if (p>max)
		return max;
	return p;
}

vec3 vertex_normal(vec3 p){

	//Is vertex it's on the edge calculates the normal of a vertex at distance 1 from the edge
	p.x = clamp(p.x,1,TILE-1);
	p.z = clamp(p.z,1,TILE-1);
	p.y = p.x*(TILE+1)+p.y;

	int xx=int(p.x);
	int zz=int(p.z);
	vec3 p1, p2, p3;

	vec3[6] n;

	p1=vec3(xx, getHeight(xx*(TILE+1)+zz-1), zz-1);
	p2=vec3(xx-1, getHeight((xx-1)*(TILE+1)+zz), zz);
	p3=p;
	n[0]=normal(p1, p2, p);

	p1=vec3(xx-1, getHeight((xx-1)*(TILE+1)+zz), zz);
	p2=vec3(xx-1, getHeight((xx-1)*(TILE+1)+zz+1), zz+1);
	p3=p;
	n[1]=normal(p1, p2, p);


	p1=  vec3(xx-1, getHeight((xx-1)*(TILE+1)+zz+1), zz+1);
	p2=vec3(xx, getHeight((xx)*(TILE+1)+zz+1), zz+1);
	p3=p;
	n[2]=normal(p1, p2, p);

	p1=vec3(xx, getHeight((xx)*(TILE+1)+zz+1), zz+1);
	p2=vec3(xx+1, getHeight((xx+1)*(TILE+1)+zz), zz);
	p3=p;
	n[3]=normal(p1, p2, p);

	p1=vec3(xx+1, getHeight((xx+1)*(TILE+1)+zz), zz);
	p2=vec3(xx+1, getHeight((xx+1)*(TILE+1)+zz-1), zz-1);
	p3=p;
	n[4]=normal(p1, p2, p);

	p1=  vec3(xx+1, getHeight((xx+1)*(TILE+1)+zz-1), zz-1);
	p2= vec3(xx, getHeight((xx)*(TILE+1)+zz-1), zz-1);
	p3=p;
	n[5]=normal(p1, p2, p);

	vec3 norm= (n[0]+n[1]+n[2]+n[3]+n[4]+n[5]);
	return norm;
}

void main() {
	int xTile = int(inPosition.x);
	int zTile = int(inPosition.z);
	//int xTileTranslation = int((translation.x/TILESIZE)/4);
	//int zTileTranslation = int((translation.x/TILESIZE)/4);

	vec2 translatedXZ = vec2(inPosition.x,inPosition.z)+tubo.translation;
	int heightIdx = int(inPosition.y);
	int heightidx2 = (xTile*(TILE+1))+zTile;
	vec4 translatedPos = vec4(translatedXZ.x,getHeight(heightIdx),translatedXZ.y, 1.0);
	//translatedPos = vec4(translatedXZ.x,inPosition.y,translatedXZ.y, 1.0);

	gl_Position = ubo.mvpMat * translatedPos;
	fragPos = (ubo.mMat * translatedPos).xyz;
	fragNorm = mat3(ubo.nMat) * (vertex_normal(inPosition.xyz));
	fragTexCoord = translatedPos.xz*0.1;//The texture cordinates are the position of the vertex in the plane multiplied by the texture scaling
}
