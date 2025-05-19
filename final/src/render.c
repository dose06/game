#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "render.h"
#include "game.h"

extern SDL_Renderer* renderer;

SDL_Texture* backgroundTexture = NULL;
SDL_Texture* blockTexture = NULL;
SDL_Texture* ballTexture = NULL;
SDL_Texture* goalTexture = NULL;
SDL_Texture* sloperightTexture = NULL;
SDL_Texture* slopeleftTexture = NULL;
SDL_Texture* go_r_springTexture = NULL;
SDL_Texture* ladderTexture = NULL;
SDL_Texture* leftLadderTexture = NULL;

void loadTextures() {
    backgroundTexture = IMG_LoadTexture(renderer, "assets/background.png");
    blockTexture = IMG_LoadTexture(renderer, "assets/block.png");
    ballTexture = IMG_LoadTexture(renderer, "assets/ball.png");
    goalTexture = IMG_LoadTexture(renderer, "assets/goal.png");
    sloperightTexture = IMG_LoadTexture(renderer, "assets/sloperight.png");
    slopeleftTexture = IMG_LoadTexture(renderer, "assets/slopeleft.png");
    go_r_springTexture = IMG_LoadTexture(renderer, "assets/go_r_spring.png");
    ladderTexture = IMG_LoadTexture(renderer, "assets/ladder.png");
    leftLadderTexture = IMG_LoadTexture(renderer, "assets/leftLadder.png");

    if (!backgroundTexture || !blockTexture || !ballTexture || !goalTexture || !sloperightTexture|| !slopeleftTexture || !go_r_springTexture || !ladderTexture || !leftLadderTexture) {
        SDL_Log("Failed to load textures: %s", SDL_GetError());
    }
}

void renderGame() {
    SDL_RenderClear(renderer);

    // 배경 렌더링
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // 블록 렌더링
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].active) {
                SDL_Rect dst = blocks[i][j].rect;
                SDL_RenderCopy(renderer, blockTexture, NULL, &dst);
            }
        }
    }

    // 공 렌더링
    SDL_RenderCopy(renderer, ballTexture, NULL, &ball.rect);

    // 오른쪽 경사로 렌더링
    SDL_RenderCopy(renderer, sloperightTexture, NULL, &sloperightBlock);
    // 왼쪽 경사로 렌더링
    SDL_RenderCopy(renderer, slopeleftTexture, NULL, &slopeleftBlock);


    // 골대 렌더링
    SDL_RenderCopy(renderer, goalTexture, NULL, &goal);

    // 용수철 렌더링 
    SDL_RenderCopy(renderer, go_r_springTexture, NULL, &go_r_spring);

    // 사다리 렌더링
    SDL_RenderCopy(renderer, ladderTexture, NULL, &ladder);
    
    // 왼쪽 사다리 렌더링
    SDL_RenderCopy(renderer, leftLadderTexture, NULL, &leftLadder);

    SDL_RenderPresent(renderer);
}

void cleanupRender() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(blockTexture);
    SDL_DestroyTexture(ballTexture);
    SDL_DestroyTexture(goalTexture);
    SDL_DestroyTexture(sloperightTexture);
    SDL_DestroyTexture(slopeleftTexture);
    SDL_DestroyTexture(go_r_springTexture); 
    SDL_DestroyTexture(ladderTexture); 
    SDL_DestroyTexture(leftLadderTexture); 
}







   



