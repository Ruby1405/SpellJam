#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"
// #include "terrainGen.c"
#include "magicCircle.c"

typedef enum gameState
{
    menu,
    game,
    pause,
    gameOver
} gameState;

bool rectCollision(Rectangle rect, Vector2 point)
{
    return (rect.x < point.x && rect.x + rect.width > point.x && rect.y < point.y && rect.y + rect.height > point.y);
}

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
                                if (spellEntities[k].lifetime <= 0)
                                {
                                    spellEntities[k] = (SpellEntity){spellBook[i].startingLifeTime, spellBook[i].name, playerPosition, Vector2Normalize(playerAim)};
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

        // -------------
        // SPELL UPDATES
        // -------------
        for (int i = 0; i < maxSpellEntities; i++)
        {
            if (spellEntities[i].lifetime > 0)
            {
                switch (spellEntities[i].name)
                {
                case manaSpark:
                    spellEntities[i].position.x += spellEntities[i].aim.x * 500 * GetFrameTime();
                    spellEntities[i].position.y += spellEntities[i].aim.y * 500 * GetFrameTime();
                    if (!rectCollision((Rectangle){0, 0, windowSize.x, windowSize.y}, spellEntities[i].position))
                    {
                        spellEntities[i].lifetime = 0;
                    }
                    break;

                case fireBall:
                    spellEntities[i].position.x += spellEntities[i].aim.x * 500 * GetFrameTime();
                    spellEntities[i].position.y += spellEntities[i].aim.y * 500 * GetFrameTime();
                    if (!rectCollision((Rectangle){0, 0, windowSize.x, windowSize.y}, spellEntities[i].position))
                    {
                        spellEntities[i].lifetime = 0;
                    }
                    break;

                case block:
                    spellEntities[i].position = playerPosition;
                    spellEntities[i].lifetime -= GetFrameTime();
                    break;

                case moonBeam:
                    spellEntities[i].lifetime -= GetFrameTime();
                    break;

                case chromaticOrb:
                    spellEntities[i].position.x += spellEntities[i].aim.x * 300 * GetFrameTime();
                    spellEntities[i].position.y += spellEntities[i].aim.y * 300 * GetFrameTime();
                    spellEntities[i].lifetime += GetFrameTime();
                    if (!rectCollision((Rectangle){0, 0, windowSize.x, windowSize.y}, spellEntities[i].position))
                    {
                        spellEntities[i].lifetime = 0;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircle(playerPosition.x, playerPosition.y, 20, (Color){100, 255, 100, 255});
        DrawCircle(playerPosition.x - 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
        DrawCircle(playerPosition.x + 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
        DrawRectangle(playerPosition.x - 8, playerPosition.y + 2, 16, 2, (Color){0, 0, 0, 255});

        for (int i = 0; i < maxSpellEntities; i++)
        {
            if (spellEntities[i].lifetime > 0)
            {
                switch (spellEntities[i].name)
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
                    DrawSpellMoonBeam(spellEntities[i].position, spellEntities[i].lifetime);
                    break;

                case chromaticOrb:
                    DrawSpellChromaticOrb(spellEntities[i].position, spellEntities[i].aim, spellEntities[i].lifetime);
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