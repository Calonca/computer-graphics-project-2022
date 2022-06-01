#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;

void main() {

	const vec3 sunPos = vec3(10.0 * cos(1.5f * 0.025), 2.0, 10.0 * sin(2.3f * 0.025));
	const vec3 sunDir = normalize(sunPos);
	outColor = texture(skybox, fragTexCoord)*vec4(0.0f,0.0f,0.35f,0.0f);
	outColor = mix(outColor, vec4(0.01, 0.3, 0.8, 1.0), (sunDir.xyz,1));
	//outColor = vec4(0.0f,0.1f,1.1f,1.0f);
}