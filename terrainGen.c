#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"

const int roomSize = 28;
const int roomGridSize = 21;

typedef enum TileType
{
    TILE_TYPE_EMPTY,
    TILE_TYPE_WALL,
    TILE_TYPE_WALL_NORTH,
    TILE_TYPE_WALL_EAST,
    TILE_TYPE_WALL_SOUTH,
    TILE_TYPE_WALL_WEST,
    TILE_TYPE_CORNER_NORTH_EAST,
    TILE_TYPE_CORNER_NORTH_WEST,
    TILE_TYPE_CORNER_SOUTH_EAST,
    TILE_TYPE_CORNER_SOUTH_WEST,
    TILE_TYPE_DOOR,
    TILE_TYPE_DOOR_NORTH,
    TILE_TYPE_DOOR_EAST,
    TILE_TYPE_DOOR_SOUTH,
    TILE_TYPE_DOOR_WEST,
    SCHEDULED_FOR_DELETE,
    ABYSS
} TileType;

typedef enum Directions
{
    North,
    East,
    South,
    West
} directions;
typedef struct Point
{
    int x;
    int y;
} Point;
typedef struct Room
{
    bool empty;
    TileType data[roomSize][roomSize][2]; 
    int exitDir;
    Point exitPOS;
} Room;
typedef struct RoomGrid
{
    Room data[roomGridSize][roomGridSize];
} RoomGrid;


int getRandomDir()
{
    return rand() % 4;
}

