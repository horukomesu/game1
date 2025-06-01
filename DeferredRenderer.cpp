#include "DeferredRenderer.h"
#include "rlgl.h"
#include <string>
#include "raymath.h"

DeferredRenderer::DeferredRenderer()
    : texUnitPosition(0), texUnitNormal(1), texUnitAlbedoSpec(2), mode(DeferredPassMode::SHADING)
{
    sunDir = { -0.5f, -1.0f, -0.5f };
    sunDir = Vector3Normalize(sunDir);

    sunColor = { 1.0f, 0.95f, 0.8f };
    ambientColor = { 0.13f, 0.15f, 0.18f };
}

DeferredRenderer::~DeferredRenderer() { Destroy(); }

bool DeferredRenderer::Init(int screenWidth, int screenHeight, const char* shaderPath) {
    std::string path = shaderPath;

    geometryShader = LoadShader(
        (path + "/gbuffer.vs").c_str(),
        (path + "/gbuffer.fs").c_str()
    );
    deferredShader = LoadShader(
        (path + "/deferred_shading.vs").c_str(),
        (path + "/deferred_shading.fs").c_str()
    );
    deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition");

    // Привязка текстурных сэмплеров
    rlEnableShader(deferredShader.id);
    SetShaderValue(deferredShader, rlGetLocationUniform(deferredShader.id, "gPosition"), &texUnitPosition, RL_SHADER_UNIFORM_SAMPLER2D);
    SetShaderValue(deferredShader, rlGetLocationUniform(deferredShader.id, "gNormal"), &texUnitNormal, RL_SHADER_UNIFORM_SAMPLER2D);
    SetShaderValue(deferredShader, rlGetLocationUniform(deferredShader.id, "gAlbedoSpec"), &texUnitAlbedoSpec, RL_SHADER_UNIFORM_SAMPLER2D);
    rlDisableShader();

    // --- Получаем локации uniform для солнца и неба
    sunDirLoc = GetShaderLocation(deferredShader, "sunDir");
    sunColorLoc = GetShaderLocation(deferredShader, "sunColor");
    ambientLoc = GetShaderLocation(deferredShader, "ambientColor");

    return geometryShader.id > 0 && deferredShader.id > 0;
}

void DeferredRenderer::Destroy() {
    if (geometryShader.id > 0) {
        UnloadShader(geometryShader);
        geometryShader.id = 0;
    }
    if (deferredShader.id > 0) {
        UnloadShader(deferredShader);
        deferredShader.id = 0;
    }
}


void DeferredRenderer::RenderLightingPass(GBuffer& gBuffer, const Camera& camera, DeferredPassMode mode) {
    // Отключаем FBO, очищаем экран
    gBuffer.Unbind();
    rlClearScreenBuffers();

    switch (mode) {
    case DeferredPassMode::SHADING:
    {
        BeginMode3D(camera);
        rlDisableColorBlend();
        rlEnableShader(deferredShader.id);

        // Установка uniform для освещения
        SetShaderValue(deferredShader, sunDirLoc, &sunDir, SHADER_UNIFORM_VEC3);
        SetShaderValue(deferredShader, sunColorLoc, &sunColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(deferredShader, ambientLoc, &ambientColor, SHADER_UNIFORM_VEC3);

        // Обновить камеру (позиция observer)
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(deferredShader, deferredShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Bind G-Buffer textures
        rlActiveTextureSlot(texUnitPosition);   rlEnableTexture(gBuffer.positionTexture);
        rlActiveTextureSlot(texUnitNormal);     rlEnableTexture(gBuffer.normalTexture);
        rlActiveTextureSlot(texUnitAlbedoSpec); rlEnableTexture(gBuffer.albedoSpecTexture);

        rlLoadDrawQuad();

        rlDisableShader();
        rlEnableColorBlend();
        EndMode3D();
    } break;

    case DeferredPassMode::POSITION:
    case DeferredPassMode::NORMAL:
    case DeferredPassMode::ALBEDO:
    {
        unsigned int tex = 0;
        if (mode == DeferredPassMode::POSITION) tex = gBuffer.positionTexture;
        if (mode == DeferredPassMode::NORMAL)   tex = gBuffer.normalTexture;
        if (mode == DeferredPassMode::ALBEDO)   tex = gBuffer.albedoSpecTexture;

        Texture2D texture;
        texture.id = tex;
        texture.width = gBuffer.width;
        texture.height = gBuffer.height;
        texture.mipmaps = 1;
        texture.format = 0;

        DrawTextureRec(
            texture,
            Rectangle{ 0, 0, (float)gBuffer.width, (float)-gBuffer.height },
            Vector2{ 0, 0 },
            RAYWHITE
        );
    } break;
    }
}
