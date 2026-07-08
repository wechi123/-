// ============================================================
// main_gui.cpp - 校园RPG冒险游戏 (Win32 GUI 中文版)
// ============================================================
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0500
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>

#include "Character.h"
#include "LevelSystem.h"
#include "SaveManager.h"

#pragma comment(lib, "comctl32.lib")

using namespace RPG;

// ==================== 全局 ====================
HINSTANCE g_hInst = nullptr;
Character g_player;
HWND g_hWnd = nullptr;
HWND g_hStatusBar = nullptr;
HWND g_hInfoText = nullptr;
HFONT g_hFontConsolas = nullptr;
HFONT g_hFontYahei = nullptr;

#define IDC_EDIT_INPUT  301
#define IDC_LIST_SAVES  401
#define IDC_RADIO_WAR   1001
#define IDC_RADIO_MAGE  1002
#define IDC_RADIO_ARCH  1003

#define IDC_BTN_CREATE  201
#define IDC_BTN_EXP     202
#define IDC_BTN_SAVE    203
#define IDC_BTN_LOAD    204
#define IDC_BTN_LIST    205
#define IDC_BTN_DEL     206
#define IDC_BTN_EXIT    207

// ==================== 辅助 ====================

static inline int WideToInt(const wchar_t* s) {
    return (int)wcstol(s, nullptr, 10);
}

static inline int WidePrint(wchar_t* buf, size_t n, const wchar_t* fmt, int val) {
    return wsprintfW(buf, fmt, val);
}

void SetStatusText(const wchar_t* text) {
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)text);
}

void SetStatusText(const std::string& utf8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
    SetStatusText(wstr.c_str());
}

std::wstring Utf8ToWide(const std::string& utf8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
    return wstr;
}

std::string WideToUtf8(const wchar_t* wide) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, &utf8[0], len, nullptr, nullptr);
    if (!utf8.empty()) utf8.pop_back();
    return utf8;
}

std::wstring IntToWide(int value) {
    wchar_t buf[32];
    wsprintfW(buf, L"%d", value);
    return std::wstring(buf);
}

std::wstring BuildCharInfo() {
    if (g_player.getName().empty() || g_player.getName() == "未命名" || g_player.getName() == "读取失败") {
        return L"  ╔════ 角色信息 ════╗\n\n  ⚠ 尚未创建角色\n\n  请点击右侧「创建角色」按钮";
    }
    std::ostringstream oss;
    oss << "  ╔════ 角色信息 ════╗\n\n"
        << "  名称: " << g_player.getName() << "\n"
        << "  职业: " << Character::classNameToString(g_player.getCharClass()) << "\n"
        << "  等级: " << g_player.getLevel() << "\n"
        << "  生命: " << g_player.getHp() << " / " << g_player.getMaxHp() << "\n"
        << "  经验: " << g_player.getExp() << " / " << g_player.getMaxExp() << "\n"
        << "  金币: " << g_player.getGold() << "\n\n"
        << "  攻击: " << g_player.getAttack() << "\n"
        << "  防御: " << g_player.getDefense() << "\n"
        << "  速度: " << g_player.getSpeed() << "\n";
    return Utf8ToWide(oss.str());
}

void RefreshInfo() {
    SetWindowTextW(g_hInfoText, BuildCharInfo().c_str());
}

// ==================== 对话框过程 ====================

INT_PTR CALLBACK CreateCharDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_RADIO_WAR, IDC_RADIO_ARCH, IDC_RADIO_WAR);
        SetFocus(GetDlgItem(hDlg, IDC_EDIT_INPUT));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[64] = {0};
            GetDlgItemTextW(hDlg, IDC_EDIT_INPUT, buf, 64);
            std::string name = WideToUtf8(buf);
            if (name.empty()) name = "勇者";
            CharacterClass cls = CharacterClass::WARRIOR;
            if (IsDlgButtonChecked(hDlg, IDC_RADIO_MAGE) == BST_CHECKED) cls = CharacterClass::MAGE;
            else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ARCH) == BST_CHECKED) cls = CharacterClass::ARCHER;
            g_player = Character(name, cls);
            EndDialog(hDlg, IDOK);
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
        }
        return TRUE;
    }
    return FALSE;
}

