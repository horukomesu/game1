#pragma once
#include "raylib.h"

// --- Определение блока (Voxel)
struct Block {
    unsigned char id; // id типа блока
};

// --- Размер чанка
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_SIZE_Y = 128; // Новая константа для Y
class TerrainGenerator {
public:
    TerrainGenerator();
    virtual ~TerrainGenerator() = default;
    virtual void GenerateChunk(Vector3 chunkPos, Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE]);
protected:
    int worldHeight;
    float noiseScale; // Масштаб шума
    int seaLevel;     // Уровень воды
};
