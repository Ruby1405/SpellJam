#include "raylib.h"
#include "raymath.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct Vector2 v2f;

typedef enum
{
    none,
    square,
    triangle,
    cicle,
    execute
} magicCircleRing;

int main()
{
    v2f windowMiddle = {500, 500};
    InitWindow(windowMiddle.x * 2, windowMiddle.y * 2, "Spell Jam");
    bool aPressed = false;
    bool wPressed = false;
    bool dPressed = false;
    bool spacePressed = false;

    magicCircleRing magicCircle[16];
    int ringCount = 0;
    float angle = 0;
    float angleCoef = 1;

    while (!WindowShouldClose())
    {
        // ------
        // INPUTS
        // ------
        aPressed = IsKeyPressed(KEY_A);
        wPressed = IsKeyPressed(KEY_W);
        dPressed = IsKeyPressed(KEY_D);
        spacePressed = IsKeyPressed(KEY_SPACE);

        if (spacePressed)
        {
            ringCount = 0;
        }
        if (aPressed)
        {
            magicCircle[ringCount] = square;
            ringCount++;
        }
        if (wPressed)
        {
            magicCircle[ringCount] = triangle;
            ringCount++;
        }
        if (dPressed)
        {
            magicCircle[ringCount] = cicle;
            ringCount++;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw magic circle
        int radius = 10;
        int squareCount = 0;
        int triangleCount = 0;
        angle = fabs(angle);
        angle += 1 * GetFrameTime();
        if (angle > 360)
        {
            angle -= 360;
        }
        for (int i = 0; i < ringCount; i++)
        {
            switch (magicCircle[i])
            {
            case square:
            {
                if (squareCount > 0)
                {    
                    DrawPolyLines(windowMiddle,4,radius,(-angle * angleCoef * radius) + 45,PINK);
                    squareCount = 0;
                    continue;
                }
                radius = Vector2Length((Vector2){radius, radius});
                DrawPolyLines(
                    windowMiddle,
                    4,
                    radius,
                    angle * angleCoef * radius,
                    PINK);
                angle = -angle;
                squareCount ++;
                triangleCount = 0;
            }
            break;
            case triangle:
            {
               if (triangleCount > 0)
                {
                    DrawPolyLines(windowMiddle,3,radius,(-angle * angleCoef * radius) + 60, PINK);
                    triangleCount = 0;
                    continue;
                }
                radius = radius / sin(30 * DEG2RAD);
                DrawPolyLines(windowMiddle,3,radius,angle * angleCoef * radius, PINK);
                squareCount = 0;
                angle = -angle;
                triangleCount ++;
            }
            break;
            default:
            {
                if (i != 0)
                {
                    if (magicCircle[i-1] == cicle)
                    {
                        radius += 10;
                    }
                }
                DrawCircleLines(
                    windowMiddle.x,
                    windowMiddle.y,
                    radius, PINK);
                    squareCount = 0;
                    triangleCount = 0;
            } break;
            }
        }
        DrawText(TextFormat("%f",angle),20,20,18,WHITE);
        
        EndDrawing();
    }
    return 0;
}