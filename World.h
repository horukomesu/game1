#pragma once
#include <vector>
#include "chunk.h"
#include "TerrainGenerator.h"
#include <map>
class Chunk; // <-- вот это обязательно!

class World {
public:
    std::map<std::pair<int, int>, Chunk*> chunks; // ключ: (chunkX, chunkZ)
    int chunkWindowSize; // например, 6
    Vector2 cameraPos;   // положение камеры (в мировых координатах)
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
