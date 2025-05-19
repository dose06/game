#include <SDL2/SDL.h>
#include "game.h"

void handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            setRunning(false);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            processMouseClick(x, y);
        }
    }
}

