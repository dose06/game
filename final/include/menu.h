#ifndef MENU_H
#define MENU_H

typedef enum {
    MENU, // 메뉴 화면
    GAME  // 게임 중
} GameState;

void loadMenuTextures();
void renderMenu();
void cleanupMenuTextures();
void handleMenuInput(GameState* state, int* selectedLevel);

#endif

