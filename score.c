#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"


bool IsRoomCleared (Enemy enemies[32]){
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
int CalculateScore(int roomsCleared, int health, int maxHealth, float timeElapsed){
    int score = floor(1000*roomsCleared*(1+(health/maxHealth))/((timeElapsed+120)/60));
    return score;
}
int UpdateHighScore(int score){
    int previousScore = 0;
    FILE *fptr;
    fptr=fopen("highscore.txt","r");
    if(fptr == NULL){
        printf("does not exist");
    }
    else{
        fscanf(fptr,"%d", &previousScore);
        printf("read %d\n", previousScore);
        fclose(fptr);
    }
    if(score > previousScore){
        fptr=fopen("highscore.txt","w");
        fprintf(fptr, "%d",score);
        printf("%d > %d ",score,previousScore);
        fclose(fptr);
        return score;
    }
    else{
        return previousScore;
    }
}