#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragTexCoord;
vec3 positions[3] = vec3[](
    vec3(10.0, 7.5,10),
    vec3(12.5, 8.5,10),
    vec3(11.5,5, 6.5)
);

void main()

{


    fragTexCoord = inPosition;
    vec4 pos = ubo.mvpMat * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
   // gl_Position =(ubo.mvpMat * vec4(positions[gl_VertexIndex],  1.0)).xyww;
}  