#include <stdlib.h>
#include <stdbool.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raymath.h"

#define CircleRotation(radius, angle) ((angle * 3) / (radius * 0.0002 + 0.01))

typedef struct Vector2 v2f;

typedef enum SpellName
{
    manaSpark,
    block,
    fireBall,
    moonBeam,
    chromaticOrb,
    magicMissile
} SpellName;

typedef struct SpellEntity
{
    float lifetime;
    SpellName name;
    Vector2 position;
    Vector2 aim;
    int targetIndex;
} SpellEntity;

typedef enum
{
    square,
    triangle,
    circle,
    execute
} Incantation;

typedef struct Spell
{
    SpellName name;
    float startingLifeTime;
    Incantation Incantation[16];
} Spell;

const int spellBookCount = 6;
const Spell spellBook[spellBookCount] = {
    (Spell){manaSpark, 1, {square, square, execute}},
    (Spell){block, 1, {circle, circle, execute}},
    (Spell){fireBall, 1, {square, circle, square, execute}},
    (Spell){moonBeam, 5, {square, square, circle, square, triangle, execute}},
    (Spell){chromaticOrb, 1, {square, triangle, circle, square, execute}},
    (Spell){magicMissile, 1, {square, triangle, triangle, square, execute}}
};

void DrawSpellMoonBeam(Vector2 targetLocation, float lifeTime)
{
    float a = 2;
    float b = 4.8;
    float c = 3.6;
    float curve = 80 - pow(a, b * (spellBook[moonBeam].startingLifeTime - lifeTime - c));
    int gitter = GetRandomValue(80 + 20 * lifeTime, 155 + 20 * lifeTime);
    for (int i = 40 + 8 * lifeTime; i > 20; i--)
    {
        DrawEllipse(targetLocation.x, targetLocation.y, i * 2, i, (Color){gitter, gitter, 255, 15});
    }
    DrawRectangle(targetLocation.x - curve / 2, targetLocation.y - 1000, curve, 1000, (Color){gitter, gitter, 255, 255});
    DrawRectangle(targetLocation.x - (curve / 2 + 1), targetLocation.y - 1000, curve + 2, 1000, (Color){gitter, gitter, 255, 100});
    DrawEllipse(targetLocation.x, targetLocation.y, curve / 2, curve / 4, (Color){gitter, gitter, 255, 255});
}

void DrawSpellManaSpark(Vector2 targetLocation, Vector2 aim)
{
    aim = Vector2Scale(Vector2Normalize(aim), 4);
    int r = 0;
    int b = 0;
    for (int i = 0; i < 9; i++)
    {
        r = GetRandomValue(200, 255);
        b = GetRandomValue(200, 255);
        DrawCircle(targetLocation.x + GetRandomValue(i - 10, 10 - i) / 2 - i * aim.x, targetLocation.y + GetRandomValue(i - 10, 10 - i) / 2 - i * aim.y, 10 - i + GetRandomValue(i - 10, 10 - i) / 3, (Color){r, 0, b, 200});
    }
    DrawCircle(targetLocation.x, targetLocation.y, 12, (Color){r, 0, b, 200});
}

void DrawSpellFireBall(Vector2 targetLocation, Vector2 aim)
{
    aim = Vector2Scale(aim, 4);
    int r = 0;
    for (int i = 0; i < 19; i++)
    {
        r = GetRandomValue(200, 255);
        DrawCircle(targetLocation.x + GetRandomValue(-20, 20) / 2 - i * aim.x, targetLocation.y + GetRandomValue(-20, 20) / 2 - i * aim.y, 20 - i + GetRandomValue(i - 20, 20 - i) / 5, (Color){r, 200 - 20 * i / 2, 0, 200});
    }
}

void DrawSpellBlock(Vector2 playerPosition)
{
    DrawCircle(playerPosition.x, playerPosition.y, 30, (Color){0, 200, 255, 100});
    DrawCircleLines(playerPosition.x, playerPosition.y, 31, (Color){255, 255, 255, 100});
    DrawCircleLines(playerPosition.x, playerPosition.y, 30, (Color){255, 255, 255, 100});
    DrawCircleLines(playerPosition.x, playerPosition.y, 29, (Color){255, 255, 255, 100});
}

