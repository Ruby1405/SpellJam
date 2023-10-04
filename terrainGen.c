#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "/opt/homebrew/Cellar/raylib/4.5.0/include/raylib.h"

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

typedef struct Room
{
    bool empty;
    TileType data[roomSize][roomSize][2];
} Room;
typedef struct RoomGrid
{
    Room data[roomGridSize][roomGridSize];
} RoomGrid;

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

//counts the amount of booleans in an array which are true
int trueCount(bool boolArray[],int length){
    int sum;
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
            map.data[i][j][0] = TILE_TYPE_WALL;
            map.data[i][j][1] = ABYSS;
        }
    }

    int i = 0;
    while (i <= staggering || !drunkardOutOfBounds)
    {
        printf("%d\n", i);
        // Sets the drunkard loose
        map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_EMPTY;
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
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_WEST;
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
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_EAST;
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
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_NORTH;
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
                map.data[drunkardsPOS.x][drunkardsPOS.y][0] = TILE_TYPE_DOOR_SOUTH;
            }
            else if(south){
                puts("Come away");
            }
        }
        i++;
    }
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
    Point startPOS;
    startPOS.x=roomSize/2;
    startPOS.y=roomSize/2;
    
    bool treasureRoom=false;

    RoomGrid roomGrid;
    for(int i= 0; i < roomGridSize; i++){
        for(int j = 0; j<roomGridSize ;j++){
            roomGrid.data[j][i].empty = true;
        }
    }
    Point currentPOS;
    currentPOS.x=0;
    currentPOS.y=0;
    for(int i = 0; i < floor(roomGridSize/2); i++){
       bool doorAvailability[4]={true};
       bool failsafe = true;
        while(failsafe)
        {
            //Room occupancy checker
            if(roomGrid.data[currentPOS.x][currentPOS.y-1].empty==true){//North
                doorAvailability[North]=false;
            }
            if(roomGrid.data[currentPOS.x+1][currentPOS.y].empty==true){//East
                doorAvailability[East]=false;
            }
            if(roomGrid.data[currentPOS.x][currentPOS.y+1].empty==true){//South
                doorAvailability[South]=false;
            }
            if(roomGrid.data[currentPOS.x-1][currentPOS.y].empty==true){//West
                doorAvailability[West]=false;
            }
            switch (getRandomDir())
            {
            case North: // North
                if(doorAvailability[North]==false){
                    currentPOS.y--;
                    failsafe=false;
                    break;
                }
            case East: // East
                if(doorAvailability[East]==false){
                    currentPOS.x++;
                    failsafe=false;
                    break;
                }
            case South: // South
                if(doorAvailability[South==false]){
                    currentPOS.y++;
                    failsafe=false;
                    break;
                }
            case West: // West
                if(doorAvailability[West]==false){
                    currentPOS.x--;
                    failsafe=false;
                    break;
                }
            default:
                break;
            }
        }
        
        if(trueCount(doorAvailability,4)==4){
            treasureRoom=true;
        }
        //while true loop för gilltig rum värde, ingen double back
        if(treasureRoom){
            //Treasure room
            break;
        }
        //224*224 är 50176
        DrunkardsWalk(doorAvailability[North],doorAvailability[East],doorAvailability[South],doorAvailability[West],2500,startPOS);

    }
    //Boss room

    return roomGrid;
}

