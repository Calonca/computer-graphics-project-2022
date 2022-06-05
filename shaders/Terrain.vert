#version 450
#extension GL_ARB_separate_shader_objects : enable
const int TILE = 60;
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
	vec3 v1=p1-p2;
	vec3 v2=p1-p3;
	return cross(v1,v2);
}

vec3 vertex_normal(vec3 p){
int xx=int(p.x);
int zz=int(p.z);
		 vec3 p1,p2,p3;


if(xx==0 || zz==0 || xx==TILE || zz==TILE)
{

		if(zz==0){
			if(xx==0){
				p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
				p2=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz].x, zz);
				p3=vec3(xx,tubo.height[(xx)*(TILE+1)+zz+1].x, zz+1);
				
				return normal(p1,p2,p3);
			}
			
			else
			if(xx==TILE){
					vec3[2] n;
					p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
					p2=vec3(xx-1,tubo.height[(xx-1)*(TILE+1)+zz].x, zz);
					p3=vec3(xx-1,tubo.height[(xx-1)*(TILE+1)+zz+1].x, zz+1);
					n[0]=normal(p,p2,p3);

					p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
					p2=vec3(xx,tubo.height[(xx)*(TILE+1)+zz+1].x, zz+1);
					p3=vec3(xx-1,tubo.height[(xx-1)*(TILE+1)+zz+1].x, zz+1);
					n[1]=normal(p,p2,p3);

					vec3 norm=n[0]+n[1]/2;

				return norm;
			}
				

			else{
				vec3[3] n;
				p1=vec3(xx-1, tubo.height[(xx-1)*(TILE+1)+zz].x ,zz);
				p2=vec3(xx-1, tubo.height[(xx-1)*(TILE+1)+zz+1].x ,zz+1);
				p3=p;
				n[0]=normal(p1,p2,p);

				p1=vec3(xx, tubo.height[(xx)*(TILE+1)+zz+1].x ,zz+1);
				p2=vec3(xx-1, tubo.height[(xx-1)*(TILE+1)+zz+1].x ,zz+1);
				p3=p;
				n[1]=normal(p1,p2,p);


				p1=vec3(xx, tubo.height[(xx)*(TILE+1)+zz+1].x ,zz+1);
				p2=vec3(xx+1, tubo.height[(xx+1)*(TILE+1)+zz].x ,zz);
				p3=p;
				n[2]=normal(p1,p2,p);
				vec3 norm=n[0]+n[1]+n[2]/3;
				return norm;
			}
		
		}
		else
		if(xx==0){
			if(zz==TILE){
					vec3[2] n;
					p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
					p2=vec3(xx,tubo.height[(xx)*(TILE+1)+zz-1].x, zz-1);
					p3=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz-1].x, zz-1);
					n[0]=normal(p,p2,p3);

					p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
					p2=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz-1].x, zz-1);
					p3=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz].x, zz);
					n[1]=normal(p,p2,p3);

					vec3 norm=n[0]+n[1]/2;

				return norm;
			}
				

			else{
				vec3[3] n;
				p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
				p2=vec3(xx,tubo.height[(xx)*(TILE+1)+zz-1].x, zz-1);
				p3=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz-1].x, zz-1);
				n[0]=normal(p,p2,p3);

				p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
				p2=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz-1].x, zz-1);
				p3=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz].x, zz);
				n[1]=normal(p,p2,p3);



				
				p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
				p2=vec3(xx+1,tubo.height[(xx+1)*(TILE+1)+zz].x, zz);
				p3=vec3(xx,tubo.height[(xx)*(TILE+1)+zz+1].x, zz+1);
				n[2]=normal(p1,p2,p);
				vec3 norm=n[0]+n[1]+n[2]/3;
				return norm;
			}
		
		}

		else
		if(xx==TILE && zz==TILE){
			    p1=vec3(xx, tubo.height[xx*(TILE+1)+zz].x ,zz);
				p2=vec3(xx-1,tubo.height[(xx-1)*(TILE+1)+zz].x, zz);
				p3=vec3(xx,tubo.height[(xx)*(TILE+1)+zz-1].x, zz-1);
				
				return normal(p1,p2,p3);

		}

}
else
	{
	 vec3[6] n;
	 vec3 p1,p2,p3;

	 p1=vec3(xx, tubo.height[xx*(TILE+1)+zz-1].x ,zz-1);
	 p2=vec3(xx-1, tubo.height[(xx-1)*(TILE+1)+zz].x  ,zz);
	 p3=p;
	 n[0]=normal(p1,p2,p);
	
	 p1=vec3(xx-1,  tubo.height[(xx-1)*(TILE+1)+zz].x    ,zz);
	 p2=vec3(xx-1,  tubo.height[(xx-1)*(TILE+1)+zz+1].x   ,zz+1);
	 p3=p;
	 n[1]=normal(p1,p2,p);


	 p1=vec3(xx,  tubo.height[(xx)*(TILE+1)+zz+1].x   ,zz+1);
	 p2=vec3(xx-1, tubo.height[(xx-1)*(TILE+1)+zz+1].x     ,zz+1);
	 p3=p;
	 n[2]=normal(p1,p2,p);

	 p1=vec3(xx,  tubo.height[(xx)*(TILE+1)+zz+1].x   ,zz+1);
	 p2=vec3(xx+1,  tubo.height[(xx+1)*(TILE+1)+zz].x   ,zz);
	 p3=p;
	 n[3]=normal(p1,p2,p);

	 p1=vec3(xx+1, tubo.height[(xx+1)*(TILE+1)+zz].x   ,zz);
	 p2=vec3(xx+1,  tubo.height[(xx+1)*(TILE+1)+zz-1].x  ,zz-1);
	 p3=p;
	 n[4]=normal(p1,p2,p);

	 p1=vec3(xx,  tubo.height[(xx)*(TILE+1)+zz-1].x   ,zz-1);
	 p2=vec3(xx+1, tubo.height[(xx+1)*(TILE+1)+zz-1].x   ,zz-1);
	 p3=p;
	 n[5]=normal(p1,p2,p);
	 
	 vec3 norm= n[0]+n[1]+n[2]+n[3]+n[4]+n[5]/6;
	 return norm;
	 
	}

}



void main() {
	int xTile = int(inPosition.x);
	int zTile = int(inPosition.z);
	//int xTileTranslation = int((translation.x/TILESIZE)/4);
	//int zTileTranslation = int((translation.x/TILESIZE)/4);

	vec2 translatedXZ = vec2(inPosition.x,inPosition.z)+tubo.translation;
	int heightIdx = int(inPosition.y);
	int heightidx2 = (xTile*(TILE+1))+zTile;
	vec4 translatedPos = vec4(translatedXZ.x,tubo.height[heightIdx].x,translatedXZ.y, 1.0);
	//translatedPos = vec4(translatedXZ.x,inPosition.y,translatedXZ.y, 1.0);

	gl_Position = ubo.mvpMat * translatedPos;
	fragPos = (ubo.mMat * translatedPos).xyz;
	fragNorm = mat3(ubo.nMat) * vertex_normal(inPosition.xyz);
	fragTexCoord = translatedPos.xz*0.1;//The texture cordinates are the position of the vertex in the plane multiplied by the texture scaling
}
