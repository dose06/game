#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"
#include "game.h"

extern SDL_Renderer* renderer;

SDL_Texture* titleTexture = NULL;
SDL_Texture* button1Texture = NULL;
SDL_Texture* button2Texture = NULL;
SDL_Texture* button3Texture = NULL;
SDL_Texture* button4Texture = NULL;

SDL_Rect button1Rect = {300, 250, 200, 50};
SDL_Rect button2Rect = {300, 320, 200, 50};
SDL_Rect button3Rect = {300, 390, 200, 50};
SDL_Rect button4Rect = {300, 460, 200, 50};

void loadMenuTextures() {
    titleTexture = IMG_LoadTexture(renderer, "assets/title.png");
    button1Texture = IMG_LoadTexture(renderer, "assets/button1.png");
    button2Texture = IMG_LoadTexture(renderer, "assets/button2.png");
    button3Texture = IMG_LoadTexture(renderer, "assets/button3.png");
    button4Texture = IMG_LoadTexture(renderer, "assets/button4.png");
}

void renderMenu() {
    SDL_RenderClear(renderer);

    SDL_Rect titleRect = {150, 50, 500, 100};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_RenderCopy(renderer, button1Texture, NULL, &button1Rect);
    SDL_RenderCopy(renderer, button2Texture, NULL, &button2Rect);
    SDL_RenderCopy(renderer, button3Texture, NULL, &button3Rect);
    SDL_RenderCopy(renderer, button4Texture, NULL, &button4Rect);

    SDL_RenderPresent(renderer);
}

void handleMenuInput(GameState* state, int* selectedLevel) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            setRunning(false);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point point = {event.button.x, event.button.y};
            if (SDL_PointInRect(&point, &button1Rect)) {
                *selectedLevel = 1;
                *state = GAME;
            } else if (SDL_PointInRect(&point, &button2Rect)) {
                *selectedLevel = 2;
                *state = GAME;
            } else if (SDL_PointInRect(&point, &button3Rect)) {
                *selectedLevel = 3;
                *state = GAME;
            } else if (SDL_PointInRect(&point, &button4Rect)) {
                *selectedLevel = 4;
                *state = GAME;
            }
        }
    }
}

void cleanupMenuTextures() {
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(button1Texture);
    SDL_DestroyTexture(button2Texture);
    SDL_DestroyTexture(button3Texture);
    SDL_DestroyTexture(button4Texture);
}



