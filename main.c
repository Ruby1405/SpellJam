#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "stdbool.h"
#include "stdlib.h"
// #include "terrainGen.c"
#include "magicCircle.c"

int main()
{
    // 1120,1280 är *5
    v2f windowSize = {1120, 1286};
    InitWindow(windowSize.x, windowSize.y, "SpellJam");
    
    bool squarePressed = false;
    bool trianglePressed = false;
    bool circlePressed = false;
    bool executePressed = false;
    
    v2f playerPosition = {windowSize.x/2, windowSize.y/2};
    
    magicCircleRing magicCircle[16];
    int ringCount = 0;
    float angle = 0;

    while (!WindowShouldClose())
    {
        // ------
        // INPUTS
        // ------
        squarePressed = IsKeyPressed(KEY_LEFT);
        trianglePressed = IsKeyPressed(KEY_UP);
        circlePressed = IsKeyPressed(KEY_RIGHT);
        executePressed = IsKeyPressed(KEY_DOWN);
        
        if (executePressed)
        {
        ringCount = 0;
        }
        if (squarePressed)
        {
            magicCircle[ringCount] = square;
            ringCount++;
        }
        if (trianglePressed)
        {
            magicCircle[ringCount] = triangle;
            ringCount++;
        }
        if (circlePressed)
        {
            magicCircle[ringCount] = circle;
            ringCount++;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawMagicCircle(playerPosition, magicCircle, ringCount, &angle);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}