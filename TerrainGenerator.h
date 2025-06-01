#pragma once
#include "raylib.h"

// --- ����������� ����� (Voxel)
struct Block {
    unsigned char id; // id ���� �����
};

// --- ������ �����
constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_SIZE_Y = 128; // ����� ��������� ��� Y
class TerrainGenerator {
public:
    TerrainGenerator();
    virtual ~TerrainGenerator() = default;
    virtual void GenerateChunk(Vector3 chunkPos, Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE]);
protected:
    int worldHeight;
    float noiseScale; // ������� ����
    int seaLevel;     // ������� ����
};
