#include <stdlib.h>
#include <stdbool.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raymath.h"

/*
TODO
 - Separate cooldown and attacking action
*/

typedef enum AIType
{
    archer,
    mage,
    warrior
} AIType;

typedef enum AIState
{
    idle,
    chase,
    attack,
    flee
} AIState;

typedef struct Buff
{
    float duration;
    float damage;
} Buff;

const int enemyRadius = 20;
const float attackCooldown = 1.0f;
const int maxBuffCount = 8;

typedef struct Enemy
{
    Vector2 position;
    Vector2 direction;
    float speed;
    float health;
    int maxHealth;
    float attackCooldown;
    Buff buffs[maxBuffCount];
    AIType type;
    AIState state;
} Enemy;

void DrawEnemy(Enemy *enemy)
{
    DrawCircle(enemy->position.x + 1, enemy->position.y + 1, 20, (Color){0, 0, 0, 255});
    DrawCircle(enemy->position.x, enemy->position.y, 20, (Color){255, 100, 100, 255});
    DrawCircle(enemy->position.x - 12, enemy->position.y - 1, 2, (Color){0, 0, 0, 255});
    DrawCircle(enemy->position.x + 12, enemy->position.y - 1, 2, (Color){0, 0, 0, 255});
    DrawRectangle(enemy->position.x - 8, enemy->position.y + 2, 16, 2, (Color){0, 0, 0, 255});

    // Draw health bar
    DrawRectangle(enemy->position.x - 20, enemy->position.y + 30, 40, 5, (Color){255, 0, 0, 255});
    DrawRectangle(enemy->position.x - 20, enemy->position.y + 30, 40 * ((float)enemy->health / (float)enemy->maxHealth), 5, (Color){0, 255, 0, 255});

    // Draw attack stick
    DrawLineEx(Vector2Add(enemy->position,Vector2Scale(enemy->direction,enemyRadius)), Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemyRadius + (20 * enemy->attackCooldown))), 7, BLACK);
    DrawLineEx(Vector2Add(enemy->position,Vector2Scale(enemy->direction,enemyRadius)), Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemyRadius + (20 * enemy->attackCooldown))), 3, WHITE);
}

int UpdateEnemy(Enemy *enemy, Vector2 playerPosition, float *playerHealth, float shield, Room *room)
{
    int tileSize = 40;
    int returnValue = -1;
    for (int i = 0; i < maxBuffCount; i++)
    {
        if (enemy->buffs[i].duration > 0)
        {
            enemy->health -= enemy->buffs[i].damage * GetFrameTime();
            enemy->buffs[i].duration -= GetFrameTime();
        }
    }
    if (enemy->health > 0)
    {
        switch (enemy->type)
        {
        case warrior:
        {
            int engagementDistance = 50;
            switch (enemy->state)
            {
            case chase:
            {
                if (Vector2Distance(enemy->position, playerPosition) < engagementDistance)
                {
                    enemy->state = attack;
                    break;
                }
                else
                {
                    enemy->direction = Vector2Normalize(Vector2Subtract(playerPosition, enemy->position));
                    //enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemy->speed * GetFrameTime()));

                    // Broad phase collision
                    v2f enemyRadiusVector = {enemyRadius * enemy->direction.x, enemyRadius * enemy->direction.y};
                    v2f enemyMovePosition = Vector2Add(enemy->position, Vector2Scale(enemy->direction, (enemy->speed * GetFrameTime())));
                    if (room->data[(int)((enemyMovePosition.x + enemyRadiusVector.x) / tileSize)][(int)((enemy->position.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.x = enemy->position.x;
                    }
                    if (room->data[(int)((enemy->position.x + enemyRadiusVector.x) / tileSize)][(int)((enemyMovePosition.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.y = enemy->position.y;
                    }
                    enemy->position = enemyMovePosition;
                }
            }
            break;
            case attack:
            {
                if (Vector2Distance(enemy->position, playerPosition) > engagementDistance)
                {
                    enemy->state = chase;
                    break;
                }
                if (enemy->attackCooldown <= 0)
                {
                    if (shield <= 0)
                    {
                        *playerHealth -= 10;
                    }
                    enemy->attackCooldown = attackCooldown;
                }
                if (enemy->attackCooldown > 0)
                {
                    enemy->attackCooldown -= GetFrameTime();
                }
            }

            default:
                break;
            }
        }
        break;
        case mage:
        {
            int engagementDistance = 500;
            int disengageDistance = 400;
            switch (enemy->state)
            {
            case chase:
            {
                if (Vector2Distance(enemy->position, playerPosition) < engagementDistance)
                {
                    enemy->state = attack;
                }
                else
                {
                    enemy->direction = Vector2Normalize(Vector2Subtract(playerPosition, enemy->position));
                    
                    // Broad phase collision
                    v2f enemyRadiusVector = {enemyRadius * enemy->direction.x, enemyRadius * enemy->direction.y};
                    v2f enemyMovePosition = Vector2Add(enemy->position, Vector2Scale(enemy->direction, (enemy->speed * GetFrameTime())));
                    if (room->data[(int)((enemyMovePosition.x + enemyRadiusVector.x) / tileSize)][(int)((enemy->position.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.x = enemy->position.x;
                    }
                    if (room->data[(int)((enemy->position.x + enemyRadiusVector.x) / tileSize)][(int)((enemyMovePosition.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.y = enemy->position.y;
                    }
                    enemy->position = enemyMovePosition;
                }
            }
            break;

            case attack:
            {
                if (Vector2Distance(enemy->position, playerPosition) > engagementDistance)
                {
                    enemy->state = chase;
                    break;
                }
                if (Vector2Distance(enemy->position, playerPosition) < disengageDistance)
                {
                    enemy->state = flee;
                    break;
                }
                if (enemy->attackCooldown <= 0)
                {
                    enemy->attackCooldown = attackCooldown;
                    returnValue = manaSpark;
                }
                if (enemy->attackCooldown > 0)
                {
                    enemy->attackCooldown -= GetFrameTime();
                }
            }
            break;

            case flee:
            {
                if (Vector2Distance(enemy->position, playerPosition) > engagementDistance)
                {
                    enemy->state = chase;
                }
                else
                {
                    enemy->direction = Vector2Normalize(Vector2Subtract(enemy->position, playerPosition));
                    
                    // Broad phase collision
                    v2f enemyRadiusVector = {enemyRadius * enemy->direction.x, enemyRadius * enemy->direction.y};
                    v2f enemyMovePosition = Vector2Add(enemy->position, Vector2Scale(enemy->direction, (enemy->speed * GetFrameTime())));
                    if (room->data[(int)((enemyMovePosition.x + enemyRadiusVector.x) / tileSize)][(int)((enemy->position.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.x = enemy->position.x;
                    }
                    if (room->data[(int)((enemy->position.x + enemyRadiusVector.x) / tileSize)][(int)((enemyMovePosition.y + enemyRadiusVector.y) / tileSize)][0] == TILE_TYPE_WALL)
                    {
                        enemyMovePosition.y = enemy->position.y;
                    }
                    enemy->position = enemyMovePosition;
                }
            }
            break;

            default:
                break;
            }
        }
        break;

        default:
            break;
        }
    }
    return returnValue;
}