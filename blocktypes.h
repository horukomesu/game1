#pragma once
#include <vector>
#include "raylib.h" // ���� ����� Vector3 ��� ��. �� raylib

// --- ��� ����� (�������� ��� ������)
struct BlockType {
    int id;            // ���������� id
    int tileIDs[6];    // tileID �� ������ ��� ������ ����� (UP, DOWN, FRONT, BACK, RIGHT, LEFT)
};

// --- ������ ���� ����� ������
extern std::vector<BlockType> blockTypes;

// --- ������� ��� ������������� ������
void InitBlockTypes();
