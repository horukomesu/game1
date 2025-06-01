#include "chunk.h"
// ---- offsets ��� ������ ����� (UP, DOWN, FRONT, BACK, RIGHT, LEFT)
static const Vector3 offsets[6][4] = {
    // UP
    {{0,1,0},{0,1,1},{1,1,1},{1,1,0}},
    // DOWN
    {{0,0,0},{0,0,1},{1,0,1},{1,0,0}},
    // FRONT
    {{0,0,1},{1,0,1},{1,1,1},{0,1,1}},
    // BACK
    {{1,0,0},{0,0,0},{0,1,0},{1,1,0}},
    // RIGHT
    {{1,0,1},{1,0,0},{1,1,0},{1,1,1}},
    // LEFT
    {{0,0,0},{0,0,1},{0,1,1},{0,1,0}}
};

// --- ��������������� �������: ��������� UV ��� tileID �� ������
void GetAtlasUVs(int tileID, int atlasSize, int tileSize, Vector2 uv[4]) {
    int tilesPerRow = atlasSize / tileSize;
    int tx = tileID % tilesPerRow;
    int ty = tileID / tilesPerRow;

    float u0 = (float)(tx * tileSize) / atlasSize;
    float v0 = (float)(ty * tileSize) / atlasSize;
    float u1 = u0 + (float)tileSize / atlasSize;
    float v1 = v0 + (float)tileSize / atlasSize;

    // (�����-���, ������-���, ������-����, �����-����)
    uv[0] = { u0, v1 };
    uv[1] = { u1, v1 };
    uv[2] = { u1, v0 };
    uv[3] = { u0, v0 };
}

Chunk::Chunk(Vector3 pos, Texture2D* atlas_, int tileSize_, TerrainGenerator* generator_, World* world_)
    : chunkPos(pos), atlas(atlas_), tileSize(tileSize_), generator(generator_), world(world_)
{
    generator->GenerateChunk(chunkPos, blocks);
    dirty = true;
}











bool Chunk::HasBlock(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE) {
        int wx = int(chunkPos.x) + x;
        int wy = y;
        int wz = int(chunkPos.z) + z;
        return world->GetBlockId(wx, wy, wz) != 0;
    }
    return blocks[x][y][z].id != 0;
}

void Chunk::BuildMesh() {
    vertices.clear();



    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                if (blocks[x][y][z].id == 0) continue;
                Vector3 pos = { chunkPos.x + x, chunkPos.y + y, chunkPos.z + z };
                // ����� BlockType �� id
                const BlockType& type = blockTypes[blocks[x][y][z].id];
                // 0:UP 1:DOWN 2:FRONT 3:BACK 4:RIGHT 5:LEFT
                if (!HasBlock(x, y + 1, z)) AddFace(pos, 0, type.tileIDs[1]);
                //          if (!HasBlock(x, y - 1, z)) AddFace(pos, 1, type.tileIDs[1]);
                if (!HasBlock(x, y, z + 1)) AddFace(pos, 2, type.tileIDs[2]);
                if (!HasBlock(x, y, z - 1)) AddFace(pos, 3, type.tileIDs[3]);
                if (!HasBlock(x + 1, y, z)) AddFace(pos, 4, type.tileIDs[4]);
                if (!HasBlock(x - 1, y, z)) AddFace(pos, 5, type.tileIDs[5]);



            }
    dirty = false;
}


/*void Chunk::BuildMesh() {
    vertices.clear();
    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                if (blocks[x][y][z].id == 0) continue;
                Vector3 pos = { chunkPos.x + x, chunkPos.y + y, chunkPos.z + z };
                // ����� BlockType �� id
                const BlockType& type = blockTypes[blocks[x][y][z].id];
                // 0:UP 1:DOWN 2:FRONT 3:BACK 4:RIGHT 5:LEFT
                if (!HasBlock(x, y + 1, z)) AddFace(pos, 0, type.tileIDs[0]);
                if (!HasBlock(x, y - 1, z)) AddFace(pos, 1, type.tileIDs[1]);
                if (!HasBlock(x, y, z + 1)) AddFace(pos, 2, type.tileIDs[2]);
                if (!HasBlock(x, y, z - 1)) AddFace(pos, 3, type.tileIDs[3]);
                if (!HasBlock(x + 1, y, z)) AddFace(pos, 4, type.tileIDs[4]);
                if (!HasBlock(x - 1, y, z)) AddFace(pos, 5, type.tileIDs[5]);
            }
    dirty = false;
}*/


void Chunk::AddFace(Vector3 pos, int face, int tileID) {
    Vector2 uv[4];
    GetAtlasUVs(tileID, atlas->width, tileSize, uv);

    for (int i = 0; i < 4; ++i) {
        Vector3 vertPos = Vector3Add(pos, offsets[face][i]);
        vertices.push_back({ vertPos, uv[i], 1.0f });
    }
}

void Chunk::Draw() {



    rlSetTexture(atlas->id);

    rlBegin(RL_QUADS);
    rlColor3f(1.f, 1.f, 1.f); // �����, ����� �� �������� ��������
    for (size_t i = 0; i < vertices.size(); i += 4) {
        for (int j = 0; j < 4; ++j) {
            rlTexCoord2f(vertices[i + j].uv.x, vertices[i + j].uv.y);
            rlVertex3f(vertices[i + j].pos.x, vertices[i + j].pos.y, vertices[i + j].pos.z);
        }
    }
    rlEnd();

    rlSetTexture(0);
}
void Chunk::DrawGeometryPass(Shader* shader) {
    rlSetTexture(atlas->id);

    rlEnableShader(shader->id);

    rlBegin(RL_QUADS);
    rlColor3f(1.f, 1.f, 1.f); // Можно убрать если цвет задается в шейдере
    for (size_t i = 0; i < vertices.size(); i += 4) {
        for (int j = 0; j < 4; ++j) {
            rlTexCoord2f(vertices[i + j].uv.x, vertices[i + j].uv.y);
            rlVertex3f(vertices[i + j].pos.x, vertices[i + j].pos.y, vertices[i + j].pos.z);
        }
    }
    rlEnd();

    rlDisableShader();
    rlSetTexture(0);
}
