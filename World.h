#pragma once
#include <vector>
#include "chunk.h"
#include "TerrainGenerator.h"
#include <map>
class Chunk; // <-- ��� ��� �����������!

class World {
public:
    std::map<std::pair<int, int>, Chunk*> chunks; // ����: (chunkX, chunkZ)
    int chunkWindowSize; // ��������, 6
    Vector2 cameraPos;   // ��������� ������ (� ������� �����������)
    Texture2D* atlas;
    int tileSize;
    int GetBlockId(int wx, int wy, int wz);
    World(Texture2D* atlas, int tileSize, int chunkWindowSize, TerrainGenerator* generator);
    ~World();
    void Update(Vector3 cameraWorldPos);
    void DrawGeometryPass(Shader* shader);


private:
    void UpdateChunksWindow(int centerChunkX, int centerChunkZ);
    TerrainGenerator* generator;
};
