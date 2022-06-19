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

vec3 Phong_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float gamma)  {
	// Phong Specular BRDF model
	// additional parameter:
	// float gamma : exponent of the cosine term

	vec3 reflection = -reflect(L,N);
	float cosViewAndReflection = dotZeroOne(V,reflection);
	float factor = pow(cosViewAndReflection,gamma);
	return C*factor;
}

vec3 fogColor(vec3 rayDir, vec3 sunDir){
	float sunAmount = max(dot( rayDir, sunDir ), 0.0);
	if (sunDir.y < 0.0){//sun is below the horizon
		sunAmount += sunDir.y*2;
		sunAmount= max(sunAmount, 0.0);
	}

	vec3  fogColor  = mix(
	vec3(0.5,0.6,0.7), // bluish
	vec3(1.0,0.9,0.7), // yellowish
	pow(sunAmount,4.0));//Multiply by distance to fake glowing/blooming
	return fogColor;
}

vec3 applySeaFog(
in vec3  rgb,
float density,
float distance,
in vec3  rayDir,   // camera to point vector
in vec3  sunDir )
{
	float factor=0;
	const float LOG2 = -1.442695;
	if(fragPos.y<-3.5){
		distance = gl_FragCoord.z / gl_FragCoord.w * 0.1;//Ovverride distance, it looks better
		float d = density * distance;
		factor = 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);
	}

	return mix(rgb, fogColor(rayDir,sunDir), factor);
}

const float b = 0.0008;

//https://iquilezles.org/articles/fog/
vec3 applyColoredFog( in vec3  rgb,      // original color of the pixel
in float distance, // camera to point distance
in vec3  rayDir,   // camera to point vector
in vec3  sunDir )  // sun light direction
{
	float fogAmount = 1.0 - exp( -distance*b );
	vec3 fogColor = fogColor(rayDir,sunDir);
	return mix( rgb, fogColor, fogAmount );
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);

	vec3 DiffColor = texture(texSampler, fragTexCoord).rgb;
	float AmbFact = 0.01f;

	vec3 diffuse = Lambert_Diffuse_BRDF(gubo.lightDir, Norm, EyeDir, DiffColor);
	//vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(gubo.lightDir, Norm)),0.0f), 16.0f));
	vec3 Ambient = AmbFact * DiffColor;

	vec3 HemiDir =  vec3(0.0f, 1.0f, 0.0f);
	vec3 topColor = vec3(0.3f, 0.3f, 1.0f);
	vec3 bottomColor = vec3(0.3f,1.0f,0.3f);
	vec3 ambient_light = ((dot(Norm,HemiDir)+1.0f)/2)*topColor + ((1.0f-dot(Norm,HemiDir))/2)*bottomColor;
	vec3 specular = Phong_Specular_BRDF(gubo.lightDir, Norm, EyeDir, DiffColor, 64.0f);

	vec3 color = ((diffuse+specular)*gubo.lightColor.xyz)+ (ambient_light*AmbFact*3*DiffColor);
	//vec3 color =  (ambient_light*AmbFact*8*DiffColor);

	//Adding fog
	float distance = length(fragPos - gubo.eyePos);
	color  = applySeaFog(color.rgb,0.55,distance,-EyeDir,gubo.lightDir);
	color  = applyColoredFog(color.rgb,distance,-EyeDir,gubo.lightDir);

	outColor = vec4(color,1.0);

}