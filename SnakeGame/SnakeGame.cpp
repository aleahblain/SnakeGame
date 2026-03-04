// SnakeGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color purple = { 216, 211, 233, 255 };
Color darkPurple = DARKPURPLE; // { 199, 193, 219, 255 }; 

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element))
            return true;
    }
    return false;
}

class Food {

public:

    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> chimataBody) {
        Image sushi = LoadImage("sushi.png");
        texture = LoadTextureFromImage(sushi);
        UnloadImage(sushi);
        position = generateRandomPos(chimataBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 generateRandomcell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y };
    }

    Vector2 generateRandomPos(deque<Vector2> chimataBody) {
        Vector2 position = generateRandomcell();
        while (elementInDeque(position, chimataBody)) {
            position = generateRandomcell();
        }
        return position;
    }
};

class Player {
public:

    Texture2D headTexture;
    Texture2D bodyTexture;
    deque<Vector2> body = { Vector2{ 6, 9 }, Vector2{ 5, 9 }, Vector2{ 4, 9 } };
    Vector2 direction = { 1, 0 };
    bool addPlate = false;

    Player() {
        Image chimata = LoadImage("chimata.png");
        headTexture = LoadTextureFromImage(chimata);
        UnloadImage(chimata);
        Image plate = LoadImage("plate.png");
        bodyTexture = LoadTextureFromImage(plate);
        UnloadImage(plate);
    }

    ~Player() {
        UnloadTexture(headTexture);
        UnloadTexture(bodyTexture);
    }

    void Draw() {
        
        for (unsigned int i = 0; i < body.size(); i++) {
            int x = body[i].x;
            int y = body[i].y;
            if (i > 0) {
                DrawTexture(bodyTexture, offset + x * cellSize, offset + y * cellSize + 30, WHITE);
            }
            else {
                DrawTexture(headTexture, offset + x * cellSize, offset + y * cellSize, WHITE);
            }
            
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addPlate == true) {
            addPlate = false;
        }
        else {
            body.pop_back();
        }
    }

    void reset() {
        body = { Vector2{ 6, 9 }, Vector2{ 5, 9 }, Vector2{ 4, 9 } };
        direction = { 1, 0 };
    }
};

class Game {
public:
    Player chimata = Player();
    Food food = Food(chimata.body);
    bool isRunning = true;
    int score = 0;
    
    void Draw() {
        food.Draw();
        chimata.Draw();
    }

    void Update() {
        if (isRunning) {
            chimata.Update();
            checkFoodCollision();
            checkEdgeCollision();
            checkPlateCollision();
        }
    }

    void checkFoodCollision() {
        if (Vector2Equals(chimata.body[0], food.position)) {
            food.position = food.generateRandomPos(chimata.body);
            chimata.addPlate = true;
            score++;
        }
    }

    void checkEdgeCollision() {
        if (chimata.body[0].x == cellCount || chimata.body[0].x == -1) {
            gameOver();
        }
        if (chimata.body[0].y == cellCount || chimata.body[0].y == -1) {
            gameOver();
        }
    }

    void checkPlateCollision() {
        deque<Vector2> noPlates = chimata.body;
        noPlates.pop_front();
        if (elementInDeque(chimata.body[0], noPlates)) {
            gameOver();
        }
    }

    void gameOver() {
        chimata.reset();
        food.position = food.generateRandomPos(chimata.body);
        isRunning = false;
        score = 0;
    }
};

int main()
{
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Chimata Sushi Game");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (eventTriggered(0.2)) {
            game.Update();
        }

        // Control player
        if (IsKeyPressed(KEY_LEFT) && game.chimata.direction.x != 1) {
            game.chimata.direction = { -1, 0 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.chimata.direction.x != -1) {
            game.chimata.direction = { 1, 0 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_UP) && game.chimata.direction.y != 1) {
            game.chimata.direction = { 0, -1 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.chimata.direction.y != -1) {
            game.chimata.direction = { 0, 1 };
            game.isRunning = true;
        }

        // Drawing BG
        ClearBackground(purple);
        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, darkPurple);
        DrawText("Feed Chimata!", offset - 5, 20, 40, darkPurple);
        DrawText(TextFormat("Sushi Eaten: %i", game.score), offset - 5, offset + cellSize * cellCount + 10, 30, darkPurple);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}