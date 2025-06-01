#pragma once
#include "raylib.h"

class GBuffer {
public:
    GBuffer();
    ~GBuffer();

    bool Init(int width, int height);
    void Bind();
    void Unbind();
    void Destroy();

    unsigned int framebuffer;
    unsigned int positionTexture;
    unsigned int normalTexture;
    unsigned int albedoSpecTexture;
    unsigned int depthRenderbuffer;
    int width, height;
};
