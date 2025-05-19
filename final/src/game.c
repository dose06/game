#include <SDL2/SDL.h>
#include <pthread.h>
#include <unistd.h> // sleep() 함수를 위해 추가
#include "game.h"
#include "audio.h"
#include "menu.h"


Block blocks[BLOCK_ROWS][BLOCK_COLS];
Ball ball = {{380, 530, 20, 20}, 0.0f, 0.0f}; // 공의 초기 위치를 바닥에 가깝게 변경
SDL_Rect goal = {700, 480, 50, 50}; // 지면에 위치한 골대
SDL_Rect sloperightBlock = {}; // 지면에 위치한 경사로
SDL_Rect slopeleftBlock = {}; // 지면에 위치한 경사로
SDL_Rect go_r_spring = {}; // 용수철 추가
SDL_Rect ladder = {}; // 사다리 추가
SDL_Rect leftLadder = {}; // 왼쪽 사다리 추가

static bool running = true;
static float ladderSpeed = 1.0f;
static int ladderDirection = 1;
static float leftLadderSpeed = 1.5f; // 왼쪽 사다리 속도
static int leftLadderDirection = 1;  // 왼쪽 사다리 이동 방향

bool onBlock = false; // 공이 블록 위에 있는지 여부
pthread_mutex_t ballMutex = PTHREAD_MUTEX_INITIALIZER;

// 이동 거리에 따른 마찰력을 계산하는 함수
float distanceFriction(float distance, float frictionCoefficient) {
    return -distance * frictionCoefficient;
}

