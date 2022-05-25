#include "PerlinNoise.h"


float tile_len = 1.0;
int tile = 2;
int tiles = (tile + 1) * (1 + tile);
void makeModels() {

	
	
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
			e = (pn.noise(xoff, yoff, xoff + yoff) + 0.5 * pn.noise(80 * xoff, 80 * yoff, xoff + yoff));
			terrain[c] = pow(e, 1.2);// pn.noise(xoff, yoff, xoff + yoff);//pn.noise(sin(xoff * M_PI*2), cos(yoff* M_PI*2), 0.8);
			//std::cout << " terrain val  " << terrain[c];
			xoff += 0.15;
			c++;
		}

		yoff += 0.15;
	}

	for (int i = 0; i < tiles; i++) {

		M1_vertices[i * 3 + 0] =startx+ (i / (tile + 1)) * tile_len; // x of the vertex 0,0,0  (i/(tile+1))*tile_len
		M1_vertices[i * 3 + 1] =  terrain[i]; // y of the vertex 0
		std::cout << " val noise " << terrain[i];
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

void tile_pos(float x ,float y,float z) { 
	
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

	//to access corresponding vertices of the triangle
	for (int i = 0; i < 6; i++) { 
		std::cout << "\n vertex value x \t" << M1_vertices[3 * pos_index[i] + 0];
		std::cout << "\n vertex value y \t" << M1_vertices[3 * pos_index[i] + 1];
		std::cout << "\n vertex value z \t" << M1_vertices[3 * pos_index[i] + 2];
	}

}