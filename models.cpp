#include "PerlinNoise.h"
void makeModels() {

	int tile = 20;
	int tiles = (tile + 1) * (1 + tile);
	float tile_len = 1.0;
	M1_vertices.resize(3 * tiles);
	PerlinNoise pn;
	//top face 4
	float startx = -10.0f;
	float startz=-10.0f;
	double terrain[50000];
	float yoff = 0;
	int c = 0;
	double e;
	for (int a = 0; a < tile + 1; a++) {
		float xoff = 0;

		for (int b = 0; b < tile + 1; b++) {
			e = (pn.noise(xoff, yoff, xoff + yoff) + 0.5 * pn.noise(80 * xoff, 80 * yoff, xoff + yoff));
			terrain[c] = pow(e, 1.2);// pn.noise(xoff, yoff, xoff + yoff);//pn.noise(sin(xoff * M_PI*2), cos(yoff* M_PI*2), 0.8);
			std::cout << " terrain val  " << terrain[c];
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

	// Resizes the indices array. Repalce the values with the correct number of
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