// 백그라운드에서 마찰력을 계산하는 함수
void *calculateFriction(void* arg) {
    while (running) {
        // 이전 프레임의 공 위치 저장
        float previousBallX = ball.rect.x;
        float previousBallY = ball.rect.y;

        // 이동 거리 계산
        float distanceX = ball.rect.x - previousBallX;
        float distanceY = ball.rect.y - previousBallY;

        // 마찰력 계산 및 적용
        float frictionX = distanceFriction(distanceX, FRICTION_COEFFICIENT);
        float frictionY = distanceFriction(distanceY, FRICTION_COEFFICIENT);

        // 공 데이터에 접근하기 전에 뮤텍스 잠금
        pthread_mutex_lock(&ballMutex);
        ball.speedX += frictionX;
        ball.speedY += frictionY;

        // 최소 속도 설정
        if (fabs(ball.speedX) < 0.01f) {
            ball.speedX = 0;
        }
        if (fabs(ball.speedY) < 0.01f) {
            ball.speedY = 0;
        }

        // 현재 공 위치를 이전 위치로 업데이트
        previousBallX = ball.rect.x;
        previousBallY = ball.rect.y;

        // 뮤텍스 해제
        pthread_mutex_unlock(&ballMutex);

        // 잠시 대기 (CPU 사용량 조절)
        sleep(10000); // 10ms 대기
    }
    return NULL;
}
void initGame(int level) {
   
    // 모든 블록을 비활성화
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            blocks[i][j].active = false;
        }
    }
    ball.speedX = 0.0f;
    ball.speedY = 0.0f;

    sloperightBlock = (SDL_Rect){0, 0, 0, 0};       // 경사로 블럭 초기화 
    slopeleftBlock = (SDL_Rect){0, 0, 0, 0};        // 용수철 초기화 

    go_r_spring = (SDL_Rect){0, 0, 0, 0 };               
    leftLadder = (SDL_Rect){0,0,0,0};

    if (level == 1) {
        // 공중 블록 초기화
        blocks[0][0].rect.x = 350;
        blocks[0][0].rect.y = 200;
        blocks[0][0].rect.w = 100;
        blocks[0][0].rect.h = 20;
        blocks[0][0].active = true;
        // 바닥의 오른쪽 경사로추가 
        sloperightBlock.x = 350; // x 좌표: 650
        sloperightBlock.y = 480; // y 좌표: 280
        sloperightBlock.w = 50; // 너비: 100
        sloperightBlock.h = 50; // 높이: 20
        
        // 공 초기화 
        ball.rect.x = 380;
        ball.rect.y = 200 - ball.rect.h; // 블록 y 좌표 - 공의 높이
        ball.speedX = 0.0f;
        ball.speedY = 0.0f;

        ladder.x = goal.x - 60; // 골대의 왼쪽에 위치
        ladder.y = goal.y - 60; // 골대와 같은 높이에서 시작
        ladder.w = 50;          // 사다리 너비
        ladder.h = 60 * 3;      // 사다리 높이 (블록 3개 크기)

    }
    else if (level == 2) {
    // 공중 블록 6개 초기화 (가운데 4개 + 양쪽 끝 1개씩)
        for (int i = 0; i < 4; i++) { // 가운데 4개 블록
            blocks[0][i].rect.x = 250 + i * 100; // x 좌표: 250부터 100 간격으로
            blocks[0][i].rect.y = 300; // y 좌표: 300
            blocks[0][i].rect.w = 100; // 너비: 100
            blocks[0][i].rect.h = 20; // 높이: 20
            blocks[0][i].active = true;
        }
        blocks[0][4].rect.x = 150; // 왼쪽 끝 블록
        blocks[0][4].rect.y = 300;
        blocks[0][4].rect.w = 100;
        blocks[0][4].rect.h = 20;
        blocks[0][4].active = true;
        blocks[0][5].rect.x = 650; // 오른쪽 끝 블록
        blocks[0][5].rect.y = 300;
        blocks[0][5].rect.w = 100;
        blocks[0][5].rect.h = 20;
        blocks[0][5].active = true;

        // 가장 왼쪽 블록 위에 블록 추가
        blocks[1][0].rect.x = 150; // x 좌표: 150
        blocks[1][0].rect.y = 280; // y 좌표: 300 - 20
        blocks[1][0].rect.w = 100; // 너비: 100
        blocks[1][0].rect.h = 20; // 높이: 20
        blocks[1][0].active = true;


        // 가장 오른쪽 블록 위에 경사로 추가
        slopeleftBlock.x = 705; // x 좌표: 655
        slopeleftBlock.y = 255; // y 좌표: 280
        slopeleftBlock.w = 50; // 너비: 100
        slopeleftBlock.h = 50; // 높이: 20
       
        // 경사로 위에 띄워진 블록 추가
        blocks[1][5].rect.x = 700; // x 좌표: 700
        blocks[1][5].rect.y = 180; // y 좌표: 220
        blocks[1][5].rect.w = 100; // 너비: 100
        blocks[1][5].rect.h = 20; // 높이: 20
        blocks[1][5].active = true;
        // 지면 위에 경사로추가 
        sloperightBlock.x = 150; // x 좌표: 250
        sloperightBlock.y = 480; // y 좌표: 580
        sloperightBlock.w = 50; // 너비: 150
        sloperightBlock.h = 50; // 높이: 20
       
        // 공 초기화 (경사로 위 블록 위)
        ball.rect.x = 720; // x 좌표: 730
        ball.rect.y = 180 - ball.rect.h; // y 좌표: 220 - 공의 높이
        ball.speedX = 0.0f;
        ball.speedY = 0.0f;

        ladder.x = goal.x - 60; // 골대의 왼쪽에 위치
        ladder.y = goal.y - 60; // 골대와 같은 높이에서 시작
        ladder.w = 50;          // 사다리 너비
        ladder.h = 60 * 3;      // 사다리 높이 (블록 3개 크기)

    }
    else if (level == 3){

        // 공 초기화
        ball.rect.x = 130; // 공의 x 좌표
        ball.rect.y = 150 - ball.rect.h; // 공의 y 좌표
        ball.speedX = 0.0f;
        ball.speedY = 0.0f;

        // 블록 배열 초기화 (평평한 블록 + 경사로)
        for (int i = 0; i < 5; i++) {
            blocks[0][i].rect.x = 100 + i * 100; // x 좌표: 150부터 100 간격으로
            blocks[0][i].rect.y = 300; // y 좌표: 400 (지면에서 조금 떨어진 높이)
            blocks[0][i].rect.w = 100;  // 너비: 100
            blocks[0][i].rect.h = 20;  // 높이: 20
            blocks[0][i].active = true;
        }

        blocks[1][5].rect.x = 100; // x 좌표: 700
        blocks[1][5].rect.y = 150; // y 좌표: 220
        blocks[1][5].rect.w = 100; // 너비: 100
        blocks[1][5].rect.h = 20; // 높이: 20
        blocks[1][5].active = true;

        // 오른쪽 경사로 블록 추가
        sloperightBlock.x = 105; // x 좌표: 150
        sloperightBlock.y = 255; // y 좌표: 580
        sloperightBlock.w = 50; // 너비: 50
        sloperightBlock.h = 50;

        // 왼쪽 경사로 블록 추가
        slopeleftBlock.x = 550;  // x 좌표
        slopeleftBlock.y = 255;  // y 좌표
        slopeleftBlock.w = 50;   // 너비
        slopeleftBlock.h = 50; 

        // 용수철 초기화 (이미지의 오른쪽에 위치)
        go_r_spring = (SDL_Rect){20, 440, 100, 80}; // 용수철의 위치와 크기 설정

        ladder.x = goal.x - 60; // 골대의 왼쪽에 위치
        ladder.y = goal.y - 60; // 골대와 같은 높이에서 시작
        ladder.w = 50;          // 사다리 너비
        ladder.h = 60 * 3;      // 사다리 높이 (블록 3개 크기)

    }

    else if (level == 4){

        // 공 초기화
        ball.rect.x = 130; // 공의 x 좌표
        ball.rect.y = 150 - ball.rect.h; // 공의 y 좌표
        ball.speedX = 0.0f;
        ball.speedY = 0.0f;

        // 블록 배열 초기화 (평평한 블록 + 경사로)
        for (int i = 0; i < 5; i++) {
            blocks[0][i].rect.x = 100 + i * 100; // x 좌표: 150부터 100 간격으로
            blocks[0][i].rect.y = 300; // y 좌표: 400 (지면에서 조금 떨어진 높이)
            blocks[0][i].rect.w = 100;  // 너비: 100
            blocks[0][i].rect.h = 20;  // 높이: 20
            blocks[0][i].active = true;
        }

        blocks[1][5].rect.x = 100; // x 좌표: 700
        blocks[1][5].rect.y = 150; // y 좌표: 220
        blocks[1][5].rect.w = 100; // 너비: 100
        blocks[1][5].rect.h = 20; // 높이: 20
        blocks[1][5].active = true;

        // 오른쪽 경사로 블록 추가
        sloperightBlock.x = 105; // x 좌표: 150
        sloperightBlock.y = 255; // y 좌표: 580
        sloperightBlock.w = 50; // 너비: 50
        sloperightBlock.h = 50;

        // 왼쪽 경사로 블록 추가
        slopeleftBlock.x = 550;  // x 좌표
        slopeleftBlock.y = 255;  // y 좌표
        slopeleftBlock.w = 50;   // 너비
        slopeleftBlock.h = 50; 
        // 용수철 초기화 (이미지의 오른쪽에 위치)
        go_r_spring = (SDL_Rect){20, 470, 80, 60}; // 용수철의 위치와 크기 설정

        // 오른쪽 사다리 초기화
        ladder.x = goal.x - 60; // 골대의 왼쪽에 위치
        ladder.y = goal.y - 60; // 골대와 같은 높이에서 시작
        ladder.w = 50;          // 사다리 너비
        ladder.h = 60 * 3;      // 사다리 높이 (블록 3개 크기)

        // 왼쪽 사다리 초기화
        leftLadder.x = ladder.x -150;  // 기존 사다리의 왼쪽
        leftLadder.y = goal.y - 60;
        leftLadder.w = 50;
        leftLadder.h = 60 * 3;

    }
}




