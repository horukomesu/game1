#include "TerrainGenerator.h"
#include <cmath>
#include <raymath.h>
#include <cassert>

static float smoothstep(float edge0, float edge1, float x)
{
    x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

// IDs:
// 0 - воздух
// 1 - земля
// 2 - трава
// 3 - камень
// 4 - вода
// 5 - песок
// 6 - глина
// 7 - гравий
// 8 - лед
// 9 - болото

static inline float getPixelF(const Color* img, int x, int z, int w) {
    assert(x >= 0 && x < w && z >= 0 && z < w);
    return img[z * w + x].r / 255.0f;
}

TerrainGenerator::TerrainGenerator()
    : worldHeight(56), noiseScale(0.04f), seaLevel(44) // повышаем уровень воды!
{
}

void TerrainGenerator::GenerateChunk(Vector3 chunkPos, Block blocks[CHUNK_SIZE][CHUNK_SIZE_Y][CHUNK_SIZE])
{
    int ox = (int)chunkPos.x;
    int oy = (int)chunkPos.y;
    int oz = (int)chunkPos.z;

    Image continentMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox, oz, 0.035f);
    Image hillsMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 1111, oz + 7777, 0.10f);
    Image peaksMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 2222, oz + 8888, 0.18f);
    Image riverMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 3333, oz + 9999, 0.017f);
    Image swampMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 4444, oz + 4444, 0.08f);
    Image iceMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 5555, oz + 1234, 0.045f);
    Image cliffMap = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, ox + 8888, oz + 4321, 0.16f);

    Color* continentPixels = LoadImageColors(continentMap);
    Color* hillsPixels = LoadImageColors(hillsMap);
    Color* peaksPixels = LoadImageColors(peaksMap);
    Color* riverPixels = LoadImageColors(riverMap);
    Color* swampPixels = LoadImageColors(swampMap);
    Color* icePixels = LoadImageColors(iceMap);
    Color* cliffPixels = LoadImageColors(cliffMap);

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            float continent = powf(getPixelF(continentPixels, x, z, CHUNK_SIZE), 1.12f);
            float hills = getPixelF(hillsPixels, x, z, CHUNK_SIZE);
            float peaks = getPixelF(peaksPixels, x, z, CHUNK_SIZE);
            float peakVal = hills * 0.4f + peaks * 0.6f;

            float cliff = getPixelF(cliffPixels, x, z, CHUNK_SIZE);
            float cliffSmooth = smoothstep(0.73f, 0.88f, cliff);
            float cliffPeak = powf(cliff, 2.1f) * 22.0f;

            float riverBase = getPixelF(riverPixels, x, z, CHUNK_SIZE);
            float riverValue = fabs(riverBase - 0.5f) * 2.0f;
            float riverDepth = 1.0f - powf(riverValue, 1.6f);
            float riverShape = smoothstep(0.69f, 0.82f, riverDepth);
            float riverMod = -22.0f * riverShape;

            float swamp = getPixelF(swampPixels, x, z, CHUNK_SIZE);
            float iceZone = getPixelF(icePixels, x, z, CHUNK_SIZE);

            float landBase = (float)seaLevel - 12.0f + continent * (float)(worldHeight + 32 - (seaLevel - 12));
            float peakMod = powf(peakVal, 2.1f) * 22.0f;
            float swampMod = swamp > 0.74f ? (swamp - 0.74f) * 13.0f : 0.0f;

            float finalHeight = landBase + peakMod + riverMod - swampMod;
            finalHeight = (1.0f - cliffSmooth) * finalHeight + cliffSmooth * (landBase + cliffPeak);

            int height = (int)roundf(finalHeight);

            // -- Новая зона для гравия только глубоко под водой (русла с отступом от берега) --
            bool isOcean = continent < 0.18f;
            bool isBeach = (height <= seaLevel + 2 && height >= seaLevel - 3 && !isOcean);
            bool isDeepRiver = (riverDepth > 0.79f && riverDepth < 0.98f && !isOcean && height <= seaLevel - 2); // глубже на 2 блока от воды!
            bool isRiverEdge = (riverDepth > 0.73f && riverDepth <= 0.79f && !isOcean && height <= seaLevel); // переход берег/река
            bool isSwamp = swamp > 0.82f && !isOcean;
            bool isIce = iceZone > 0.81f && !isOcean;
            bool isCliff = (cliffSmooth > 0.55f);

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                int wy = oy + y;
                int b = 0; // Воздух

                if (wy > height) {
                    if (wy <= seaLevel && !isSwamp)
                        b = 4; // вода над всем дном ниже seaLevel!
                    else if (wy <= seaLevel && isSwamp)
                        b = 9;
                    else
                        b = 0;
                }
                else if (wy == height) {
                    if (isCliff)
                        b = 3;
                    else if (isSwamp)
                        b = 9;
                    else if (isDeepRiver)
                        b = 7; // гравий ТОЛЬКО в центре реки и на глубине
                    else if (isRiverEdge)
                        b = 1; // земля или песок по краю реки
                    else if (isBeach)
                        b = 5;
                    else if (isIce)
                        b = 8;
                    else if (wy <= seaLevel + 1)
                        b = 5;
                    else
                        b = 2;
                }
                else if (wy > height - 4) {
                    if (isCliff)
                        b = 3;
                    else if (isSwamp)
                        b = 6;
                    else if (isDeepRiver)
                        b = 7;
                    else if (isRiverEdge)
                        b = 1;
                    else if (isBeach)
                        b = 5;
                    else
                        b = 1;
                }
                else {
                    b = 3;
                }
                blocks[x][y][z].id = (unsigned char)b;
            }
        }
    }

    UnloadImageColors(continentPixels);
    UnloadImageColors(hillsPixels);
    UnloadImageColors(peaksPixels);
    UnloadImageColors(riverPixels);
    UnloadImageColors(swampPixels);
    UnloadImageColors(icePixels);
    UnloadImageColors(cliffPixels);

    UnloadImage(continentMap);
    UnloadImage(hillsMap);
    UnloadImage(peaksMap);
    UnloadImage(riverMap);
    UnloadImage(swampMap);
    UnloadImage(iceMap);
    UnloadImage(cliffMap);
}
