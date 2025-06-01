#pragma once
#include <vector>
#include "raylib.h" // если нужен Vector3 или др. из raylib

// --- Тип блока (текстуры для граней)
struct BlockType {
    int id;            // уникальный id
    int tileIDs[6];    // tileID на атласе для каждой грани (UP, DOWN, FRONT, BACK, RIGHT, LEFT)
};

// --- Список всех типов блоков
extern std::vector<BlockType> blockTypes;

// --- Функция для инициализации блоков
void InitBlockTypes();
