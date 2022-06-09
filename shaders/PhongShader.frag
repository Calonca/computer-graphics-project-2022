#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;
	vec3 leftSpotlightDir;
	vec3 leftSpotlightPos;
	vec3 leftSpotlightColor;
	vec4 leftSpotlightParams;
	vec3 rightSpotlightDir;
	vec3 rightSpotlightPos;
	vec3 rightSpotlightColor;
	vec4 rightSpotlightParams;
} gubo;

float dotZeroOne(vec3 v1,vec3 v2){
	return clamp(dot(v1,v2),0,1);
}

vec3 Lambert_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C) {
	// Lambert Diffuse BRDF model
	// in all BRDF parameters are:
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)

	float refFactor = dotZeroOne(N,L);




	return C*refFactor;
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);

	vec3 DiffColor = texture(texSampler, fragTexCoord).rgb;
	float AmbFact = 0.1f;

	vec3 Diffuse = Lambert_Diffuse_BRDF(gubo.lightDir, Norm, EyeDir, DiffColor);
	//vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(gubo.lightDir, Norm)),0.0f), 16.0f));
	vec3 Ambient = AmbFact * DiffColor;
		vec3 HemiDir = vec3(0.0f, 1.0f, 0.0f);
	vec3 ambient_light = ((dot(Norm,HemiDir)+1.0f)/2)*vec3(0,0.0f,0.1f)
			+((1.0f-dot(Norm,HemiDir))/2)*vec3(0.0f,0.1f,0.0f);

	outColor = vec4((Diffuse*gubo.lightColor.xyz+ (ambient_light*AmbFact)), 1.0f);

}