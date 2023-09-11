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
bool* DrunkardsWalk (bool* mapIn, int height, int width, int staggering){
    int drunkardsXPOS = width/2;
    
    int drunkardsYPOS = height/2;

    bool drunkardOutOfBounds = false;
    bool map[height][height]=mapIn;

    int i = 0;
    while (staggering&& drunkardOutOfBounds)
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

int main(void)
{   
    //1120,1280 är *5 
    InitWindow(1120,1286 , "SpellJam"); //Ration ska vara 224, 256. Höjden behöver ökas med 6 för window rounding på mac

    Rectangle rec = {0,0,1120,1280};
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleLinesEx(rec, 5, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}