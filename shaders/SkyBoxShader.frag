#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragTexCoord;
layout(location = 2) in vec2 time;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;
float b1,b2;

//https://www.shadertoy.com/view/4djSRW
float hash2to1(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}

//https://github.com/Rudraksha20/CIS565-GPU-Final-Project-Vulkan-Procedural-Terrain
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);

	float res = mix(
	mix(hash2to1(ip),hash2to1(ip+vec2(1.0,0.0)),u.x),
	mix(hash2to1(ip+vec2(0.0,1.0)),hash2to1(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}
// http://flafla2.github.io/2014/08/09/perlinnoise.html
float smoothNoise(vec2 p){
	float total = 0.0;
	float ampl = 0.2;
	float freq = 1/ampl;
	float maxVal = 0.0;
	for (int i = 0; i < 6; i++) {
		total += noise(p * freq) * ampl;
		maxVal += ampl;
		ampl *= 0.5;
		freq *= 2.0;
	}
	return total / maxVal;
}


float dotZeroOne(vec3 v1,vec3 v2){
	return clamp(dot(v1,v2),0,1);
}

vec3 lambert(vec3 L, vec3 N, vec3 C) {
	return C*dotZeroOne(N,L);
}


vec3 getSphereCoordinates(vec2 point,float radius)
{
	vec3 sphereCoordinates;
	sphereCoordinates.x = point.x;
	sphereCoordinates.y = point.y;
	sphereCoordinates.z = sqrt(radius*radius - sphereCoordinates.x*sphereCoordinates.x - sphereCoordinates.y*sphereCoordinates.y);
	return sphereCoordinates;
}

vec4 moon(vec4 color,float radius,vec3 lightDir){

	vec3 sphereCoords = getSphereCoordinates(fragTexCoord.xy,radius);
	float dist = length(sphereCoords.xy);
	if (dist<radius){
		vec2 rotation = vec2(0,-time.x/20);
		float sphereDistortion = pow(1+dist,2.2f);
		vec3 moonColor = vec3(1.0f,1.0f,1.0f)*smoothNoise((sphereCoords.xy*sphereDistortion)+rotation);
		moonColor *= 9.0f;
		vec3 diffuse = lambert(lightDir,sphereCoords,moonColor);
		vec3 ambient = moonColor*0.01;
		return vec4(diffuse+ambient,1);
	}
	else{
		return color;
	}
}

//https://www.shadertoy.com/view/MsVSWt
vec3 getSky(vec3 sunPos)
{
	float atmosphere = sqrt(1.0-fragPos.y);
	vec3 skyColor = vec3(0.2,0.4,0.8);

	float scatter = pow(sunPos.y,1.0 / 15.0);
	scatter = 1.0 - clamp(scatter,0.8,1.0);

	vec3 scatterColor = mix(skyColor,vec3(1.0,0,0.0) * 2.5,scatter);
	if (sunPos.y<0.0){
		//return vec3(0.0,0.0,0.0);
		scatterColor = mix(scatterColor,vec3(0),-sunPos.y*7);
	}
	return scatterColor;
}

//https://www.shadertoy.com/view/MsVSWt
vec4 sun(float radius,vec3 sunPos){

	if (fragPos.y<0.0){
		return vec4(0);
	}

	vec3 sphereCoords = getSphereCoordinates(fragTexCoord.xy,radius);
	float dist = length(vec3(sphereCoords.xy,fragTexCoord.z-1));

	float sun = 1.0 - dist/radius;
	sun = clamp(sun,0.0,1.0);
	float glow = sun;

	glow = pow(glow,6.0) * 1.0;
	glow = clamp(glow,0.0,1.0);
	glow *= pow(fragPos.y,1.2);

	sun = pow(sun,100.0);
	sun *= 100.0;
	sun = clamp(sun,0.0,1.0);
	sun *= pow(fragPos.y, 2.0 / 1.65);

	sun += glow;

	vec3 sunColor = vec3(1.0,0.55,0.04) * sun;
	//sunColor = sunColor-0.1;
	if (sunPos.y<0.0){
		sunColor = mix(sunColor,vec3(0),pow(-sunPos.y,1/4));
	}
	return vec4(sunColor,1);
}

void main() {
	vec4 col=texture(skybox,fragTexCoord);
	vec3 sunPos = vec3(0,sin(radians(time.y)),-cos(radians(-time.y)));
	vec3 lightDir = -sunPos;

	float sunRadius = 2;

	outColor = vec4(getSky(sunPos),1);
	outColor += sun(sunRadius, sunPos);

	//Add moon
	float moonRadius = 0.4;
	if (fragTexCoord.z<-0.9){
		outColor = moon(outColor,moonRadius,lightDir);
	}

	
}