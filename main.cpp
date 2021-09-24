#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define WINDOW_TITLE "WinAPI - Display Font"
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void GetWindowSize(HWND hWnd, int& width, int& height);
void CalculateCellSize();
void DrawTable(HDC hdc, int cWidth, int cHeight, int wndWidth, int wndHeight);
void FillTable(HDC hdc, int cWidth, int cHeight, int colsCount, int rowsCount);
void ReadFile(std::string path);

char szClassName[] = "DisplayFontClass";

int windowWidth;
int windowHeight;

int cellWidth;
int cellHeight;

int cols;
int rows;

std::vector<std::string> table;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    std::cout << "Enter number of rows and columns" << "\n";

    std::cin >> rows;
    std::cin >> cols;

    //ReadFile("../text1.txt");
    ReadFile("../text2.txt");

    MSG msg; HWND hWnd; WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = szClassName;
    wc.hIconSm = wc.hIcon;

    if (!RegisterClassEx(&wc))
    {
        return EXIT_FAILURE;
    }

    if (hWnd = CreateWindow
                (
                        wc.lpszClassName, WINDOW_TITLE,
                        WS_OVERLAPPEDWINDOW, 0, 0,
                        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, nullptr, nullptr, wc.hInstance, nullptr
                );
            hWnd == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    GetWindowSize(hWnd, windowWidth, windowHeight);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    switch (uMsg)
    {
        case WM_PAINT:
        {
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawTable(hdc, cellWidth, cellHeight, windowWidth, windowHeight);
            FillTable(hdc, cellWidth, cellHeight, cols, rows);

            ReleaseDC(hWnd, hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_SIZE:
        {
            GetWindowSize(hWnd, windowWidth, windowHeight);
            CalculateCellSize();
            InvalidateRect(hWnd, nullptr, false);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(EXIT_SUCCESS);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

    }

    return 0;
}

void GetWindowSize(HWND hWnd, int& width, int& height)
{
    RECT rect;
    if(GetClientRect(hWnd, &rect))
    {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
}

void CalculateCellSize()
{
    cellWidth = windowWidth / cols;
    cellHeight = windowHeight / rows;
}

void DrawTable(HDC hdc, int cWidth, int cHeight, int wndWidth, int wndHeight)
{
    for (int i = 0; i <= cols; i++)
    {
        MoveToEx(hdc, cWidth * i, 0, nullptr);
        LineTo(hdc, cWidth * i, wndHeight);
    }

    for (int i = 0; i <= rows; i++)
    {
        MoveToEx(hdc, 0, cHeight * i, nullptr);
        LineTo(hdc, wndWidth, cHeight * i);
    }
}

void FillTable(HDC hdc, int cWidth, int cHeight, int colsCount, int rowsCount)
{
    int fontWidth = std::min(64 * (cWidth + cHeight) / 1024, cHeight / 2);
    int fontHeight = fontWidth / 2;

    HFONT font = CreateFont(fontWidth, fontHeight, 0, 0,
                            FW_DONTCARE, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, VARIABLE_PITCH, nullptr);

    SelectObject(hdc, font);

    for (int i = 0; i <= colsCount; i++)
    {
        for (int j = 0; j <= rowsCount; j++)
        {
            RECT cell;
            SetRect(&cell, cWidth * i, cHeight * j, cWidth * (i + 1), cHeight * (j + 1));
            std::string word = std::string(table[i + j * cols].begin(), table[i + j * cols].end());
            DrawText(hdc, word.c_str(), -1, &cell, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);
        }
    }

    DeleteObject(font);
}

void ReadFile(std::string path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "No such file" << "\n";
    }
    else
    {
        std::string word;

        int i = 0;
        table.reserve(1024);
        while (file >> word && i < rows * cols)
        {
            i++;
            table.push_back(word);
        }

        file.close();
    }
}