void updateGame() {
    
    ball.speedY += 0.25f; // 중력 가속   
    ball.rect.x += (int)ball.speedX; // 공의 x 좌표 업데이트
    ball.rect.y += (int)ball.speedY; // 공의 y 좌표 업데이트
    
    static float previousBallX = 0;  // 초기값을 0으로 설정
    // 첫 프레임인 경우 previousBallX를 현재 ball.rect.x로 초기화
    if (previousBallX == 0) {
        previousBallX = ball.rect.x;
    }
    printf("ball.speedX: %f\n", ball.speedX); // ball.speedX 값 출력
    // 블록 충돌 처리
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].active && SDL_HasIntersection(&ball.rect, &blocks[i][j].rect)) {
                float ballCenterX = ball.rect.x + ball.rect.w / 2.0f;
                float ballCenterY = ball.rect.y + ball.rect.h / 2.0f;

                // 공이 블록 위쪽에 부딪혔을 때
                if (ballCenterY < blocks[i][j].rect.y && ball.speedY > 0) {
                    ball.speedY = -ball.speedY * 0.7f;
                    ball.rect.y = blocks[i][j].rect.y - ball.rect.h;
                } 
                // 공이 블록 아래쪽에 부딪혔을 때
                else if (ballCenterY > blocks[i][j].rect.y + blocks[i][j].rect.h && ball.speedY < 0) {
                    ball.speedY = -ball.speedY * 0.7f;
                    ball.rect.y = blocks[i][j].rect.y + blocks[i][j].rect.h;
                } 
                // 공이 블록 왼쪽에 부딪혔을 때
                else if (ballCenterX < blocks[i][j].rect.x && ball.speedX > 0) {
                    ball.speedX = -ball.speedX * 0.8f;
                    ball.rect.x = blocks[i][j].rect.x - ball.rect.w;
                } 
                // 공이 블록 오른쪽에 부딪혔을 때
                else if (ballCenterX > blocks[i][j].rect.x + blocks[i][j].rect.w && ball.speedX < 0) {
                    ball.speedX = -ball.speedX * 0.8f;
                    ball.rect.x = blocks[i][j].rect.x + blocks[i][j].rect.w;
                }

                onBlock = true; // 공이 블록 위에 있음  
            
            }
        }
    }
   // 블록 위에서 마찰력 적용
        if (onBlock) {
            float frictionX = distanceFriction(ball.speedX, FRICTION_COEFFICIENT);
            ball.speedX += frictionX;
            if (fabs(ball.speedX) < 0.01f) {
                ball.speedX = 0;
            }
        }       
    // 지면 충돌 처리 (for 문 바깥으로 이동)
    if (ball.rect.y + ball.rect.h >= 530) { // 게임 화면 높이를 600으로 가정
        ball.rect.y = 530 - ball.rect.h; // 공의 아랫면이 바닥에 닿도록 y 좌표 조정
        ball.speedY = -ball.speedY * 0.3f; // y축 속도 반전 및 감소
        ball.speedX *= 0.999999f; // x축 속도 감소 (마찰력)
    }

    // 사다리 장애물 움직임
    ladder.y += ladderSpeed * ladderDirection;

    // 사다리 이동 범위 제한
    if (ladder.y == goal.y || ladder.y == goal.y - 400) {
        ladderDirection *= -1;
    }
    // 왼쪽 사다리 움직임
    leftLadder.y += leftLadderSpeed * leftLadderDirection;

    if (leftLadder.y == goal.y || leftLadder.y == goal.y - 400) {
        leftLadderDirection *= -1;
    }

    // 사다리와 공의 충돌 처리
    if (SDL_HasIntersection(&ball.rect, &ladder)) {
        ball.speedY = -ball.speedY * 0.7f;
        ball.speedX = -ball.speedX * 0.7f;
    }

    // 왼쪽 사다리와 공의 충돌 처리
    if (SDL_HasIntersection(&ball.rect, &leftLadder)) {
        ball.speedY = -ball.speedY * 0.7f;
        ball.speedX = -ball.speedX * 0.7f;
    }
