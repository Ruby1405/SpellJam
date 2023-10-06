#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

//Ej klar implementation av pathfinding med A*

typedef struct Vector2 v2f;

typedef struct Path
{
    v2f position;
    v2f firstPosition;
    float pathDistance;
    bool wall;
    float pathWeight;
}Path;

typedef struct ValidPositions{
    Path paths[8];
}ValidPositions;

float GetWeight(Path path, v2f target){
    puts("get weight");
    return path.pathDistance + Vector2Distance(path.position , target);
}
ValidPositions ValidPositionFinder(Room map, Path positionToCheck, v2f target){
    puts("Valid position finder");
    printf("position to check x%f y%f \n",positionToCheck.position.x,positionToCheck.position.y);
    int roomPOSx =(int)((positionToCheck.position.x) / tileSize);
    int roomPOSy =(int)((positionToCheck.position.y) / tileSize);
    printf("x%d y%d\n",roomPOSx,roomPOSy);
    ValidPositions validPositions;
    int index = 0;
    for(int i = -1; i <= 1; i ++){
        for(int j = -1; j <= 1; j++){
            validPositions.paths[index].wall=true;
            if(i == 0 && j == 0){
                continue;
            }
            if(map.data[roomPOSx-j][roomPOSy-i][0]==TILE_TYPE_EMPTY){
                printf("%d\n",index);
                validPositions.paths[index].wall=false;
                float x = (float)(roomPOSx-j)*tileSize;
                float y = (float)(roomPOSy-i)*tileSize;
                v2f position = { x, y};
                validPositions.paths[index].position = position;
                printf("position is %f ,%f\n",position.x, position.y);
                if(positionToCheck.pathDistance == 0){
                    validPositions.paths[index].firstPosition=position;
                }
                validPositions.paths[index].pathDistance = positionToCheck.pathDistance+ sqrt(i*i + j*j)*tileSize;
                validPositions.paths[index].pathWeight = GetWeight(validPositions.paths[index], target);
            }
            else{
                puts("wall");
            }
            index++;
        }
    }
    puts("valid position found");
    return validPositions;
}
int sortBasedOnWeight(Path* path1, Path* path2){
    return (int)(path2->pathWeight - path1->pathWeight);
}
v2f PathFinder(Room map, v2f playerPosition, v2f enemyPosition){
    puts("path finder");
    int size = 0;
    Path queue [784] = { 0 };
    Path enemyPath = (Path){enemyPosition, {0,0} , 0, false, 0};
    enemyPath.pathWeight=GetWeight(enemyPath,playerPosition);
    queue[0]=enemyPath; size ++;
    while (true)
    {
        printf("size %d \n",size);
        Path positionToCheck = queue[size];
        size --;
        ValidPositions validPositions = ValidPositionFinder(map,positionToCheck,playerPosition);
        for(int i = 0; i <8; i++){
            if(validPositions.paths[i].wall){
                continue;
            }
            // Check if we hit player
            if((int)((validPositions.paths[i].position.x) / tileSize) == (int)(playerPosition.x / tileSize)&&(int)((validPositions.paths[i].position.y) / tileSize) == (int)(playerPosition.y / tileSize)){
                puts("path found");
                return validPositions.paths[i].firstPosition;
            }
            printf("add \n");
            // Otherwise: Add position to queue
            queue[size++] = validPositions.paths[i];
        }
        qsort(queue,size,sizeof(Path),sortBasedOnWeight);
    }
}
