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
v2f bossPosition = {tileSize * roomSize * 0.5, tileSize *roomSize * 0.2};
int bossMaxHealth = 1000;
// Constant added onto enemy radius meaning the actual radius is enemyRadius + bossRadius
const int bossRadius = 30; 
int minionCount = 0;
BossAttack bossAttacks[bossAttackCount] = {
    (BossAttack){BOSSATTACKTYPE_MANA_SPARK_BURST, 11, 6, 2, 2},
    (BossAttack){BOSSATTACKTYPE_MOON_BEAMS, 16, 8, 4, 0},
    (BossAttack){BOSSATTACKTYPE_SUMMON_MINIONS, 33, 30, 6, 0}};
const int maxBossSpellEntities = 40;
SpellEntity bossSpellEntities[maxBossSpellEntities];
const int moonBeamCount = 10;
v2f moonBeamPositions[moonBeamCount] = {0};

bool UpdateBoss(Vector2 playerPosition, float *playerHealth, Enemy (*enemies)[roomGridSize][roomGridSize][maxEnemies], Point roomPOS)
{
    if((*enemies)[roomPOS.x][roomPOS.y][0].health <= 0)
    {
        return true;
    }
    else
    {
        int availableAttacks[bossAttackCount] = {0, 0, 0};
        int availableAttackCount = 0;

        float durations = 0;
        for (int i = 0; i < bossAttackCount; i++)
        {
            durations += bossAttacks[i].duration;
        }

        minionCount = 0;
        for (int i = 1; i < maxEnemies+1; i++)
        {
            if ((*enemies)[roomPOS.x][roomPOS.y][i].health > 0)
            {
                minionCount++;
            }
        }

        if (durations <= 0)
        {
            puts("Duration was 0");
            if (bossAttacks[BOSSATTACKTYPE_MANA_SPARK_BURST - 1].cooldown <= 0)
            {
                availableAttacks[availableAttackCount] = BOSSATTACKTYPE_MANA_SPARK_BURST;
                availableAttackCount++;
                printf("added %d but it turned into %d, mana burst\n", BOSSATTACKTYPE_MANA_SPARK_BURST, availableAttacks[availableAttackCount - 1]);
            }
            if (bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].cooldown <= 0)
            {
                availableAttacks[availableAttackCount] = BOSSATTACKTYPE_MOON_BEAMS;
                availableAttackCount++;
                printf("added %d but it turned into %d, moon beams\n", BOSSATTACKTYPE_MOON_BEAMS, availableAttacks[availableAttackCount - 1]);
            }
            if (bossAttacks[BOSSATTACKTYPE_SUMMON_MINIONS - 1].cooldown <= 0 && minionCount == 0)
            {
                availableAttacks[availableAttackCount] = BOSSATTACKTYPE_SUMMON_MINIONS;
                availableAttackCount++;
                printf("added %d but it turned into %d, summon minions\n", BOSSATTACKTYPE_SUMMON_MINIONS, availableAttacks[availableAttackCount - 1]);
            }
            if (availableAttackCount > 0)
            {
                int attackIndex = GetRandomValue(0, availableAttackCount - 1);

                printf("%d attacks available. Picked %d with type %d\n", availableAttackCount, attackIndex, bossAttacks[availableAttacks[attackIndex]].type);

                bossAttacks[availableAttacks[attackIndex] - 1].cooldown = bossAttacks[availableAttacks[attackIndex] - 1].startCooldown;
                bossAttacks[availableAttacks[attackIndex] - 1].duration = bossAttacks[availableAttacks[attackIndex] - 1].startDuration;
            }
        }

        for (int i = 0; i < bossAttackCount; i++)
        {
            if (bossAttacks[i].duration > 0)
            {
                switch (bossAttacks[i].type)
                {
                case BOSSATTACKTYPE_MANA_SPARK_BURST:
                {
                    if (bossAttacks[i].duration == bossAttacks[i].startDuration)
                    {
                        int manaSparkCount = 10;
                        puts("mana spark burst\n");
                        for (int j = manaSparkCount - 1; j > -1; j--)
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
                    if (bossAttacks[i].duration == bossAttacks[i].startDuration)
                    {
                        puts("moon beams");
                        for (int j = 0; j < moonBeamCount; j++)
                        {
                            moonBeamPositions[j] = (v2f){GetRandomValue(tileSize, (roomSize - 1) * tileSize), GetRandomValue(tileSize, (roomSize - 1) * tileSize)};
                        }
                    }
                    if (bossAttacks[i].duration > bossAttacks[i].startDuration / 2 &&
                        bossAttacks[i].duration - GetFrameTime() <= bossAttacks[i].startDuration / 2)
                    {
                        for (int j = 0; j < moonBeamCount; j++)
                        {
                            for (int k = 0; k < maxBossSpellEntities; k++)
                            {
                                if (bossSpellEntities[k].lifetime <= 0)
                                {
                                    bossSpellEntities[k] = (SpellEntity){spellBook[moonBeam].startingLifeTime, moonBeam, moonBeamPositions[k], (v2f){0, 0}, 0};
                                    break;
                                }
                            }
                        }
                    }
                    break;

                case BOSSATTACKTYPE_SUMMON_MINIONS:
                    if (bossAttacks[i].duration == bossAttacks[i].startDuration)
                    {
                        puts("summon minions");
                        int warriorCount = GetRandomValue(1, 3);
                        for (int j = 1; j < warriorCount+1; j++)
                        {
                            (*enemies)[roomPOS.x][roomPOS.y][j] = (Enemy){(Vector2){GetRandomValue(tileSize,(roomSize-1)*tileSize), bossPosition.y - 2 * tileSize}, (Vector2){1, 1}, 100, 70, 70, 0, {0}, AITYPE_WARRIOR, AISTATE_CHASE};
                        }
                        //minionCount += warriorCount;
                        int mageCount = GetRandomValue(-3,2);
                        mageCount = 0 + (mageCount * (mageCount > 0));
                        for (int j = 4; j < mageCount+4; j++)
                        {
                            (*enemies)[roomPOS.x][roomPOS.y][j] = (Enemy){(Vector2){GetRandomValue(tileSize,(roomSize-1)*tileSize), bossPosition.y - 2 * tileSize}, (Vector2){1, 1}, 70, 100, 100, 0, {0}, AITYPE_MAGE, AISTATE_CHASE};
                        }
                        //minionCount += mageCount;
                    }
                    break;
                default:
                    break;
                }
                bossAttacks[i].duration -= GetFrameTime();
                if (bossAttacks[i].duration < 0)
                {
                    bossAttacks[i].duration = 0;
                }
            }
        }
        for (int i = 0; i < bossAttackCount; i++)
        {
            if (bossAttacks[i].cooldown > 0)
            {
                bossAttacks[i].cooldown -= GetFrameTime();
            }
        }
        return false;
    }
}

void DrawBoss(float health)
{
    DrawCircle(bossPosition.x + 1, bossPosition.y + 1, enemyRadius + bossRadius, (Color){0, 0, 0, 255});
    DrawCircleV(bossPosition, enemyRadius + bossRadius, (Color){255, 100, 100, 255});
    DrawRectangle(bossPosition.x - 40, bossPosition.y + 5, 16, 2, (Color){0, 0, 0, 255});
    DrawRectangle(bossPosition.x + 24, bossPosition.y + 5, 16, 2, (Color){0, 0, 0, 255});
    DrawCircle(bossPosition.x, bossPosition.y + 30, 10, (Color){0, 0, 0, 255});

    // Draw health bar
    DrawRectangle(bossPosition.x - (enemyRadius + bossRadius), bossPosition.y - (enemyRadius + bossRadius + 20), (enemyRadius + bossRadius) * 2, 5, (Color){255, 0, 0, 255});
    DrawRectangle(bossPosition.x - (enemyRadius + bossRadius), bossPosition.y - (enemyRadius + bossRadius + 20), (enemyRadius + bossRadius) * 2 * (health / bossMaxHealth), 5, (Color){0, 255, 0, 255});


    if (bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].duration > bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].startDuration / 2)
    {
        for (int i = 0; i < moonBeamCount; i++)
        {
            DrawCircleV(Vector2Lerp(moonBeamPositions[i], bossPosition, (bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].duration - (bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].startDuration / 2)) / (bossAttacks[BOSSATTACKTYPE_MOON_BEAMS - 1].startDuration / 2)), 20, (Color){255, 255, 255, 100});
        }
    }

    DrawText(TextFormat("Mana Duration %f", bossAttacks[0].duration), 10, 70, 20, (Color){255, 255, 255, 255});
    DrawText(TextFormat("Mana Cooldown %f", bossAttacks[0].cooldown), 10, 90, 20, (Color){255, 255, 255, 255});
    DrawText(TextFormat("Moon Duration %f", bossAttacks[1].duration), 10, 110, 20, (Color){255, 255, 255, 255});
    DrawText(TextFormat("Moon Cooldown %f", bossAttacks[1].cooldown), 10, 130, 20, (Color){255, 255, 255, 255});
    DrawText(TextFormat("Minion Duration %f", bossAttacks[2].duration), 10, 150, 20, (Color){255, 255, 255, 255});
    DrawText(TextFormat("Minion Cooldown %f", bossAttacks[2].cooldown), 10, 170, 20, (Color){255, 255, 255, 255});
}