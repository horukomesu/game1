#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "blocktypes.h"
#include "World.h"
#include "TerrainGenerator.h"
#include <vector>
class World;
struct ChunkVertex {
    Vector3 pos;
    Vector2 uv;
    float brightness;

};

class Chunk {
public:
    Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE];
    std::vector<ChunkVertex> vertices;

    Vector3 chunkPos;
    bool dirty = true;

    Texture2D* atlas;  
    int tileSize;      







    Chunk(Vector3 pos, Texture2D* atlas, int tileSize, TerrainGenerator* generator, World* world);
    void BuildMesh();
    void Draw();
    void DrawGeometryPass(Shader* shader);

private:
    World* world;
    bool HasBlock(int x, int y, int z);
    void AddFace(Vector3 pos, int face, int tileID);
    TerrainGenerator* generator;
};