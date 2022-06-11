// server_AsyncSelectGUI.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include "framework.h"
#include "server.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#pragma warning(disable:4996)

#define MAX_LOADSTRING 100
#define WM_SOCKET WM_USER + 1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
SOCKET listener;
SOCKET clients[64];
int numClients = 0;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SERVER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 450, 410, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    WSAAsyncSelect(listener, hWnd, WM_SOCKET, FD_ACCEPT);


    // Tao listbox
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 10, 10,
        300, 300, hWnd, (HMENU)IDC_LIST_MESSAGES, GetModuleHandle(NULL),
        NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 320, 10,
        100, 300, hWnd, (HMENU)IDC_LIST_CLIENTS, GetModuleHandle(NULL),
        NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 310, 300, 30, hWnd,
        (HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("SEND"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP, 320, 310, 100, 30, hWnd,
        (HMENU)IDC_BUTTON_SEND, GetModuleHandle(NULL), NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SOCKET:
    {
        if (WSAGETSELECTERROR(lParam))
        {
            // co loi xay ra (true)
            //printf("Loi ket noi bi huy\n");
            char buf[256];
            sprintf(buf, "Loi ket noi: %d");
            SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGES, LB_ADDSTRING, 0,
                (LPARAM)buf);
            closesocket(wParam);
            return TRUE;
        }
        // khong loi xay ra
        if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
        {
            SOCKET client = accept(wParam, NULL, NULL);

            //printf("Co ket noi moi: %d\n", client);
            char buf[256];
            sprintf(buf, "Co ket noi moi: %d", client);
            SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGES, LB_ADDSTRING, 0,
                (LPARAM)buf);

            const char* msg = "Hello Client!\n";
            send(client, msg, strlen(msg), 0);

            WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

            sprintf(buf, "%d", client); //Chuyen luon chuoi sang so(hoac co the dung itoa(client, buf, 10)
            SendDlgItemMessageA(hWnd, IDC_LIST_CLIENTS, LB_ADDSTRING, 0,
                (LPARAM)buf);
            clients[numClients] = client; // moi khi socket ket noi thanh cong thi them vao trong mang
            numClients++;

        }
        //Kiem tra ma su kien
        if (WSAGETSELECTEVENT(lParam) == FD_READ)
        {
            char buf[256];
            int ret = recv(wParam, buf, sizeof(buf), 0);

            // xu ly du lieu
            buf[ret] = 0;
            //printf("Receive from %d: %s\n", wParam, buf);
            char sbuf[256];
            sprintf(sbuf, "Received from %d: %s", wParam, buf);
            SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGES, LB_ADDSTRING, 0,
                (LPARAM)sbuf);
        }
        if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
        {
            //printf("Ket noi bi dong: %dưn", wParam);
            char buf[256];
            sprintf(buf, "Ket noi bi dong", wParam);
            SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGES, LB_ADDSTRING, 0,
                (LPARAM)buf);
            closesocket(wParam);
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDC_BUTTON_SEND:
        {
            // lay ID(socket cua nguoi dunh)
            int i = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENTS, LB_GETCURSEL,
                0, 0); // lay chỉ số của dong duoc chon
            SOCKET client = clients[i]; // truy nhap vao mang client de biet duoc do la client nao

            //Xu ly khi nguoi dung nhan vao nut SEND
            // buffer chua thong diep nguoi dung da nhap
            char buf[256];
            GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, buf, sizeof(buf));
            send(client, buf, strlen(buf), 0);
            send(client, "\n", 1, 0); // chen them ky tu enter

            SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, "");
        }
        break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
