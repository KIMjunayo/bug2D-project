#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <atlimage.h>
//필요한 수정사항 배경에 맞춰 해상도
//배경 음악 및 적 제거 소리
//보스전 ? 아 시발 힘들다
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Fly Bug";

float distance(float x1, float y1, float x2, float y2) {
    return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

typedef struct {
    float x, y;
    float speed;
    bool baby_mode;
} player;

typedef struct {
    float x, y;
    float speed;
    bool active;
    float dx, dy;
} enemy;

typedef struct {
    float x, y;
    float speed;
    int active;
    bool e; //먹었을 때 아이템 효과 나오게
    int e_time; //아이템 효과 시간
    time_t cool; //쿨타임
    float large_speed;
    float dx, dy;
}bee;

typedef struct {
    float x, y;
    float speed;
    int active;
    bool e; //
    time_t cool;
    float dx, dy;
}baby;

typedef struct {
    float x, y;
    float dx, dy;
    float speed;
    bool active;
} bullet;

typedef struct {
    float x, y;
    float speed;
    int active;
    bool e;
    time_t cool;
    float dx, dy;
    DWORD effect;
}guard;

typedef struct {
    float x, y;
    bool active;
    DWORD time;
}effect;

effect effects[50];
bullet bullets[10]; // 최대 10발
static time_t lastBulletTime; // 마지막 총알 발사 시간
player play;
enemy e[50]; // 10마리
time_t start_time = 0;
bee b;
baby by;
guard g;

void spawnenemy(enemy* e, player* p) {
    e->x = (float)(rand() % 750);
    e->y = (float)-50;
    e->speed = 2.5f;
    e->active = true;
    float dist = distance(e->x, e->y, p->x, p->y);
    if (dist > 0) {
        e->dx = (p->x - e->x) / dist;
        e->dy = (p->y - e->y) / dist;
    }
    else {
        e->dx = 0;
        e->dy = 1;
    }
}

void spawnbee(bee* b) {
    b->active = 1;
    b->x = (float)(rand() % 400);
    b->y = 50;
    b->e = false;
    b->speed = 3.0f;
    float angle = (float)(rand() % 360) * 3.141592f / 180.0f;
    b->dx = cosf(angle);
    b->dy = sinf(angle);
}

void spawnbaby(baby* by) {
    by->active = 1;
    by->x = (float)(rand() % 400);
    by->y = 50;
    by->e = false;
    by->speed = 3.0f;
    float angle = (float)(rand() % 360) * 3.141592f / 180.0f;
    by->dx = cosf(angle);
    by->dy = sinf(angle);
}

void spawnBullet(bullet* bul, player* p, float posx, float posy) {
    bul->active = true;
    bul->x = p->x; // 플레이어 위치에서 시작
    bul->y = p->y;
    bul->speed = 8.0f; // 총알 속도
    float dist = distance(p->x, p->y, posx, posy);
    if (dist > 0) {
        bul->dx = (posx - p->x) / dist;
        bul->dy = (posy - p->y) / dist;
    }
    else {
        bul->dx = 0;
        bul->dy = -1; // 기본적으로 위로
    }
}

void spawnguard(guard* g) {
    g->active = 1;
    g->x = (float)(rand() % 400);
    g->y = 50;
    g->e = false;
    g->speed = 3.0f;
    float angle = (float)(rand() % 360) * 3.141592f / 180.0f;
    g->dx = cosf(angle);
    g->dy = sinf(angle);
}

void spawneffect(effect* eff, float x, float y) {
    for (int i = 0;i < 50;i++) {
        if (!effects[i].active == true) {
            effects[i].x = x;
            effects[i].y = y;
            effects[i].active = true;
            effects[i].time = GetTickCount();
            break;
        }
    }
}

void boss(player*p) {
    for (int i = 0;i < 50;i++) {
        e[i].active = false;
    }
    for (int i = 0;i < 10;i++) {
        bullets[i].active = false;
    }
    for (int i = 0;i < 50;i++) {
        effects[i].active = false;
    }
    b.active = 0;
    b.e = false;
    b.cool = time(NULL);
    by.active = 0;
    by.e = false;
    by.cool = time(NULL);
    g.active = 0;
    g.e = false;
    g.cool = time(NULL);
    play.baby_mode = false;

    p->x = 400;
    p->y = 500;

}

void spawnboss(enemy*e,int index) {
    int zones[] = { 133,400,667 };
    e->x = (float)(zones[index]-50+(rand()%101));
    e->y = (float)-50;
    e->speed = 5.0f;
    e->active = true;
    e->dx = 0;
    e->dy = 1;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
    srand((unsigned int)time(NULL));
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;


    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    //(800x600 크기)
    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 600, 1000, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    HDC hDC, hMemDC;
    PAINTSTRUCT ps;
    BITMAP bmp;
    static float posx, posy;
    static HBITMAP hBackBitmap, hDoubleBuffer; // 배경, 플레이어, 적, 더블 버퍼링 비트맵
    static CImage player,enemy, bee, baby,large_bee,bullet,guard,play_guard,explosion,over;
    static int backWidth, backHeight, chaWidth, chaHeight, enemyWidth, enemyHeight; // 이미지 크기
    static RECT rectView;
    static HBITMAP OldBit;
    static bool end = false;
    static bool clear = false; //클리어 사진도 만들어야 됨
    static time_t start_time;
    static int killenemy;
    static bool bossmode = false;
    static DWORD spawn_time = 0;

	static int backgroundY = 0; // 배경 스크롤 위치

    switch (iMessage) {
    case WM_CREATE: {
        GetClientRect(hWnd, &rectView);
        hDC = GetDC(hWnd);
        enemy.Load(L"enemy.png");
        bee.Load(L"bee.png");
        baby.Load(L"baby.png");
        large_bee.Load(L"bigbee.png");
        bullet.Load(L"baby bullet.png");
        guard.Load(L"guard.png");
        play_guard.Load(L"play.guard.png");
        player.Load(L"player.png");
        explosion.Load(L"explosion.png");
        over.Load(L"over.png");
        start_time = time(NULL);
        lastBulletTime = GetTickCount();
        // 배경 이미지
        hBackBitmap = (HBITMAP)LoadImage(g_hInst, TEXT("trem background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

        GetObject(hBackBitmap, sizeof(BITMAP), &bmp);
        backWidth = bmp.bmWidth;
        backHeight = bmp.bmHeight;

        // 플레이어 초기화
        play.x = 400;
        play.y = 300;
        play.speed = 10.0f;
        posx = play.x;
        posy = play.y;
        play.baby_mode = false;
        // 적 50마리 초기화
        for (int i = 0; i < 50; i++) {
            spawnenemy(&e[i], &play);
        }
        //총알 10개 초기화
        for (int i = 0; i < 10; i++) {
            bullets[i].active = false;
        }
        //이펙트 초기화
        for (int i = 0; i < 50;i++) {
            effects[i].active = false;
        }
        //벌 아이템 
        spawnbee(&b);
        b.cool = 0;
        b.e = false;
        b.large_speed = 2.0f;

        //베이비 아이템
        spawnbaby(&by);
        by.cool = 0;
        by.e = false;
        play.baby_mode = false;

        //가드 아이템 
        spawnguard(&g);
        g.cool = 0;
        g.e = false;

        // 더블 버퍼링용 비트맵


        hDoubleBuffer = CreateCompatibleBitmap(hDC, rectView.right, rectView.bottom);
        bossmode = false;
        SetTimer(hWnd, 1, 16, NULL);
        ReleaseDC(hWnd, hDC);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (end == true) return 0;
        posx = (float)LOWORD(lParam);
        posy = (float)HIWORD(lParam);
        return 0;
    }

    case WM_TIMER: {
        if (end == true) return 0;

        backgroundY += 2; // 배경 스크롤 속도
        if(backgroundY > 10000000){
		backgroundY %= backHeight; // 배경 위치 초기화
	}

        // 플레이어 이동
        float dx = posx - play.x;
        float dy = posy - play.y;
        float dist = distance(play.x, play.y, posx, posy);
        if (dist > 5) {
            float m = play.speed / dist;
            play.x += dx * m;
            play.y += dy * m;
        }
        if (killenemy >= 100 && bossmode == false) {
            bossmode = true;
            boss(&play);
            spawn_time = GetTickCount();
        }

        if (bossmode && (GetTickCount() - spawn_time) >= 1000) {
            // 랜덤한 두 구역 선택
            int zones[3] = { 0, 1, 2 };
            for (int i = 0; i < 2; i++) { // 두 구역 선택
                int index = rand() % (3 - i);
                int zone = zones[index];
                for (int j = 0; j < 50; j++) {
                    if (e[j].active==false) {
                        spawnboss(&e[j], zone);
                        break;
                    }
                }
                // 선택된 구역 제거
                zones[index] = zones[2 - i];
            }
            spawn_time = GetTickCount();
        }
        // 적 이동 (플레이어 쫓기)
        for (int i = 0; i < 50; i++) {
            if (e[i].active) {
                e[i].x += e[i].dx * e[i].speed;
                e[i].y += e[i].dy * e[i].speed;
                if (e[i].y > 600) {
                    spawnenemy(&e[i], &play);
                }
            }
            else {
                e[i].y += e[i].speed;
                if (e[i].y > 600) {
                    e[i].active = false;
                }
            }
                // 플레이어와 충돌 감지
                float playerLeft = play.x - 20;
                float playerRight = play.x + 20;
                float playerTop = play.y - 20;
                float playerBottom = play.y + 20;
                float enemyLeft = e[i].x - 20;
                float enemyRight = e[i].x + 20;
                float enemyTop = e[i].y - 20;
                float enemyBottom = e[i].y + 20;

                if (g.e == true) {
                    float playerLeft = play.x - 35;
                    float playerRight = play.x + 35;
                    float playerTop = play.y - 35;
                    float playerBottom = play.y + 35;
                    float enemyLeft = e[i].x - 35;
                    float enemyRight = e[i].x + 35;
                    float enemyTop = e[i].y - 35;
                    float enemyBottom = e[i].y + 35;
                }
                if (g.e == true) {
                    if (playerLeft < enemyRight && playerRight > enemyLeft &&
                        playerTop < enemyBottom && playerBottom > enemyTop) {
                        e[i].active = false; // 적 비활성화
                        killenemy++; // 처치한 적 카운트 증가
                        spawneffect(effects, e[i].x, e[i].y);
                        spawnenemy(&e[i], &play); // 새로운 적 생성
                    }
                }
                if (playerLeft < enemyRight && playerRight > enemyLeft &&
                    playerTop < enemyBottom && playerBottom > enemyTop) {
                    if (!end && g.e == false) {
                        end = true;
                        MessageBox(hWnd, L"게임 오버: 플레이어가 적과 충돌했습니다!", L"게임 종료", MB_OK | MB_ICONERROR);
                        InvalidateRect(hWnd, NULL, true);
                       // PostQuitMessage(0); // 게임 종료
                        return 0;
                    }                 
                }
            
        }

        //벌 아이템
        if (b.active == 1){
            b.x += b.dx * b.speed;
            b.y += b.dy * b.speed;

            if (b.x < 25 || b.x > 600) { 
                b.dx = -b.dx; // x 방향 반사
                b.x = (b.x < 25) ? 25 : 775; // 경계 내로 위치 조정
            }
            if (b.y < 25 || b.y > 575) { 
                b.dy = -b.dy; // y 방향 반사
                b.y = (b.y < 25) ? 25 : 575; // 경계 내로 위치 조정
                if (b.y >= 1000) {
                    b.active = 0; // 화면 아래로 벗어나면 비활성화
                    b.cool = time(NULL); // 쿨타임 시작
                }
            }

            float playerLeft = play.x - 20;
            float playerRight = play.x + 20;
            float playerTop = play.y - 20;
            float playerBottom = play.y + 20;
            float beeLeft = b.x - 20;
            float beeRight = b.x + 20;
            float beeTop = b.y - 20;
            float beeBottom = b.y + 20;
            if (playerLeft < beeRight && playerRight > beeLeft &&
                playerTop < beeBottom && playerBottom > beeTop) {
                b.active = 0;
                b.e = true;
                b.cool = time(NULL);
            }
        }
        //10초 쿨다운
        if (b.active == 0 && difftime(time(NULL), b.cool) >= 10) {
            spawnbee(&b);
        }
        //화면 밖 넘어가면 다시 나오게 
        if (b.active == 1 && b.y > 1000) {
            b.active = 0;
            b.cool = time(NULL); // 비활성화 시간 기록
        }

        if (b.e == true) {
            b.y -= b.large_speed; // 위로 이동
            // 큰 벌과 적 충돌 감지
            float largeBeeLeft = b.x - 50;
            float largeBeeRight = b.x + 50;
            float largeBeeTop = b.y - 50;
            float largeBeeBottom = b.y + 50;

            for (int i = 0; i < 50; i++) {
                if (e[i].active) {
                    float enemyLeft = e[i].x - 20;
                    float enemyRight = e[i].x + 20;
                    float enemyTop = e[i].y - 20;
                    float enemyBottom = e[i].y + 20;

                    if (largeBeeLeft < enemyRight && largeBeeRight > enemyLeft &&
                        largeBeeTop < enemyBottom && largeBeeBottom > enemyTop) {
                        e[i].active = false; // 적 비활성화
                        killenemy++; // 처치한 적 카운트 증가
                        spawneffect(effects, e[i].x, e[i].y);
                        spawnenemy(&e[i], &play); // 새로운 적 생성
                    }
                }
            }
        }
        //baby 아이템 
        if (by.active == 1) {
            by.x += by.dx * by.speed;
            by.y += by.dy * by.speed;

            if (by.x < 25 || by.x > 775) {
                by.dx = -by.dx; // x 방향 반사
                by.x = (by.x < 25) ? 25 : 775; // 경계 내로 위치 조정
            }
            if (by.y < 25 || by.y > 575) {
                by.dy = -by.dy; // y 방향 반사
                by.y = (by.y < 25) ? 25 : 575; // 경계 내로 위치 조정
                if (by.y >= 575) {
                    by.active = 0; // 화면 아래로 벗어나면 비활성화
                    by.cool = time(NULL); // 쿨타임 시작
                }
            }

            float playerLeft = play.x - 20;
            float playerRight = play.x + 20;
            float playerTop = play.y - 20;
            float playerBottom = play.y + 20;
            float babyLeft = by.x - 20;
            float babyRight = by.x + 20;
            float babyTop = by.y - 20;
            float babyBottom = by.y + 20;
            if (playerLeft < babyRight && playerRight > babyLeft &&
                playerTop < babyBottom && playerBottom > babyTop) {
                by.active = 0;
                by.e = true;
                by.cool = time(NULL);
                play.baby_mode = true; // baby 모드 활성화
                for (int i = 0; i < 10; i++) {
                    bullets[i].active = false;
                }
                lastBulletTime = GetTickCount(); // 발사 시간 초기화
            }
        }

        // 5초 쿨다운
        if (by.active == 0 && difftime(time(NULL), by.cool) >= 5) {
            spawnbaby(&by);
        }

        // 화면 밖 넘어가면 다시 나오게
        if (by.active == 1 && by.y > 600) {
            by.active = 0;
            by.cool = time(NULL); // 비활성화 시간 기록
        }

        if (play.baby_mode) {
            if (difftime(time(NULL), lastBulletTime) >= 0.1) { // 0.1초마다 발사
                for (int i = 0; i < 10; i++) {
                    if (!bullets[i].active) {
                        spawnBullet(&bullets[i], &play, posx, posy);
                        lastBulletTime = time(NULL);
                        break;
                    }
                }
            }
            // 총알 이동 및 충돌
            for (int i = 0; i < 10; i++) {
                if (bullets[i].active) {
                    bullets[i].x += bullets[i].dx * bullets[i].speed;
                    bullets[i].y += bullets[i].dy * bullets[i].speed;
                    if (bullets[i].y < 0) { // 화면 밖
                        bullets[i].active = false;
                    }
                    // 총알과 적 충돌
                    float bulletLeft = bullets[i].x - 10; // 20x20 크기에 맞춤
                    float bulletRight = bullets[i].x + 10;
                    float bulletTop = bullets[i].y - 10;
                    float bulletBottom = bullets[i].y + 10;
                    for (int j = 0; j < 50; j++) {
                        if (e[j].active) {
                            float enemyLeft = e[j].x - 20;
                            float enemyRight = e[j].x + 20;
                            float enemyTop = e[j].y - 20;
                            float enemyBottom = e[j].y + 20;
                            if (bulletLeft < enemyRight && bulletRight > enemyLeft &&
                                bulletTop < enemyBottom && bulletBottom > enemyTop) {
                                e[j].active = false;
                                killenemy++;
                                spawneffect(effects, e[j].x, e[j].y);
                                spawnenemy(&e[j], &play);
                            }
                        }
                    }
                }
            }
            // baby_mode 지속 시간
            if (by.e == true && difftime(time(NULL), by.cool) >= 5) {
                by.e = false;
                play.baby_mode = false;
            }
        }
        //꽃잎 보호막 아이템 
        if (g.active == 1) {
            g.x += g.dx * g.speed;
            g.y += g.dy * g.speed;
            if (g.x < 25 || g.x > 775) {
                g.dx = -g.dx;
                g.x = (g.x < 25) ? 25 : 775;
            }
            if (g.y < 25 || g.y > 575) {
                g.dy = -g.dy;
                g.y = (g.y < 25) ? 25 : 575;
                if (g.y >= 575) {
                    g.active = 0;
                    g.cool = time(NULL);
                }
            }
            float playerLeft = play.x - 20;
            float playerRight = play.x + 20;
            float playerTop = play.y - 20;
            float playerBottom = play.y + 20;
            float guardLeft = g.x - 20;
            float guardRight = g.x + 20;
            float guardTop = g.y - 20;
            float guardBottom = g.y + 20;
            if (playerLeft < guardRight && playerRight > guardLeft &&
                playerTop < guardBottom && playerBottom > guardTop) {
                g.active = 0;
                g.e = true;
                g.cool = time(NULL);
                g.effect = GetTickCount();
            }
        }
        if (g.active == 0 && difftime(time(NULL), g.cool) >= 5) {
            spawnguard(&g);
        }//쿨타임 
        if (g.active == 1 && g.y > 600) {
            g.active = 0;
            g.cool = time(NULL);
        }
        for (int i = 0; i < 50; i++) {
            if (effects[i].active && (GetTickCount() - effects[i].time) >= 50) {
                effects[i].active = false; // 500ms 후 비활성화
            }
        }
        
        
        InvalidateRect(hWnd, NULL, FALSE); // 화면 갱신
        return 0;
    }

    case WM_PAINT: {
        hDC = BeginPaint(hWnd, &ps);
        hMemDC = CreateCompatibleDC(hDC);
        OldBit = (HBITMAP)SelectObject(hMemDC, hDoubleBuffer);

        // 배경 그리기
        {   
            if (end == false) {
            HDC hTempDC = CreateCompatibleDC(hMemDC);
            HBITMAP hOldTemp = (HBITMAP)SelectObject(hTempDC, hBackBitmap);
			// 배경 이미지 스크롤

			int y = backgroundY % backHeight; // 배경 위치 계산

            //첫번째 배경
            StretchBlt(hMemDC, 0, y, rectView.right, rectView.bottom, hTempDC, 0, 0, backWidth, backHeight, SRCCOPY);
			//두번째 배경
            StretchBlt(hMemDC, 0, y - rectView.bottom, rectView.right, rectView.bottom, hTempDC, 0, 0, backWidth, backHeight, SRCCOPY);

            SelectObject(hTempDC, hOldTemp);
            DeleteDC(hTempDC);
            }
            else if (end == true) {
            over.Draw(hMemDC, 0,0, 800,600);
            }
        }


        // 플레이어 그리기
        if (g.e == false)
            player.Draw(hMemDC, (int)(play.x - 25), (int)(play.y - 25), 50, 50);
        else if (g.e == true)
            play_guard.Draw(hMemDC, (int)(play.x - 25), (int)(play.y - 25), 80, 80);
        
        // 적 그리기
        if (end == false) {
            if (bossmode == false) {
                for (int i = 0; i < 50; i++) {
                    if (e[i].active) {
                        enemy.Draw(hMemDC, (int)(e[i].x - 25), (int)(e[i].y - 25), 50, 50);
                    }
                }
                if (b.active == 1) {
                    bee.Draw(hMemDC, (int)(b.x - 25), (int)(b.y - 25), 50, 50);
                }

                if (b.e == true) {
                    large_bee.Draw(hMemDC, (int)(b.x - 50), (int)(b.y - 50), 100, 100); // 큰 이미지 (100x100)
                }

                if (by.active == 1) {
                    baby.Draw(hMemDC, (int)(by.x - 25), (int)(by.y - 25), 50, 50);
                }
                for (int i = 0; i < 10; i++) {
                    if (bullets[i].active) {
                        bullet.Draw(hMemDC, (int)(bullets[i].x - 20), (int)(bullets[i].y - 20), 20, 20);
                    }
                }
                if (g.active == 1) {
                    guard.Draw(hMemDC, (int)(g.x - 25), (int)(g.y - 25), 50, 50);
                }
                for (int i = 0; i < 50; i++) {
                    if (effects[i].active) {
                        explosion.Draw(hMemDC, (int)(effects[i].x - 25), (int)(effects[i].y - 25), 50, 50);
                    }
                }
            }
        }
        if(bossmode==true) {
            for (int i = 0; i < 50; i++) {
                if (e[i].active) {
                    enemy.Draw(hMemDC, (int)(e[i].x - 25), (int)(e[i].y - 25), 50, 50);
                }
            }
        }
        
        //생존 시간 출력
        {
            TCHAR buffer[100];
            double survival_time = difftime(time(NULL), start_time);
            swprintf(buffer, 100, L"생존 시간: %.0f 초\n 처치한 적:%d", survival_time, killenemy);
            SetBkMode(hMemDC, TRANSPARENT); // 텍스트 배경 투명 설정
            SetTextColor(hMemDC, RGB(0, 0, 0)); // 텍스트 색상 (검정)
            TextOut(hMemDC, 10, 10, buffer, wcslen(buffer));
        
        if (play.baby_mode==true) {
            int bulletCount = 0;
            for (int i = 0; i < 10; i++) {
                if (!bullets[i].active) bulletCount++;
            }
           //베이비 총알 수  출력
            swprintf(buffer, 100, L"총알 수: %d", bulletCount);
            TextOut(hMemDC, (int)(play.x + 30), (int)(play.y - 40), buffer, wcslen(buffer));
        }

        if (g.e == true) {
            DWORD ms = GetTickCount() - g.effect;
            float retime = 5.0f - (ms / 1000.0f);
            if (retime < 0) retime = 0;
            swprintf(buffer, 100, L"보호막: %.2f초", retime);
            TextOut(hMemDC, (int)(play.x - 80), (int)(play.y - 40), buffer, wcslen(buffer));
        }
    }

        BitBlt(hDC, 0, 0, rectView.right, rectView.bottom, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hMemDC, OldBit);
        DeleteDC(hMemDC);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_CHAR:
        GetDC(hWnd);
        if (wParam == 'q') {
            PostQuitMessage(0);
        }
    case WM_DESTROY:
        DeleteObject(hBackBitmap);
        player.Destroy();
        enemy.Destroy();
        bee.Destroy();
        baby.Destroy();
        large_bee.Destroy();
        bullet.Destroy();
        guard.Destroy();
        play_guard.Destroy();
        explosion.Destroy();
        DeleteObject(hDoubleBuffer);
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
