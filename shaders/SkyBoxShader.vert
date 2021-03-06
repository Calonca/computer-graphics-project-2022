#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    vec4 ti;
    mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
    

} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragTexCoord;
layout(location = 2) out vec2 time;
void main()
{
    time.x=ubo.ti.x;
    time.y=ubo.ti.y;
    fragTexCoord = inPosition;

    vec4 pos = ubo.mvpMat * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
    fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;
}  