INT_PTR CALLBACK InputNumberDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, IDC_EDIT_INPUT));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[32] = {0};
            GetDlgItemTextW(hDlg, IDC_EDIT_INPUT, buf, 32);
            EndDialog(hDlg, WideToInt(buf));
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, -1);
        }
        return TRUE;
    }
    return FALSE;
}

INT_PTR CALLBACK SlotDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG: {
        SetWindowTextW(hDlg, (const wchar_t*)lParam);
        SetFocus(GetDlgItem(hDlg, IDC_EDIT_INPUT));
        HWND hList = GetDlgItem(hDlg, IDC_LIST_SAVES);
        auto slots = SaveManager::listSaves();
        for (size_t i = 0; i < slots.size(); ++i) {
            wchar_t buf[32];
            wsprintfW(buf, L"槽位 %d", slots[i]);
            SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)buf);
        }
        return FALSE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buf[32] = {0};
            GetDlgItemTextW(hDlg, IDC_EDIT_INPUT, buf, 32);
            int slot = WideToInt(buf);
            if (slot >= 0 && slot <= 9) EndDialog(hDlg, slot);
            else MessageBoxW(hDlg, L"请输入 0-9 的槽位号！", L"提示", MB_OK | MB_ICONWARNING);
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, -1);
        } else if (HIWORD(wParam) == LBN_DBLCLK) {
            HWND hList = (HWND)lParam;
            int sel = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR) {
                wchar_t buf[32];
                SendMessageW(hList, LB_GETTEXT, sel, (LPARAM)buf);
                EndDialog(hDlg, WideToInt(buf + 3));
            }
        }
        return TRUE;
    }
    return FALSE;
}

