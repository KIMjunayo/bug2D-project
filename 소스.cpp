#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <atlimage.h>
//���� ���̵� ���״� ó���� ������ ������ ���� 
//�÷��̾ �Ѵµ� �̰� �Ȱ��� �ؾ����� ��� �Ѱ� �ؾ�����
//�ָ��ϳ�
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name"; 
LPCTSTR lpszWindowName = L"Windows Program 2"; 


float distance(float x1, float y1, float x2, float y2) {
    return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

typedef struct {
    float x, y;
    float speed;
} player;

typedef struct {
    float x, y;
    float speed;
    bool active; 
} enemy;

player play;
enemy e[10]; // 10����

void spawnenemy(enemy* e) {
    e->x = (float)(rand() % 750); 
    e->y = (float)-50; 
    e->speed = 2.5f;
    e->active = true;
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

   //(800x600 ũ��)
    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
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
    CImage lmage;
    static float posx, posy; 
    static HBITMAP hBackBitmap, hPlayer, hEnemy, hDoubleBuffer; // ���, �÷��̾�, ��, ���� ���۸� ��Ʈ��
    static int backWidth, backHeight, chaWidth, chaHeight, enemyWidth, enemyHeight; // �̹��� ũ��
    static RECT rectView; 
    static HBITMAP OldBit;
    static bool end = false; 

    switch (iMessage) {
    case WM_CREATE: {
        GetClientRect(hWnd, &rectView);
        hDC = GetDC(hWnd);

        // ��� �̹���
        hBackBitmap = (HBITMAP)LoadImage(g_hInst, TEXT("image2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (hBackBitmap == NULL) {
            MessageBox(hWnd, L"image2.bmp �ε� ����!", L"����", MB_OK | MB_ICONERROR);
            PostQuitMessage(0);
            return -1;
        }
   
        hPlayer = (HBITMAP)LoadImage(g_hInst, TEXT("player.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
     

        // �� �̹���
        hEnemy = (HBITMAP)LoadImage(g_hInst, TEXT("enemy.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
     

        GetObject(hBackBitmap, sizeof(BITMAP), &bmp);
        backWidth = bmp.bmWidth;
        backHeight = bmp.bmHeight;

        GetObject(hPlayer, sizeof(BITMAP), &bmp);
        chaWidth = bmp.bmWidth;
        chaHeight = bmp.bmHeight;

        GetObject(hEnemy, sizeof(BITMAP), &bmp);
        enemyWidth = bmp.bmWidth;
        enemyHeight = bmp.bmHeight;

        // �÷��̾� �ʱ�ȭ
        play.x = 400;
        play.y = 300; 
        play.speed = 10.0f;
        posx = play.x;
        posy = play.y;

        // �� 10���� �ʱ�ȭ
        for (int i = 0; i < 10; i++) {
            spawnenemy(&e[i]);
        }

        // ���� ���۸��� ��Ʈ��
        hDoubleBuffer = CreateCompatibleBitmap(hDC, rectView.right, rectView.bottom);
        SetTimer(hWnd, 1, 16, NULL); 
        ReleaseDC(hWnd, hDC);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (end==true) return 0;
        posx = (float)LOWORD(lParam);
        posy = (float)HIWORD(lParam);
        return 0;
    }

    case WM_TIMER: {
        if (end==true) return 0; 

        // �÷��̾� �̵�
        float dx = posx - play.x;
        float dy = posy - play.y;
        float dist = distance(play.x, play.y, posx, posy);
        if (dist > 5) {
            float m = play.speed / dist; 
            play.x += dx * m;
            play.y += dy * m;
        }

        // �� �̵� (�÷��̾� �ѱ�)
        for (int i = 0; i < 10; i++) {
            if (e[i].active) {
                float ex = play.x - e[i].x;
                float ey = play.y - e[i].y;
                float edist = distance(e[i].x, e[i].y, play.x, play.y);
                if (edist > 5) {
                    float m = e[i].speed / edist; 
                    e[i].x += ex * m;
                    e[i].y += ey * m;
                }
                if (e[i].active) {
                    
                }
                // �÷��̾�� �浹 ����
                float playerLeft = play.x - 25;
                float playerRight = play.x + 25;
                float playerTop = play.y - 25;
                float playerBottom = play.y + 25;
                float enemyLeft = e[i].x - 25;
                float enemyRight = e[i].x + 25;
                float enemyTop = e[i].y - 25;
                float enemyBottom = e[i].y + 25;

                if (playerLeft < enemyRight && playerRight > enemyLeft &&
                    playerTop < enemyBottom && playerBottom > enemyTop) {
                    if (!end) {
                        end = true;
                        MessageBox(hWnd, L"���� ����: �÷��̾ ���� �浹�߽��ϴ�!", L"���� ����", MB_OK | MB_ICONERROR);
                        PostQuitMessage(0); // ���� ����
                    }
                    return 0;
                }
            }
        }

        InvalidateRect(hWnd, NULL, FALSE); // ȭ�� ����
        return 0;
    }

    case WM_PAINT: {
        hDC = BeginPaint(hWnd, &ps);
        hMemDC = CreateCompatibleDC(hDC);
        OldBit = (HBITMAP)SelectObject(hMemDC, hDoubleBuffer);

        // ��� �׸���
        {
            HDC hTempDC = CreateCompatibleDC(hMemDC);
            HBITMAP hOldTemp = (HBITMAP)SelectObject(hTempDC, hBackBitmap);
            StretchBlt(hMemDC, 0, 0, rectView.right, rectView.bottom, hTempDC, 0, 0, backWidth, backHeight, SRCCOPY);
            SelectObject(hTempDC, hOldTemp);
            DeleteDC(hTempDC);
        }

        // �÷��̾� �׸���
        {
            HDC hTempDC = CreateCompatibleDC(hMemDC);
            HDC hMaskDC = CreateCompatibleDC(hMemDC);
            HBITMAP hOldTemp = (HBITMAP)SelectObject(hTempDC, hPlayer);
            HBITMAP hMask = CreateBitmap(chaWidth, chaHeight, 1, 1, NULL);
            HBITMAP hOldMask = (HBITMAP)SelectObject(hMaskDC, hMask);
            SetBkColor(hTempDC, RGB(0, 0, 0)); // �÷��̾� ���� (����ũ��)
            BitBlt(hMaskDC, 0, 0, chaWidth, chaHeight, hTempDC, 0, 0, SRCCOPY);
            StretchBlt(hMemDC, (int)(play.x - 25), (int)(play.y - 25), 50, 50, hMaskDC, 0, 0, chaWidth, chaHeight, SRCAND); 
            StretchBlt(hMemDC, (int)(play.x - 25), (int)(play.y - 25), 50, 50, hTempDC, 0, 0, chaWidth, chaHeight, SRCPAINT); 
            SelectObject(hMaskDC, hOldMask);
            SelectObject(hTempDC, hOldTemp);
            DeleteObject(hMask);
            DeleteDC(hMaskDC);
            DeleteDC(hTempDC);
        }

        // �� �׸���
        for (int i = 0; i < 10; i++) {
            if (e[i].active) {
                HDC hTempDC = CreateCompatibleDC(hMemDC);
                HDC hMaskDC = CreateCompatibleDC(hMemDC);
                HBITMAP hOldTemp = (HBITMAP)SelectObject(hTempDC, hEnemy);
                HBITMAP hMask = CreateBitmap(enemyWidth, enemyHeight, 1, 1, NULL);
                HBITMAP hOldMask = (HBITMAP)SelectObject(hMaskDC, hMask);
                SetBkColor(hTempDC, RGB(255, 255, 255)); // �� ���� (����ũ��)
                BitBlt(hMaskDC, 0, 0, enemyWidth, enemyHeight, hTempDC, 0, 0, SRCCOPY);
                StretchBlt(hMemDC, (int)(e[i].x - 25), (int)(e[i].y - 25), 50, 50, hMaskDC, 0, 0, enemyWidth, enemyHeight, SRCAND); 
                StretchBlt(hMemDC, (int)(e[i].x - 25), (int)(e[i].y - 25), 50, 50, hTempDC, 0, 0, enemyWidth, enemyHeight, SRCPAINT); 
                SelectObject(hMaskDC, hOldMask);
                SelectObject(hTempDC, hOldTemp);
                DeleteObject(hMask);
                DeleteDC(hMaskDC);
                DeleteDC(hTempDC);
            }
        }

        
        BitBlt(hDC, 0, 0, rectView.right, rectView.bottom, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hMemDC, OldBit);
        DeleteDC(hMemDC);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:

        DeleteObject(hBackBitmap);
        DeleteObject(hPlayer);
        DeleteObject(hEnemy);
        DeleteObject(hDoubleBuffer);
        KillTimer(hWnd, 1); 
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}