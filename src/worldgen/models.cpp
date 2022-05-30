#pragma once

#include "PerlinNoise.h"
#include "models.h"

#define TILE_NUMBER 200

float tile_len = 1.0;   // length of each tile. Set to 1 for now for stable working
int tile = TILE_NUMBER;        // No of square tiles row wise(or column). Used to form the terrain
int tiles = (tile + 1) * (1 + tile); //total tiles(tile * tile)

float getHeight(PerlinNoise &pn, float xoff, float yoff);
float interpolate_y(float x, float z, float close1, float close2, float* t1, float* t2);
static PerlinNoise pn;

//Create Terrain with perlin noise
void  makeModels() {

	M1_vertices.resize(3 * tiles);
    pn = PerlinNoise();
	float startx = 0;
	float startz=0;
	double terrain[50000];
	float yoff = 0;
	int c = 0;

	/*for (int a = 0; a < tile + 1; a++) {
		float xoff = 0;

		for (int b = 0; b < tile + 1; b++) {
            terrain[c] = getHeight(pn, xoff, yoff);
			xoff += 0.15;
			c++;
		}

		yoff += 0.15;
	}*/

	for (int i = 0; i < tiles; i++) {

		M1_vertices[i * 3 + 0] =startx+ (i / (tile + 1)) * tile_len; // x of the vertex 0,0,0  (i/(tile+1))*tile_len
		M1_vertices[i * 3 + 2] = startz + (i % (tile + 1)) * tile_len;// z of the vertex 0,1,2
		
		M1_vertices[i * 3 + 1] =  (float)getHeight(pn, M1_vertices[i * 3 + 0], M1_vertices[i * 3 + 2]); // y of the vertex 0
		//std::cout << " val noise " << terrain[i];
		

	}

	// indices (3 * number of triangles)
	M1_indices.resize(3 * tile * tile * 2);
	int j, k;

	for (int i = 0; i < tile * tile; i++) {

		j = i / tile;

		M1_indices[i * 6 + 0] = j + i;
		M1_indices[i * 6 + 1] = j + (i + 1);
		M1_indices[i * 6 + 2] = j + (i + tile + 1);
		M1_indices[i * 6 + 3] = j + (i + tile + 1);// j + (i + 1);
		M1_indices[i * 6 + 4] = j + (i + 1);//j + (i + tile + 1);
		M1_indices[i * 6 + 5] = j + (i + tile + 2);




	}
}

float getHeight(PerlinNoise &pn, float xoff, float yoff) {
	xoff *= 0.15;
	yoff *= 0.15;
    float e = (float)(pn.noise(xoff, 0, yoff) + 2.8 * pn.noise(0.4 * xoff, 0, 0.4 * yoff));//xoff and and yoff defines the frequency of slopes and the multiplication factor 2.5 defines the amplitude or max heights
    return pow(e, 1.55);
}


std::vector<vec3> models::tile_pos(float x ,float y,float z) {
	if (x<M1_vertices[0] || x>M1_vertices[M1_vertices.size() - 3] || z < M1_vertices[2] || z > M1_vertices[M1_vertices.size() - 1]) {
		return { { 0,0,0},{0,0,0},{0,0,0},{10,-std::numeric_limits<float>::infinity(),-10} };

	}
	 // index containing the 2 traingles(forming square tile) corrsponding to point in 3d
	float xx =floor( x );
	float zz = floor(z );
	
	float t1[9] = { xx,getHeight(pn,xx,zz),zz,
		xx,getHeight(pn,xx,zz+1),zz+1,
		xx+1,getHeight(pn,xx+1,zz),zz
	};

	float t2[9] = { xx+1,getHeight(pn,xx+1,zz),zz,
		xx,getHeight(pn,xx,zz + 1),zz + 1,
		xx + 1,getHeight(pn,xx + 1,zz+1),zz+1
	};
	

    //std::cout<<elevation<<std::endl;
	float close1 = pow((pow(x - t1[0], 2) + pow(y - t1[1], 2) + pow(z - t1[2], 2)), 0.5);
	float close2 = pow((pow(x - t2[6], 2) + pow(y - t2[7], 2) + pow(z - t2[8], 2)), 0.5);
	float yy = interpolate_y(x,z,close1, close2, t1,t2);
	return { { 0,0,0},{0,0,0},{0,0,0},{0,yy,0} };
}


float interpolate_y(float x,float z,float close1, float close2, float *t1, float *t2) {
	vec3 p1, p2, p3;


	if (close1 > close2) {
		p1 = { t1[0],t1[1],t1[2] };
		p2 = { t1[3],t1[4],t1[5] };
		p3 = { t1[6],t1[7],t1[8] };


		vec3 v0 = p1 - p2, v1 = p1 - p3;
		vec3 nor = cross(v0, v1); //A,B,C
		float con = nor[0] * p1[0] + nor[1] * p1[1] + nor[2] * p1[2];
		float yy = (con - nor[0] * x - nor[2] * z) / nor[1];

		std::cout << " y" << yy <<" \t orig1 y"<<t1[1] << std::endl;
		return yy;
	}


	p1 = { t2[0],t2[1],t2[2] };
	p2 = { t2[3],t2[4],t2[5] };
	p3 = { t2[6],t2[7],t2[8] };

	vec3 v0 = p1 - p2, v1 = p1 - p3;
	vec3 nor = cross(v0, v1); //A,B,C
	float con = nor[0] * p1[0] + nor[1] * p1[1] + nor[2] * p1[2];
	float yy = (con - nor[0] * x - nor[2] * z) / nor[1];

	std::cout << " y" << yy << " \t orig2 y" << t2[1]<<std::endl;
	return  yy;

}
vec3 models::normalTriangleTile(float x, float y, float z) {
    vec3 normal;
    vec3 U, V;
    std::vector<vec3> triangle_vertex = models::tile_pos(x, y, z);
    U[0] = triangle_vertex[1][0] - triangle_vertex[0][0]; //X difference of P1 and P0
    U[1] = triangle_vertex[1][1] - triangle_vertex[0][1]; //Y difference of P1 and P0
    U[2] = triangle_vertex[1][2] - triangle_vertex[0][2]; //Z difference of P1 and P0


    V[0] = triangle_vertex[2][0] - triangle_vertex[0][0]; //X difference of P2 and P0
    V[1] = triangle_vertex[2][1] - triangle_vertex[0][1]; //Y difference of P2 and P0
    V[2] = triangle_vertex[2][2] - triangle_vertex[0][2]; //Z difference of P2 and P0

    normal[0] = U[1] * V[2] - U[2] * V[1];
    normal[1] = U[2] * V[0] - U[0] * V[2];
    normal[2] = U[0] * V[1] - U[1] * V[0];
    vec3 norm = normalize(normal);

    return norm;
}


