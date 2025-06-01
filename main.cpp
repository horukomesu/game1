#include <iostream>
#include <raylib.h>
#include "RTSCamera.h"
#include "World.h"
#include <rlgl.h>
#include "blocktypes.h"
#include "mouseControl.h"

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

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(cam.GetCamera3D());
        mouseControl.DrawSelection();
        world.Draw();

        //DrawGrid(10, 5.0f);

        EndMode3D();
        //GUI Module


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //UnloadTexture(atlas);
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}




