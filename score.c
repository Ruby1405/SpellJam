#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"


bool isRoomCleared (Enemy enemies[32]){
    bool roomIsCleared = true;
    for (int i = 0; i < 32; i++)
    {
        if (enemies[i].health > 0)
        {
            roomIsCleared=false;
        }
    }
    return roomIsCleared;
}
//https://www.desmos.com/calculator/oqzyr80y6h
int calculateScore(int roomsCleared, int health, int maxHealth, float timeElapsed){
    int score = floor(1000*roomsCleared*(1+(health/maxHealth))/((timeElapsed+120)/60));
    return score;
}