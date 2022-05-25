using namespace glm;


// Create a look in direction matrix
// Pos    -> Position of the camera
// Angs.x -> direction (alpha)
// Angs.y -> elevation (beta)
// Angs.z -> roll (rho)
mat4 LookInDirMat(vec3 Pos, vec3 Angs) {
	float alpha = Angs.x;
	float beta = Angs.y;
	float rho = Angs.z;

	mat4 viewMat =
		rotate(mat4(1.0), -rho, vec3(0, 0, 1)) *
		rotate(mat4(1.0), -beta, vec3(1, 0, 0)) *
		rotate(mat4(1.0), -alpha, vec3(0, 1, 0)) *
		translate(mat4(1.0), -Pos);

	return viewMat;
}

// Create a look at matrix
// Pos    -> Position of the camera (c)
// aim    -> Posizion of the target (a)
// Roll   -> roll (rho)
mat4 LookAtMat(vec3 Pos, vec3 aim, float Roll) {
	vec3 up = vec3(0, 1, 0);

	vec3 vz = normalize(Pos - aim);
	vec3 vx = normalize(cross(up, vz));
	vec3 vy = cross(vz, vx);

	mat4 camMat = mat4(vec4(vx, 0), vec4(vy, 0), vec4(vz, 0), vec4(Pos, 1));
	mat4 viewMat = inverse(camMat);

	mat4 rollRot = rotate(mat4(1), -Roll, vec3(0, 0, 1));
	return rollRot * viewMat;
}


