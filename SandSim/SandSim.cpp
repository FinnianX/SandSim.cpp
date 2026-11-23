#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "raylib.h"
#include <vector>
#include "raymath.h"
#include <cmath>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

using namespace std;
const int WIDTH = 1600;
const int HEIGHT = 1000;

Color getRandomColor() {
    return {
        static_cast<unsigned char>(GetRandomValue(0,255)),
        static_cast<unsigned char>(GetRandomValue(0,255)),
        static_cast<unsigned char>(GetRandomValue(0,255)),
        255
    };
}
float map(float inputValue, float inputMin, float inputMax, float outputMin, float outputMax) {
    return (((inputValue - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin)) + outputMin;
}
enum particleType { WATER, SAND, NOTHING};
class Sand {
public:
    int x;
    int y;
    Color color;
    particleType type;
    Sand(int x, int y, Color color, particleType type) {
        this->x = x;
        this->y = y;
        this->color = color;
        this->type = type;
    }
};

class Grid {
public:
    int width;
    int height;
    vector<vector<Sand>> sandParticles;
    Grid(int width, int height) {
        this->width = width;
        this->height = height;
    }
    void initGrid() {
        for (int row = 0; row < height; row++) {
            vector<Sand> sandRow;
            for (int col = 0; col < width; col++) {
                sandRow.push_back(Sand(col, row, ORANGE, NOTHING));
            }
            sandParticles.push_back(sandRow);
        }
    }
    void drawGrid(int gridsize) {
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                if (sandParticles[row][col].type != NOTHING) {
                    DrawRectangle(sandParticles[row][col].x * gridsize, sandParticles[row][col].y * gridsize, gridsize, gridsize, sandParticles[row][col].color);
                }
            }
        }
    }
    void update() {
        for (int row = height - 2; row >= 0; row--) {
            for (int col = 0; col < width; col++) {
                if (sandParticles[row][col].type != NOTHING ) {
                    bool rightFree = col < width - 1 && sandParticles[row + 1][col + 1].type == NOTHING ? true : false;
                    bool leftFree = col > 0 && sandParticles[row + 1][col - 1].type == NOTHING ? true : false;
                    bool preferLeft = GetRandomValue(0, 1) == 0;
                    //bool straightLeftFree = col > 0 && sandParticles[row][col - 1].type == NOTHING ? true : false;
                    //bool straightRightFree = col < width - 1 && sandParticles[row][col + 1].type == NOTHING ? true : false;
                    // MOVEMENT HANDLING FOR SAND
                    if (sandParticles[row][col].type == SAND) {
                        if (sandParticles[row + 1][col].type == NOTHING ) {
                            sandParticles[row+1][col] = sandParticles[row][col];
                            sandParticles[row + 1][col].y += 1;
                            sandParticles[row][col].type = NOTHING;
                        }
                        else if (leftFree && rightFree) {
                            int dir = (GetRandomValue(0, 1) == 0 ? -1 : 1);
                            sandParticles[row + 1][col + dir] = sandParticles[row][col];
                            sandParticles[row + 1][col + dir].x += dir;
                            sandParticles[row + 1][col + dir].y += 1;
                            sandParticles[row][col].type = NOTHING;
                        }

                        else if (leftFree&&preferLeft) {
                            sandParticles[row + 1][col - 1] = sandParticles[row][col];
                            sandParticles[row + 1][col - 1].x -= 1;
                            sandParticles[row + 1][col - 1].y += 1;
                            sandParticles[row][col].type = NOTHING;
                        }
                        else if (rightFree&&!preferLeft) {
                            sandParticles[row + 1][col + 1] = sandParticles[row][col];
                            sandParticles[row + 1][col + 1].x += 1;
                            sandParticles[row + 1][col + 1].y += 1;
                            sandParticles[row][col].type = NOTHING;
                        }
                    }
                }
            }
        }
    }
};
const int gridsize = 4;
int main()
{
    Grid test = Grid(400, 250);
    test.initGrid();
    InitWindow(WIDTH, HEIGHT, "SandSim");
    SetTargetFPS(60);
    float brushSize = 10;
    float ticker = 0.0;
    float saturation = 0.7;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        int gridx = GetMouseX() / gridsize;
        int gridy = GetMouseY() / gridsize;
        if (gridx > ((int)brushSize - 1) && gridx < test.width - ((int)brushSize - 1) && gridy >((int)brushSize - 1) && gridy < test.height - ((int)brushSize - 1) && !CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { 80,20,100,10 }) && !CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { 80,40,100,10 })) {
                for (int i = -(int)brushSize; i < (int)brushSize; i++) {
                    for (int k = -(int)brushSize; k < (int)brushSize; k++) {
                        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                                test.sandParticles[gridy + i][gridx + k].type = SAND;
                                test.sandParticles[gridy + i][gridx + k].color = ColorFromHSV(ticker, saturation, 1);
                        }
                        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                            test.sandParticles[gridy + i][gridx + k].type = NOTHING;
                        }
                    }
                }
        }
        test.drawGrid(gridsize);
        test.update();
        EndDrawing();
        ticker += 1;
        if (ticker >= 360) {
            ticker = 0.5;
        }
        GuiSlider({80,20,100,10}, "Small brush", "Big brush", &brushSize, 1.0, 25.0);
        GuiSlider({ 80,40,100,10 }, "-Saturation", "+Saturation", &saturation, 0.0, 1.0);
    }
    CloseWindow();
    return 0;
}