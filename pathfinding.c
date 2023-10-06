#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

typedef struct Vector2 v2f;

typedef struct Paths
{
    v2f pathList[10];
}Paths;

/* Paths Pathfinding(v2f chaserPOS, v2f targetPOS){
    
} */