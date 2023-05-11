#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define M_PI 3.14159265358979323846
#define effizienz 1
#define BALLS 10
int numBalls = BALLS;
typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    int voricscode = 1;
    COLORREF color;
} Ball;

void drawBall(HDC hdc, Ball ball) {
    HBRUSH hBrush = CreateSolidBrush(ball.color);
    SelectObject(hdc, hBrush);
    Ellipse(hdc, ball.x - 10, ball.y - 10, ball.x + 10, ball.y + 10);
    DeleteObject(hBrush);
}

void moveBall(Ball *ball) {
    ball->x += ball->dx ;
    ball->y += ball->dy ;
    
}

void bounceBall(Ball *ball, RECT rect) {
    if (ball->x < 10 || ball->x > rect.right - 10) {
        ball->dx = -ball->dx;
    }
    if (ball->y < 10 || ball->y > rect.bottom - 10) {
        ball->dy = -ball->dy;
    }
}

void collideBalls(Ball *balls, int *numBalls, RECT rect) {
    for (int i = 0; i < *numBalls; i++) {
        for (int j = i + 1; j < *numBalls; j++) {
            int dx = balls[i].x - balls[j].x;
            int dy = balls[i].y - balls[j].y;
            int distanceSquared = dx * dx + dy * dy;
            if (distanceSquared <= 400) {
                // Calculate the total velocity of the two balls
                float totalVelocity = sqrt(balls[i].dx * balls[i].dx + balls[i].dy * balls[i].dy) + sqrt(balls[j].dx * balls[j].dx + balls[j].dy * balls[j].dy);

                // Calculate the angle between the two balls
                float angle = atan2(dy, dx);
                // Calculate the velocity of ball 1 in the new direction
                float v1x = totalVelocity * cos(angle + atan2(balls[i].dy, balls[i].dx) - M_PI / 2);
                float v1y = totalVelocity * sin(angle + atan2(balls[i].dy, balls[i].dx) - M_PI / 2);

                // Calculate the velocity of ball 2 in the new direction
                float v2x = totalVelocity * cos(angle + atan2(balls[j].dy, balls[j].dx) - M_PI / 2);
                float v2y = totalVelocity * sin(angle + atan2(balls[j].dy, balls[j].dx) - M_PI / 2);

                // Update the velocities of the two balls
                balls[i].dx = v2x * effizienz;
                balls[i].dy = v2y * effizienz;
                balls[j].dx = v1x * effizienz;
                balls[j].dy = v1y * effizienz;

                // If there is room, add a new ball
                if (*numBalls < BALLS) {
                    balls[*numBalls] = (Ball){
                        rand() % rect.right,
                        rand() % rect.bottom,
                        rand() % 7 - 3,
                        rand() % 7 - 3,
                        RGB(rand() % 256, rand() % 256, rand() % 256)
                    };
                     (*numBalls)++;
                }
            }
        }
    }
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static Ball balls[BALLS];
    static RECT rect;

    switch (msg) {
        case WM_CREATE:
            srand(time(NULL) ^ GetCurrentProcessId());
            GetClientRect(hwnd, &rect);
            for (int i = 0; i < BALLS; i++) {
                balls[i] = (Ball){
                    rand() % rect.right,
                    rand() % rect.bottom,
                    rand() % 7 - 3,
                    rand() % 7 - 3,
                    RGB(rand() % 256, rand() % 256, rand() % 256)
                };
            }
            SetTimer(hwnd, 1, 10, NULL);
            break;

       case WM_TIMER:
            for (int i = 0; i < numBalls; i++) {
                moveBall(&balls[i]);
                bounceBall(&balls[i], rect);
            }
            collideBalls(balls, &numBalls, rect);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            for (int i = 0; i < BALLS; i++) {
                drawBall(hdc, balls[i]);
            }
            EndPaint(hwnd, &ps);
        } break;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "WindowClass";
    RegisterClass(&wc);

    HWND hwnd =
        CreateWindow("WindowClass", "Blaack Balls", WS_MAXIMIZE,
                     CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
                     NULL, NULL, hInstance, NULL);


    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);


    MSG msg;
    while (GetMessage(&msg,NULL ,0 ,0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
    
}
