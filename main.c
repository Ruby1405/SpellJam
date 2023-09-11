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
    circle,
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
            magicCircle[ringCount] = circle;
            ringCount++;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw magic circle
        int radius = 50;
        int squareCount = 0;
        int triangleCount = 0;
        Color ringColor = 
        {
            magicCircle[0] == circle? Clamp(0,255,255-(43*(ringCount-2))):255,
            magicCircle[0] == triangle? Clamp(0,255,43*ringCount):0,
            magicCircle[0] == square? Clamp(0,255,255-(43*(ringCount-2))):255,
            255
        };
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
                    DrawPolyLines(windowMiddle, 4, radius, -CircleRoation(radius, angle) + 45, ringColor);
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
                    CircleRoation(radius, angle),
                    ringColor);
                angle = -angle;
                squareCount++;
                triangleCount = 0;
            }
            break;
            case triangle:
            {

                squareCount = 0;
                if (triangleCount == 1)
                {
                    DrawPolyLines(windowMiddle, 3, radius, -CircleRoation(radius, angle) + 60, ringColor);
                    triangleCount = 0;
                    continue;
                }
                if (radius < 100)
                {
                    if (i != 0)
                    {
                        radius = radius / sin(30 * DEG2RAD);
                    }
                    DrawPolyLines(windowMiddle, 3, radius, CircleRoation(radius, angle), ringColor);
                    angle = -angle;
                    triangleCount = 1;
                }
                else
                {
                    if (triangleCount == 2)
                    {
                        radius -= 10;
                        for (int j = 0; j < 12; j++)
                        {
                            DrawPolyLines(Vector2Add(windowMiddle, Vector2Rotate((Vector2){0, radius}, (j * 30 - 15 - CircleRoation(radius, -angle)) * DEG2RAD)), 3, 10, j * -30 -45 + CircleRoation(radius, -angle), ringColor);
                        }
                        radius += 10;
                        triangleCount = 0;
                    }
                    else
                    {
                        radius += 10;
                        for (int j = 0; j < 12; j++)
                        {
                            DrawPolyLines(Vector2Add(windowMiddle, Vector2Rotate((Vector2){0, radius}, (j * 30 - CircleRoation(radius, angle)) * DEG2RAD)), 3, 10, j * -30 + CircleRoation(radius, angle), ringColor);
                        }
                        radius += 10;
                        angle = -angle;
                        triangleCount = 2;
                    }
                }
            }
            break;
            default:
            {
                if (i != 0)
                {
                    if (magicCircle[i - 1] == circle)
                    {
                        radius += 10;
                    }
                }
                DrawCircleLines(
                    windowMiddle.x,
                    windowMiddle.y,
                    radius, ringColor);
                squareCount = 0;
                triangleCount = 0;
            }
            break;
            }
        }
        DrawText(TextFormat("%d", radius), 20, 20, 18, WHITE);

        EndDrawing();
    }
    return 0;
}