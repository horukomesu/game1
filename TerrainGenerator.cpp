#include "TerrainGenerator.h"
#include <raymath.h>
#include <cmath>

TerrainGenerator::TerrainGenerator()
    : worldHeight(32), noiseScale(0.5f), seaLevel(28)
{
}

void TerrainGenerator::GenerateChunk(Vector3 chunkPos, Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE])
{
    // �������� ������-��� ��� ������� ����� (���� x, z)
    // ���������� raylib GenImagePerlinNoise ��� ��������� ����� ����� (2D)
    int offsetX = (int)chunkPos.x;
    int offsetZ = (int)chunkPos.z;
    float scale = noiseScale; // ��� ������ � ��� "����" �����

    // ���������� ����� ����� ��� ������� �����
    Image heightMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, offsetX, offsetZ, scale);

    Color* pixels = LoadImageColors(heightMap);

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            // �������� �������� ���� [0..255], ����������� � ������ ��������
            int pixelIndex = z * CHUNK_SIZE + x;
            unsigned char noiseVal = pixels[pixelIndex].r;
            // ������� ���� � voxel ������, �������� [worldHeight - 8, worldHeight + 8]
            int height = worldHeight + ((int)noiseVal - 128) / 8;

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                int globalY = (int)chunkPos.y + y;

                if (globalY > height) {
                    if (globalY <= seaLevel) {
                        blocks[x][y][z].id = 4; // ���� (��������, id=4, ����� �������� ��� � blocktypes!)
                    }
                    else {
                        blocks[x][y][z].id = 0; // ������
                    }
                }
                else if (globalY == height) {
                    if (globalY <= seaLevel + 1)
                        blocks[x][y][z].id = 5; // ����� (id=5), ���� ������ � ����
                    else
                        blocks[x][y][z].id = 2; // ����� (id=2)
                }
                else if (globalY > height - 3) {
                    blocks[x][y][z].id = 1; // ����� (id=1)
                }
                else {
                    blocks[x][y][z].id = 3; // ������ (id=3)
                }
            }
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(heightMap);
}
