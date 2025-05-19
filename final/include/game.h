#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define BLOCK_ROWS 10
#define BLOCK_COLS 10
// 마찰 계수
#define FRICTION_COEFFICIENT 0.001f

// 공 데이터 접근 동기화를 위한 뮤텍스
pthread_mutex_t ballMutex; 

typedef struct {
    SDL_Rect rect;
    bool active;
} Block;


typedef struct {
    SDL_Rect rect;
    float speedX;
    float speedY;
} Ball;

extern Block blocks[BLOCK_ROWS][BLOCK_COLS];
extern Ball ball;
extern SDL_Rect goal;
extern SDL_Rect sloperightBlock;
extern SDL_Rect slopeleftBlock;
extern SDL_Rect go_r_spring;
extern SDL_Rect ladder;
extern SDL_Rect leftLadder;

void *calculateFriction(void* arg);
void initGame(int level);
void updateGame();
void processMouseClick(int x, int y);
bool isRunning();
void setRunning(bool running);

#endif









