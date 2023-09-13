#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"

typedef enum TileType {
    TILE_TYPE_EMPTY,
    TILE_TYPE_BLOCKED,
    TILE_TYPE_DOOR_NORTH,
    TILE_TYPE_DOOR_EAST,
    TILE_TYPE_DOOR_SOUTH,
    TILE_TYPE_DOOR_WEST
} TileType;

typedef enum Directions {
    North,
    East,
    South,
    West
}directions;


typedef struct Room {
    bool empty;
    TileType data[224][224];
}Room;

typedef struct Point {
    int x;
    int y;
} Point;

//Sebastian kod
// Point directions[] = {
//     (Point) {0, -1},
//     (Point) {1, 0},
//     (Point) {0, 1},
//     (Point) {-1, 0},
// };
//Sebastian kod
// Point getRandomDir() {
//     time_t t;
//     srand((unsigned) time(&t));
//     return directions[rand() % 4];
// }

int getRandomDir() {
    time_t t;
    srand((unsigned) time(&t));
    return rand() % 4;
}

//Sebastian kod
// bool carvePath(Point current, Point direction) {

// }

Room* RoomCreator(){
    bool doorAvailability[4]={false};
    Room roomGrid[21][21]={0};
    for(int i= 0; i < 21; i++){
        for(int j = 0; j<21 ;j++){
            roomGrid[i][j].empty = true;
        }
    }
    int currentXPOS;
    int currentYPOS;
    
    //Room occupancy checker
    if(roomGrid[currentXPOS-1][currentYPOS].empty==true){//North
        doorAvailability[0]=true;
    }
    if(roomGrid[currentXPOS][currentYPOS+1].empty==true){//East
        doorAvailability[1]=true;
    }
    if(roomGrid[currentXPOS+1][currentYPOS].empty==true){//South
        doorAvailability[2]=true;
    }
    if(roomGrid[currentXPOS][currentYPOS-1].empty==true){//West
        doorAvailability[0]=true;
    }

    return roomGrid;
}

//En funktion för random terrain generation med "the drunkards walk" algoritmen
Room DrunkardsWalk (bool north, bool east, bool south, bool west, int staggering, Point StartPOS){
    Room map;

    Point drunkardsPOS = StartPOS;


    bool drunkardOutOfBounds = false;

    //makes all the tiles in the map into walls
    for(int i=0; i <224; i++){
        for(int j=0; j < 224; j++){
            map.data[i][j]=TILE_TYPE_BLOCKED;
        }
    }

    int i = 0;
    while (i<=staggering || !drunkardOutOfBounds)
    {
        //Sets the drunkard loose
        switch (getRandomDir())
        {
        case North://North
            drunkardsPOS.y--;
            break;
        case East://East
            drunkardsPOS.x++;
            break;
        case South://South
            drunkardsPOS.y++;
            break;
        case West://West
            drunkardsPOS.x--;
            break;
        }
        map.data[drunkardsPOS.x][drunkardsPOS.y]=TILE_TYPE_EMPTY;

        //checks if the drunkard is out of bounds

        if(drunkardsPOS.x<0)//THE WEST HAS FALLEN
        {
            drunkardsPOS.x++;
            if(!west){
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y]=TILE_TYPE_DOOR_WEST;
            }
        } 
        else if(drunkardsPOS.x>224)//East bound and down, loaded up and truckin'
        {
            drunkardsPOS.x--;
            if(!east){
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y]=TILE_TYPE_DOOR_EAST;
            }
        }
        else if(drunkardsPOS.y<0)//King in the north
        {    
            drunkardsPOS.y++;
            if(!north){
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y]=TILE_TYPE_DOOR_NORTH;
            }
        }
        else if(drunkardsPOS.y>224)//Away down South in the land of traitors, rattlesnakes and alligators
        {
            drunkardsPOS.y--;
            if(!south){
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y]=TILE_TYPE_DOOR_SOUTH;
            }
        }
        i++;
    }
    return map;
}
