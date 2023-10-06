#include <stdlib.h>
#include <stdbool.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raymath.h"

typedef enum BossAttackType
{
    BOSSATTACKTYPE_NONE,
    BOSSATTACKTYPE_MANA_SPARK_BURST,
    BOSSATTACKTYPE_MOON_BEAMS,
    BOSSATTACKTYPE_SUMMON_MINIONS
} BossAttackType;

typedef struct BossAttack
{
    BossAttackType type;
    float startCooldown;
    float cooldown;
    float startDuration;
    float duration;
} BossAttack;
const int bossAttackCount = 3;

bool bossNotSpawned = true;
v2f bossPosition = {tileSize * roomSize * 0.5, tileSize * roomSize * 0.2};
int bossMaxHealth = 1000;
int bossHealth = 1000;
int minionCount = 0;
BossAttack bossAttacks[bossAttackCount] = {
    (BossAttack){BOSSATTACKTYPE_MANA_SPARK_BURST, 11, 6, 2, 2},
    (BossAttack){BOSSATTACKTYPE_MOON_BEAMS, 16, 8, 4, 4},
    (BossAttack){BOSSATTACKTYPE_SUMMON_MINIONS, 33, 30, 6, 6}};
const int maxBossSpellEntities = 40;
SpellEntity bossSpellEntities[maxBossSpellEntities];

void UpdateBoss(Vector2 playerPosition, float *playerHealth)
{
    int availableAttacks[bossAttackCount] = {0, 0, 0};
    int availableAttackCount = 0;

    int durations = 0;
    for (int i = 0; i < bossAttackCount; i++)
    {
        durations += bossAttacks[i].duration;
    }

    if (durations <= 0)
    {
        for (int i = 0; i < bossAttackCount; i++)
        {
            switch (bossAttacks[i].type)
            {
            case BOSSATTACKTYPE_MANA_SPARK_BURST:
            case BOSSATTACKTYPE_MOON_BEAMS:
            {
                if (bossAttacks[i].cooldown <= 0)
                {
                    availableAttacks[availableAttackCount] = bossAttacks[i].type;
                    availableAttackCount++;
                }
            }
            break;
            case BOSSATTACKTYPE_SUMMON_MINIONS:
            {
                if (bossAttacks[i].cooldown <= 0 && minionCount == 0)
                {
                    availableAttacks[availableAttackCount] = bossAttacks[i].type;
                    availableAttackCount++;
                }
            }
            break;

            default:
                break;
            }
        }
    }

    if (availableAttackCount > 0)
    {
        int attackIndex = GetRandomValue(0, availableAttackCount - 1);

        bossAttacks[attackIndex].cooldown = bossAttacks[attackIndex].startCooldown;
        bossAttacks[attackIndex].duration = bossAttacks[attackIndex].startDuration;
    }

    for (int i = 0; i < bossAttackCount; i++)
    {
        if (bossAttacks[i].duration > 0)
        {
            switch (bossAttacks[i].type)
            {
            case BOSSATTACKTYPE_MANA_SPARK_BURST:
            {
                int manaSparkCount = 10;
                for (int j = manaSparkCount - 1; j > -1; j--)
                {
                    if (
                        bossAttacks[i].duration > bossAttacks[i].startDuration / manaSparkCount * j &&
                        bossAttacks[i].duration - GetFrameTime() <= bossAttacks[i].startDuration / manaSparkCount * j)
                    {
                        for (int k = 0; k < maxBossSpellEntities; k++)
                        {
                            if (bossSpellEntities[k].lifetime <= 0)
                            {
                                float distanceFromBossToCast = 100;
                                v2f bossSpellAim = Vector2Rotate((v2f){0, 1}, ((120 / (manaSparkCount - 1) * j) - (120 / 2)) * DEG2RAD);
                                bossSpellEntities[k] = (SpellEntity){1, manaSpark, Vector2Add(bossPosition, Vector2Scale(bossSpellAim, distanceFromBossToCast)), bossSpellAim, 0};
                                break;
                            }
                        }
                    }
                }
            }
            break;

            case BOSSATTACKTYPE_MOON_BEAMS:

                break;

            case BOSSATTACKTYPE_SUMMON_MINIONS:

                break;
            default:
                break;
            }
        }
    }
}

void DrawBoss()
{
    DrawCircleV(bossPosition, 50, (Color){255, 0, 0, 255});
    DrawCircleV(bossPosition, 40, (Color){255, 255, 255, 255});
    DrawCircleV(bossPosition, 30, (Color){255, 0, 0, 255});
    DrawCircleV(bossPosition, 20, (Color){255, 255, 255, 255});
    DrawCircleV(bossPosition, 10, (Color){255, 0, 0, 255});
    DrawCircleV(bossPosition, 5, (Color){255, 255, 255, 255});
}