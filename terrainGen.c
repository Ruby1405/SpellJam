#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"

const int roomSize = 28;

typedef enum TileType
{
    TILE_TYPE_EMPTY,
    TILE_TYPE_BLOCKED,
    TILE_TYPE_DOOR_NORTH,
    TILE_TYPE_DOOR_EAST,
    TILE_TYPE_DOOR_SOUTH,
    TILE_TYPE_DOOR_WEST,
    SCHEDULED_FOR_DELETE
} TileType;

typedef enum Directions
{
    North,
    East,
    South,
    West
} directions;

typedef struct Room
{
    bool empty;
    TileType data[roomSize][roomSize];
} Room;

typedef struct Point
{
    int x;
    int y;
} Point;

// Sebastian kod
//  Point directions[] = {
//      (Point) {0, -1},
//      (Point) {1, 0},
//      (Point) {0, 1},
//      (Point) {-1, 0},
//  };
// Sebastian kod
//  Point getRandomDir() {
//      time_t t;
//      srand((unsigned) time(&t));
//      return directions[rand() % 4];
//  }

int getRandomDir()
{
    return rand() % 4;
}

// Sebastian kod
//  bool carvePath(Point current, Point direction) {

// }

// En funktion för random terrain generation med "the drunkards walk" algoritmen
Room DrunkardsWalk(bool north, bool east, bool south, bool west, int staggering, Point StartPOS)
{
    Room map;

    Point drunkardsPOS = StartPOS;

    bool drunkardOutOfBounds = false;

    // makes all the tiles in the map into walls
    for (int i = 0; i < roomSize; i++)
    {
        for (int j = 0; j < roomSize; j++)
        {
            map.data[i][j] = TILE_TYPE_BLOCKED;
        }
    }

    int i = 0;
    while (i <= staggering || !drunkardOutOfBounds)
    {
        printf("%d\n", i);
        // Sets the drunkard loose
        map.data[drunkardsPOS.x][drunkardsPOS.y] = TILE_TYPE_EMPTY;
        switch (getRandomDir())
        {
        case North: // North
            drunkardsPOS.y--;
            break;
        case East: // East
            drunkardsPOS.x++;
            break;
        case South: // South
            drunkardsPOS.y++;
            break;
        case West: // West
            drunkardsPOS.x--;
            break;
        }
        

        // checks if the drunkard is out of bounds

        if (drunkardsPOS.x < 0) // THE WEST HAS FALLEN
        {
            puts("The west has fallen");
            drunkardsPOS.x++;
            if (!west && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y] = TILE_TYPE_DOOR_WEST;
            }
            else{
                puts("smhing my hed");
            }
        }
        else if (drunkardsPOS.x >= roomSize) // East bound and down, loaded up and truckin'
        {
            puts("East bound and down, loaded up and truckin'");
            drunkardsPOS.x--;
            if (!east && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y] = TILE_TYPE_DOOR_EAST;
            }
            else{
                puts("We're gonna do what they say cant be done");
            }
        }
        else if (drunkardsPOS.y < 0) // King in the north
        {
            puts("King in the north");
            drunkardsPOS.y++;
            if (!north && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y] = TILE_TYPE_DOOR_NORTH;
            }
            else{
                puts("He ded");
            }
        }
        else if (drunkardsPOS.y >= roomSize) // Away down South in the land of traitors, rattlesnakes and alligators
        {
            puts("Away down South in the land of traitors, rattlesnakes and alligators");
            drunkardsPOS.y--;
            if (!south && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y] = TILE_TYPE_DOOR_SOUTH;
            }
            else if(south){
                puts("Come away");
            }
        }
        i++;
    }
    //Dunkard cleanup
    for(int i = 0; i< roomSize; i++){
        for(int j = 0; j<roomSize; j++){
            if(i == 0 || i == roomSize-1 || j == 0 || j == roomSize-1){}
            else{
                int borderWalls = 4 ;
                int leftRight =0;
                int upDown = 0 ;
                if(map.data[i][j]==TILE_TYPE_BLOCKED){
                    if(map.data[i-1][j]==TILE_TYPE_EMPTY || map.data[i-1][j]==SCHEDULED_FOR_DELETE ){
                        borderWalls--;
                        leftRight++;
                    };//Left
                    if(map.data[i+1][j]==TILE_TYPE_EMPTY || map.data[i+1][j]==SCHEDULED_FOR_DELETE){
                        borderWalls--;
                        leftRight++;
                    };//Right
                    if(map.data[i][j-1]==TILE_TYPE_EMPTY || map.data[i][j-1]==SCHEDULED_FOR_DELETE){
                        borderWalls--;
                        upDown++;
                    };//Up
                    if(map.data[i][j+1]==TILE_TYPE_EMPTY || map.data[i][j+1]==SCHEDULED_FOR_DELETE){
                        borderWalls--;
                        upDown++;
                    };//Down
                }
                if(borderWalls<2||leftRight==2||upDown==2)
                {
                    map.data[i][j]=SCHEDULED_FOR_DELETE;
                }
            }
        }
    }
    for(int i = 0; i<roomSize; i++){
        for(int j = 0; j < roomSize; j ++)
        {if(i == 0 || i == roomSize-1 || j == 0 || j == roomSize-1){}
                else
                {
                    int borderWalls = 4 ;
                    int leftRight =0;
                    int upDown = 0 ;
                    if(map.data[i][j]==TILE_TYPE_BLOCKED){
                        if(map.data[i-1][j]==TILE_TYPE_EMPTY || map.data[i-1][j]==SCHEDULED_FOR_DELETE ){
                            borderWalls--;
                            leftRight++;
                        };//Left
                        if(map.data[i+1][j]==TILE_TYPE_EMPTY || map.data[i+1][j]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            leftRight++;
                        };//Right
                        if(map.data[i][j-1]==TILE_TYPE_EMPTY || map.data[i][j-1]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            upDown++;
                        };//Up
                        if(map.data[i][j+1]==TILE_TYPE_EMPTY || map.data[i][j+1]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            upDown++;
                        };//Down
                    }
                    if(borderWalls<2||leftRight==2||upDown==2){
                        map.data[i][j]=SCHEDULED_FOR_DELETE;
                    }
                }
            if(map.data[i][j]==SCHEDULED_FOR_DELETE){
                
                map.data[i][j]=TILE_TYPE_EMPTY;
            }
        }
    }
    return map;
}


/*
TODO
- Implement a way to randomise rooms in certain ways such as
    - Treasure rooms 
    - Multiple doors in a room
    - Boss rooms if the drunkard is trapped
- Boss room 
*/
Room* RoomCreator(){
    Point startPOS;
    startPOS.x=112;
    startPOS.y=112;
    bool doorAvailability[4]={false};
    Room roomGrid[21][21]={0};
    for(int i= 0; i < 21; i++){
        for(int j = 0; j<21 ;j++){
            roomGrid[i][j].empty = true;
        }
    }
    for(int i = 0; i < 10; i++){
        int currentXPOS =0;
        int currentYPOS =0;
        
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
            doorAvailability[3]=true;
        }

        //224*224 är 50176
        DrunkardsWalk(doorAvailability[0],doorAvailability[1],doorAvailability[2],doorAvailability[3],33450,startPOS);

    }
    
    return roomGrid;
}

