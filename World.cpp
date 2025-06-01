#include "world.h"

World::World(Texture2D* atlas, int tileSize, int chunkWindowSize, TerrainGenerator* generator)
    : chunkWindowSize(chunkWindowSize), atlas(atlas), tileSize(tileSize), generator(generator)
{
}

World::~World() {
    for (auto& kv : chunks) delete kv.second;
}

void World::Update(Vector3 cameraWorldPos) {
    // Ќаходим координаты чанка, где находитс€ камера
    int centerChunkX = static_cast<int>(cameraWorldPos.x) / CHUNK_SIZE;
    int centerChunkZ = static_cast<int>(cameraWorldPos.z) / CHUNK_SIZE;
    UpdateChunksWindow(centerChunkX, centerChunkZ);
    for (auto& kv : chunks) {
        if (kv.second->dirty) kv.second->BuildMesh();
    }
}

int World::GetBlockId(int wx, int wy, int wz) {
    int cx = wx / CHUNK_SIZE;
    int cz = wz / CHUNK_SIZE;
    if (wx < 0 && wx % CHUNK_SIZE != 0) cx -= 1;
    if (wz < 0 && wz % CHUNK_SIZE != 0) cz -= 1;

    int lx = wx - cx * CHUNK_SIZE;
    int ly = wy;
    int lz = wz - cz * CHUNK_SIZE;
    auto it = chunks.find({ cx, cz });
    if (it == chunks.end())
        return 0;
    if (ly < 0 || ly >= CHUNK_SIZE_Y || lx < 0 || lx >= CHUNK_SIZE || lz < 0 || lz >= CHUNK_SIZE)
        return 0;
    return it->second->blocks[lx][ly][lz].id;
}



void World::UpdateChunksWindow(int centerChunkX, int centerChunkZ) {
    int half = chunkWindowSize / 2;
    std::map<std::pair<int, int>, bool> needed;
    for (int dx = -half; dx <= half; ++dx) {
        for (int dz = -half; dz <= half; ++dz) {
            int cx = centerChunkX + dx;
            int cz = centerChunkZ + dz;
            needed[{cx, cz}] = true;
            if (chunks.find({ cx, cz }) == chunks.end()) {
                Vector3 position = { float(cx * CHUNK_SIZE), 0, float(cz * CHUNK_SIZE) };
                chunks[{cx, cz}] = new Chunk(position, atlas, tileSize, generator, this);
            }
        }
    }
    // 2. ”дал€ем все чанки, которые не нужны
    std::vector<std::pair<int, int>> toDelete;
    for (const auto& kv : chunks) {
        if (!needed.count(kv.first)) {
            toDelete.push_back(kv.first);
        }
    }
    for (auto& key : toDelete) {
        delete chunks[key];
        chunks.erase(key);
    }
}

/*void World::Draw() {
    for (auto& kv : chunks) {
        kv.second->Draw();
    }
}*/


void World::DrawGeometryPass(Shader* shader) {
    for (auto& kv : chunks) {
        kv.second->DrawGeometryPass(shader);
    }
}
