#include <stdlib.h>
#include <stdbool.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raymath.h"

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

typedef struct Enemy
{
    Vector2 position;
    Vector2 direction;
    float speed;
    int health;
    int maxHealth;
    AIType type;
    AIState state;
} Enemy;

void DrawEnemy(Enemy enemy)
{
    DrawCircle(enemy.position.x, enemy.position.y, 20, (Color){255, 100, 100, 255});
    DrawCircle(enemy.position.x - 12, enemy.position.y - 1, 2, (Color){0, 0, 0, 255});
    DrawCircle(enemy.position.x + 12, enemy.position.y - 1, 2, (Color){0, 0, 0, 255});
    DrawRectangle(enemy.position.x - 8, enemy.position.y + 2, 16, 2, (Color){0, 0, 0, 255});

    //Draw health bar
    DrawRectangle(enemy.position.x - 20, enemy.position.y + 30, 40, 5, (Color){255, 0, 0, 255});
    DrawRectangle(enemy.position.x - 20, enemy.position.y + 30, 40 * (enemy.health / enemy.maxHealth), 5, (Color){0, 255, 0, 255});
}

void UpdateEnemy(Enemy *enemy, Vector2 playerPosition)
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
            }
            else
            {
                enemy->direction = Vector2Normalize(Vector2Subtract(playerPosition, enemy->position));
                enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemy->speed * GetFrameTime()));
            }
        }
        break;
        case attack:
        {
            if (Vector2Distance(enemy->position, playerPosition) > engagementDistance)
            {
                enemy->state = chase;
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
                enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemy->speed * GetFrameTime()));
            }
        }
        break;

        case attack:
        {
            if (Vector2Distance(enemy->position, playerPosition) > engagementDistance)
            {
                enemy->state = chase;
            }
            if (Vector2Distance(enemy->position, playerPosition) < disengageDistance)
            {
                enemy->state = flee;
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
                enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->direction, enemy->speed * GetFrameTime()));
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