//counts the amount of booleans in an array which are true
int trueCount(bool boolArray[],int length){
    int sum = 0;
    for(int i = 0; i < length; i++){
        if(boolArray[i]==true){sum++;}    
    }
    return sum;
}
Room DrunkardsCleanup(Room map){
    bool blockChanged = true;
    while(blockChanged){
        blockChanged=false;
        for(int i = 0; i< roomSize; i++){
            for(int j = 0; j<roomSize; j++){
                int borderWalls = 4 ;
                int leftRight =0;
                int upDown = 0 ;
                if(map.data[j][i][0]==TILE_TYPE_WALL){
                    if(!(j == 0 || j == roomSize-1 )){//Not on the west or east sides
                        if(map.data[j-1][i][0]==TILE_TYPE_EMPTY || map.data[j-1][i][0]==SCHEDULED_FOR_DELETE ){
                            borderWalls--;
                            leftRight++;
                        };//Left
                        if(map.data[j+1][i][0]==TILE_TYPE_EMPTY || map.data[j+1][i][0]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            leftRight++;
                        };//Right
                    }
                    if(!(i == 0 || i == roomSize-1)){//Not on the north or south sides
                        if(map.data[j][i-1][0]==TILE_TYPE_EMPTY || map.data[j][i-1][0]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            upDown++;
                        };//Up
                        if(map.data[j][i+1][0]==TILE_TYPE_EMPTY || map.data[j][i+1][0]==SCHEDULED_FOR_DELETE){
                            borderWalls--;
                            upDown++;
                        };//Down
                    }
                    
                    if(borderWalls<2||leftRight==2||upDown==2)
                    {
                        map.data[j][i][0]=TILE_TYPE_EMPTY;
                        blockChanged=true;
                    }
                }
                
            }
        }
    }
    
    
    return map;
}
// En funktion för random terrain generation med "the drunkards walk" algoritmen
Room DrunkardsWalk(bool north, bool east, bool south, bool west, int staggering, int StartPOSx,int StartPOSy,int previousExitDir)
{
    Room map;
    Point drunkardsPOS;

    bool drunkardOutOfBounds = false;

    // makes all the tiles in the map into walls
    for (int i = 0; i < roomSize; i++)
    {
        for (int j = 0; j < roomSize; j++)
        {
            map.data[i][j][0] = TILE_TYPE_WALL;
            map.data[i][j][1] = ABYSS;
        }
    }
    switch (previousExitDir)
            {
            case North:
                StartPOSy=roomSize-1;
                map.data[StartPOSx][StartPOSy][0]=TILE_TYPE_DOOR_SOUTH;
                map.data[StartPOSx][StartPOSy][1] = TILE_TYPE_DOOR;
                StartPOSy--;
                break;
            case East:
                StartPOSx = 0;
                map.data[StartPOSx][StartPOSy][0]=TILE_TYPE_DOOR_WEST;
                map.data[StartPOSx][StartPOSy][1] = TILE_TYPE_DOOR;
                StartPOSx++;
                break;
            case South:
                StartPOSy=0;
                map.data[StartPOSx][StartPOSy][0]=TILE_TYPE_DOOR_NORTH;
                map.data[StartPOSx][StartPOSy][1] = TILE_TYPE_DOOR;
                StartPOSx--;
                break;
            case West:
                StartPOSx = roomSize-1;
                map.data[StartPOSx][StartPOSy][0]=TILE_TYPE_DOOR_EAST;
                map.data[StartPOSx][StartPOSy][1] = TILE_TYPE_DOOR;
                StartPOSy++;
                break;
            default:
                break;
            }
    drunkardsPOS.x = StartPOSx;
    drunkardsPOS.y = StartPOSy;
    int i = 0;
    while (!drunkardOutOfBounds)
    {
        // Sets the drunkard loose
        if(map.data[drunkardsPOS.x][drunkardsPOS.y][1]!= TILE_TYPE_DOOR){
            map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_EMPTY;
        }
        
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
            // puts("The west has fallen");
            drunkardsPOS.x++;
            if (!west && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_WEST;
                map.data[drunkardsPOS.x][drunkardsPOS.y][1] = TILE_TYPE_DOOR;
                map.exitDir=West;
                map.exitPOS.x=drunkardsPOS.x;
                map.exitPOS.y=drunkardsPOS.y;
            }
            else{
                // puts("smhing my hed");
            }
        }
        else if (drunkardsPOS.x >= roomSize) // East bound and down, loaded up and truckin'
        {
            // puts("East bound and down, loaded up and truckin'");
            drunkardsPOS.x--;
            if (!east && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_EAST;
                map.data[drunkardsPOS.x][drunkardsPOS.y][1] = TILE_TYPE_DOOR;
                map.exitDir=East;
                map.exitPOS.x=drunkardsPOS.x;
                map.exitPOS.y=drunkardsPOS.y;
            }
            else{
                // puts("We're gonna do what they say cant be done");
            }
        }
        else if (drunkardsPOS.y < 0) // King in the north
        {
            // puts("King in the north");
            drunkardsPOS.y++;
            if (!north && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_NORTH;
                map.data[drunkardsPOS.x][drunkardsPOS.y][1] = TILE_TYPE_DOOR;
                map.exitDir=North;
                map.exitPOS.x=drunkardsPOS.x;
                map.exitPOS.y=drunkardsPOS.y;
            }
            else{
                // puts("He ded");
            }
        }
        else if (drunkardsPOS.y >= roomSize) // Away down South in the land of traitors, rattlesnakes and alligators
        {
            // puts("Away down South in the land of traitors, rattlesnakes and alligators");
            drunkardsPOS.y--;
            if (!south && i >= staggering)
            {
                drunkardOutOfBounds = true;
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_SOUTH;
                map.data[drunkardsPOS.x][drunkardsPOS.y][1] = TILE_TYPE_DOOR;
                map.exitDir=South;
                map.exitPOS.x=drunkardsPOS.x;
                map.exitPOS.y=drunkardsPOS.y;
            }
            else if(south){
                // puts("Come away");
            }
        }
        i++;
    }
    printf("%d\n", i);
    //Dunkard cleanup
    map = DrunkardsCleanup(map);

    //Corners
    //God has forsaken me for this disgusting code
    //Forgive me father for i have sinned

    //The center
    for(int i = 1; i < roomSize-1; i++){
        for(int j = 1; j < roomSize-1; j++){
            if(map.data[j][i][0]==TILE_TYPE_WALL){
                int north = map.data[j][i-1][0];
                int east = map.data[j+1][i][0];
                int south = map.data[j][i+1][0];
                int west = map.data[j-1][i][0];
                int northWest = map.data[j-1][i][0] + map.data[j-1][i-1][0] + map.data[j][i-1][0];
                int northEast = map.data[j+1][i][0] + map.data[j+1][i-1][0] + map.data[j][i-1][0];
                int southWest = map.data[j-1][i][0] + map.data[j-1][i+1][0] + map.data[j][i+1][0];
                int southEast = map.data[j+1][i][0] + map.data[j+1][i+1][0] + map.data[j][i+1][0];
                
                //CORNERS
                if(northWest==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_CORNER_NORTH_WEST;
                }
                else if(northEast==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_CORNER_NORTH_EAST;
                }
                else if(southWest==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_CORNER_SOUTH_WEST;
                }
                else if(southEast==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_CORNER_SOUTH_EAST;
                }
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(east==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_WALL_EAST;
                }
                else if(south==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_WALL_SOUTH;
                }
                else if(west==TILE_TYPE_EMPTY){
                    map.data[j][i][1]=TILE_TYPE_WALL_WEST;
                }
            }
        }
    }

    //North except for north east corner
    for(int i = 0; i< roomSize-1; i++){
        if(map.data[i][0][0]==TILE_TYPE_WALL){
            if(i==0){
                int east = map.data[i+1][0][0];
                int south = map.data[i][1][0];
                int southEast = map.data[i+1][0][0] + map.data[i+1][1][0] + map.data[i][1][0];
                //CORNERS
                if(southEast==TILE_TYPE_EMPTY){
                    map.data[i][0][1]=TILE_TYPE_CORNER_SOUTH_EAST;
                }
                //WALLS
                else if(east!=TILE_TYPE_WALL){
                    map.data[i][0][1]=TILE_TYPE_WALL_EAST;
                }
                else if(south!=TILE_TYPE_WALL){
                    map.data[i][0][1]=TILE_TYPE_WALL_SOUTH;
                }
            }
            else{
                int east = map.data[i+1][0][0];
                int south = map.data[i][1][0];
                int west = map.data[i-1][0][0];
                int southWest = map.data[i-1][0][0] + map.data[i-1][1][0] + map.data[i][1][0];
                int southEast = map.data[i+1][0][0] + map.data[i+1][1][0] + map.data[i][1][0];
                //CORNERS
                if(southWest==TILE_TYPE_EMPTY){
                    map.data[i][0][1]=TILE_TYPE_CORNER_SOUTH_WEST;
                }
                else if(southEast==TILE_TYPE_EMPTY){
                    map.data[i][0][1]=TILE_TYPE_CORNER_SOUTH_EAST;
                }
                //WALLS
                else if(east!=TILE_TYPE_WALL){
                    map.data[i][0][1]=TILE_TYPE_WALL_EAST;
                }
                else if(south!=TILE_TYPE_WALL){
                    map.data[i][0][1]=TILE_TYPE_WALL_SOUTH;
                }
                else if(west!=TILE_TYPE_WALL){
                    map.data[i][0][1]=TILE_TYPE_WALL_WEST;
                }
            }
        }
        
    }
    //East except for south east corner
    for(int i = 0; i< roomSize-1; i++){
        if(map.data[roomSize-1][i][0]==TILE_TYPE_WALL){
            if(i==0){
                int south = map.data[roomSize-1][i+1][0];
                int west = map.data[roomSize-2][i][0];
                int southWest = map.data[roomSize-2][i][0] + map.data[roomSize-2][i+1][0] + map.data[roomSize-1][i+1][0];
                
                //CORNERS
                if(southWest==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_CORNER_SOUTH_WEST;
                }
                //WALLS
                else if(south==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_WALL_SOUTH;
                }
                else if(west==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_WALL_WEST;
                }
            }
            else{
                int north = map.data[roomSize-1][i-1][0];
                int south = map.data[roomSize-1][i+1][0];
                int west = map.data[roomSize-2][i][0];
                int northWest = map.data[roomSize-2][i][0] + map.data[roomSize-2][i-1][0] + map.data[roomSize-1][i-1][0];
                int southWest = map.data[roomSize-2][i][0] + map.data[roomSize-2][i+1][0] + map.data[roomSize-1][i+1][0];
                
                //CORNERS
                if(northWest==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_CORNER_NORTH_WEST;
                }
                else if(southWest==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_CORNER_SOUTH_WEST;
                }
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(south==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_WALL_SOUTH;
                }
                else if(west==TILE_TYPE_EMPTY){
                    map.data[roomSize-1][i][1]=TILE_TYPE_WALL_WEST;
                }
            }
        }
    }
    //South except for south west corner
    for(int i = 0; i<roomSize-1; i++){
        if(map.data[i][roomSize-1][0]==TILE_TYPE_WALL){
            if(i==0){
                int north = map.data[i][roomSize-2][0];
                int west = map.data[i-1][roomSize-1][0];
                int northWest = map.data[i-1][roomSize-1][0] + map.data[i-1][roomSize-2][0] + map.data[i][roomSize-2][0];
                
                //CORNERS
                if(northWest==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_CORNER_NORTH_WEST;
                }
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(west==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_WALL_WEST;
                }
            }
            else{
                int north = map.data[i][roomSize-2][0];
                int east = map.data[i+1][roomSize-1][0];
                int west = map.data[i-1][roomSize-1][0];
                int northWest = map.data[i-1][roomSize-1][0] + map.data[i-1][roomSize-2][0] + map.data[i][roomSize-2][0];
                int northEast = map.data[i+1][roomSize-1][0] + map.data[i+1][roomSize-2][0] + map.data[i][roomSize-2][0];
                
                //CORNERS
                if(northWest==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_CORNER_NORTH_WEST;
                }
                else if(northEast==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_CORNER_NORTH_EAST;
                }
                
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(east==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_WALL_EAST;
                }
                else if(west==TILE_TYPE_EMPTY){
                    map.data[i][roomSize-1][1]=TILE_TYPE_WALL_WEST;
                }
            }
        }
    }
    //West except for north west corner
    for(int i = roomSize; i>0; i--){
        if(map.data[0][i][0]==TILE_TYPE_WALL){
            if(i==0){
                int north = map.data[0][i-1][0];
                int east = map.data[1][i][0];
                int northEast = map.data[1][i][0] + map.data[1][i-1][0] + map.data[0][i-1][0];
                
                //CORNERS
                if(northEast==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_CORNER_NORTH_EAST;
                }
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(east==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_WALL_EAST;
                }
            }
            else{
                int north = map.data[0][i-1][0];
                int east = map.data[1][i][0];
                int south = map.data[0][i+1][0];
                int northEast = map.data[1][i][0] + map.data[1][i-1][0] + map.data[0][i-1][0];
                int southEast = map.data[1][i][0] + map.data[1][i+1][0] + map.data[0][i+1][0];
                
                //CORNERS
                if(northEast==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_CORNER_NORTH_EAST;
                }
                else if(southEast==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_CORNER_SOUTH_EAST;
                }
                //WALLS
                else if(north==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_WALL_NORTH;
                }
                else if(east==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_WALL_EAST;
                }
                else if(south==TILE_TYPE_EMPTY){
                    map.data[0][i][1]=TILE_TYPE_WALL_SOUTH;
                }
            }
        }
    }
    map.empty=false;
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
RoomGrid RoomCreator(){
    puts("room creator has been called");
    Point startPOS;
    startPOS.x=floor(roomSize/2);
    startPOS.y=floor(roomSize/2);
    
    bool treasureRoom=false;
    int previousDir = -1;

    RoomGrid roomGrid;
    for(int i= 0; i < roomGridSize; i++){
        for(int j = 0; j<roomGridSize ;j++){
            roomGrid.data[j][i].empty = true;
        }
    }
    Point currentPOS;
    currentPOS.x=floor(roomGridSize/2);
    currentPOS.y=floor(roomGridSize/2);
    bool doorAvailability[4]={false};
    for(int i = 0; i < floor(roomGridSize/2); i++){
       printf("room %d: ",i+1);
       
       printf("Drunkard is walking at %d ,%d\n",currentPOS.x,currentPOS.y);

        roomGrid.data[currentPOS.x][currentPOS.y] = DrunkardsWalk(doorAvailability[North],doorAvailability[East],doorAvailability[South],doorAvailability[West],2500,startPOS.x,startPOS.y,previousDir);
        startPOS.x = roomGrid.data[currentPOS.x][currentPOS.y].exitPOS.x;
        startPOS.y = roomGrid.data[currentPOS.x][currentPOS.y].exitPOS.y;
        for(int j = 0; j < 4; j++){
            doorAvailability[j]=false;
        }
        if(treasureRoom){
            //Treasure room
            
        }
        switch (roomGrid.data[currentPOS.x][currentPOS.y].exitDir)
        {
        case North:
            currentPOS.y++;
            previousDir = North;
            puts("Drunkard staggered North");
            break;
        case East:
            currentPOS.x++;
            previousDir = East;
            puts("Drunkard staggered East");
            break;
        case South:
            currentPOS.y--;
            previousDir = South;
            puts("Drunkard staggered South");
            break;
        case West:
            currentPOS.x--;
            previousDir = West;
            puts("Drunkard staggered West");
            break;
        default:
            break;
        }
        if(roomGrid.data[currentPOS.x][currentPOS.y+1].empty==false){//North
            doorAvailability[North]=true;
        }
        if(roomGrid.data[currentPOS.x+1][currentPOS.y].empty==false){//East
            doorAvailability[East]=true;
        }
        if(roomGrid.data[currentPOS.x][currentPOS.y-1].empty==false){//South
            doorAvailability[South]=true;
        }
        if(roomGrid.data[currentPOS.x-1][currentPOS.y].empty==false){//West
            doorAvailability[West]=true;
        }
        
    }
    //Boss room

    return roomGrid;
}

