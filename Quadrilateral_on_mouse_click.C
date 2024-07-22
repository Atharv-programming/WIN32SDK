#include <Windows.h> 
#include <windowsx.h> 
#include <stdlib.h> 

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

typedef long long ssize_t; 

typedef struct tagVectorOfPoints{
    PPOINT pPoint; 
    ssize_t N; 
}POINTVECTOR, *PPOINTVECTOR;

PPOINTVECTOR CreateVector(VOID); 
VOID PushBack(PPOINTVECTOR pPointVec, LONG x, LONG y); 
VOID DestroyVector(PPOINTVECTOR pPointVec); 

HWND ghwnd = NULL; 

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    static TCHAR szClassName[] = TEXT("The standard window"); 
    static TCHAR szAppName[] = TEXT("Quadrilateral Demo"); 

    HWND hwnd = NULL; 

    WNDCLASSEX wnd; 
    MSG msg; 

    wnd.cbSize = sizeof(wnd);
    wnd.cbClsExtra = 0; 
    wnd.cbWndExtra = 0; 
    wnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wnd.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW); 
    wnd.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION); 
    wnd.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION); 
    wnd.hInstance = hInstance; 
    wnd.lpfnWndProc = WndProc; 
    wnd.lpszClassName = szClassName; 
    wnd.lpszMenuName = NULL; 
    wnd.style = CS_HREDRAW | CS_VREDRAW; 

    if(!RegisterClassEx(&wnd))
    {
        MessageBox(
            (HWND)NULL, 
            TEXT("Failed to regisgter a window class"), 
            TEXT("RegisterClassEx"), 
            MB_ICONERROR | MB_TOPMOST
        ); 
        ExitProcess(EXIT_FAILURE); 
    }

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW, 
        szClassName, 
        szAppName, 
        WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        (HWND)NULL, 
        (HMENU)NULL, 
        hInstance, 
        (LPVOID)NULL
    ); 

    if(hwnd == NULL)
    {
        MessageBox(
            (HWND)NULL, 
            TEXT("Failed to create an application window"), 
            TEXT("CreateWindowEx"), 
            MB_ICONERROR | MB_TOPMOST
        ); 
        ExitProcess(EXIT_FAILURE); 
    }

    ghwnd = hwnd; 

    ShowWindow(hwnd, nShowCmd); 
    UpdateWindow(hwnd); 

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    }

    return ((int)msg.wParam); 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{   
    #define NR_POINTS_IN_ARRAY      5 
    #define NR_POINTS_IN_RECTANGLE   4 
    
    static int cxScreen, cyScreen;
    static int iCnt;
    static PPOINTVECTOR pPointVector = NULL; 
    static BOOL bVar; 

    LONG x, y; 
    int iIndex; 

    HDC hdc = NULL; 

    PAINTSTRUCT ps; 

    switch(uMsg)
    {   
        case WM_CREATE: 
            pPointVector = CreateVector(); 
            break; 

        case WM_SIZE: 
            cxScreen = LOWORD(lParam); 
            cyScreen = HIWORD(lParam); 
            break; 

        case WM_PAINT: 
            hdc = BeginPaint(hwnd, &ps); 
            iIndex = 0; 
            while(iIndex < pPointVector->N)
            {
                MoveToEx(hdc, pPointVector->pPoint[iIndex].x, pPointVector->pPoint[iIndex].y, NULL);
                LineTo(hdc, pPointVector->pPoint[iIndex + 1].x, pPointVector->pPoint[iIndex + 1].y);
                LineTo(hdc, pPointVector->pPoint[iIndex + 2].x, pPointVector->pPoint[iIndex + 2].y);
                LineTo(hdc, pPointVector->pPoint[iIndex + 3].x, pPointVector->pPoint[iIndex + 3].y);
                LineTo(hdc, pPointVector->pPoint[iIndex + 4].x, pPointVector->pPoint[iIndex + 4].y);

                iIndex = iIndex + NR_POINTS_IN_ARRAY; 
            }
            EndPaint(hwnd, &ps); 

            break; 

        case WM_LBUTTONDOWN: 
            bVar = FALSE; 
            x = GET_X_LPARAM(lParam); 
            y = GET_Y_LPARAM(lParam); 
            PushBack(pPointVector, x, y); 
            if(iCnt == 3){
                bVar = TRUE;
                PushBack(
                    pPointVector, 
                    pPointVector->pPoint[pPointVector->N-4].x, 
                    pPointVector->pPoint[pPointVector->N-4].y
                );  
            }

            iCnt = (iCnt + 1) % (NR_POINTS_IN_RECTANGLE);  
            if(bVar)
                InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case WM_DESTROY: 
            DestroyVector(pPointVector); 
            pPointVector = NULL; 
            PostQuitMessage(0); 
            break; 
    }

    return (DefWindowProc(hwnd, uMsg, wParam, lParam)); 
}

PPOINTVECTOR CreateVector(VOID)
{
    PPOINTVECTOR pPointVector = NULL; 

    pPointVector = (PPOINTVECTOR)malloc(sizeof(POINTVECTOR)); 
    if(pPointVector == NULL)
        DestroyWindow(ghwnd); 

    pPointVector->pPoint = NULL; 
    pPointVector->N = 0; 

    return (pPointVector); 
}

VOID PushBack(PPOINTVECTOR pPointVec, LONG x, LONG y)
{
    pPointVec->pPoint = (PPOINT)realloc(pPointVec->pPoint, 
                                    (pPointVec->N+1) * sizeof(POINT)
                                ); 
    if(pPointVec->pPoint == NULL)
        DestroyWindow(ghwnd);

    pPointVec->N += 1; 
    pPointVec->pPoint[pPointVec->N - 1].x = x; 
    pPointVec->pPoint[pPointVec->N - 1].y = y; 
}

VOID DestroyVector(PPOINTVECTOR pPointVec)
{
    free(pPointVec->pPoint); 
    free(pPointVec); 
}