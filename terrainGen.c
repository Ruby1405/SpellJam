#include <stdio.h>
#include "raylib.h"

typedef struct door{
    int xpos;
    int ypos;
    char direction;//N,E,S,W
}door;

typedef struct room{
    door[4];
}room;

room* RoomCreator(){
    room[20][20] roomGrid={0};
    return roomGrid;
}

//En funktion för random terrain generation med "the drunkards walk" algoritmen
bool* DrunkardsWalk (bool* mapIn, int height, int width, int staggering){
    int drunkardsXPOS = width/2;
    
    int drunkardsYPOS = height/2;

    bool drunkardOutOfBounds = false;
    bool map[height][height]=mapIn;

    int i = 0;
    while (staggering&& drunkardOutOfBounds)
    {
        i++;
        switch (expression)
        {
        case drunkardsXPOS<0:
            /* code */
            break;
        
        default:
            break;
        }
    }
    
}