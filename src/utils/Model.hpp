#ifndef CGPROJECT_MODEL_HPP
#define CGPROJECT_MODEL_HPP

enum PipelineType { Flat, TerrainPipe };
struct Model {
    const char* ObjFile;
    const char* TextureFile;
    float scale;
    PipelineType pt;
};

#endif //CGPROJECT_MODEL_HPP
