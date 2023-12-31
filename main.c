#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "magic.c"
#include "terrainGen.c"
#include "pathfinding.c"
#include "enemy.c"
#include "score.c"
#include "boss.c"


const int playerRadius = 20;
const int playerMaxHealth = 200;

typedef enum GameState
{
    STATE_MENU,
    STATE_GAME,
    STATE_SPELLBOOK,
    STATE_GAMEOVER
} GameState;

bool rectCollision(Rectangle rect, Vector2 point)
{
    return (rect.x < point.x && rect.x + rect.width > point.x && rect.y < point.y && rect.y + rect.height > point.y);
}

int main()
{
    GameState gameState = STATE_MENU;
    time_t t;
    srand((unsigned)time(&t));

    // ----------------
    // DISPLAY SETTINGS
    // ----------------
    // 1120,1280 är *5
    v2f windowSize = {1120, 1286};
    // v2f windowSize = {560, 606};
    // v2f windowSize = {1440, 900};
    // v2f windowSize = {2560, 1440};
    InitWindow(windowSize.x, windowSize.y, "SpellJam");
    //ToggleFullscreen();
    // SetTargetFPS(10);

    // Load textures
    Texture2D worldSprites = LoadTexture("Assets/world.png");

    // Initialize inputs
    bool upDown = false;
    bool downDown = false;
    bool leftDown = false;
    bool rightDown = false;
    bool eDown =false;
    bool squarePressed = false;
    bool trianglePressed = false;
    bool circlePressed = false;
    bool executePressed = false;

    // Initialize spell entities
    const int maxSpellEntities = 16;
    SpellEntity spellEntities[maxSpellEntities];
    for (int i = 0; i < maxSpellEntities; i++)
    {
        spellEntities[i] = (SpellEntity){0};
    }
    SpellEntity enemySpellEntities[maxSpellEntities];
    for (int i = 0; i < maxSpellEntities; i++)
    {
        enemySpellEntities[i] = (SpellEntity){0};
    }
    for (int i = 0; i < maxBossSpellEntities; i++)
    {
        bossSpellEntities[i] = (SpellEntity){0};
    }

    // Initialize player
    v2f playerPosition = {roomSize / 2 * tileSize + tileSize / 2, roomSize / 2 * tileSize + tileSize / 2};
    v2f playerAim = {0, 0};
    v2f spellAim = {0, -1};
    int playerMoveSpeed = 150; // 150
    float playerHealth = playerMaxHealth;
    float playerMana = 200;
    float manaAngle = 0;
    float shield = 0;

    // Initialize spell casting
    Incantation magicCircle[16];
    int ringCount = 0;
    float angle = 0;

    //Initialize room
    Room room;
    RoomGrid roomGrid = RoomCreator();
    Point roomPOS;
    roomPOS.x = floor(roomGridSize / 2);
    roomPOS.y = floor(roomGridSize / 2);
    bool hasLeftDoor = true;
    printf("current room is %d, %d\n", roomPOS.x, roomPOS.y);

    //Initialize variables for score
    float timeElapsed = 0;
    bool gameOverFirstPass = true;
    int roomsCleared = 0;
    int highscore = 0;
    int bossCleared = 0;

    // Spawn enemies
    Enemy enemies[roomGridSize][roomGridSize][maxEnemies] = {0};
    for (int i = 0; i < roomGridSize; i++)
    {
        for (int j = 0; j < roomGridSize; j++)
        {
            if (!roomGrid.data[i][j].empty && (i != roomPOS.x || j != roomPOS.y))
            {
                int enemyCountSpawning = 0;
                for (int k = 0; k < 3; k++)
                {
                    int spawnAttempts = 1;
                    for (int l = 0; l < spawnAttempts; l++)
                    {
                        int a = GetRandomValue(5, roomSize - 6);
                        int b = GetRandomValue(5, roomSize - 6);
                        if (roomGrid.data[i][j].data[a][b][0] == TILE_TYPE_EMPTY)
                        {
                            enemies[i][j][enemyCountSpawning] = (Enemy){(Vector2){a * tileSize, b * tileSize}, (Vector2){1, 1}, 100, 70, 70, 0, {0}, AITYPE_WARRIOR, AISTATE_CHASE};
                            enemyCountSpawning++;
                            break;
                        }
                    }
                    for (int l = 0; l < spawnAttempts; l++)
                    {
                        int a = GetRandomValue(5, roomSize - 6);
                        int b = GetRandomValue(5, roomSize - 6);
                        if (roomGrid.data[i][j].data[a][b][0] == TILE_TYPE_EMPTY)
                        {
                            enemies[i][j][enemyCountSpawning] = (Enemy){(Vector2){a * tileSize, b * tileSize}, (Vector2){1, 1}, 70, 100, 100, 0, {0}, AITYPE_MAGE, AISTATE_CHASE};
                            enemyCountSpawning++;
                            break;
                        }
                    }
                }
                // printf("Spawned %d enemies in room %d, %d\n", enemyCountSpawning, i, j);
            }
        }
    }
    // Place boss in boss room
    enemies[roomGrid.bossRoomPOS.x][roomGrid.bossRoomPOS.y][0] = (Enemy){bossPosition, (v2f){0,0}, 0, bossMaxHealth, bossMaxHealth, 0, {0}, AITYPE_BOSS, AISTATE_IDLE};
    

    // ---------
    // GAME LOOP
    // ---------
    while (!WindowShouldClose())
    {
        switch (gameState)
        {
        case STATE_MENU:
        {
            executePressed = IsKeyPressed(KEY_DOWN);
            eDown = IsKeyPressed(KEY_E);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("This is where your joureny starts traveller.\nBeware, danger lies this way \n \n Press down arrrow to continue"), 20, roomGridSize*tileSize/2, 40, (Color){200, 200, 250, 255});
            DrawText(TextFormat("Move with WASD.\nCast spells with arrow keys\nOpen spell book with 'E'"), 20, roomGridSize+1*tileSize/2, 20, (Color){200, 200, 250, 255});
            EndDrawing();
            if(executePressed){
                gameState = STATE_GAME;
            }
            else if(eDown){
                gameState = STATE_SPELLBOOK;
            }
        }
        break;
        case STATE_GAME:
        {
            timeElapsed += GetFrameTime();
            room = roomGrid.data[roomPOS.x][roomPOS.y];
            // ------
            // INPUTS
            // ------
            upDown = IsKeyDown(KEY_W);
            downDown = IsKeyDown(KEY_S);
            leftDown = IsKeyDown(KEY_A);
            rightDown = IsKeyDown(KEY_D);
            eDown = IsKeyPressed(KEY_E);
            squarePressed = IsKeyPressed(KEY_LEFT);
            trianglePressed = IsKeyPressed(KEY_UP);
            circlePressed = IsKeyPressed(KEY_RIGHT);
            executePressed = IsKeyPressed(KEY_DOWN);
            //Checks wheter or not to open spell book
            if(eDown){
                gameState=STATE_SPELLBOOK;
            }

            // Determine where the player is aimed
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
            v2f playerMovePosition = Vector2Add(playerPosition, Vector2Scale(playerAim, (playerMoveSpeed * GetFrameTime())));
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
            // DOORS
            if (hasLeftDoor)
            {
                // North
                if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_DOOR_NORTH)
                {
                    // Exits south in the next room
                    roomPOS.y++;
                    playerMovePosition.y = ((roomSize - 1) * tileSize) + tileSize / 2;
                    hasLeftDoor = false;
                    puts("North facing door collision");
                    printf("current room is %d, %d\n", roomPOS.x, roomPOS.y);
                }
                // East
                if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_DOOR_EAST)
                {
                    // Exits west in the next room
                    playerMovePosition.x = tileSize / 2;
                    roomPOS.x++;
                    hasLeftDoor = false;
                    puts("East facing door collision");
                    printf("current room is %d, %d\n", roomPOS.x, roomPOS.y);
                }
                // South
                if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_DOOR_SOUTH)
                {
                    // Exits North in the next room
                    playerMovePosition.y = tileSize / 2;
                    roomPOS.y--;
                    hasLeftDoor = false;
                    puts("South facing door collision");
                    printf("current room is %d, %d\n", roomPOS.x, roomPOS.y);
                }
                // West
                if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][0] == TILE_TYPE_DOOR_WEST)
                {
                    // Exits East in the next room
                    playerMovePosition.x = ((roomSize - 1) * tileSize) + tileSize / 2;
                    roomPOS.x--;
                    hasLeftDoor = false;
                    puts("West facing door collision");
                    printf("current room is %d, %d\n", roomPOS.x, roomPOS.y);
                }
                if (!hasLeftDoor)
                {
                    for (int i = 0; i < maxSpellEntities; i++)
                    {
                        spellEntities[i] = (SpellEntity){0};
                        enemySpellEntities[i] = (SpellEntity){0};
                    }
                }
            }
            else if (room.data[(int)((playerPosition.x + playerRadiusVector.x) / tileSize)][(int)((playerPosition.y + playerRadiusVector.y) / tileSize)][1] != TILE_TYPE_DOOR)
            {
                for (int x = -1; x < 2; x++)
                {
                    for (int y = -1; y < 2; y++)
                    {
                        if (x + (int)(playerPosition.x / tileSize) >= 0 && y + (int)(playerPosition.y / tileSize) >= 0 && x + (int)(playerPosition.x / tileSize) < roomSize && y + (int)(playerPosition.y / tileSize) < roomSize)
                        {
                            if (room.data[x + (int)(playerPosition.x / tileSize)][y + (int)(playerPosition.y / tileSize)][1] == TILE_TYPE_DOOR)
                            {
                                goto stillInDoor;
                            }
                        }
                    }
                }
                hasLeftDoor = true;
            stillInDoor:
            {
            }
            }

            // ---------
            // CORNERING
            // ---------
            // For each of the 9 tiles around the player
            for (int x = -1; x < 2; x++)
            {
                for (int y = -1; y < 2; y++)
                {
                    // Avoid index out of range
                    if (x + (int)(playerPosition.x / tileSize) >= 0 && y + (int)(playerPosition.y / tileSize) >= 0 && x + (int)(playerPosition.x / tileSize) < roomSize && y + (int)(playerPosition.y / tileSize) < roomSize)
                    {
                        // Switch based on tile type
                        switch (room.data[x + (int)(playerPosition.x / tileSize)][y + (int)(playerPosition.y / tileSize)][1])
                        {
                        case TILE_TYPE_CORNER_NORTH_EAST:
                        {
                            // Vector for the position of the colliding corner
                            v2f cornerVector = (Vector2){
                                (x + 1 + (int)(playerPosition.x / tileSize)) * tileSize,
                                (y + (int)(playerPosition.y / tileSize)) * tileSize};
                            // Check for collision with the corner
                            if (CheckCollisionPointCircle(cornerVector, playerPosition, playerRadius))
                            {
                                if (Vector2Distance(playerMovePosition, cornerVector) < playerRadius)
                                {
                                    // Slide the player around the corner based on where they are headed
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

            // Collision with map edges
            if (playerMovePosition.x > roomSize * tileSize - playerRadius)
            {
                playerMovePosition.x = roomSize * tileSize - playerRadius;
            }
            if (playerMovePosition.x < playerRadius)
            {
                playerMovePosition.x = playerRadius;
            }
            if (playerMovePosition.y > roomSize * tileSize - playerRadius)
            {
                playerMovePosition.y = roomSize * tileSize - playerRadius;
            }
            if (playerMovePosition.y < playerRadius)
            {
                playerMovePosition.y = playerRadius;
            }

            // Actually move the player, finally
            playerPosition = playerMovePosition;

            // -------------
            // Spell Casting
            // -------------
            if (playerMana < 200)
            {
                playerMana += 20 * GetFrameTime();
                if (playerMana > 200)
                {
                    playerMana = 200;
                }
            }

            // Avoid making the spell aim 0, 0
            if (playerAim.x != 0 || playerAim.y != 0)
            {
                spellAim = playerAim;
            }
            if (executePressed)
            {
                // Assign the last incantation of the magic circle as execute
                magicCircle[ringCount] = execute;
                // For each spell match the player's incantations with the spell's
                for (int spellIndex = 0; spellIndex < spellBookCount; spellIndex++)
                {
                    for (int ringIndex = 0; ringIndex < ringCount + 1; ringIndex++)
                    {
                        // If there's no match move on to the next spell
                        if (magicCircle[ringIndex] != spellBook[spellIndex].Incantation[ringIndex])
                        {
                            break;
                        }
                        else
                        {
                            // If everything has been matching and we have reached execute cast the spell
                            if (magicCircle[ringIndex] == execute)
                            {
                                if (playerMana < (ringCount - 1) * 20)
                                {
                                    goto spellCast;
                                }
                                playerMana -= (ringCount - 1) * 20;

                                switch (spellBook[spellIndex].name)
                                {
                                // Basic projectile spells
                                case fireBall:
                                case manaSpark:
                                case chromaticOrb:
                                {
                                    // Find an empty slot in the spell entity array and create a projectile
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
                                    // Make three projectiles
                                    for (int i = 0; i < 3; i++)
                                    {
                                        // Find an empty slot in the spell entity array
                                        for (int k = 0; k < maxSpellEntities; k++)
                                        {
                                            if (spellEntities[k].lifetime <= 0)
                                            {
                                                // Index of the target enemy
                                                int targetIndex = -1;
                                                float magicMissileStartSpeed = 800;

                                                spellEntities[k] = (SpellEntity){spellBook[spellIndex].startingLifeTime, spellBook[spellIndex].name, Vector2Add(playerPosition, Vector2Rotate(Vector2Scale(spellAim, 30), (-70 + i * 70) * DEG2RAD)), Vector2Scale(Vector2Rotate(spellAim, (-10 + i * 10) * DEG2RAD), magicMissileStartSpeed), targetIndex};

                                                // Set the target index to the index of the closest enemy
                                                for (int enemyIndex = 0; enemyIndex < maxEnemies; enemyIndex++)
                                                {
                                                    if (enemies[roomPOS.x][roomPOS.y][enemyIndex].health > 0)
                                                    {
                                                        targetIndex = enemyIndex;
                                                        break;
                                                    }
                                                }
                                                if (targetIndex != -1)
                                                {
                                                    for (int enemyIndex = 0; enemyIndex < maxEnemies; enemyIndex++)
                                                    {
                                                        if (enemies[roomPOS.x][roomPOS.y][enemyIndex].health > 0)
                                                        {
                                                            if (Vector2Distance(spellEntities[k].position, enemies[roomPOS.x][roomPOS.y][targetIndex].position) > Vector2Distance(spellEntities[k].position, enemies[roomPOS.x][roomPOS.y][enemyIndex].position))
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
                                    // Find an empty slot and place moon beam in front of the player
                                    for (int k = 0; k < maxSpellEntities; k++)
                                    {
                                        if (spellEntities[k].lifetime <= 0)
                                        {
                                            spellEntities[k] = (SpellEntity){spellBook[spellIndex].startingLifeTime, spellBook[spellIndex].name, Vector2Add(Vector2Scale(spellAim, 100), playerPosition), (v2f){0, 0}, 0};
                                            break;
                                        }
                                    }
                                    goto spellCast;
                                }
                                case block:
                                {
                                    shield = spellBook[spellIndex].startingLifeTime;
                                    break;
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
            // Clear the magic circle of incantations
            spellCast:
                ringCount = 0;
            }
            // Add incantations to the magic circle
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
            
            // ------------
            // ENEMY UPDATE
            // ------------
            for (int i = 0; i < maxEnemies; i++)
            {
                if (enemies[roomPOS.x][roomPOS.y][i].health > 0)
                {
                    switch (enemies[roomPOS.x][roomPOS.y][i].type)
                    {
                    case AITYPE_WARRIOR:
                        UpdateEnemy(&enemies[roomPOS.x][roomPOS.y][i], playerPosition, &playerHealth, shield, &room);
                        break;

                    case AITYPE_MAGE:
                    {
                        if (UpdateEnemy(&enemies[roomPOS.x][roomPOS.y][i], playerPosition, &playerHealth, shield, &room) != -1)
                        {
                            // Find an empty slot in the spell entity array and create a projectile
                            for (int k = 0; k < maxSpellEntities; k++)
                            {
                                if (enemySpellEntities[k].lifetime <= 0)
                                {
                                    enemySpellEntities[k] = (SpellEntity){1, manaSpark, enemies[roomPOS.x][roomPOS.y][i].position, Vector2Normalize(Vector2Subtract(playerPosition, enemies[roomPOS.x][roomPOS.y][i].position)), 0};
                                    break;
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

            // -----------
            // BOSS UPDATE
            // -----------
            if (roomPOS.x == roomGrid.bossRoomPOS.x && roomPOS.y == roomGrid.bossRoomPOS.y && bossNotSpawned && CheckCollisionPointCircle(playerPosition, (v2f){tileSize * (roomSize * 0.5), tileSize * (roomSize * 0.8 + 1)}, 60))
            {
                bossNotSpawned = false;
                puts("Boss spawned");
            }
            if (roomPOS.x == roomGrid.bossRoomPOS.x && roomPOS.y == roomGrid.bossRoomPOS.y && !bossNotSpawned)
            {
                if(UpdateBoss(playerPosition, &playerHealth, &enemies, roomPOS)){
                    gameState=STATE_GAMEOVER;
                    bossCleared=2500;
                }
            }

            // -------------
            // SPELL UPDATES
            // -------------
            if (shield > 0)
            {
                shield -= GetFrameTime();
            }
            for (int i = 0; i < maxSpellEntities; i++)
            {
                if (spellEntities[i].lifetime > 0)
                {
                    switch (spellEntities[i].name)
                    {
                    case manaSpark:
                        spellEntities[i].position.x += spellEntities[i].aim.x * 500 * GetFrameTime();
                        spellEntities[i].position.y += spellEntities[i].aim.y * 500 * GetFrameTime();
                        // Check that is inside the map
                        if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, spellEntities[i].position))
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        // Check room collision
                        if (room.data[(int)(spellEntities[i].position.x / tileSize)][(int)(spellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        for (int j = 0; j < maxEnemies; j++)
                        {
                            if (enemies[roomPOS.x][roomPOS.y][j].health > 0)
                            {
                                if (CheckCollisionCircles(spellEntities[i].position, 10, enemies[roomPOS.x][roomPOS.y][j].position, enemyRadius + (bossRadius * (enemies[roomPOS.x][roomPOS.y][j].type == AITYPE_BOSS))))
                                {
                                    enemies[roomPOS.x][roomPOS.y][j].health -= 10;
                                    spellEntities[i].lifetime = 0;
                                }
                            }
                        }
                        break;

                    case fireBall:
                        spellEntities[i].position.x += spellEntities[i].aim.x * 500 * GetFrameTime();
                        spellEntities[i].position.y += spellEntities[i].aim.y * 500 * GetFrameTime();
                        // Check that is inside the map
                        if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, spellEntities[i].position))
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        // Check room collision
                        if (room.data[(int)(spellEntities[i].position.x / tileSize)][(int)(spellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        for (int j = 0; j < maxEnemies; j++)
                        {
                            if (enemies[roomPOS.x][roomPOS.y][j].health > 0)
                            {
                                if (CheckCollisionCircles(spellEntities[i].position, 10, enemies[roomPOS.x][roomPOS.y][j].position, enemyRadius + (bossRadius * (enemies[roomPOS.x][roomPOS.y][j].type == AITYPE_BOSS))))
                                {
                                    enemies[roomPOS.x][roomPOS.y][j].health -= 20;
                                    spellEntities[i].lifetime = 0;
                                    for (int buffIndex = 0; buffIndex < 8; buffIndex++)
                                    {
                                        if (enemies[roomPOS.x][roomPOS.y][j].buffs[buffIndex].duration <= 0)
                                        {
                                            enemies[roomPOS.x][roomPOS.y][j].buffs[buffIndex] = (Buff){5, 5};
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        break;

                    case moonBeam:
                        spellEntities[i].lifetime -= GetFrameTime();
                        for (int j = 0; j < maxEnemies; j++)
                        {
                            if (enemies[roomPOS.x][roomPOS.y][j].health > 0)
                            {
                                if (CheckCollisionCircles(spellEntities[i].position, 50, enemies[roomPOS.x][roomPOS.y][j].position, enemyRadius + (bossRadius * (enemies[roomPOS.x][roomPOS.y][j].type == AITYPE_BOSS))))
                                {
                                    enemies[roomPOS.x][roomPOS.y][j].health -= 20 * GetFrameTime();
                                }
                            }
                        }
                        break;

                    case chromaticOrb:

                        spellEntities[i].position.x += spellEntities[i].aim.x * 300 * GetFrameTime();
                        spellEntities[i].position.y += spellEntities[i].aim.y * 300 * GetFrameTime();
                        spellEntities[i].lifetime += GetFrameTime();
                        // Check that is inside the map
                        if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, spellEntities[i].position))
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        // Check room collision
                        if (room.data[(int)(spellEntities[i].position.x / tileSize)][(int)(spellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        for (int j = 0; j < maxEnemies; j++)
                        {
                            if (enemies[roomPOS.x][roomPOS.y][j].health > 0)
                            {
                                if (CheckCollisionCircles(spellEntities[i].position, 16, enemies[roomPOS.x][roomPOS.y][j].position, enemyRadius + (bossRadius * (enemies[roomPOS.x][roomPOS.y][j].type == AITYPE_BOSS))))
                                {
                                    enemies[roomPOS.x][roomPOS.y][j].health -= 150 * GetFrameTime();
                                }
                            }
                        }
                        break;

                    case magicMissile:
                    {
                        // Check that is inside the map
                        if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, spellEntities[i].position))
                        {
                            spellEntities[i].lifetime = 0;
                        }
                        // If the target is dead find a new target
                        if (enemies[roomPOS.x][roomPOS.y][spellEntities[i].targetIndex].health <= 0)
                        {
                            int targetIndex = -1;
                            for (int enemyIndex = 0; enemyIndex < maxEnemies; enemyIndex++)
                            {
                                if (enemies[roomPOS.x][roomPOS.y][enemyIndex].health > 0)
                                {
                                    targetIndex = enemyIndex;
                                    break;
                                }
                            }
                            if (targetIndex != -1)
                            {
                                for (int enemyIndex = 0; enemyIndex < maxEnemies; enemyIndex++)
                                {
                                    if (enemies[roomPOS.x][roomPOS.y][enemyIndex].health > 0)
                                    {
                                        if (Vector2Distance(spellEntities[i].position, enemies[roomPOS.x][roomPOS.y][targetIndex].position) > Vector2Distance(spellEntities[i].position, enemies[roomPOS.x][roomPOS.y][enemyIndex].position))
                                        {
                                            targetIndex = enemyIndex;
                                        }
                                    }
                                }
                            }
                            spellEntities[i].targetIndex = targetIndex;
                        }

                        float magicMissileSpeed = 800;
                        float magicMissileSteering = 1600 * GetFrameTime();

                        // If there's a target perform a seeking steering behaviour
                        if (spellEntities[i].targetIndex != -1)
                        {
                            v2f desired = Vector2Scale(Vector2Normalize(Vector2Subtract(enemies[roomPOS.x][roomPOS.y][spellEntities[i].targetIndex].position, spellEntities[i].position)), magicMissileSpeed);
                            v2f steering = Vector2Subtract(desired, spellEntities[i].aim);
                            steering = Vector2Scale(Vector2Normalize(steering), magicMissileSteering);
                            spellEntities[i].aim = Vector2Add(spellEntities[i].aim, steering);
                        }

                        // Add aim to position accounting for frametime
                        spellEntities[i].position.x += spellEntities[i].aim.x * GetFrameTime();
                        spellEntities[i].position.y += spellEntities[i].aim.y * GetFrameTime();

                        // Check enemy collision
                        for (int j = 0; j < maxEnemies; j++)
                        {
                            if (enemies[roomPOS.x][roomPOS.y][j].health > 0)
                            {
                                if (CheckCollisionPointCircle(spellEntities[i].position, enemies[roomPOS.x][roomPOS.y][j].position, enemyRadius + (bossRadius * (enemies[roomPOS.x][roomPOS.y][j].type == AITYPE_BOSS))))
                                {
                                    enemies[roomPOS.x][roomPOS.y][j].health -= 10;
                                    spellEntities[i].lifetime = 0;
                                }
                            }
                        }
                        // Check room collision
                        if (room.data[(int)(spellEntities[i].position.x / tileSize)][(int)(spellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                        {
                            spellEntities[i].lifetime = 0;
                        }
                    }
                    break;

                    default:
                        break;
                    }
                }
            }
            // Enemy spell updates
            for (int i = 0; i < maxSpellEntities; i++)
            {
                if (enemySpellEntities[i].lifetime > 0)
                {
                    switch (enemySpellEntities[i].name)
                    {
                    case manaSpark:
                    {
                        enemySpellEntities[i].position.x += enemySpellEntities[i].aim.x * 500 * GetFrameTime();
                        enemySpellEntities[i].position.y += enemySpellEntities[i].aim.y * 500 * GetFrameTime();
                        // Check that is inside the map
                        if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, enemySpellEntities[i].position))
                        {
                            enemySpellEntities[i].lifetime = 0;
                        }
                        // Check room collision
                        if (room.data[(int)(enemySpellEntities[i].position.x / tileSize)][(int)(enemySpellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                        {
                            enemySpellEntities[i].lifetime = 0;
                        }
                        if (CheckCollisionCircles(enemySpellEntities[i].position, 10, playerPosition, playerRadius))
                        {
                            if (shield <= 0)
                            {
                                playerHealth -= 10;
                            }
                            enemySpellEntities[i].lifetime = 0;
                        }
                    }
                    break;

                    default:
                        break;
                    }
                }
            }

            // Boss spell updates
            if (roomPOS.x == roomGrid.bossRoomPOS.x && roomPOS.y == roomGrid.bossRoomPOS.y)
            {
                for (int i = 0; i < maxBossSpellEntities; i++)
                {
                    if (bossSpellEntities[i].lifetime > 0)
                    {
                        switch (bossSpellEntities[i].name)
                        {
                        case manaSpark:
                        {
                            bossSpellEntities[i].position.x += bossSpellEntities[i].aim.x * 500 * GetFrameTime();
                            bossSpellEntities[i].position.y += bossSpellEntities[i].aim.y * 500 * GetFrameTime();
                            // Check that is inside the map
                            if (!rectCollision((Rectangle){0, 0, roomSize * tileSize, roomSize * tileSize}, bossSpellEntities[i].position))
                            {
                                bossSpellEntities[i].lifetime = 0;
                            }
                            // Check room collision
                            if (room.data[(int)(bossSpellEntities[i].position.x / tileSize)][(int)(bossSpellEntities[i].position.y / tileSize)][0] == TILE_TYPE_WALL)
                            {
                                bossSpellEntities[i].lifetime = 0;
                            }
                            if (CheckCollisionCircles(bossSpellEntities[i].position, 10, playerPosition, playerRadius))
                            {
                                if (shield <= 0)
                                {
                                    playerHealth -= 10;
                                }
                                bossSpellEntities[i].lifetime = 0;
                            }
                        }
                        break;

                        case moonBeam:
                        {
                            bossSpellEntities[i].lifetime -= GetFrameTime();
                            if (CheckCollisionCircles(bossSpellEntities[i].position, 50, playerPosition, playerRadius))
                            {
                                playerHealth -= 20 * GetFrameTime();
                            }
                        } break;

                        default:
                            break;
                        }
                    }
                }
            }

            // checks if player has perished
            if(playerHealth<=0 ){
                gameState=STATE_GAMEOVER;
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
                    default:
                        break;
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

            if (roomPOS.x == roomGrid.bossRoomPOS.x && roomPOS.y == roomGrid.bossRoomPOS.y)
            {
                DrawCircle(tileSize * (roomSize * 0.5), tileSize * (roomSize * 0.8 + 1), 60, (Color){255, 255, 255, 100});
            }

            DrawCircle(playerPosition.x + 1, playerPosition.y + 1, playerRadius, (Color){0, 0, 0, 255});
            DrawCircle(playerPosition.x, playerPosition.y, playerRadius, (Color){100, 255, 100, 255});
            DrawCircle(playerPosition.x - 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
            DrawCircle(playerPosition.x + 12, playerPosition.y - 1, 2, (Color){0, 0, 0, 255});
            DrawRectangle(playerPosition.x - 8, playerPosition.y + 2, 16, 2, (Color){0, 0, 0, 255});

            // Draw Enemies
            for (int i = 0; i < maxEnemies; i++)
            {
                if (enemies[roomPOS.x][roomPOS.y][i].health > 0)
                {
                    if (enemies[roomPOS.x][roomPOS.y][i].type == AITYPE_BOSS)
                    {
                        DrawBoss(enemies[roomPOS.x][roomPOS.y][i].health);
                    }
                    else
                    {
                        DrawEnemy(&enemies[roomPOS.x][roomPOS.y][i]);
                    }
                }
            }

            if (shield > 0)
            {
                DrawSpellBlock(playerPosition);
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
                        DrawSpellMagicMissile(spellEntities[i].position, spellEntities[i].aim, enemies[roomPOS.x][roomPOS.y][spellEntities[i].targetIndex].position);
                        break;

                    default:
                        break;
                    }
                }
            }
            for (int i = 0; i < maxSpellEntities; i++)
            {
                if (enemySpellEntities[i].lifetime > 0)
                {
                    switch (enemySpellEntities[i].name)
                    {
                    case manaSpark:
                        DrawSpellManaSpark(enemySpellEntities[i].position, enemySpellEntities[i].aim);
                        break;

                    default:
                        break;
                    }
                }
            }
            if (roomPOS.x == roomGrid.bossRoomPOS.x && roomPOS.y == roomGrid.bossRoomPOS.y)
            {
                for (int i = 0; i < maxBossSpellEntities; i++)
                {
                    if (bossSpellEntities[i].lifetime > 0)
                    {
                        switch (bossSpellEntities[i].name)
                        {
                        case manaSpark:
                            DrawSpellManaSpark(bossSpellEntities[i].position, bossSpellEntities[i].aim);
                            break;

                        case moonBeam:
                            DrawSpellMoonBeam(bossSpellEntities[i].position, bossSpellEntities[i].lifetime);
                            break;

                        default:
                            break;
                        }
                    }
                }
            }

            angle += 1 * GetFrameTime();
            if (angle > 360)
            {
                angle -= 360;
            }
            DrawMagicCircle(playerPosition, magicCircle, ringCount, angle);

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
            DrawText(TextFormat("%f", GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)), 10, 40, 20, (Color){255, 255, 255, 255});
            DrawText(TextFormat("%d", (int)littleBoolOfMine), 10, 70, 20, WHITE);
            */

            /*
            TODO
            - Find out why polygons can't be drawn here
            */
            // DRAW HUD
            v2f manaCirclePosition = Vector2Subtract(windowSize, (v2f){50, 50});
            int radius = 50 / sin(30 * DEG2RAD);
            int lineThickness = 3;
            manaAngle += 1 * GetFrameTime();
            DrawPolyLinesEx((v2f){100, 100}, 3, radius, ((manaAngle * 3) / (radius * 0.0002 + 0.01)), lineThickness + 5, (Color){0, 255, 255});
            DrawPolyLinesEx(manaCirclePosition, 3, radius, ((manaAngle * 3) / (radius * 0.0002 + 0.01)) + 60, lineThickness + 5, (Color){0, 255, 255});
            DrawPoly(playerPosition, 3, 50, 0, (Color){0, 255, 255});
            // DrawCircle(500,500,50,WHITE);
            // manaAngle = -manaAngle;
            DrawText(TextFormat("Health %f", playerHealth), 10, 10, 20, (Color){255, 255, 255, 255});
            DrawText(TextFormat("Mana %f", playerMana), 10, 40, 20, (Color){255, 255, 255, 255});
            // DrawText(TextFormat("Has left door %d", hasLeftDoor), 10, 70, 20, (Color){255, 255, 255, 255});
            
            EndDrawing();
        }
        break;
        case STATE_SPELLBOOK:
        {
            executePressed = IsKeyPressed(KEY_DOWN);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("cast spells\nmanaSpark: left, left, down\nblock: right, right, down\nfireBall: left, right, left, down\nmoonBeam: left, left, right, left, up, down\nchromaticOrb: left, up, right, left, down\nmagicMissile: left, up, up, left, down\n\nPress down to exit"), 20, roomGridSize-1*tileSize/2, 20, (Color){200, 200, 250, 255});
            EndDrawing();
            if(executePressed){
                gameState = STATE_GAME;
            }
        }
        break;
        case STATE_GAMEOVER:
        {
            if(gameOverFirstPass == true){
                for(int i = 0; i < roomGridSize; i ++){
                    for(int j = 0; j < roomGridSize; j++){
                        if(roomGrid.data[j][i].empty==false){
                            if(IsRoomCleared(enemies[j][i])){
                                roomsCleared++;
                            }
                        }
                    }
                }
                
                highscore = UpdateHighScore(CalculateScore(roomsCleared, playerHealth, playerMaxHealth, timeElapsed, bossCleared));
                gameOverFirstPass = false;
            }
            
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("This is where your journey ends traveller\n your final score was %d, the high score is %d", CalculateScore(roomsCleared, playerHealth, playerMaxHealth, timeElapsed, bossCleared) , highscore), 20, roomGridSize*tileSize/2, 40, (Color){200, 200, 250, 255});
            EndDrawing();
        }
        break;

        default:
            break;
        }
    }
    CloseWindow();
    return 0;
}