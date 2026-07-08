// ============================================================
// main_gui.cpp — 校园RPG (Win32 GUI 中文版)
// 适配: BaseCharacter + Player + Skill + SaveManager
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

#include "BaseCharacter.h"
#include "Player.h"
#include "Skill.h"
#include "LevelSystem.h"
#include "SaveManager.h"

#pragma comment(lib, "comctl32.lib")

using namespace RPG;

// ==================== 控件 ID ====================
#define IDC_EDIT_INPUT  301
#define IDC_LIST_SKILLS 401
#define IDC_LIST_SAVES  402
#define IDC_TEXT_INFO   501
#define IDC_RADIO_WAR   1001
#define IDC_RADIO_MAGE  1002
#define IDC_RADIO_ARCH  1003

#define IDC_BTN_CREATE  201
#define IDC_BTN_EXP     202
#define IDC_BTN_SAVE    203
#define IDC_BTN_LOAD    204
#define IDC_BTN_SKILL   205
#define IDC_BTN_EXIT    206

// ==================== 全局 ====================
HINSTANCE g_hInst = nullptr;
Player g_player;
HWND g_hWnd = nullptr;
HWND g_hStatusBar = nullptr;
HWND g_hInfoText = nullptr;
HFONT g_hFontMono = nullptr;
HFONT g_hFontYahei = nullptr;

static inline int WideToInt(const wchar_t* s) { return (int)wcstol(s, nullptr, 10); }

void SetStatus(const wchar_t* t) { SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)t); }
void SetStatus(const std::string& u) {
    int n = MultiByteToWideChar(CP_UTF8,0,u.c_str(),-1,nullptr,0);
    std::wstring w(n,0); MultiByteToWideChar(CP_UTF8,0,u.c_str(),-1,&w[0],n); SetStatus(w.c_str());
}

std::wstring U2W(const std::string& u) {
    int n = MultiByteToWideChar(CP_UTF8,0,u.c_str(),-1,nullptr,0);
    std::wstring w(n,0); MultiByteToWideChar(CP_UTF8,0,u.c_str(),-1,&w[0],n); return w;
}

std::string W2U(const wchar_t* w) {
    int n = WideCharToMultiByte(CP_UTF8,0,w,-1,nullptr,0,nullptr,nullptr);
    std::string u(n,0); WideCharToMultiByte(CP_UTF8,0,w,-1,&u[0],n,nullptr,nullptr);
    if(!u.empty()) u.pop_back(); return u;
}

std::wstring I2W(int v) { wchar_t b[32]; wsprintfW(b,L"%d",v); return b; }

// ==================== 构建信息文本 ====================

std::wstring BuildInfo() {
    if (g_player.getLevel() <= 0 || g_player.getName().empty() || g_player.getName()=="读取失败") {
        return L"  尚未创建角色，请点击【创建角色】按钮";
    }
    std::ostringstream o;
    o << "  角色名称: " << g_player.getName() << "\n"
      << "  等    级: " << g_player.getLevel() << "\n"
      << "  经    验: " << g_player.getExp() << " / " << g_player.getExpNeed() << "\n"
      << "  -------------------------------\n"
      << "  力量    " << g_player.getStr()
      << "  体质    " << g_player.getVit() << "\n"
      << "  敏捷    " << g_player.getAgi()
      << "  智力    " << g_player.getIntl() << "\n"
      << "  -------------------------------\n"
      << "  HP: " << g_player.getHp() << "/" << g_player.getMaxHp() << "\n"
      << "  MP: " << g_player.getMp() << "/" << g_player.getMaxMp() << "\n"
      << "  物防 " << g_player.getDef()
      << "  魔防 " << g_player.getMdef() << "\n"
      << "  -------------------------------\n"
      << "  金币: " << g_player.getGold() << "\n"
      << "  技能点: " << g_player.getSkillPoint() << "\n"
      << "  已学技能: " << g_player.getSkillCount() << " 个";
    return U2W(o.str());
}

void Refresh() {
    if (g_hInfoText) SetWindowTextW(g_hInfoText, BuildInfo().c_str());
}

// ==================== 对话框 ====================

