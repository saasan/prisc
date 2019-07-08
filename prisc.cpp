#ifndef UNICODE
#define UNICODE
#endif

#include <string>
#include <sstream>
#include <windows.h>

// 指定された位置が含まれるモニター情報を取得
BOOL GetMonitorInfoFromPoint(POINT pt, LPMONITORINFO lpmi)
{
    HMONITOR hMonitor = NULL;

    // ウィンドウがあるモニターのハンドルを取得
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);

    if (hMonitor == NULL)
    {
        return FALSE;
    }

    //モニター情報を取得する
    return GetMonitorInfoW(hMonitor, lpmi);
}

// タイトルバーを取り除く
BOOL RemoveTitleBar(HWND hWnd)
{
    // ウィンドウのスタイルを取得
    LONG style = GetWindowLong(hWnd, GWL_STYLE);

    // 取得に失敗した場合は終了
    if (style == 0)
    {
        return FALSE;
    }

    // WS_CAPTIONなどを取り除く
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX);

    // ウィンドウのスタイルを設定
    if (SetWindowLong(hWnd, GWL_STYLE, style) == 0)
    {
        return FALSE;
    }

    return TRUE;
}

// 引数の解像度でフルスクリーン化が可能かを返す
BOOL IsFullscreenable(int width, int height) {
    BOOL isFullHd = (width == 1920 && height == 1080);
    BOOL isHd = (width == 1280 && height == 720);

    // 解像度がフルHDまたはHDの場合のみフルスクリーン化可能
    return (isFullHd || isHd);
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    // アプリ名
    const wchar_t *title = L"プリスク";

    // プリコネのウィンドウを探す
    HWND hWndPriconne = FindWindowW(L"UnityWndClass", L"PrincessConnectReDive");

    // 見つからない場合は終了
    if (hWndPriconne == NULL)
    {
        MessageBoxW(NULL, L"プリコネのウィンドウが見つかりません。", title, MB_ICONERROR);
        return 0;
    }

    // プリコネのウィンドウ位置
    RECT windowRect = { 0, 0, 0, 0 };

    // プリコネのウィンドウ位置を取得
    if (GetWindowRect(hWndPriconne, &windowRect) == FALSE)
    {
        MessageBoxW(NULL, L"プリコネのウィンドウ位置が取得できません。", title, MB_ICONERROR);
        return 0;
    }

#if _DEBUG
    {
        std::wstringstream wss;
        wss << L"\nPriconne Window Left: " << windowRect.left << L" Top: " << windowRect.top << L" Right: " << windowRect.right << L" Bottom: " << windowRect.bottom << L"\n\n";
        OutputDebugStringW(wss.str().c_str());
    }
#endif

    // モニター情報
    MONITORINFOEXW monitorInfoEx;
    monitorInfoEx.cbSize = sizeof(monitorInfoEx);

    // プリコネのウィンドウ左上の位置
    POINT windowPos = { windowRect.left, windowRect.top };

    // プリコネのウィンドウが含まれるモニターの情報を取得する
    if (GetMonitorInfoFromPoint(windowPos, &monitorInfoEx) == FALSE)
    {
        MessageBoxW(NULL, L"モニターの情報が取得できません。", title, MB_ICONERROR);
        return 0;
    }

#if _DEBUG
    {
        std::wstringstream wss;
        wss << L"\nMonitor Left: " << monitorInfoEx.rcMonitor.left << L" Top: " << monitorInfoEx.rcMonitor.top << L" Right: " << monitorInfoEx.rcMonitor.right << L" Bottom: " << monitorInfoEx.rcMonitor.bottom << L"\n\n";
        OutputDebugStringW(wss.str().c_str());
    }
#endif

    // ウィンドウのサイズ
    int width, height;
    width = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
    height = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;

    // フルスクリーン化可能な解像度でない場合
    if (!IsFullscreenable(width, height)) {
        LPCWSTR message =
LR"(フルスクリーン化できることを確認済みの解像度は以下の2つです。

・1920 x 1080
・1280 x 720

現在のディスプレイの解像度ではフルスクリーン化できない可能性があります。続行しますか？)";

        int yesno = MessageBoxW(NULL, message, title, MB_YESNO | MB_ICONWARNING);

        if (yesno == IDNO) {
            return 0;
        }
    }

    // タイトルバーを取り除く
    if (RemoveTitleBar(hWndPriconne) == FALSE)
    {
        MessageBox(NULL, L"タイトルバーの削除に失敗しました", title, MB_ICONERROR);
        return 0;
    }

    // ウィンドウを移動&サイズ変更
    MoveWindow(hWndPriconne, monitorInfoEx.rcMonitor.left, monitorInfoEx.rcMonitor.top, width, height, FALSE);

    return 0;
}
