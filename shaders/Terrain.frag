#version 450#extension GL_ARB_separate_shader_objects : enablelayout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragTexCoord;layout(location = 3) in vec4 posi;layout(location = 0) out vec4 outColor;layout(binding = 1) uniform sampler2D texSampler;layout(binding = 2) uniform GlobalUniformBufferObject {	vec3 lightDir;	vec4 lightColor;	vec3 eyePos;	vec3 leftSpotlightDir;	vec3 leftSpotlightPos;	vec3 leftSpotlightColor;	vec4 leftSpotlightParams;	vec3 rightSpotlightDir;	vec3 rightSpotlightPos;	vec3 rightSpotlightColor;	vec4 rightSpotlightParams;} gubo;layout(binding = 3) uniform TerrainUniformBufferObject {	vec2 translation; //Translation of the terrain on x,z} tubo;/*float rayMarchShadows(vec3 ro, vec3 rd, float mint, float maxt, vec3 normal) {	// offset ro.y by a small epsilon to handle shadow acne	float epsilon = 1.025;	vec3 newPos = ro + normal * epsilon;	//rd = normalize(vec3(1,-1,0));	float t=10;	newPos = ro + t * -rd;	int xPos = int(newPos.x-tubo.translation.x);	int zPos = int(newPos.z-tubo.translation.y);	float newH = getHeight(xPos*(TILE+1)+zPos);	if(newPos.y<newH) {		return 1.0;	}	return 0.2;	/*	while( newPos.y<60 && newPos.y>-50 && t<100) {		// travel along rd by t		newPos = ro + t * rd;		int xPos = int(newPos.x-tubo.translation.x);		int zPos = int(newPos.z-tubo.translation.y);		float newH = getHeight(xPos*(TILE+1)+zPos);		if(newPos.y<newH) {			return 1.0;		}		t += 2.0;	}}*/float dotZeroOne(vec3 v1,vec3 v2){	return clamp(dot(v1,v2),0,1);}vec3 spot_light_dir(vec3 fragPosition,vec3 lightPos) {	vec3 surfaceToLight = lightPos - fragPosition;	return normalize(surfaceToLight);}vec3 spot_light_color(vec3 fragPosition,vec3 lightDir,vec3 lightPos,vec3 lightColor,vec4 lightParameters) {	vec3 d = lightDir;	float beta = lightParameters.z;	float g = lightParameters.w;	float cosIn = lightParameters.x;	float cosOut = lightParameters.y;	vec3 surfaceToLight = lightPos - fragPosition;	float decay = pow(g/length(surfaceToLight),beta);	decay = clamp(decay, 0, 1);	float coneDimming = (dot(spot_light_dir(fragPosition,lightPos),d)-cosOut)	/	(cosIn-cosOut);	coneDimming = clamp(coneDimming,0,1);	return decay*coneDimming*lightColor;}vec3 Lambert_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C) {	// Lambert Diffuse BRDF model	// in all BRDF parameters are:	// vec3 L : light direction	// vec3 N : normal vector	// vec3 V : view direction	// vec3 C : main color (diffuse color, or specular color)	float refFactor = dotZeroOne(N,L);	return C*refFactor;}vec3 fogColor(vec3 rayDir, vec3 sunDir){	float sunAmount = max(dot( rayDir, sunDir ), 0.0);	if (sunDir.y < 0.0){//sun is below the horizon		sunAmount += sunDir.y*2;		sunAmount= max(sunAmount, 0.0);	}	vec3  fogColor  = mix(		vec3(0.5,0.6,0.7), // bluish		vec3(1.0,0.9,0.7), // yellowish		pow(sunAmount,4.0));//Multiply by distance to fake glowing/blooming	return fogColor;}vec3 applySeaFog(in vec3  rgb,float density,float distance,in vec3  rayDir,   // camera to point vectorin vec3  sunDir ){	float factor=0;	const float LOG2 = -1.442695;	if(fragPos.y<-3.5){		distance = gl_FragCoord.z / gl_FragCoord.w * 0.1;//Ovverride distance, it looks better		float d = density * distance;		factor = 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);	}	return mix(rgb, fogColor(rayDir,sunDir), factor);}const float b = 0.08;//https://iquilezles.org/articles/fog/vec3 applyColoredFog( in vec3  rgb,      // original color of the pixelin float distance, // camera to point distancein vec3  rayDir,   // camera to point vectorin vec3  sunDir )  // sun light direction{	float fogAmount = 1.0 - exp( -distance*b );	return mix( rgb, fogColor(rayDir,sunDir), fogAmount );}/*//https://iquilezles.org/articles/fog/const float a = 0.3;const float fogYTranslate = 5;//Fog y translationvec3 applyExpFog( in vec3  rgb,      // original color of the pixelin float distance, // camera to point distancein vec3  rayOri,   // camera positionin vec3  rayDir )  // camera to point vector{	rayOri.y += fogYTranslate;	float fogAmount = (a/b) * exp(-rayOri.y*b) * (1.0-exp( -distance*rayDir.y*b ))/rayDir.y;	vec3  fogColor  = vec3(0.5,0.6,0.7);	return mix( rgb, fogColor, fogAmount );}*/void main() {	vec3 Norm =normalize(fragNorm);	vec3 EyeDir = normalize(gubo.eyePos - fragPos);	float occ = 1;// rayMarchShadows(fragPos, normalize(gubo.lightDir), -5, 100, Norm);	vec3 leftLightDir = spot_light_dir(fragPos,gubo.leftSpotlightPos);//Light direction for each pixel	vec3 leftLightColor = spot_light_color(fragPos,gubo.leftSpotlightDir,gubo.leftSpotlightPos,gubo.leftSpotlightColor,gubo.leftSpotlightParams);//Light color for each pixel	vec3 rightLightDir = spot_light_dir(fragPos,gubo.rightSpotlightPos);//Light direction for each pixel	vec3 rightLightColor = spot_light_color(fragPos,gubo.rightSpotlightDir,gubo.rightSpotlightPos,gubo.rightSpotlightColor,gubo.rightSpotlightParams);//Light color for each pixel	float AmbientFact = 0.1f;	vec3 Ambient = occ*vec3(1,1,1)*AmbientFact;	vec3 DiffColor = texture(texSampler, fragTexCoord).rgb;	if(fragPos.y>2.8){		DiffColor *= vec3(0.98,0.2,0);	}	float AmbFact = 0.01f;	vec3 Diffuse = Lambert_Diffuse_BRDF(leftLightDir, Norm, EyeDir, DiffColor) * leftLightColor;	Diffuse += Lambert_Diffuse_BRDF(rightLightDir, Norm, EyeDir, DiffColor) * rightLightColor;	Diffuse += Lambert_Diffuse_BRDF(gubo.lightDir, Norm, EyeDir, DiffColor) * gubo.lightColor.rgb;	Ambient = AmbFact * DiffColor;	//outColor =vec4(Norm, 1.0f);	vec3 color = vec3(Diffuse + Ambient);	//outColor = vec4(0,fragPos.y/3.8f,0,1);//	outColor = vec4(texture(texSampler, fragTexCoord).rgb * gubo.lightColor.rgb * (max(dot(Norm, gubo.lightDir),0.0f) * 0.9f + 0.1f), 1.0f);//	outColor = vec4(texture(texSampler, fragTexCoord).rgb * (max(dot(Norm, gubo.lightDir),0.0f) * 0.9f + 0.1f), 1.0f);//	outColor = vec4(vec3(max(dot(Norm, gubo.lightDir),0.0f) * 0.85f + 0.15f), 1.0f);//	outColor = vec4(10.0f,1.0f,0.0f,0.0f);		//const vec4 fogColor = vec4(0.47, 0.5, 0.67, 1.0);	float distance = length(fragPos - gubo.eyePos)/100;	color  = applySeaFog(color.rgb,0.55,distance,-EyeDir,gubo.lightDir);	color  = applyColoredFog(color.rgb,distance,-EyeDir,gubo.lightDir);	outColor = vec4(color,1);	//outColor = vec4(distance,distance,distance,1);	//outColor = vec4(0,(newH+10)/20,0,1);	//outColor=vec4(posi.xyz,1.0);	//outColor =vec4(Norm, 1.0f);}