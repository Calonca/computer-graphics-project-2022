#pragma once

#include "PerlinNoise.h"
#include "models.h"



float tile_len = 1.0;   // length of each tile. Set to 1 for now for stable working
int tile = 200;        // No of square tiles row wise(or column). Used to form the terrain
int tiles = (tile + 1) * (1 + tile); //total tiles(tile * tile)

//Create Terrain with perlin noise
void  makeModels() {

	
	
	M1_vertices.resize(3 * tiles);
	PerlinNoise pn;

	float startx = 0;
	float startz=0;
	double terrain[50000];
	float yoff = 0;
	int c = 0;
	double e;
	for (int a = 0; a < tile + 1; a++) {
		float xoff = 0;

		for (int b = 0; b < tile + 1; b++) {
			e = (pn.noise(xoff, yoff, xoff + yoff) + 2.8 * pn.noise(0.4 * xoff, 0.4 * yoff, xoff + yoff));//xoff and and yoff defines the frequency of slopes and the multiplication factor 2.5 defines the amplitude or max heights
			terrain[c] = pow(e, 1.2);
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



std::vector<vec3> models::tile_pos(float x ,float y,float z) {
	if (x<M1_vertices[0] || x>M1_vertices[M1_vertices.size() - 3] || z < M1_vertices[2] || z > M1_vertices[M1_vertices.size() - 1]) {
		return { { 0,0,0},{0,0,0},{0,0,0},{10,-std::numeric_limits<float>::infinity(),-10} };


	}

	int pos_index[6]; // index containing the 2 traingles(forming square tile) corrsponding to point in 3d
	int xx =floor( x / tile_len);
	int zz = floor(z / tile_len);
	int val = xx * tile + z;
	pos_index[0] = M1_indices[6 * val +0];
	pos_index[1] = M1_indices[6 * val +1];
	pos_index[2] = M1_indices[6 * val +2];
									
	pos_index[3] = M1_indices[6 * val +3];
	pos_index[4] = M1_indices[6 * val +4];
	pos_index[5] = M1_indices[6 * val +5];


	std::vector<vec3> v1, v2; //Vertices of 2 Triangles forming the square tile
	v1.push_back({ // X,Y,Z of triangle 1 vertex 1
		M1_vertices[3 * pos_index[0] + 0],
		M1_vertices[3 * pos_index[0] + 1],
		M1_vertices[3 * pos_index[0] + 2]
	});

	v1.push_back( { // X,Y,Z of triangle 1 vertex 2
		M1_vertices[3 * pos_index[1] + 0],
		M1_vertices[3 * pos_index[1] + 1],
		M1_vertices[3 * pos_index[1] + 2]
	});

	v1.push_back({ // X,Y,Z of triangle 1 vertex 3
		M1_vertices[3 * pos_index[2] + 0],
		M1_vertices[3 * pos_index[2] + 1],
		M1_vertices[3 * pos_index[2] + 2]
	});


	v1.push_back({ // X,Y,Z of centroid of triangle 1
		(v1[0][0] + v1[1][0] + v1[2][0]) / 3,
		(v1[0][1] + v1[1][1] + v1[2][1]) / 3,
		(v1[0][2] + v1[1][2] + v1[2][2]) / 3
	});



	v2.push_back({  // X,Y,Z of triangle 2 vertex 1
		M1_vertices[3 * pos_index[3] + 0],
		M1_vertices[3 * pos_index[3] + 1],
		M1_vertices[3 * pos_index[3] + 2]
	});

	v2.push_back({  // X,Y,Z of triangle 2 vertex 2
		M1_vertices[3 * pos_index[4] + 0],
		M1_vertices[3 * pos_index[4] + 1],
		M1_vertices[3 * pos_index[4] + 2]
	});

	v2.push_back({  // X,Y,Z of triangle 2 vertex 3
		M1_vertices[3 * pos_index[5] + 0],
		M1_vertices[3 * pos_index[5] + 1],
		M1_vertices[3 * pos_index[5] + 2]
	});


	v2.push_back({  // X,Y,Z of centroid of triangle 2
		(v2[0][0] + v2[1][0] + v2[2][0]) / 3,
		(v2[0][1] + v2[1][1] + v2[2][1]) / 3,
		(v2[0][2] + v2[1][2] + v2[2][2]) / 3
	});



	//find which is the closes triangle to the point and return centroid vertices of those triangles.
	float close1 = pow((pow(x-v1[3][0],2)+ pow(y - v1[3][1], 2)+ pow(z - v1[3][2], 2)),0.5);
	float close2 = pow((pow(x - v2[3][0], 2) + pow(y - v2[3][1], 2) + pow(z - v2[3][2], 2)), 0.5);
	
	if (close1 < close2) {

		return v2;
	}
	return v1;
}


vec3 normalTriangleTile(float x, float y, float z) {
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