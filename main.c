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

float CircleRoation(int radius, float angle)
{
    return (angle * 3) / (radius * 0.0002 + 0.01);
}

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
    float angleCoef = 5;

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
        int radius = 50;
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
                    DrawPolyLines(windowMiddle,4,radius,-CircleRoation(radius,angle) + 45,PINK);
                    squareCount = 0;
                    continue;
                }
                if (i != 0)
                {
                    radius = Vector2Length((Vector2){radius, radius});
                }
                DrawPolyLines(
                    windowMiddle,
                    4,
                    radius,
                    CircleRoation(radius,angle),
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
                    DrawPolyLines(windowMiddle,3,radius,-CircleRoation(radius,angle) + 60, PINK);
                    triangleCount = 0;
                    continue;
                }
                if (radius < 100)
                {
                    if (i != 0)
                    {
                        radius = radius / sin(30 * DEG2RAD);
                    }
                    DrawPolyLines(windowMiddle,3,radius,CircleRoation(radius,angle), PINK);
                    squareCount = 0;
                    angle = -angle;
                    triangleCount ++;
                }
                else
                {
                    radius += 10;
                    for (int j = 0; j < 6; j++)
                    {   
                        DrawPolyLines(Vector2Add(windowMiddle,Vector2Rotate((Vector2){0,radius},j * 60 * DEG2RAD)),3,10,j*60,PINK);
                    }
                }
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
        DrawText(TextFormat("%d",radius),20,20,18,WHITE);
        
        EndDrawing();
    }
    return 0;
}