INT_PTR CALLBACK DlgCreate(HWND h, UINT m, WPARAM w, LPARAM) {
    switch(m) {
    case WM_INITDIALOG:
        CheckRadioButton(h, IDC_RADIO_WAR, IDC_RADIO_ARCH, IDC_RADIO_WAR);
        SetFocus(GetDlgItem(h, IDC_EDIT_INPUT));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(w)==IDOK) {
            wchar_t b[64]={0}; GetDlgItemTextW(h,IDC_EDIT_INPUT,b,64);
            std::string n = W2U(b); if(n.empty()) n=u8"勇者";
            g_player = Player(n);
            EndDialog(h,IDOK);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h,IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgNumber(HWND h, UINT m, WPARAM w, LPARAM) {
    switch(m) {
    case WM_INITDIALOG: SetFocus(GetDlgItem(h,IDC_EDIT_INPUT)); return FALSE;
    case WM_COMMAND:
        if(LOWORD(w)==IDOK) {
            wchar_t b[32]={0}; GetDlgItemTextW(h,IDC_EDIT_INPUT,b,32);
            EndDialog(h,WideToInt(b));
        } else if(LOWORD(w)==IDCANCEL) EndDialog(h,-1);
        return TRUE;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgSkill(HWND h, UINT m, WPARAM w, LPARAM) {
    static const std::vector<Skill*>* allSkills = nullptr;

    switch(m) {
    case WM_INITDIALOG: {
        allSkills = &SkillFactory::getAllSkills();

        wchar_t buf[32]; wsprintfW(buf, L"%d", g_player.getSkillPoint());
        SetDlgItemTextW(h, IDC_TEXT_INFO, buf);

        HWND hList = GetDlgItem(h, IDC_LIST_SKILLS);
        for (size_t i = 0; i < allSkills->size(); ++i) {
            const Skill* s = (*allSkills)[i];
            std::wstring text = I2W(s->getId()) + L". " + U2W(s->getName())
                + L"  (Lv需求" + I2W(s->getUnlockLv()) + L" MP:" + I2W(s->getMpCost()) + L")";
            if (g_player.hasSkill(s->getId())) {
                text += L" [已学Lv" + I2W(g_player.getSkill(s->getId())->getLevel()) + L"]";
            }
            int idx = (int)SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)text.c_str());
            SendMessage(hList, LB_SETITEMDATA, idx, (LPARAM)s->getId());
        }
        return FALSE;
    }
    case WM_COMMAND: {
        HWND hList = GetDlgItem(h, IDC_LIST_SKILLS);
        int sel = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
        int skillId = (sel != LB_ERR) ? (int)SendMessage(hList, LB_GETITEMDATA, sel, 0) : -1;

        if (LOWORD(w) == 10001) {
            if (skillId < 0) { MessageBoxW(h, L"请先选择一个技能！", L"提示", MB_OK); break; }
            if (g_player.learnSkill(skillId)) {
                EndDialog(h, 1);
            } else {
                MessageBoxW(h, L"无法学习：技能点不足或等级不够。", L"提示", MB_OK);
            }
        }
        else if (LOWORD(w) == 10002) {
            if (skillId < 0) { MessageBoxW(h, L"请先选择一个技能！", L"提示", MB_OK); break; }
            if (!g_player.hasSkill(skillId)) {
                MessageBoxW(h, L"尚未学习该技能，无法升级！", L"提示", MB_OK); break;
            }
            if (g_player.upgradeSkill(skillId)) {
                EndDialog(h, 1);
            } else {
                MessageBoxW(h, L"技能点不足或技能已达最高级。", L"提示", MB_OK);
            }
        }
        else if (LOWORD(w) == IDCANCEL) {
            EndDialog(h, 0);
        }
        return TRUE;
    }
    }
    return FALSE;
}

// ==================== 主窗口 ====================

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // 信息面板使用 微软雅黑（支持中文）
        g_hFontMono = CreateFontW(18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,DEFAULT_PITCH,L"微软雅黑");
        g_hFontYahei = CreateFontW(16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,DEFAULT_PITCH,L"微软雅黑");

        g_hStatusBar = CreateWindowW(STATUSCLASSNAMEW,nullptr,
            WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,0,0,0,0,hWnd,nullptr,g_hInst,nullptr);

        g_hInfoText = CreateWindowW(L"STATIC", BuildInfo().c_str(),
            WS_CHILD|WS_VISIBLE|SS_LEFT|WS_BORDER,16,16,400,400,
            hWnd,nullptr,g_hInst,nullptr);
        SendMessage(g_hInfoText, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);

        const int bw=140,bh=40,bx=435;
        int by=20;
        struct{int id;const wchar_t* t;} btns[]={
            {IDC_BTN_CREATE,L"[创建角色]"},
            {IDC_BTN_EXP,   L"[获得经验]"},
            {IDC_BTN_SKILL, L"[技能管理]"},
            {IDC_BTN_SAVE,  L"[保存存档]"},
            {IDC_BTN_LOAD,  L"[读取存档]"},
            {IDC_BTN_EXIT,  L"[退出游戏]"},
        };
        for(int i=0;i<6;++i){
            HWND h = CreateWindowW(L"BUTTON",btns[i].t,
                WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,bx,by,bw,bh,
                hWnd,(HMENU)btns[i].id,g_hInst,nullptr);
            SendMessage(h,WM_SETFONT,(WPARAM)g_hFontYahei,TRUE);
            by += bh+6;
        }
        SetStatus(L"欢迎来到校园RPG冒险游戏！");
        break;
    }

    case WM_SIZE:
        SendMessage(g_hStatusBar, WM_SIZE, 0, 0);
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDC_BTN_CREATE:
            if(DialogBoxW(g_hInst,MAKEINTRESOURCEW(100),hWnd,DlgCreate)==IDOK){
                Refresh();
                SetStatus("角色 " + g_player.getName() + " 创建成功！");
            }
            break;
        case IDC_BTN_EXP:{
            int e=(int)DialogBoxW(g_hInst,MAKEINTRESOURCEW(200),hWnd,DlgNumber);
            if(e>0){ LevelSystem::gainExp(g_player,e); Refresh();
                SetStatus("获得 " + std::to_string(e) + " 经验值！"); }
            break;
        }
        case IDC_BTN_SKILL:
            if(DialogBoxW(g_hInst,MAKEINTRESOURCEW(400),hWnd,DlgSkill)){
                Refresh();
            }
            break;
        case IDC_BTN_SAVE:
            SaveManager::savePlayerData(g_player);
            SetStatus("存档已保存！");
            break;
        case IDC_BTN_LOAD:
            if(SaveManager::loadPlayerData(g_player)){
                Refresh(); SetStatus("存档已加载！");
            } else {
                MessageBoxW(hWnd, L"未找到存档文件，将创建新角色。",
                    L"提示", MB_OK|MB_ICONINFORMATION);
            }
            break;
        case IDC_BTN_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        if(g_hFontMono) DeleteObject(g_hFontMono);
        if(g_hFontYahei) DeleteObject(g_hFontYahei);
        SkillFactory::cleanup();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ==================== WinMain ====================

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    g_hInst = hInst;

    INITCOMMONCONTROLSEX icc;
    icc.dwSize=sizeof(icc); icc.dwICC=ICC_STANDARD_CLASSES|ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    WNDCLASSW wc={};
    wc.lpfnWndProc=WndProc; wc.hInstance=hInst;
    wc.lpszClassName=L"RPGAdventureWindow";
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    wc.hIcon=LoadIcon(nullptr,IDI_APPLICATION);
    RegisterClassW(&wc);

    SaveManager::initDataFolder();

    g_player = Player();
    if (!SaveManager::loadPlayerData(g_player)) {
        g_player = Player(u8"勇者");
    }

    g_hWnd = CreateWindowExW(0, L"RPGAdventureWindow", L"校园RPG冒险游戏",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 620, 500,
        nullptr, nullptr, hInst, nullptr);
    if(!g_hWnd){ MessageBoxA(nullptr,"窗口创建失败！","错误",MB_OK); return 1; }

    ShowWindow(g_hWnd, nCmdShow); UpdateWindow(g_hWnd);
    Refresh();

    MSG msg;
    while(GetMessage(&msg,nullptr,0,0)){
        if(!IsDialogMessage(g_hWnd,&msg)){
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
    }

    if (SaveManager::saveExists() || g_player.getExp() > 0) {
        SaveManager::savePlayerData(g_player);
    }

    return 0;
}
