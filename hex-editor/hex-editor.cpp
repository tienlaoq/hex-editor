#include <windows.h>
#include <CommCtrl.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // Регистрация класса окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"HexEditorClass";
    RegisterClass(&wc);

    // Создание окна
    HWND hwnd = CreateWindowEx(
        0,                              // Опции окна
        L"HexEditorClass",              // Имя класса окна
        L"Простой Hex Editor",         // Заголовок окна
        WS_OVERLAPPEDWINDOW,            // Стиль окна
        CW_USEDEFAULT, CW_USEDEFAULT,   // Позиция окна (по умолчанию)
        800, 600,                       // Размер окна
        0, 0,                           // Родительское окно и меню (не используются)
        hInstance,                      // Дескриптор экземпляра приложения
        0);                             // Дополнительные параметры

    // Отображение окна
    ShowWindow(hwnd, nCmdShow);

    // Запуск цикла обработки сообщений
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Создание контролла для отображения текста
        HWND hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,              // Стиль рамки
            L"EDIT",                       // Класс окна
            L"",                           // Текст
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            0, 0, 0, 0,                     // Позиция и размер будут установлены позже
            hwnd,                          // Родительское окно
            (HMENU)1,                      // Идентификатор контролла
            GetModuleHandle(NULL),         // Дескриптор экземпляра приложения
            NULL);                         // Дополнительные параметры

        // Установка шрифта для контролла
        HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Courier New");
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Чтение содержимого файла и отображение в контролле
        const wchar_t* filename = L"example.txt";  // Укажите ваш файл
        HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD dwFileSize = GetFileSize(hFile, NULL);
            if (dwFileSize != INVALID_FILE_SIZE) {
                LPBYTE lpFileContent = (LPBYTE)GlobalAlloc(GPTR, dwFileSize);
                if (lpFileContent != NULL) {
                    DWORD dwBytesRead;
                    if (ReadFile(hFile, lpFileContent, dwFileSize, &dwBytesRead, NULL)) {
                        SetWindowText(hEdit, (LPCWSTR)lpFileContent);
                    }
                    GlobalFree(lpFileContent);
                }
            }
            CloseHandle(hFile);
        }

        // Установка размеров и позиции контролла
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}