void DrawSpellChromaticOrb(Vector2 targetLocation, Vector2 aim, float lifeTime)
{
    aim = Vector2Scale(aim, 4);
    int h = (int)(lifeTime * 100) % 36;
    v2f pos = {0, 8};
    pos = Vector2Rotate(pos, -10 * h * DEG2RAD);
    DrawCircle(targetLocation.x, targetLocation.y, 20, ColorAlpha(ColorFromHSV(h * 10, 1, 1), 0.5));
    for (int i = 0; i < 16; i++)
    {
        pos = Vector2Rotate(pos, 22.5 * DEG2RAD);
        DrawCircle(targetLocation.x + pos.x, targetLocation.y + pos.y, 8, ColorFromHSV(i * 22.5, 1, 1));
    }
}

void DrawSpellMagicMissile(Vector2 position, Vector2 aim, Vector2 targetLocation)
{
    //DrawLine(position.x, position.y, targetLocation.x, targetLocation.y, (Color){255, 255, 255, 255});
    DrawCircle(position.x, position.y, 5, (Color){255, 0, 255, 255});
    aim = Vector2Scale(Vector2Normalize(aim), 5);
    for (int i = 0; i < 10; i++)
    {
        DrawCircle(position.x - aim.x * i, position.y - aim.y * i, 5-(i/2), (Color){255, 0, 255, 255});
    }
}

void DrawMagicCircle(Vector2 playerPosition, Incantation magicCircle[16], int ringCount, float angle)
{
    float lineThickness = 3;
    int radius = 50;
    int squareCount = 0;
    int triangleCount = 0;
    Color ringColor = {
        magicCircle[0] == circle ? Clamp(0, 255, 255 - (43 * (ringCount - 2))) : 255,
        magicCircle[0] == triangle ? Clamp(0, 255, 43 * ringCount) : 0,
        magicCircle[0] == square ? Clamp(0, 255, 255 - (43 * (ringCount - 2))) : 255,
        255};
    for (int i = 0; i < ringCount; i++)
    {
        switch (magicCircle[i])
        {
        case square:
        {
            if (squareCount > 0)
            {
                DrawPolyLinesEx(playerPosition, 4, radius, -CircleRotation(radius, angle) + 45, lineThickness + 3, ringColor);
                squareCount = 0;
                continue;
            }
            if (i != 0)
            {
                radius = Vector2Length((Vector2){radius, radius});
            }
            DrawPolyLinesEx(
                playerPosition,
                4,
                radius,
                CircleRotation(radius, angle),
                lineThickness + 3,
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
                DrawPolyLinesEx(playerPosition, 3, radius, -CircleRotation(radius, angle) + 60, lineThickness + 5, ringColor);
                triangleCount = 0;
                continue;
            }
            if (radius < 100)
            {
                if (i != 0)
                {
                    radius = radius / sin(30 * DEG2RAD);
                }
                DrawPolyLinesEx(playerPosition, 3, radius, CircleRotation(radius, angle), lineThickness + 5, ringColor);
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
                        DrawPolyLinesEx(Vector2Add(playerPosition, Vector2Rotate((Vector2){0, radius}, (j * 30 - 15 - CircleRotation(radius, -angle)) * DEG2RAD)), 3, 10, j * -30 - 45 + CircleRotation(radius, -angle), lineThickness + 5, ringColor);
                    }
                    radius += 10;
                    triangleCount = 0;
                }
                else
                {
                    radius += 10;
                    for (int j = 0; j < 12; j++)
                    {
                        DrawPolyLinesEx(Vector2Add(playerPosition, Vector2Rotate((Vector2){0, radius}, (j * 30 - CircleRotation(radius, angle)) * DEG2RAD)), 3, 10, j * -30 + CircleRotation(radius, angle), lineThickness + 5, ringColor);
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
            DrawPolyLinesEx(
                (v2f){
                    playerPosition.x,
                    playerPosition.y},
                60, radius,
                1, lineThickness,
                ringColor);
            squareCount = 0;
            triangleCount = 0;
        }
        break;
        }
    }
}