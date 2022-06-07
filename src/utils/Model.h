//
// Created by calonca on 6/7/22.
//

#ifndef CGPROJECT_MODEL_H
#define CGPROJECT_MODEL_H

enum PipelineType { Flat, Terrain };
struct Model {
    const char* ObjFile;
    const char* TextureFile;
    float scale;
    PipelineType pt;
};

#endif //CGPROJECT_MODEL_H
