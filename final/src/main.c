
#include "game.h"
#include <SDL2/SDL.h>
#include <pthread.h>
#include <unistd.h> // sleep() 함수를 위해 추가
#include "init.h"
#include "input.h"
#include "menu.h"
#include "render.h"
#include "audio.h"

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return -1;
    }

    loadSounds();
    loadTextures();          // 텍스처 로드
    loadMenuTextures();      // 메뉴 화면 텍스처 로드

    GameState state = MENU; // 초기 상태는 메뉴 화면
    int selectedLevel = 0;  // 선택된 단계 (1, 2, 3, 4)

    // 마찰력 계산 스레드 생성
    pthread_t frictionThread;
    pthread_create(&frictionThread, NULL, calculateFriction, NULL);

    while (isRunning()) {
        if (state == MENU) {
            pthread_cancel(frictionThread); // 메뉴 상태일 때 스레드 종료
            handleMenuInput(&state, &selectedLevel); // 메뉴 입력 처리
            renderMenu();                            // 메뉴 렌더링
        } else if (state == GAME) {
            if (selectedLevel == 1) {
                initGame(1); // 1단계 초기화
                while (isRunning() && state == GAME) {
                    handleInput();
                    updateGame();
                    renderGame();
                }
                setRunning(true);       // 메뉴로 돌아가기위해 초기화 
                state = MENU;
              
            } else if (selectedLevel == 2) {
               initGame(2); // 2단계 초기화
                while (isRunning() && state == GAME) {
                    handleInput();
                    updateGame();
                    renderGame();
                }
                state = MENU; 
                setRunning(true);
            } else if (selectedLevel == 3) {
                //SDL_Log("Level 3 is not implemented yet!");
                initGame(3); // 3단계 초기화
                while (isRunning() && state == GAME) {
                    handleInput();
                    updateGame();
                    renderGame();
                }
                setRunning(true); 
                state = MENU;
            } else if (selectedLevel == 4) {
                //SDL_Log("Level 4 is not implemented yet!");
                initGame(4); // 4단계 초기화
                while (isRunning() && state == GAME) {
                    handleInput();
                    updateGame();
                    renderGame();
                }
                setRunning(true); 
                state = MENU;
            }
        }
    }

    pthread_join(frictionThread, NULL);

    cleanupAudio();
    cleanupRender();        // 텍스처 해제
    cleanupMenuTextures();
    cleanup();

    return 0;
}











