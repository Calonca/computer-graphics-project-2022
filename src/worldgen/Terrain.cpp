#pragma once

#include "Terrain.hpp"

float tile_len = 1.0;   // length of each tile. Set to 1 for now for stable working. Do not change.
int tile = Terrain::TILE_NUMBER;        // No of square tiles row wise(or column). Used to form the terrain
int tiles = (tile + 1) * (1 + tile); //total tiles(tile * tile)


//Create Terrain with perlin noise
VertAndIndices Terrain::makeModels() {
    VertAndIndices vAndI;

    vAndI.vertices.resize(3 * tiles);
	float startx = 0;
	float startz=0;

	for (int i = 0; i < tiles; i++) {

		vAndI.vertices[i * 3 + 0] = startx+ (i / (tile + 1)) * tile_len; // x of the vertex 0,0,0  (i/(tile+1))*tile_len
		vAndI.vertices[i * 3 + 2] = startz + (i % (tile + 1)) * tile_len;// z of the vertex 0,1,2
		
		vAndI.vertices[i * 3 + 1] =  (float) getHeight(vAndI.vertices[i * 3 + 0], vAndI.vertices[i * 3 + 2]); // y of the vertex 0
		//std::cout << " val noise " << terrain[i];
		

	}

	// indices (3 * number of triangles)
	vAndI.indices.resize(3 * tile * tile * 2);
	int j;

	for (int i = 0; i < tile * tile; i++) {

		j = i / tile;

		vAndI.indices[i * 6 + 0] = j + i;
		vAndI.indices[i * 6 + 1] = j + (i + 1);
		vAndI.indices[i * 6 + 2] = j + (i + tile + 1);
		vAndI.indices[i * 6 + 3] = j + (i + tile + 1);// j + (i + 1);
		vAndI.indices[i * 6 + 4] = j + (i + 1);//j + (i + tile + 1);
		vAndI.indices[i * 6 + 5] = j + (i + tile + 2);
        
	}
    return vAndI;
}

float Terrain::getHeight(float xoff, float zoff) {
    vec2 p = vec2(xoff, zoff);
    return smoothNoise(p)*50-13;
}

std::vector<vec3> Terrain::tile_pos(float x , float y, float z) {

	 // index containing the 2 traingles(forming square tile) corrsponding to point in 3d
	float xx =floor( x );
	float zz = floor(z );

	float t1[9] = {xx, getHeight(xx, zz), zz,
                   xx, getHeight(xx, zz + 1), zz + 1,
		xx+1, getHeight(xx + 1, zz), zz
	};

	float t2[9] = { xx+1, getHeight(xx + 1, zz), zz,
                    xx, getHeight(xx, zz + 1), zz + 1,
		xx + 1, getHeight(xx + 1, zz + 1), zz + 1
	};


    //std::cout<<elevation<<std::endl;
	float close1 = pow((pow(x - t1[0], 2) + pow(y - t1[1], 2) + pow(z - t1[2], 2)), 0.5);
	float close2 = pow((pow(x - t2[6], 2) + pow(y - t2[7], 2) + pow(z - t2[8], 2)), 0.5);
	float yy = interpolate_y(x,z,close1, close2, t1,t2);
	if(close1>close2)

		return { { t1[0],t1[1],t1[2]},{t1[3],t1[4],t1[5]},{t1[6],t1[7],t1[8]},{0,yy,0}};
	return { { t2[0],t2[1],t2[2]},{t2[3],t2[4],t2[5]},{t2[6],t2[7],t2[8]},{0,yy,0} };

}
float Terrain::interpolate_y(float x,float z,float close1, float close2, float *t1, float *t2) {
	vec3 p1, p2, p3;


	if (close1 > close2) {
		p1 = { t1[0],t1[1],t1[2] };
		p2 = { t1[3],t1[4],t1[5] };
		p3 = { t1[6],t1[7],t1[8] };


		vec3 v0 = p1 - p2, v1 = p1 - p3;
		vec3 nor = cross(v0, v1); //A,B,C
		float con = nor[0] * p1[0] + nor[1] * p1[1] + nor[2] * p1[2];
		float yy = (con - nor[0] * x - nor[2] * z) / nor[1];


		return yy;
	}


	p1 = { t2[0],t2[1],t2[2] };
	p2 = { t2[3],t2[4],t2[5] };
	p3 = { t2[6],t2[7],t2[8] };

	vec3 v0 = p1 - p2, v1 = p1 - p3;
	vec3 nor = cross(v0, v1); //A,B,C
	float con = nor[0] * p1[0] + nor[1] * p1[1] + nor[2] * p1[2];
	float yy = (con - nor[0] * x - nor[2] * z) / nor[1];

	//std::cout << " y" << yy << " \t orig2 y" << t2[1]<<std::endl;
	return  yy;

}


vec3 Terrain::normalTriangleTile(float x, float y, float z) {
    vec3 normal;
    vec3 U, V;
	std::vector<vec3> triangle_vertex;



    triangle_vertex = Terrain::tile_pos(x, y, z);
    U[0] = triangle_vertex[1][0] - triangle_vertex[0][0]; //X difference of P1 and P0
    U[1] = triangle_vertex[1][1] - triangle_vertex[0][1]; //Y difference of P1 and P0
    U[2] = triangle_vertex[1][2] - triangle_vertex[0][2]; //Z difference of P1 and P0


    V[0] = triangle_vertex[2][0] - triangle_vertex[0][0]; //X difference of P2 and P0
    V[1] = triangle_vertex[2][1] - triangle_vertex[0][1]; //Y difference of P2 and P0
    V[2] = triangle_vertex[2][2] - triangle_vertex[0][2]; //Z difference of P2 and P0

	normal = cross(U, V);
    vec3 norm = normalize(normal);

    return norm;
}
