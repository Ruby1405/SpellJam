#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"
// #include "terrainGen.c"
#include "magicCircle.c"

int main()
{
    // 1120,1280 är *5
    v2f windowSize = {1120, 1286};
    InitWindow(windowSize.x, windowSize.y, "SpellJam");

    bool upDown = false;
    bool downDown = false;
    bool leftDown = false;
    bool rightDown = false;
    bool squarePressed = false;
    bool trianglePressed = false;
    bool circlePressed = false;
    bool executePressed = false;

    int moveSpeed = 150;

    const int maxSpellEntities = 16;
    SpellEntity spellEntities[maxSpellEntities];
    for (int i = 0; i < maxSpellEntities; i++)
    {
        spellEntities[i] = (SpellEntity){0};
    }

    v2f playerPosition = {windowSize.x / 2, windowSize.y / 2};
    v2f playerAim = {-1, 0};

    Incantation magicCircle[16];
    int ringCount = 0;
    float angle = 0;

    while (!WindowShouldClose())
    {
        // ------
        // INPUTS
        // ------
        upDown = IsKeyDown(KEY_W);
        downDown = IsKeyDown(KEY_S);
        leftDown = IsKeyDown(KEY_A);
        rightDown = IsKeyDown(KEY_D);
        squarePressed = IsKeyPressed(KEY_LEFT);
        trianglePressed = IsKeyPressed(KEY_UP);
        circlePressed = IsKeyPressed(KEY_RIGHT);
        executePressed = IsKeyPressed(KEY_DOWN);

        if (upDown)
        {
            playerPosition.y -= moveSpeed * GetFrameTime();
            playerAim.y = -1;
        }
        if (downDown)
        {
            playerPosition.y += moveSpeed * GetFrameTime();
            playerAim.y = 1;
        }
        if (leftDown)
        {
            playerPosition.x -= moveSpeed * GetFrameTime();
            playerAim.x = -1;
        }
        if (rightDown)
        {
            playerPosition.x += moveSpeed * GetFrameTime();
            playerAim.x = 1;
        }
        if (executePressed)
        {
            magicCircle[ringCount] = execute;
            for (int i = 0; i < spellBookCount; i++)
            {
                for (int j = 0; j < ringCount + 1; j++)
                {
                    if (magicCircle[j] != spellBook[i].Incantation[j])
                    {
                        break;
                    }
                    else
                    {
                        if (magicCircle[j] == execute)
                        {
                            for (int k = 0; k < maxSpellEntities; k++)
                            {
                                if (spellEntities[k].lifetime == 0)
                                {
                                    spellEntities[k] = (SpellEntity){100, spellBook[i].spellType, playerPosition, playerAim};
                                    goto spellCast;
                                }
                            }
                        }
                    }
                }
            }
        spellCast:
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

        DrawCircle(playerPosition.x, playerPosition.y, 30, (Color){100, 255, 100, 255});

        for (int i = 0; i < maxSpellEntities; i++)
        {
            if (spellEntities[i].lifetime > 0)
            {
                switch (spellEntities[i].type)
                {
                case manaSpark:
                    DrawSpellManaSpark(spellEntities[i].position, spellEntities[i].aim);
                    break;

                case block:
                    DrawSpellBlock(spellEntities[i].position);
                    break;

                case fireBall:
                    DrawSpellFireBall(spellEntities[i].position, spellEntities[i].aim);
                    break;

                case moonBeam:
                    DrawSpellMoonBeam(spellEntities[i].position);
                    break;

                default:
                    break;
                }
            }
        }

        DrawMagicCircle(playerPosition, magicCircle, ringCount, &angle);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}