#include "raylib.h"
#include "raymath.h"
#include "stdbool.h"

typedef enum {
    none,
    square,
    triangle,
    cicle,
    execute
}magicCircleRing;

int main()
{
    Vector2 windowSize = {1000,1000};
    InitWindow(windowSize.x, windowSize.y, "Spell Jam");
    bool aPressed = false;
    bool bPressed = false;
    bool cPressed = false;
    bool spacePressed = false;

    while (!WindowShouldClose())
    {
        // ------
        // INPUTS
        // ------
        aPressed = IsKeyPressed(KEY_A);
        bPressed = IsKeyPressed(KEY_B);
        cPressed = IsKeyPressed(KEY_C);
        spacePressed = IsKeyPressed(KEY_SPACE);

        BeginDrawing();
        DrawCircleLines(250, 250, 50, PINK);
        Vector2 pos = Vector2Scale(Vector2Normalize((Vector2){1, 1}), 50);
        DrawRectangleLines(250 - pos.x, 250 - pos.y, 2 * pos.x, 2 * pos.y, PINK);
        EndDrawing();
    }
    return 0;
}