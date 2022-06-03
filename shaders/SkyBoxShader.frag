#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragTexCoord;
layout(location = 1) in vec2 time;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;

float b1,b2;

void main() {
	vec4 col=texture(skybox,fragTexCoord);

	
	//changes color of the sky based on the angle time.y (set from 0 to 360)

	if(time.y>0 && time.y<=90){
		b1=sin(radians(time.y));
		b2=sin(radians(time.y));
		outColor= col*vec4(0.0,b1,b2,1.0f);
		
	}

	else
		if(time.y>90 && time.y<=180){
			b1=sin(radians(time.y));
		   b2=sin(radians(time.y));
		outColor= col*vec4(0.0,b1,b2,1.0f);
		}

	else{
			outColor= col*vec4(0.01,0.01f,0.01,1.0f);}


//changes color of the sun
	if(col.x>0.7 && col.y>0.7 && col.z<0.1 && time.y>=0 && time.y<=180){
		outColor = vec4(0.9f,0.7f,0.2f,1.0f);}

	
}