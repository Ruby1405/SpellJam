#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raymath.h"
#include "terrainGen.c"
#include "magicCircle.c"
#include "enemy.c"
#include "boss.c"

const int playerRadius = 20;
const int tileSize = 40;

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
    time_t t;
    srand((unsigned)time(&t));
    // 1120,1280 är *5
    // v2f windowSize = {1120, 1286};
    // v2f windowSize = {1440, 900};
    v2f windowSize = {2560, 1440};
    InitWindow(windowSize.x, windowSize.y, "SpellJam");
    ToggleFullscreen();
    // ------------
    // Get textures
    // ------------
    Texture2D worldSprites = LoadTexture("Assets/world.png");

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

    v2f playerPosition = {roomSize / 2 * tileSize + tileSize / 2, roomSize / 2 * tileSize + tileSize / 2};
    v2f playerAim = {0, 0};
    v2f spellAim = {0, -1};

    Incantation magicCircle[16];
    int ringCount = 0;
    float angle = 0;

    Enemy enemies[32] = {0};
    enemies[0] = (Enemy){(Vector2){windowSize.x / 2, windowSize.y / 2}, (Vector2){1, 1}, 100, 100, 100, 0, {0}, warrior, chase};
    enemies[1] = (Enemy){(Vector2){windowSize.x / 2, windowSize.y / 2}, (Vector2){1, 1}, 100, 100, 100, 0, {0}, mage, chase};
    enemies[2] = (Enemy){(Vector2){windowSize.x / 2, windowSize.y / 2}, (Vector2){1, 1}, 80, 100, 100, 0, {0}, mage, chase};
    enemies[3] = (Enemy){(Vector2){windowSize.x / 2, windowSize.y / 2}, (Vector2){1, 1}, 80, 100, 100, 0, {0}, warrior, chase};

    Room room = DrunkardsWalk(false, false, false, false, 2500, (Point){14, 14});

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

        playerAim = (v2f){0, 0};
        if (upDown)
        {
            playerAim.y = -1;
        }
        if (downDown)
        {
            playerAim.y = 1;
        }
        if (leftDown)
        {
            playerAim.x = -1;
        }
        if (rightDown)
        {
            playerAim.x = 1;
        }
        v2f playerRadiusVector = {playerRadius * playerAim.x, playerRadius * playerAim.y};
        playerAim = Vector2Normalize(playerAim);

        // -----------
        // Move Player
        // -----------
        v2f playerMovePosition = Vector2Add(playerPosition, Vector2Scale(playerAim, (moveSpeed * GetFrameTime())));
        // v2f redRectx = (v2f){(int)((playerMovePosition.x + playerRadiusVector.x) / tileSize),(int)((playerPosition.y + playerRadiusVector.y) / tileSize)};
        // v2f redRecty = (v2f){(int)((playerMovePosition.x + playerRadiusVector.x) / tileSize),(int)((playerMovePosition.y + playerRadiusVector.y) / tileSize)};

        // Broad phase collision
        if (room.data[(int)((playerMovePosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
        {
            playerMovePosition.x = playerPosition.x;
        }
        if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerMovePosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
        {
            playerMovePosition.y = playerPosition.y;
        }

        for (int x = -1; x < 2; x++)
        {
            for (int y = -1; y < 2; y++)
            {
                if (x + (int)(playerPosition.x / tileSize) >= 0 && y + (int)(playerPosition.y / tileSize) >= 0 && x + (int)(playerPosition.x / tileSize) < roomSize && y + (int)(playerPosition.y / tileSize) < roomSize)
                {
                    switch (room.data[x + (int)(playerPosition.x / tileSize)][y + (int)(playerPosition.y / tileSize)][1])
                    {
                    case TILE_TYPE_CORNER_NORTH_EAST:
                    {
                        v2f cornerVector = (Vector2){
                            (x + 1 + (int)(playerPosition.x / tileSize)) * tileSize,
                            (y + (int)(playerPosition.y / tileSize)) * tileSize};
                        if (CheckCollisionPointCircle(cornerVector, playerPosition, playerRadius))
                        {
                            if (Vector2Distance(playerMovePosition, cornerVector) < playerRadius)
                            {
                                if (fabsf(playerMovePosition.x - cornerVector.x) < fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.x += cornerVector.x - (playerPosition.x - playerRadius);
                                }
                                else if (fabsf(playerMovePosition.x - cornerVector.x) > fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.y -= (playerPosition.y + playerRadius) - cornerVector.y;
                                }
                            }
                        }
                    }
                    break;
                    case TILE_TYPE_CORNER_NORTH_WEST:
                    {
                        v2f cornerVector = (Vector2){
                            (x + (int)(playerPosition.x / tileSize)) * tileSize,
                            (y + (int)(playerPosition.y / tileSize)) * tileSize};
                        if (CheckCollisionPointCircle(cornerVector, playerPosition, playerRadius))
                        {
                            if (Vector2Distance(playerMovePosition, cornerVector) < playerRadius)
                            {
                                if (fabsf(playerMovePosition.x - cornerVector.x) < fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.x -= (playerPosition.x + playerRadius) - cornerVector.x;
                                }
                                else if (fabsf(playerMovePosition.x - cornerVector.x) > fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.y -= (playerPosition.y + playerRadius) - cornerVector.y;
                                }
                            }
                        }
                    }
                    break;
                    case TILE_TYPE_CORNER_SOUTH_EAST:
                    {
                        v2f cornerVector = (Vector2){
                            (x + 1 + (int)(playerPosition.x / tileSize)) * tileSize,
                            (y + 1 + (int)(playerPosition.y / tileSize)) * tileSize};
                        if (CheckCollisionPointCircle(cornerVector, playerPosition, playerRadius))
                        {
                            if (Vector2Distance(playerMovePosition, cornerVector) < playerRadius)
                            {
                                if (fabsf(playerMovePosition.x - cornerVector.x) < fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.x += cornerVector.x - (playerPosition.x - playerRadius);
                                }
                                else if (fabsf(playerMovePosition.x - cornerVector.x) > fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.y += cornerVector.y - (playerPosition.y - playerRadius);
                                }
                            }
                        }
                    }
                    break;
                    case TILE_TYPE_CORNER_SOUTH_WEST:
                    {
                        v2f cornerVector = (Vector2){
                            (x + (int)(playerPosition.x / tileSize)) * tileSize,
                            (y + 1 + (int)(playerPosition.y / tileSize)) * tileSize};
                        if (CheckCollisionPointCircle(cornerVector, playerPosition, playerRadius))
                        {
                            if (Vector2Distance(playerMovePosition, cornerVector) < playerRadius)
                            {
                                if (fabsf(playerMovePosition.x - cornerVector.x) < fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.x -= (playerPosition.x + playerRadius) - cornerVector.x;
                                }
                                else if (fabsf(playerMovePosition.x - cornerVector.x) > fabsf(playerMovePosition.y - cornerVector.y))
                                {
                                    playerMovePosition.y += cornerVector.y - (playerPosition.y - playerRadius);
                                }
                            }
                        }
                    }
                    break;
                    default:
                        break;
                    }
                }
            }
        }
        playerPosition = playerMovePosition;

        // -------------
        // Spell Casting
        // -------------
        if (playerAim.x != 0 || playerAim.y != 0)
        {
            spellAim = playerAim;
        }
        if (executePressed)
        {
            magicCircle[ringCount] = execute;
            for (int spellIndex = 0; spellIndex < spellBookCount; spellIndex++)
            {
                for (int ringIndex = 0; ringIndex < ringCount + 1; ringIndex++)
                {
                    if (magicCircle[ringIndex] != spellBook[spellIndex].Incantation[ringIndex])
                    {
                        break;
                    }
                    else
                    {
                        if (magicCircle[ringIndex] == execute)
                        {
                            switch (spellBook[spellIndex].name)
                            {
                            case fireBall:
                            case manaSpark:
                            case chromaticOrb:
                            {
                                for (int k = 0; k < maxSpellEntities; k++)
                                {
                                    if (spellEntities[k].lifetime <= 0)
                                    {
                                        spellEntities[k] = (SpellEntity){spellBook[spellIndex].startingLifeTime, spellBook[spellIndex].name, playerPosition, Vector2Normalize(spellAim), 0};
                                        break;
                                    }
                                }
                                goto spellCast;
                            }
                            case magicMissile:
                            {
                                for (int i = 0; i < 3; i++)
                                {
                                    for (int k = 0; k < maxSpellEntities; k++)
                                    {
                                        if (spellEntities[k].lifetime <= 0)
                                        {
                                            int targetIndex = -1;
                                            spellEntities[k] = (SpellEntity){spellBook[spellIndex].startingLifeTime, spellBook[spellIndex].name, (v2f){playerPosition.x + k*5, playerPosition.y + k*5}, Vector2Normalize(spellAim), targetIndex};
                                            for (int enemyIndex = 0; enemyIndex < 32; enemyIndex++)
                                            {
                                                if (enemies[enemyIndex].health > 0)
                                                {
                                                    targetIndex = enemyIndex;
                                                    break;
                                                }
                                            }
                                            if (targetIndex != -1)
                                            {
                                                for (int enemyIndex = 0; enemyIndex < 32; enemyIndex++)
                                                {
                                                    if (enemies[enemyIndex].health > 0)
                                                    {
                                                        printf("%f\n", Vector2Distance(spellEntities[k].position, enemies[enemyIndex].position));
                                                        if (Vector2Distance(spellEntities[k].position, enemies[targetIndex].position) > Vector2Distance(spellEntities[k].position, enemies[enemyIndex].position))
                                                        {
                                                            targetIndex = enemyIndex;
                                                        }
                                                    }
                                                }
                                            }
                                            spellEntities[k].targetIndex = targetIndex;
                                            break;
                                        }
                                    }
                                }
                                goto spellCast;
                            }
                            case moonBeam:
                            {
                                for (int k = 0; k < maxSpellEntities; k++)
                                {
                                    if (spellEntities[k].lifetime <= 0)
                                    {
                                        spellEntities[k] = (SpellEntity){spellBook[spellIndex].startingLifeTime, spellBook[spellIndex].name, Vector2Add(Vector2Scale(spellAim, 100), playerPosition), Vector2Normalize(spellAim), 0};
                                        break;
                                    }
                                }
                                goto spellCast;
                            }
                            default:
                            {
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
        if (ringCount < 16)
        {
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
                    for (int j = 0; j < 32; j++)
                    {
                        if (enemies[j].health > 0)
                        {
                            if (CheckCollisionCircles(spellEntities[i].position, 10, enemies[j].position, enemyRadius))
                            {
                                enemies[j].health -= 10;
                                spellEntities[i].lifetime = 0;
                            }
                        }
                    }
                    break;

                case fireBall:
                    spellEntities[i].position.x += spellEntities[i].aim.x * 500 * GetFrameTime();
                    spellEntities[i].position.y += spellEntities[i].aim.y * 500 * GetFrameTime();
                    if (!rectCollision((Rectangle){0, 0, windowSize.x, windowSize.y}, spellEntities[i].position))
                    {
                        spellEntities[i].lifetime = 0;
                    }
                    for (int j = 0; j < 32; j++)
                    {
                        if (enemies[j].health > 0)
                        {
                            if (CheckCollisionCircles(spellEntities[i].position, 10, enemies[j].position, enemyRadius))
                            {
                                enemies[j].health -= 20;
                                spellEntities[i].lifetime = 0;
                                for (int buffIndex = 0; buffIndex < 8; buffIndex++)
                                {
                                    if (enemies[j].buffs[buffIndex].duration <= 0)
                                    {
                                        enemies[j].buffs[buffIndex] = (Buff){5, 5};
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    break;

                case block:
                    spellEntities[i].position = playerPosition;
                    spellEntities[i].lifetime -= GetFrameTime();
                    break;

                case moonBeam:
                    spellEntities[i].lifetime -= GetFrameTime();
                    for (int j = 0; j < 32; j++)
                    {
                        if (enemies[j].health > 0)
                        {
                            if (CheckCollisionCircles(spellEntities[i].position, 50, enemies[j].position, enemyRadius))
                            {
                                enemies[j].health -= 20 * GetFrameTime();
                            }
                        }
                    }
                    break;

                case chromaticOrb:
                    spellEntities[i].position.x += spellEntities[i].aim.x * 300 * GetFrameTime();
                    spellEntities[i].position.y += spellEntities[i].aim.y * 300 * GetFrameTime();
                    spellEntities[i].lifetime += GetFrameTime();
                    if (!rectCollision((Rectangle){0, 0, windowSize.x, windowSize.y}, spellEntities[i].position))
                    {
                        spellEntities[i].lifetime = 0;
                    }
                    for (int j = 0; j < 32; j++)
                    {
                        if (enemies[j].health > 0)
                        {
                            if (CheckCollisionCircles(spellEntities[i].position, 16, enemies[j].position, enemyRadius))
                            {
                                enemies[j].health -= 150 * GetFrameTime();
                            }
                        }
                    }
                    break;

                case magicMissile:
                {
                    float magicMissileSpeed = 200;
                    
                    spellEntities[i].aim = Vector2Add(spellEntities[i].aim, Vector2Normalize(Vector2Subtract(enemies[spellEntities[i].targetIndex].position, spellEntities[i].position)));
                    spellEntities[i].position = Vector2Add(spellEntities[i].position, Vector2Scale(Vector2Normalize(spellEntities[i].aim), magicMissileSpeed * GetFrameTime()));
                }
                break;

                default:
                    break;
                }
            }
        }

        // ------------
        // ENEMY UPDATE
        // ------------
        for (int i = 0; i < 32; i++)
        {
            if (enemies[i].health > 0)
            {
                UpdateEnemy(&enemies[i], playerPosition, &room);
            }
        }

        // ------
        // RENDER
        // ------
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw Rooms
        for (int x = 0; x < roomSize; x++)
        {
            for (int y = 0; y < roomSize; y++)
            {
                switch (room.data[x][y][0])
                {
                case TILE_TYPE_WALL:
                    DrawTextureRec(worldSprites, (Rectangle){0, 40, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_DOOR_NORTH:
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, (Color){255, 255, 0, 255});
                    break;
                case TILE_TYPE_DOOR_EAST:
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, (Color){255, 0, 255, 255});
                    break;
                case TILE_TYPE_DOOR_SOUTH:
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, (Color){0, 255, 255, 255});
                    break;
                case TILE_TYPE_DOOR_WEST:
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, (Color){0, 255, 0, 255});
                    break;
                case TILE_TYPE_EMPTY:
                    DrawTextureRec(worldSprites, (Rectangle){0, 0, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case SCHEDULED_FOR_DELETE:
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, (Color){80, 40, 45, 255});
                default:
                    break;
                }

                switch (room.data[x][y][1])
                {
                case TILE_TYPE_WALL_NORTH:
                    DrawTextureRec(worldSprites, (Rectangle){80, 40, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_WALL_EAST:
                    DrawTextureRec(worldSprites, (Rectangle){40, 40, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_WALL_SOUTH:
                    DrawTextureRec(worldSprites, (Rectangle){40, 0, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_WALL_WEST:
                    DrawTextureRec(worldSprites, (Rectangle){80, 0, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_CORNER_NORTH_EAST:
                    DrawTextureRec(worldSprites, (Rectangle){120, 40, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_CORNER_NORTH_WEST:
                    DrawTextureRec(worldSprites, (Rectangle){160, 40, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_CORNER_SOUTH_EAST:
                    DrawTextureRec(worldSprites, (Rectangle){120, 0, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                    break;
                case TILE_TYPE_CORNER_SOUTH_WEST:
                    DrawTextureRec(worldSprites, (Rectangle){160, 0, 40, 40}, (Vector2){x * tileSize, y * tileSize}, (Color){255, 255, 255, 255});
                }
                // Number tiles
                // if (y != 0)
                // {
                //     DrawText(TextFormat("%d", y), x * tileSize + 10, y * tileSize + 10, 20, (Color){255, 255, 255, 255});
                // }
            }
            // Number tiles
            // DrawText(TextFormat("%d", x), x * tileSize + 10, 10, 20, (Color){255, 255, 255, 255});
        }

        DrawCircle(playerPosition.x + 1, playerPosition.y + 1, playerRadius, (Color){0, 0, 0, 255});
        DrawCircle(playerPosition.x, playerPosition.y, playerRadius, (Color){100, 255, 100, 255});
        DrawCircle(playerPosition.x - 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
        DrawCircle(playerPosition.x + 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
        DrawRectangle(playerPosition.x - 8, playerPosition.y + 2, 16, 2, (Color){0, 0, 0, 255});

        for (int i = 0; i < 32; i++)
        {
            if (enemies[i].health > 0)
            {
                DrawEnemy(&enemies[i]);
            }
        }

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

                case magicMissile:
                    DrawSpellMagicMissile(spellEntities[i].position, spellEntities[i].aim, enemies[spellEntities[i].targetIndex].position);
                    break;

                default:
                    break;
                }
            }
        }

        DrawMagicCircle(playerPosition, magicCircle, ringCount, &angle);

        /* if (redRectx.x != 0 || redRectx.y != 0)
        {
            DrawRectangleLines(redRectx.x * tileSize, redRectx.y * tileSize, tileSize, tileSize, (Color){255, 0, 0, 255});
        }
        if (redRecty.x != 0 || redRecty.y != 0)
        {
            DrawRectangleLines(redRecty.x * tileSize, redRecty.y * tileSize, tileSize, tileSize, (Color){255, 0, 0, 255});
        } */

        /*
        DrawLine(playerPosition.x, playerPosition.y, playerPosition.x + spellAim.x * tileSize, playerPosition.y + spellAim.y * tileSize, (Color){255, 0, 255, 255});
        DrawLine(playerPosition.x, playerPosition.y, playerPosition.x + playerAim.x * tileSize, playerPosition.y + playerAim.y * tileSize, (Color){255, 255, 255, 255});


        for (int x = -1; x < 2; x++)
        {
            for (int y = -1; y < 2; y++)
            {
                DrawPoly((Vector2){(x + (int)(playerPosition.x / tileSize)) * tileSize, (y + (int)(playerPosition.y / tileSize)) * tileSize}, 4, 3, 0, PURPLE);
                DrawPoly((Vector2){(x + (int)(playerPosition.x / tileSize)) * tileSize + tileSize, (y + (int)(playerPosition.y / tileSize)) * tileSize + tileSize}, 4, 3, 0, ORANGE);
            }
        }
        DrawText(TextFormat("%s", GetGamepadName(1)), 10, 10, 20, (Color){255, 255, 255, 255});
        DrawText(TextFormat("%f", GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)), 10, 40, 20, (Color){255, 255, 255, 255});
        DrawText(TextFormat("%d", (int)littleBoolOfMine), 10, 70, 20, WHITE);
        */

        EndDrawing();
    }

    CloseWindow();

    return 0;
}