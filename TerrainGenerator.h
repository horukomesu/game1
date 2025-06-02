#pragma once
#include "raylib.h"
#include <raymath.h>

// --- Определение блока (Voxel)
struct Block {
    unsigned char id; // id типа блока
};

// --- Размер чанка
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_SIZE_Y = 128;

class TerrainGenerator {
public:
    TerrainGenerator();
    virtual ~TerrainGenerator() = default;
    virtual void GenerateChunk(Vector3 chunkPos, Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE]);

protected:
    int worldHeight;
    float noiseScale;
    int seaLevel;
};
