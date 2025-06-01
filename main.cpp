#include <iostream>
#include <raylib.h>
#include "RTSCamera.h"
#include "World.h"
#include <rlgl.h>
#include "blocktypes.h"
#include "mouseControl.h"
#include "GBuffer.h"
#include "DeferredRenderer.h"



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "test1");

    RTS_Camera cam(screenWidth, screenHeight);

    cam.SetMapBounds({ -5000, -5000 }, { 5000, 5000 }); // Ограничить область карты

    InitBlockTypes(); //init block types!!
    
        

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Texture2D atlas = LoadTexture("resources/cubicmap_atlas.png"); // 512, тайл 64 или твой вариант
    int tileSize = 64;
    TerrainGenerator terrainGenerator;

    World world(&atlas, tileSize, 5, &terrainGenerator); // X Z


    GBuffer gbuffer;
    if (!gbuffer.Init(screenWidth, screenHeight)) {
        TraceLog(LOG_WARNING, "Failed to create GBuffer!");
        return 1;
    }

    DeferredRenderer deferred;
    if (!deferred.Init(screenWidth, screenHeight, "resources/shaders/glsl330")) { // укажи свой путь к шейдерам
        TraceLog(LOG_WARNING, "Failed to load deferred shaders!");
        return 1;
    }

    MouseControl mouseControl(&world, &cam);
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        cam.Update();
        mouseControl.Update();
        world.Update(cam.cameraTarget);
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_F1)) deferred.mode = DeferredPassMode::POSITION;
        if (IsKeyPressed(KEY_F2)) deferred.mode = DeferredPassMode::NORMAL;
        if (IsKeyPressed(KEY_F3)) deferred.mode = DeferredPassMode::ALBEDO;
        if (IsKeyPressed(KEY_F4)) deferred.mode = DeferredPassMode::SHADING;

        // --- Geometry Pass ---
        gbuffer.Bind();
        rlClearColor(0, 0, 0, 0);
        rlClearScreenBuffers();
        BeginMode3D(cam.GetCamera3D());
        world.DrawGeometryPass(&deferred.geometryShader);
        EndMode3D();
        gbuffer.Unbind();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        deferred.RenderLightingPass(gbuffer, cam.GetCamera3D(), deferred.mode);
        //ClearBackground(RAYWHITE);
        //world.Draw();

        //DrawGrid(10, 5.0f);

        EndMode3D();
        //GUI Module
        DrawText("F1: Position  F2: Normal  F3: Albedo  F4: Shading  F5: Depth", 10, 40, 18, DARKGRAY);
        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    deferred.Destroy();
    gbuffer.Destroy();
    //UnloadTexture(atlas);
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}




