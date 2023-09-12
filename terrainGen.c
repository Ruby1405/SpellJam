#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include "raylib.h"

typedef enum TileType {
    TILE_TYPE_EMPTY,
    TILE_TYPE_BLOCKED,
    TILE_TYPE_DOOR_NORTH,
    TILE_TYPE_DOOR_EAST,
    TILE_TYPE_DOOR_SOUTH,
    TILE_TYPE_DOOR_WEST
} TileType;


typedef struct Room {
    TileType data[224][224];
}Room;

typedef struct Point {
    int x;
    int y;
} Point;


Vector2 directions[] = {
    (Point) {0, -1},
    (Point) {1, 0},
    (Point) {0, 1},
    (Point) {-1, 0},
}

Point getRandomDir() {
    srand((unsigned) time(&t));
    return directions[rand() % 4];
}

bool carvePath(Point current, Point direction) {

}

Room* RoomCreator(){
    bool doorAvailability[4]={false};
    Room roomGrid[21][21]={0};
    int currentXPOS;
    int currentYPOS;
    
    //Room occupancy checker
    if(roomGrid[currentXPOS-1][currentYPOS]==NULL){//North
        doorAvailability[0]=true
    }
    if(roomGrid[currentXPOS][currentYPOS+1]==NULL){//East
        doorAvailability[0]=true
    }
    if(roomGrid[currentXPOS+1][currentYPOS]==NULL){//South

    }
    if(roomGrid[currentXPOS][currentYPOS-1]==NULL){//West

    }

    return roomGrid;
}

//En funktion för random terrain generation med "the drunkards walk" algoritmen
bool* DrunkardsWalk (bool north, bool east, bool south, bool west, int staggering){
    int map[224][224] = {0};
    int drunkardsXPOS = width/2;
    
    int drunkardsYPOS = height/2;

    bool drunkardOutOfBounds = false;

    //makes all the tiles in the map into walls
    for(int i=0; i <224; i++){
        for(int j=0; j < 224; j++){
            map[i][j]=;
        }
    }
    
    

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
