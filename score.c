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
    
}

int calculateScore(int roomsCleared, int health, int maxHealth, int timeElapsed){
    int score = floor(roomsCleared*(1+(health/maxHealth))/(timeElapsed/60000));
    return score;
}