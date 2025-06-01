#include "mouseControl.h"
#include "raymath.h"

MouseControl::MouseControl(World* world_, RTS_Camera* camera_)
    : world(world_), camera(camera_) {
}

// Главная функция обновления
void MouseControl::Update() {
    currentSelection = RaycastBlock(50.0f);

    if (currentSelection.found) {
        // Разрушение (ЛКМ)
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TryDestroyBlock();
        }
        // Строительство (ПКМ)
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            TryPlaceBlock();
        }
    }
}

// Wireframe выделенного блока
void MouseControl::DrawSelection() const {
    if (currentSelection.found) {
        Vector3 center = Vector3Add(currentSelection.blockPos, { 0.5f, 0.5f, 0.5f });
        DrawCubeWires(center, 1.01f, 1.01f, 1.01f, BLACK);
    }
}

// Raycast по миру — шагами 0.1 от камеры вперед
MouseBlockSelection MouseControl::RaycastBlock(float maxDistance) const {
    MouseBlockSelection result;

    Camera3D cam = camera->GetCamera3D();
    Vector2 mouse = GetMousePosition();
    Ray ray = GetMouseRay(mouse, cam);

    Vector3 pos = ray.position;
    Vector3 dir = Vector3Normalize(ray.direction);

    // Текущая позиция в мире (целые координаты)
    int x = (int)floorf(pos.x);
    int y = (int)floorf(pos.y);
    int z = (int)floorf(pos.z);

    // Длина луча до конца блока по каждой оси
    float deltaX = dir.x != 0 ? fabsf(1.0f / dir.x) : 1e30f;
    float deltaY = dir.y != 0 ? fabsf(1.0f / dir.y) : 1e30f;
    float deltaZ = dir.z != 0 ? fabsf(1.0f / dir.z) : 1e30f;

    int stepX = dir.x < 0 ? -1 : 1;
    int stepY = dir.y < 0 ? -1 : 1;
    int stepZ = dir.z < 0 ? -1 : 1;

    float sideDistX = (stepX > 0 ? (x + 1 - pos.x) : (pos.x - x)) * deltaX;
    float sideDistY = (stepY > 0 ? (y + 1 - pos.y) : (pos.y - y)) * deltaY;
    float sideDistZ = (stepZ > 0 ? (z + 1 - pos.z) : (pos.z - z)) * deltaZ;

    float traveled = 0.0f;
    Vector3 faceNormal = { 0, 0, 0 };

    while (traveled < maxDistance) {
        int blockId = world->GetBlockId(x, y, z);
        if (blockId != 0) {
            result.found = true;
            result.blockPos = { (float)x, (float)y, (float)z };
            result.faceNormal = faceNormal;
            return result;
        }

        // Найти ближайшую грань
        if (sideDistX < sideDistY && sideDistX < sideDistZ) {
            traveled = sideDistX;
            sideDistX += deltaX;
            x += stepX;
            faceNormal = { (float)-stepX, 0.0f, 0.0f }; // Знак МИНУС! Minecraft-style
        }
        else if (sideDistY < sideDistZ) {
            traveled = sideDistY;
            sideDistY += deltaY;
            y += stepY;
            faceNormal = { 0.0f, (float)-stepY, 0.0f };
        }
        else {
            traveled = sideDistZ;
            sideDistZ += deltaZ;
            z += stepZ;
            faceNormal = { 0.0f, 0.0f, (float)-stepZ };
        }
    }
    result.found = false;
    return result;
}

// Разрушение блока
void MouseControl::TryDestroyBlock() {
    if (currentSelection.found) {
        int bx = (int)currentSelection.blockPos.x;
        int by = (int)currentSelection.blockPos.y;
        int bz = (int)currentSelection.blockPos.z;
        // Разрушаем — id=0 (air)
        int id = world->GetBlockId(bx, by, bz);
        if (id != 0) {
            // Поиск чанка и блока внутри него
            int cx = bx / CHUNK_SIZE;
            int cz = bz / CHUNK_SIZE;
            if (bx < 0 && bx % CHUNK_SIZE != 0) cx -= 1;
            if (bz < 0 && bz % CHUNK_SIZE != 0) cz -= 1;
            int lx = bx - cx * CHUNK_SIZE;
            int ly = by;
            int lz = bz - cz * CHUNK_SIZE;
            auto it = world->chunks.find({ cx, cz });
            if (it != world->chunks.end()) {
                if (ly >= 0 && ly < CHUNK_SIZE_Y && lx >= 0 && lx < CHUNK_SIZE && lz >= 0 && lz < CHUNK_SIZE) {
                    it->second->blocks[lx][ly][lz].id = 0;
                    it->second->dirty = true;
                }
            }
        }
    }
}

// Строительство блока
void MouseControl::TryPlaceBlock() {
    if (currentSelection.found) {
        // Новый блок ставим к соседу по faceNormal
        Vector3 newPos = Vector3Add(currentSelection.blockPos, currentSelection.faceNormal);
        int bx = (int)newPos.x;
        int by = (int)newPos.y;
        int bz = (int)newPos.z;
        int id = world->GetBlockId(bx, by, bz);
        if (id == 0) { // Место свободно
            // Поиск чанка и блока внутри него
            int cx = bx / CHUNK_SIZE;
            int cz = bz / CHUNK_SIZE;
            if (bx < 0 && bx % CHUNK_SIZE != 0) cx -= 1;
            if (bz < 0 && bz % CHUNK_SIZE != 0) cz -= 1;
            int lx = bx - cx * CHUNK_SIZE;
            int ly = by;
            int lz = bz - cz * CHUNK_SIZE;
            auto it = world->chunks.find({ cx, cz });
            if (it != world->chunks.end()) {
                if (ly >= 0 && ly < CHUNK_SIZE_Y && lx >= 0 && lx < CHUNK_SIZE && lz >= 0 && lz < CHUNK_SIZE) {
                    it->second->blocks[lx][ly][lz].id = 3; // Ставим dirt (id=1)
                    it->second->dirty = true;
                }
            }
        }
    }
}