// 오른쪽 경사로 충돌 처리
    if (SDL_HasIntersection(&ball.rect, &sloperightBlock)) {
        float ballCenterX = ball.rect.x + ball.rect.w / 2.0f;
        float ballCenterY = ball.rect.y + ball.rect.h / 2.0f;

        // 공이 경사로의 왼쪽 면에 닿았을 때
        if (ballCenterX < sloperightBlock.x && ball.speedX > 0) {
            ball.speedX = -ball.speedX * 0.8f;
            ball.rect.x = sloperightBlock.x - ball.rect.w;
        } 
        // 공이 경사로의 빗변에 닿았을 때
        else if (ballCenterY > sloperightBlock.y + sloperightBlock.h - (ballCenterX - sloperightBlock.x) && ball.speedY < 0) {
            float slope = -1.0f; // 빗변의 기울기
            float yIntercept = sloperightBlock.y + sloperightBlock.h - slope * sloperightBlock.x; // 빗변의 y절편
            
            // 빗변의 방정식을 사용하여 공의 위치 조정
            float desiredY = slope * ballCenterX + yIntercept - ball.rect.h / 2.0f;
            ball.rect.y = desiredY;

            if(ball.speedX<=0.01f && ball.speedX>=-0.01f){
            // 빗변에 닿았을 때의 속도 계산
            ball.speedX = -ball.speedY * 0.8f; // x축 속도 증가
            ball.speedY = -ball.speedY * 0.1f; // y축 속도 감소 (거의 0)
            }else if(ball.speedX<0){
            ball.speedX = - ball.speedX*0.8f;  
            }
            // 공의 x, y 좌표 조정 (경사로 면에 맞도록)
            ball.rect.x = sloperightBlock.x + (sloperightBlock.y + sloperightBlock.h - ball.rect.y) - ball.rect.w + 2; 
            ball.rect.y = sloperightBlock.y + sloperightBlock.h - (ballCenterX - sloperightBlock.x) - ball.rect.h + 12;
        }
    }
    // 왼쪽 경사로 충돌 처리
    if (SDL_HasIntersection(&ball.rect, &slopeleftBlock)) {
        float ballCenterX = ball.rect.x + ball.rect.w / 2.0f;
        float ballCenterY = ball.rect.y + ball.rect.h / 2.0f;

        // 공이 경사로의 오른쪽 면에 닿았을 때
        if (ballCenterX > slopeleftBlock.x + slopeleftBlock.w && ball.speedX > 0) {
            ball.speedX = -ball.speedX * 0.8f;
            ball.rect.x = slopeleftBlock.x + slopeleftBlock.w;
        } 
        // 공이 경사로의 빗변에 닿았을 때
        else if (ballCenterY > slopeleftBlock.y + slopeleftBlock.h - (slopeleftBlock.x + slopeleftBlock.w + ballCenterX) && ball.speedY < 0) {
            float slope = 1.0f; // 빗변의 기울기
            float yIntercept = slopeleftBlock.y + slopeleftBlock.h - slope * (slopeleftBlock.x + slopeleftBlock.w); // 빗변의 y절편

            // 빗변의 방정식을 사용하여 공의 위치 조정
            float desiredY = slope * ballCenterX + yIntercept - ball.rect.h / 2.0f;
            ball.rect.y = desiredY;

            // 빗변에 닿았을 때의 속도 계산
            if(ball.speedX<=0.01f && ball.speedX>=-0.01){
            ball.speedX = ball.speedY * 0.8f; // x축 속도 감소
            ball.speedY = -ball.speedY * 0.1f; // y축 속도 감소 (거의 0)
            }else if(ball.speedX>0){
            ball.speedX = - ball.speedX*0.8f;  
            }
             // 공의 x, y 좌표 조정
            ball.rect.x = slopeleftBlock.x + slopeleftBlock.w - (slopeleftBlock.y + slopeleftBlock.h - ball.rect.y) + 4;
            ball.rect.y = slopeleftBlock.y + slopeleftBlock.h - (slopeleftBlock.x + slopeleftBlock.w - ballCenterX) - ball.rect.h + 12;
        }
    }
    
    // 골대 충돌 처리
    if (SDL_HasIntersection(&ball.rect, &goal)) {
        SDL_Log("Goal!!!");
        setRunning(false); // 게임 종료       
        playGoalSound();
    }
    if (SDL_HasIntersection(&ball.rect, &go_r_spring)) {
        ball.speedX = 5.0f; // 공을 오른쪽으로 튕김
        ball.rect.y = go_r_spring.y - ball.rect.h; // 공을 용수철 위에 위치시킴
        playSpringSound(); // 용수철 소리 재생 (구현 필요)
    }

    
}

void processMouseClick(int x, int y) {
    SDL_Point point = {x, y};
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLS; j++) {
            if (blocks[i][j].active && SDL_PointInRect(&point, &blocks[i][j].rect)) {
                blocks[i][j].active = false;
                playBlockSound();
            }
        }
    }
        // 경사로 클릭 처리
    if (SDL_PointInRect(&point, &sloperightBlock)) {
        sloperightBlock.w = 0; // 경사로를 사라지게 함
        sloperightBlock.h = 0;
        playBlockSound(); // 또는 다른 소리 재생
    }
    if (SDL_PointInRect(&point, &slopeleftBlock)) {
        slopeleftBlock.w = 0; // 경사로를 사라지게 함
        slopeleftBlock.h = 0;
        playBlockSound(); // 또는 다른 소리 재생
    }
}


bool isRunning() {
    pthread_mutex_lock(&ballMutex);  // 뮤텍스 잠금
    bool result = running;           // running 값 읽기
    pthread_mutex_unlock(&ballMutex); // 뮤텍스 해제
    return result;
}

void setRunning(bool value) {
    running = value;
}









