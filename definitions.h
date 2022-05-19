#pragma once
#include <glm/glm.hpp>

enum PipelineType { Flat, Wire };

struct Model {
	const char* ObjFile;
	const char* TextureFile;
	glm::vec3 pos;
	float scale;
	PipelineType pt;
	int id;
};