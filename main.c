#include <stdio.h>
#include "raylib.h"

bool* DrunkardsWalk (bool*, int height, int width, int staggering){
    int drunkardsXPOS = width/2;
    
    int DrunkardsYPOS = height/2;

    int i = 0;
    while(((drunkardsXPOS <= width && drunkardsXPOS >= width)&&(DrunkardsYPOS <= height && DrunkardsYPOS >= height))||staggering<=i){
        i++;
    }
}

int main(void)
{   
    //1120,1280 är *5 
    InitWindow(1120,1286 , "The Shoe Salesman"); //Ration ska vara 224, 256. Höjden behöver ökas med 8 för window rounding

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