// ==================== 主窗口 ====================

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_hFontConsolas = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        g_hFontYahei = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH, L"微软雅黑");

        g_hStatusBar = CreateWindowW(STATUSCLASSNAMEW, nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, nullptr, g_hInst, nullptr);

        g_hInfoText = CreateWindowW(L"STATIC", L"  ╔════ 角色信息 ════╗\n\n  ⚠ 尚未创建角色",
            WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER, 16, 16, 400, 370, hWnd, nullptr, g_hInst, nullptr);
        SendMessage(g_hInfoText, WM_SETFONT, (WPARAM)g_hFontConsolas, TRUE);

        const int btnW = 140, btnH = 40, btnX = 435;
        int btnY = 20;
        struct { int id; const wchar_t* text; } btns[] = {
            {IDC_BTN_CREATE, L"🎮 创建角色"},
            {IDC_BTN_EXP,    L"⭐ 获得经验"},
            {IDC_BTN_SAVE,   L"💾 保存存档"},
            {IDC_BTN_LOAD,   L"📂 读取存档"},
            {IDC_BTN_LIST,   L"📋 列出存档"},
            {IDC_BTN_DEL,    L"🗑 删除存档"},
            {IDC_BTN_EXIT,   L"🚪 退出游戏"},
        };
        for (int i = 0; i < 7; ++i) {
            HWND hBtn = CreateWindowW(L"BUTTON", btns[i].text,
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                btnX, btnY, btnW, btnH, hWnd, (HMENU)btns[i].id, g_hInst, nullptr);
            SendMessage(hBtn, WM_SETFONT, (WPARAM)g_hFontYahei, TRUE);
            btnY += btnH + 6;
        }
        SetStatusText(L"🎮 欢迎！请先创建角色。");
        break;
    }

    case WM_SIZE:
        SendMessage(g_hStatusBar, WM_SIZE, 0, 0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_CREATE:
            if (DialogBoxW(g_hInst, MAKEINTRESOURCEW(100), hWnd, CreateCharDlgProc) == IDOK) {
                RefreshInfo();
                SetStatusText("✅ 角色 " + g_player.getName() + " 创建成功！");
            }
            break;
        case IDC_BTN_EXP: {
            if (g_player.getName() == "未命名" || g_player.getName() == "读取失败") {
                MessageBoxW(hWnd, L"请先创建角色！", L"提示", MB_OK | MB_ICONINFORMATION);
                break;
            }
            int exp = (int)DialogBoxW(g_hInst, MAKEINTRESOURCEW(200), hWnd, InputNumberDlgProc);
            if (exp > 0) {
                LevelSystem::gainExp(g_player, exp);
                RefreshInfo();
                SetStatusText("⭐ 获得 " + std::to_string(exp) + " 经验值！");
            }
            break;
        }
        case IDC_BTN_SAVE: {
            if (g_player.getName() == "未命名" || g_player.getName() == "读取失败") {
                MessageBoxW(hWnd, L"请先创建角色！", L"提示", MB_OK | MB_ICONINFORMATION);
                break;
            }
            int slot = (int)DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(300), hWnd, SlotDlgProc, (LPARAM)L"💾 保存存档");
            if (slot >= 0) {
                SaveManager::saveCharacter(g_player, slot);
                SetStatusText("✅ 角色已保存到槽位 " + std::to_string(slot));
            }
            break;
        }
        case IDC_BTN_LOAD: {
            int slot = (int)DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(300), hWnd, SlotDlgProc, (LPARAM)L"📂 读取存档");
            if (slot >= 0) {
                if (!SaveManager::saveExists(slot)) {
                    MessageBoxW(hWnd, L"该槽位无存档！", L"提示", MB_OK | MB_ICONWARNING);
                    break;
                }
                g_player = SaveManager::loadCharacter(slot);
                RefreshInfo();
                SetStatusText("✅ 存档已加载！");
            }
            break;
        }
        case IDC_BTN_LIST: {
            auto slots = SaveManager::listSaves();
            if (slots.empty()) {
                MessageBoxW(hWnd, L"暂无存档，请先保存。", L"📋 存档列表", MB_OK | MB_ICONINFORMATION);
            } else {
                std::wstring msg = L"现有存档：\n";
                for (size_t i = 0; i < slots.size(); ++i)
                    msg += L"  ● 槽位 " + IntToWide(slots[i]) + L"\n";
                MessageBoxW(hWnd, msg.c_str(), L"📋 存档列表", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case IDC_BTN_DEL: {
            int slot = (int)DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(300), hWnd, SlotDlgProc, (LPARAM)L"🗑 删除存档");
            if (slot >= 0) {
                if (SaveManager::deleteSave(slot))
                    SetStatusText("🗑 槽位 " + std::to_string(slot) + " 存档已删除");
                else
                    MessageBoxW(hWnd, L"该槽位无存档！", L"提示", MB_OK | MB_ICONWARNING);
            }
            break;
        }
        case IDC_BTN_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        if (g_hFontConsolas) DeleteObject(g_hFontConsolas);
        if (g_hFontYahei) DeleteObject(g_hFontYahei);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ==================== 入口 ====================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    g_hInst = hInstance;

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"RPGAdventureWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassW(&wc);

    g_hWnd = CreateWindowExW(0, L"RPGAdventureWindow", L"🎮 校园RPG冒险游戏",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 620, 480,
        nullptr, nullptr, hInstance, nullptr);
    if (!g_hWnd) { MessageBoxA(nullptr, "窗口创建失败！", "错误", MB_OK); return 1; }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    g_player = Character("勇者", CharacterClass::WARRIOR);
    RefreshInfo();
    SetStatusText(L"🎮 欢迎来到校园RPG冒险游戏！");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessage(g_hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
