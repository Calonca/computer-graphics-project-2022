#version 450
#extension GL_ARB_separate_shader_objects : enable

precision highp float;

const int TILE = 400	;
const float TILESIZE = 1.0f;

layout(binding = 0) uniform UniformBufferObject {
	vec4 ti;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;//Transform the normals in world postion
} ubo;

layout(binding = 3) uniform TerrainUniformBufferObject {
	vec2 translation; //Translation of the terrain on x,z
} tubo;

layout(location = 0) in vec3 inPosition;//The y coordinate stores the index to access the height vector
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location=3) out vec4 posi;


vec3 normal(vec3 p1, vec3 p2, vec3 p3){
	vec3 v1=p3-p1;
	vec3 v2=p3-p2;
	return cross(v1,v2);
}

float clamp(float p, int min,int max){
	if (p<min)
		return min;
	if (p>max)
		return max;
	return p;
}

//https://www.shadertoy.com/view/4djSRW
float hash(vec2 p)
{
	vec3 p3  = fract(vec3(p.x,p.y,p.x) * 0.1031f);
	p3 += dot(p3, vec3(p3.y,p3.z,p3.y) + 33.33f);
	return fract((p3.x + p3.y) * p3.z);
}

/*
//Todo Compare to see what's faster 
This is from https://www.shadertoy.com/view/XdGfRR
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UI4 uvec4(UI3, 1979697957U)
#define UIF (1.0 / float(0xffffffffU))

float hash12(uvec2 q)
{
	q *= UI2;
	uint n = (q.x ^ q.y) * UI0;
	return float(n) * UIF;
}
float hash12_second(vec2 p)
{
	uvec2 q = uvec2(ivec2(p)) * UI2;
	uint n = (q.x ^ q.y) * UI0;
	return float(n) * UIF;
}*/

//https://github.com/Rudraksha20/CIS565-GPU-Final-Project-Vulkan-Procedural-Terrain
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);

	float res = mix(
	mix(hash(ip),hash(ip+vec2(1.0,0.0)),u.x),
	mix(hash(ip+vec2(0.0,1.0)),hash(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}


// http://flafla2.github.io/2014/08/09/perlinnoise.html
float smoothNoise(vec2 p){
	float total = 0.0;
	float ampl = 100;
	float freq = 1/ampl;
	float maxVal = 0.0;
	for (int i = 0; i < 4; i++) {
		total += noise(p * freq) * ampl;
		maxVal += ampl;
		ampl *= 0.5;
		freq *= 2.0;
	}
	return total / maxVal;
}

float getHeight(vec2 p){
	return smoothNoise(p)*50-13;
}

vec3 getTerrainPoint(float x, float z){
	return vec3(x,getHeight(vec2(x,z)),z);
}

vec3 vertex_normal(vec3 p){
	int xx=int(p.x);
	int zz=int(p.z);
	vec3 p1, p2;

	vec3[6] n;

	p1=getTerrainPoint(xx,zz-1);
	p2=getTerrainPoint(xx-1, zz);
	n[0]=normal(p1, p2, p);

	p1=getTerrainPoint(xx-1, zz);
	p2=getTerrainPoint(xx-1, zz+1);
	n[1]=normal(p1, p2, p);
	
	p1=getTerrainPoint(xx-1, zz+1);
	p2=getTerrainPoint(xx, zz+1);
	n[2]=normal(p1, p2, p);

	p1=getTerrainPoint(xx, zz+1);
	p2=getTerrainPoint(xx+1, zz);
	n[3]=normal(p1, p2, p);

	p1=getTerrainPoint(xx+1, zz);
	p2=getTerrainPoint(xx+1, zz-1);
	n[4]=normal(p1, p2, p);

	p1=getTerrainPoint(xx+1, zz-1);
	p2=getTerrainPoint(xx, zz-1);
	n[5]=normal(p1, p2, p);
	
	return normalize(n[0]+n[1]+n[2]+n[3]+n[4]+n[5]);
}



void main() {
	int xTile = int(inPosition.x);
	int zTile = int(inPosition.z);

	vec2 translatedXZ = vec2(inPosition.x,inPosition.z)+tubo.translation;
	int heightIdx = int(inPosition.y);
	int heightidx2 = (xTile*(TILE+1))+zTile;
	vec4 translatedPos = vec4(translatedXZ.x,getHeight(translatedXZ),translatedXZ.y, 1.0);
	//vec4 translatedPos = vec4(translatedXZ.x,getHeightN(translatedXZ)-getHeight(heightIdx),translatedXZ.y, 1.0);
	//translatedPos = vec4(translatedXZ.x,inPosition.y,translatedXZ.y, 1.0);

	gl_Position = ubo.mvpMat * translatedPos;
	fragPos = (ubo.mMat * translatedPos).xyz;
	fragNorm = mat3(ubo.nMat) * (vertex_normal(translatedPos.xyz));
	fragTexCoord = translatedPos.xz*0.2;//The texture cordinates are the position of the vertex in the plane multiplied by the texture scaling

	posi=vec4(inPosition,1.0f);
}
