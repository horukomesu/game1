#pragma once
#include "raylib.h"
#include "GBuffer.h"

enum class DeferredPassMode {
    POSITION, NORMAL, ALBEDO, SHADING
};

class DeferredRenderer {
public:
    DeferredRenderer();
    ~DeferredRenderer();

    bool Init(int screenWidth, int screenHeight, const char* shaderPath);
    void Destroy();

    void RenderLightingPass(GBuffer& gBuffer, const Camera& camera, DeferredPassMode mode);

    Shader geometryShader;
    Shader deferredShader;

    int texUnitPosition;
    int texUnitNormal;
    int texUnitAlbedoSpec;

    // Ќовые: локации uniform дл€ освещени€
    int sunDirLoc, sunColorLoc, ambientLoc;

    // ѕараметры освещени€ (public, чтобы можно было легко мен€ть извне, например дл€ анимации солнца)
    Vector3 sunDir;
    Vector3 sunColor;
    Vector3 ambientColor;

    DeferredPassMode mode;
};
