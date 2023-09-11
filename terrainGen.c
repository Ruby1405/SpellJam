#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include "raylib.h"
typedef enum direction{
    North = 1,
    East,
    South,
    West,//has fallen
}
typedef struct door{
    int xpos;
    int ypos;
    char direction;//N,E,S,W
}door;
typedef struct room{
    door[4];
    bool[224][224];
}room;
room* RoomCreator(){
    room[20][20] roomGrid={0};
    int currentXPOS;
    int currentYPOS;
    
    //Room occupancy checker
    if()
    
    return roomGrid;
}

//En funktion för random terrain generation med "the drunkards walk" algoritmen
bool* DrunkardsWalk (bool north, bool east, bool south, bool west, int staggering){
    int drunkardsXPOS = width/2;
    
    int drunkardsYPOS = height/2;

    bool drunkardOutOfBounds = false;
    bool map[height][height]=mapIn;

    int i = 0;
    while (staggering&& !drunkardOutOfBounds)
    {
        
        switch (expression)
        {
        case drunkardsXPOS<0:
            /* code */
            break;
        
        default:
            drunkardOutOfBounds = true;
            break;
        }
        i++;
    }
    
}
