#include <SDL2/SDL_mixer.h>
#include "audio.h"

static Mix_Chunk* soundBlock = NULL;
static Mix_Chunk* soundSpring = NULL;
static Mix_Chunk* soundGoal = NULL;

void loadSounds() {
    soundBlock = Mix_LoadWAV("assets/block.wav");
    soundSpring = Mix_LoadWAV("assets/spring.wav");
    soundGoal = Mix_LoadWAV("assets/goal.wav");

    if (!soundBlock || !soundSpring || !soundGoal) {
        SDL_Log("Failed to load sounds: %s", Mix_GetError());
    }
}

void playBlockSound() {
    Mix_PlayChannel(-1, soundBlock, 0);
}

void playSpringSound() {
    Mix_PlayChannel(-1, soundSpring, 0);
}

void playGoalSound() {
    Mix_PlayChannel(-1, soundGoal, 0);
}

void cleanupAudio() {
    Mix_FreeChunk(soundBlock);
    Mix_FreeChunk(soundSpring);
    Mix_FreeChunk(soundGoal);
}

