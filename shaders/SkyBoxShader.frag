#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragTexCoord;
layout(location = 1) in float time;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;



void main() {

	outColor = texture(skybox, fragTexCoord)*vec4(0.1f,0.0f,0.08*time,1.0f);
	
	//outColor = vec4(fragTexCoord.xyz,1.0f);
}