#pragma once

#include "PerlinNoise.h"
#include "models.h"

#define TILE_NUMBER 200

float tile_len = 1.0;   // length of each tile. Set to 1 for now for stable working
int tile = TILE_NUMBER;        // No of square tiles row wise(or column). Used to form the terrain
int tiles = (tile + 1) * (1 + tile); //total tiles(tile * tile)

double getHeight(PerlinNoise &pn, float xoff, float yoff);

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

	for (int a = 0; a < tile + 1; a++) {
		float xoff = 0;

		for (int b = 0; b < tile + 1; b++) {
            terrain[c] = getHeight(pn, xoff, yoff);
			xoff += 0.15;
			c++;
		}

		yoff += 0.15;
	}

	for (int i = 0; i < tiles; i++) {

		M1_vertices[i * 3 + 0] =startx+ (i / (tile + 1)) * tile_len; // x of the vertex 0,0,0  (i/(tile+1))*tile_len
		M1_vertices[i * 3 + 1] =  terrain[i]; // y of the vertex 0
		//std::cout << " val noise " << terrain[i];
		M1_vertices[i * 3 + 2] = startz+(i % (tile + 1)) * tile_len;;// z of the vertex 0,1,2

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

double getHeight(PerlinNoise &pn, float xoff, float yoff) {
    double e = (pn.noise(xoff, 0, yoff) + 1.8 * pn.noise(0.4 * xoff, 0, 0.4 * yoff));//xoff and and yoff defines the frequency of slopes and the multiplication factor 2.5 defines the amplitude or max heights
    return pow(e, 1.2);
}


std::vector<vec3> models::tile_pos(float x ,float y,float z) {
	if (x<M1_vertices[0] || x>M1_vertices[M1_vertices.size() - 3] || z < M1_vertices[2] || z > M1_vertices[M1_vertices.size() - 1]) {
		return { { 0,0,0},{0,0,0},{0,0,0},{10,-std::numeric_limits<float>::infinity(),-10} };

	}

	int pos_index[6]; // index containing the 2 traingles(forming square tile) corrsponding to point in 3d
	float xx =floor( x / tile_len)*tile_len;
	float zz = floor(z / tile_len)*tile_len;

    float elevation = getHeight(pn,xx,zz);
    //std::cout<<elevation<<std::endl;
    return {
            {xx, getHeight(pn,xx,zz),zz},
            {xx, getHeight(pn,xx,zz+tile_len),zz+tile_len},
            {0,0,0},
            {xx+tile_len, elevation,zz+tile_len}
    };
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


