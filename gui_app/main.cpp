// ============================================================
// 校园RPG GUI — 完整版
// 9大设定：角色/背包/商店/任务/战斗/成长/存档/SQLite/多线程
// ============================================================
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0500
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "BaseCharacter.h"
#include "Player.h"
#include "Skill.h"
#include "LevelSystem.h"
#include "SaveManager.h"
#include "Inventory.h"
#include "Shop.h"
#include "TaskSystem.h"

#pragma comment(lib, "comctl32.lib")

using namespace RPG;

// ==================== 控件 ID ====================
#define IDC_EDIT_INPUT  301
#define IDC_LIST_ITEMS  401
#define IDC_LIST_SKILLS 402
#define IDC_LIST_SHOP   403
#define IDC_LIST_TASKS  404
#define IDC_TEXT_INFO   501
#define IDC_RADIO_WAR   1001
#define IDC_RADIO_MAGE  1002
#define IDC_RADIO_ARCH  1003

#define IDC_BTN_CREATE  201
#define IDC_BTN_SAVE    203
#define IDC_BTN_LOAD    204
#define IDC_BTN_SKILL   205
#define IDC_BTN_BAG     206
#define IDC_BTN_SHOP    207
#define IDC_BTN_TASK    208
#define IDC_BTN_BATTLE  210
#define IDC_BTN_EXIT    209

// ==================== 全局 ====================
HINSTANCE g_hInst = nullptr;
Player g_player;
Inventory g_inventory(99);
Shop g_shop("校园商店");
TaskSystem g_taskSystem;
HWND g_hWnd = nullptr;
HWND g_hStatusBar = nullptr;
HWND g_hInfoText = nullptr;
HFONT g_hFontMono = nullptr;
HFONT g_hFontYahei = nullptr;
bool g_initialized = false;

// 任务进度追踪
int g_killSlacker = 0;    // 击杀懒散学渣次数
int g_killNightOwl = 0;   // 击杀熬夜复习怪次数
int g_killBoss = 0;       // 击杀期末压力Boss次数
int g_totalBattles = 0;   // 总战斗次数

// 自动存档
volatile bool g_autoSaveRunning = false;
HANDLE g_autoSaveThread = NULL;

// ==================== 怪物定义 ====================
struct EnemyInfo {
    const wchar_t* name;
    const wchar_t* desc;
    int hp; int atk; int def; int expR; int goldR; int id;
};
static const EnemyInfo g_enemies[] = {
    {L"懒散学渣小怪", L"校园最弱怪物，适合新手练级", 80,  8,  2, 30,  15, 1},
    {L"熬夜复习怪",   L"长期熬夜积攒的负能量怪物",     150, 15, 5, 65,  35, 2},
    {L"期末压力Boss", L"校园最强BOSS，难度最高",       300, 28,10, 150, 90, 3},
};
static const int g_enemyCount = 3;

// ==================== 等级成长规则 ====================
const int MAX_LEVEL = 50;
const int HP_PER_LEVEL = 20;
const int ATK_PER_LEVEL = 5;

int expToNextLevel(int lv) { return lv * 30 + 20; }

void checkLevelUp() {
    while (g_player.getLevel() < MAX_LEVEL) {
        int need = expToNextLevel(g_player.getLevel());
        if (g_player.getExp() < need) break;
        g_player.setExp(g_player.getExp() - need);
        int oldLv = g_player.getLevel();
        g_player.setLevel(oldLv + 1);
        g_player.setMaxHp(g_player.getMaxHp() + HP_PER_LEVEL);
        g_player.setHp(g_player.getMaxHp());
        g_player.setStr(g_player.getStr() + ATK_PER_LEVEL);
        g_player.recalcStats();
    }
}


// ==================== 辅助函数 ====================
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
void SyncGold() { g_inventory.setOwnerGold(g_player.getGold()); }

std::wstring BuildInfo() {
    if (g_player.getLevel() <= 0 || g_player.getName().empty()) {
        return L"  尚未创建角色，请点击【创建角色】按钮\r\n\r\n  经验值通过战斗和任务获得";
    }
    std::ostringstream o;
    o << "  角色名称: " << g_player.getName() << "\r\n"
      << "  等    级: Lv." << g_player.getLevel() << " (最高" << MAX_LEVEL << ")\r\n"
      << "  经    验: " << g_player.getExp() << " / " << expToNextLevel(g_player.getLevel()) << "\r\n"
      << "  -------------------------------\r\n"
      << "  力量(ATK): " << g_player.getStr()
      << "    体质(VIT): " << g_player.getVit() << "\r\n"
      << "  敏捷(AGI): " << g_player.getAgi()
      << "    智力(INT): " << g_player.getIntl() << "\r\n"
      << "  -------------------------------\r\n"
      << "  HP: " << g_player.getHp() << "/" << g_player.getMaxHp() << "\r\n"
      << "  MP: " << g_player.getMp() << "/" << g_player.getMaxMp() << "\r\n"
      << "  物防(DEF): " << g_player.getDef()
      << "   魔防(MDEF): " << g_player.getMdef() << "\r\n"
      << "  -------------------------------\r\n"
      << "  金币: " << g_player.getGold() << " G\r\n"
      << "  技能点: " << g_player.getSkillPoint() << "\r\n"
      << "  背包: " << g_inventory.getItemCount() << " 件\r\n"
      << "  战斗: " << g_totalBattles << " 场\r\n"
      << "  技能: " << g_player.getSkillCount() << " 个";
    return U2W(o.str());
}

void Refresh() { if (g_hInfoText) SetWindowTextW(g_hInfoText, BuildInfo().c_str()); }

// ==================== 自动存档线程 ====================
void autoSaveLoop() {
    while (g_autoSaveRunning) {
        for (int i = 0; i < 60 && g_autoSaveRunning; ++i)
            Sleep(1000);
        if (!g_autoSaveRunning) break;
        if (g_player.getLevel() > 0) {
            SaveManager::savePlayerData(g_player);
        }
    }
}

DWORD WINAPI autoSaveThreadProc(LPVOID) { autoSaveLoop(); return 0; }
void startAutoSave() {
    if (g_autoSaveRunning) return;
    g_autoSaveRunning = true;
    g_autoSaveThread = CreateThread(NULL, 0, autoSaveThreadProc, NULL, 0, NULL);
}

void stopAutoSave() {
    if (!g_autoSaveRunning) return;
    g_autoSaveRunning = false;
    if (g_autoSaveThread) { WaitForSingleObject(g_autoSaveThread, 5000); CloseHandle(g_autoSaveThread); g_autoSaveThread = NULL; }
}

// ==================== 初始化系统 ====================
void InitGame() {
    if (g_initialized) return;
    g_shop.initDefaultShop();
    g_taskSystem.loadTasks("data/tasks.txt");
    g_initialized = true;
    startAutoSave();
}


// ==================== 创建角色对话框 ====================
INT_PTR CALLBACK DlgCreate(HWND h, UINT m, WPARAM w, LPARAM) {
    switch(m) {
    case WM_INITDIALOG:
        CheckRadioButton(h, IDC_RADIO_WAR, IDC_RADIO_ARCH, IDC_RADIO_WAR);
        SetFocus(GetDlgItem(h, IDC_EDIT_INPUT)); return FALSE;
    case WM_COMMAND:
        if (LOWORD(w)==IDOK) {
            wchar_t b[64]={0}; GetDlgItemTextW(h,IDC_EDIT_INPUT,b,64);
            std::string n = W2U(b); if(n.empty()) n="校园冒险者";
            g_player = Player(n);
            g_killSlacker = g_killNightOwl = g_killBoss = g_totalBattles = 0;
            InitGame(); SyncGold();
            EndDialog(h,IDOK);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h,IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

// ==================== 技能对话框 ====================
INT_PTR CALLBACK DlgSkill(HWND h, UINT m, WPARAM w, LPARAM) {
    static const std::vector<Skill*>* allSkills = nullptr;
    static HWND hList = nullptr, hInfo = nullptr;
    switch(m) {
    case WM_INITDIALOG: {
        allSkills = &SkillFactory::getAllSkills();
        hList = GetDlgItem(h, IDC_LIST_SKILLS);
        hInfo = GetDlgItem(h, IDC_TEXT_INFO);
        for (size_t i = 0; i < allSkills->size(); ++i) {
            const Skill* s = (*allSkills)[i];
            std::wstring text = I2W(s->getId()) + L". " + U2W(s->getName())
                + L"  (Lv" + I2W(s->getUnlockLv()) + L" MP:" + I2W(s->getMpCost()) + L")";
            if (g_player.hasSkill(s->getId()))
                text += L" [已学Lv" + I2W(g_player.getSkill(s->getId())->getLevel()) + L"]";
            SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)text.c_str());
        }
        wchar_t buf[32]; wsprintfW(buf, L"%d", g_player.getSkillPoint());
        SetWindowTextW(hInfo, buf); return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(w)==10001 || LOWORD(w)==10002) {
            int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel < 0) { MessageBoxW(h, L"请先选择一个技能！", L"提示", MB_OK); break; }
            int skillId = (*allSkills)[sel]->getId();
            if (LOWORD(w)==10001) {
                if (!g_player.learnSkill(skillId))
                    MessageBoxW(h, L"无法学习：技能点不足或等级不够。", L"提示", MB_OK);
            } else {
                if (!g_player.hasSkill(skillId))
                    MessageBoxW(h, L"尚未学习该技能！", L"提示", MB_OK);
                else if (!g_player.upgradeSkill(skillId))
                    MessageBoxW(h, L"技能点不足或已达最高级。", L"提示", MB_OK);
            }
            wchar_t buf[32]; wsprintfW(buf, L"%d", g_player.getSkillPoint());
            SetWindowTextW(hInfo, buf);
            EndDialog(h, 1);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h, 0);
        return TRUE;
    }
    return FALSE;
}

// ==================== 背包对话框 ====================
INT_PTR CALLBACK DlgBag(HWND h, UINT m, WPARAM w, LPARAM) {
    static HWND hList = nullptr;
    switch(m) {
    case WM_INITDIALOG: {
        hList = GetDlgItem(h, IDC_LIST_ITEMS);
        const auto& items = g_inventory.getItems();
for (size_t i = 0; i < items.size(); ++i) {
            Item* it = items[i];
            std::wstring text = L"[" + it->getTypeName() + L"] " + it->getName()
                + L"  x" + I2W(it->getQuantity()) + L"  " + it->getEffectDesc();
            SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)text.c_str());
        }
        if (items.empty()) SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"（背包是空的）");
        return TRUE;
    }
    case WM_COMMAND: {
        int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
        if (LOWORD(w)==10001) {
            if (sel < 0 || sel >= g_inventory.getItemCount())
                { MessageBoxW(h, L"请先选择一个物品！", L"提示", MB_OK); break; }
            Item* it = g_inventory.getItem(sel);
            if (it && it->getType() == ItemType::EQUIPMENT) {
                Equipment* eq = dynamic_cast<Equipment*>(it);
                if (eq && eq->isEquipped()) { eq->unequip(g_player); MessageBoxW(h, L"装备已卸下。", L"提示", MB_OK); }
                else { g_inventory.useItem(sel, g_player); MessageBoxW(h, L"物品已使用！", L"提示", MB_OK); }
            } else { g_inventory.useItem(sel, g_player); MessageBoxW(h, L"物品已使用！", L"提示", MB_OK); }
            EndDialog(h, 1);
        } else if (LOWORD(w)==10002) {
            if (sel < 0 || sel >= g_inventory.getItemCount())
                { MessageBoxW(h, L"请先选择一个物品！", L"提示", MB_OK); break; }
            Item* it = g_inventory.getItem(sel);
            if (it && it->getType() == ItemType::EQUIPMENT) {
                Equipment* eq = dynamic_cast<Equipment*>(it);
                if (eq && eq->isEquipped()) eq->unequip(g_player);
            }
            g_inventory.removeItem(sel);
            MessageBoxW(h, L"物品已删除！", L"提示", MB_OK);
            EndDialog(h, 1);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h, 0);
        return TRUE;
    }
    }
    return FALSE;
}

// ==================== 商店对话框 ====================
INT_PTR CALLBACK DlgShop(HWND h, UINT m, WPARAM w, LPARAM) {
    static HWND hShop = nullptr, hBag = nullptr, hGold = nullptr;
    switch(m) {
    case WM_INITDIALOG: {
        hShop = GetDlgItem(h, IDC_LIST_SHOP); hBag = GetDlgItem(h, IDC_LIST_ITEMS);
        hGold = GetDlgItem(h, IDC_TEXT_INFO); SyncGold();
        for (int i = 0; i < g_shop.getItemCount(); ++i) {
            Item* it = g_shop.getItem(i);
            std::wstring t = L"[" + it->getTypeName() + L"] " + it->getName()
                + L"  " + I2W(it->getPrice()) + L"G  " + it->getEffectDesc();
            SendMessageW(hShop, LB_ADDSTRING, 0, (LPARAM)t.c_str());
        }
        const auto& items = g_inventory.getItems();
for (size_t i = 0; i < items.size(); ++i) {
            Item* it = items[i];
            std::wstring t = L"[" + it->getTypeName() + L"] " + it->getName()
                + L"  x" + I2W(it->getQuantity()) + L"  (售价:" + I2W(it->getPrice()/2) + L"G)";
            SendMessageW(hBag, LB_ADDSTRING, 0, (LPARAM)t.c_str());
        }
        SetWindowTextW(hGold, (L"金币：" + I2W(g_player.getGold()) + L" G").c_str());
        return TRUE;
    }
    case WM_COMMAND: {
        if (LOWORD(w)==10001) { // 购买
            int sel = (int)SendMessageW(hShop, LB_GETCURSEL, 0, 0);
            if (sel < 0 || sel >= g_shop.getItemCount())
                { MessageBoxW(h, L"请先选择一个商品！", L"提示", MB_OK); break; }
            Item* it = g_shop.getItem(sel);
            if (g_player.getGold() < it->getPrice())
                { MessageBoxW(h, L"金币不足！", L"提示", MB_OK); break; }
            SyncGold(); g_shop.buyItem(sel, g_inventory);
            int ng = g_inventory.getOwnerGold();
            if (ng < g_player.getGold()) g_player.costGold(g_player.getGold() - ng);
            SyncGold(); SetWindowTextW(hGold, (L"金币：" + I2W(g_player.getGold()) + L" G").c_str());
            MessageBoxW(h, L"购买成功！", L"提示", MB_OK); EndDialog(h, 1);
        } else if (LOWORD(w)==10002) { // 出售
            int sel = (int)SendMessageW(hBag, LB_GETCURSEL, 0, 0);
            if (sel < 0 || sel >= g_inventory.getItemCount())
                { MessageBoxW(h, L"请先从背包选择一个物品！", L"提示", MB_OK); break; }
            Item* it = g_inventory.getItem(sel);
            if (it && it->getType() == ItemType::EQUIPMENT) {
                Equipment* eq = dynamic_cast<Equipment*>(it);
                if (eq && eq->isEquipped()) eq->unequip(g_player);
            }
            g_shop.sellItem(sel, g_inventory);
            int ng = g_inventory.getOwnerGold();
            if (ng > g_player.getGold()) g_player.gainGold(ng - g_player.getGold());
            SyncGold(); SetWindowTextW(hGold, (L"金币：" + I2W(g_player.getGold()) + L" G").c_str());
            MessageBoxW(h, L"出售成功！", L"提示", MB_OK); EndDialog(h, 1);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h, 0);
        return TRUE;
    }
    }
    return FALSE;
}


// ==================== 任务对话框 ====================
INT_PTR CALLBACK DlgTask(HWND h, UINT m, WPARAM w, LPARAM) {
    static HWND hList = nullptr, hInfo = nullptr;
    switch(m) {
    case WM_INITDIALOG: {
        hList = GetDlgItem(h, IDC_LIST_TASKS); hInfo = GetDlgItem(h, IDC_TEXT_INFO);
        for (int i = 0; i < g_taskSystem.getTaskCount(); ++i) {
            Task* t = g_taskSystem.getTask(i);
            std::string st;
            switch (t->getStatus()) {
                case TaskStatus::NotAccepted: st="[未接受]"; break;
                case TaskStatus::InProgress:  st="[进行中]"; break;
                case TaskStatus::Completed:   st="[已完成]"; break;
                case TaskStatus::RewardClaimed: st="[已领奖]"; break;
            }
            std::wstring text = I2W(i+1) + L". " + U2W(t->getName()) + L"  " + U2W(st)
                + L"  EXP:" + I2W(t->getExpReward());
            SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)text.c_str());
        }
        return TRUE;
    }
    case WM_COMMAND: {
        int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
        if (LOWORD(w)==10001) {
            if (sel < 0) { MessageBoxW(h, L"请先选择一个任务！", L"提示", MB_OK); break; }
            g_taskSystem.acceptTask(sel);
            SetWindowTextW(hInfo, U2W(g_taskSystem.getTask(sel)->getName() + " 已接受！").c_str());
            EndDialog(h, 1);
        } else if (LOWORD(w)==10002) {
            if (sel < 0) { MessageBoxW(h, L"请先选择一个任务！", L"提示", MB_OK); break; }
            Task* t = g_taskSystem.getTask(sel);
            if (t->getStatus() != TaskStatus::InProgress)
                { SetWindowTextW(hInfo, L"请先接受任务！"); EndDialog(h, 1); break; }
            bool canComplete = false;
            switch(sel) {
                case 0: canComplete = (g_totalBattles >= 1); break;
                case 1: canComplete = (g_killSlacker >= 3); break;
                case 2: canComplete = (g_killNightOwl >= 2); break;
                case 3: canComplete = (g_killBoss >= 1); break;
            }
            if (canComplete) {
                g_taskSystem.completeTask(sel);
                SetWindowTextW(hInfo, L"任务条件已满足，请领取奖励！");
            } else {
                SetWindowTextW(hInfo, L"任务条件尚未满足！");
            }
            EndDialog(h, 1);
        } else if (LOWORD(w)==10003) {
            if (sel < 0) { MessageBoxW(h, L"请先选择一个任务！", L"提示", MB_OK); break; }
            Task* t = g_taskSystem.getTask(sel);
            if (t->getStatus() == TaskStatus::Completed) {
                g_taskSystem.claimReward(sel, g_player);
                checkLevelUp();
                wchar_t buf[200];
                wsprintfW(buf, L"获得 %d EXP + %d 金币！", t->getExpReward(), t->getGoldReward());
                SetWindowTextW(hInfo, buf);
            } else { SetWindowTextW(hInfo, L"该任务尚未完成！"); }
            EndDialog(h, 1);
        } else if (LOWORD(w)==IDCANCEL) EndDialog(h, 0);
        return TRUE;
    }
    }
    return FALSE;
}

// ==================== 战斗对话框 ====================
INT_PTR CALLBACK DlgBattle(HWND h, UINT m, WPARAM w, LPARAM) {
    static HWND hList = nullptr, hInfo = nullptr;
    static int selectedEnemy = -1;
    switch(m) {
    case WM_INITDIALOG: {
        hList = GetDlgItem(h, IDC_LIST_ITEMS); hInfo = GetDlgItem(h, IDC_TEXT_INFO);
        for (int i = 0; i < g_enemyCount; ++i) {
            wchar_t buf[300];
            wsprintfW(buf, L"%s | HP:%d ATK:%d DEF:%d | 奖励 EXP:%d 金币:%d",
                g_enemies[i].name, g_enemies[i].hp, g_enemies[i].atk,
                g_enemies[i].def, g_enemies[i].expR, g_enemies[i].goldR);
            SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)buf);
        }
        return TRUE;
    }
    case WM_COMMAND: {
        if (LOWORD(w) == 10001) {
            int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel < 0 || sel >= g_enemyCount) {
                MessageBoxW(h, L"请先选择一个敌人！", L"提示", MB_OK); break;
            }
            if (g_player.getHp() <= 0) {
                MessageBoxW(h, L"你已阵亡！请先恢复HP。", L"提示", MB_OK); break;
            }
            const EnemyInfo& e = g_enemies[sel];
            int pHp = g_player.getHp(), pAtk = g_player.getAttack(), pDef = g_player.getDef();
            int eHp = e.hp, rounds = 0;
            wchar_t log[600] = {0};
            wsprintfW(log, L"挑战 %s ！\r\n", e.name);

            // 回合制战斗
            while (pHp > 0 && eHp > 0 && rounds < 50) {
                rounds++;
                int pDmg = pAtk + (rand() % (pAtk/2+1)) - e.def;
                if (pDmg < 1) pDmg = 1; eHp -= pDmg;
                if (eHp <= 0) break;
                int eDmg = e.atk + (rand() % (e.atk/2+1)) - pDef;
                if (eDmg < 1) eDmg = 1; pHp -= eDmg;
            }

            g_player.setHp(pHp > 0 ? pHp : 0);
            g_totalBattles++;

            if (eHp <= 0) {
                wsprintfW(log + wcslen(log), L"胜利！击败 %s（%d回合）\r\nEXP +%d  金币 +%d",
                    e.name, rounds, e.expR, e.goldR);
                g_player.addExp(e.expR); g_player.gainGold(e.goldR);
                checkLevelUp();
                // 击杀计数
                if (sel == 0) g_killSlacker++;
                if (sel == 1) g_killNightOwl++;
                if (sel == 2) g_killBoss++;
                SetStatus("战斗胜利！击败 " + W2U(e.name) + "！");
            } else {
                wsprintfW(log + wcslen(log), L"失败... 被 %s 击败。HP 恢复为 1", e.name);
                g_player.setHp(1);
                SetStatus("战斗失败...");
            }
            SetWindowTextW(hInfo, log);
            Refresh();
        } else if (LOWORD(w) == IDCANCEL) EndDialog(h, 0);
        return TRUE;
    }
    }
    return FALSE;
}


// ==================== 主窗口 ====================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_CREATE: {
        g_hFontMono = CreateFontW(18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,DEFAULT_PITCH,L"微软雅黑");
        g_hFontYahei = CreateFontW(15,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,DEFAULT_PITCH,L"微软雅黑");
        g_hStatusBar = CreateWindowW(STATUSCLASSNAMEW,nullptr,
            WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,0,0,0,0,hWnd,nullptr,g_hInst,nullptr);
        g_hInfoText = CreateWindowW(L"STATIC", BuildInfo().c_str(),
            WS_CHILD|WS_VISIBLE|SS_LEFT|WS_BORDER,12,12,340,520,
            hWnd,nullptr,g_hInst,nullptr);
        SendMessage(g_hInfoText, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);

        const int bw=130, bh=34, bx=370, gap=4;
        int by=12;
        struct{int id;const wchar_t* t;} btns[]={
            {IDC_BTN_CREATE, L"[创建角色]"},
            {IDC_BTN_SKILL,  L"[技能管理]"},
            {IDC_BTN_BAG,    L"[背包管理]"},
            {IDC_BTN_SHOP,   L"[商店交易]"},
            {IDC_BTN_TASK,   L"[任务系统]"},
            {IDC_BTN_BATTLE, L"[挑战战斗]"},
            {IDC_BTN_SAVE,   L"[保存存档]"},
            {IDC_BTN_LOAD,   L"[读取存档]"},
            {IDC_BTN_EXIT,   L"[退出游戏]"},
        };
        for(int i=0;i<9;++i){
            HWND hBtn = CreateWindowW(L"BUTTON",btns[i].t,
                WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,bx,by,bw,bh,
                hWnd,(HMENU)(INT_PTR)btns[i].id,g_hInst,nullptr);
            SendMessage(hBtn,WM_SETFONT,(WPARAM)g_hFontYahei,TRUE);
            by += bh+gap;
        }
        SetStatus(L"欢迎来到校园RPG冒险游戏！");
        InitGame(); SyncGold();
        break;
    }
    case WM_SIZE: SendMessage(g_hStatusBar, WM_SIZE, 0, 0); break;
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDC_BTN_CREATE:
            if(DialogBoxW(g_hInst,MAKEINTRESOURCEW(100),hWnd,DlgCreate)==IDOK)
                { Refresh(); SetStatus("角色 " + g_player.getName() + " 创建成功！"); }
            break;
        case IDC_BTN_SKILL:
            DialogBoxW(g_hInst,MAKEINTRESOURCEW(400),hWnd,DlgSkill); Refresh(); break;
        case IDC_BTN_BAG:
            DialogBoxW(g_hInst,MAKEINTRESOURCEW(500),hWnd,DlgBag); Refresh(); break;
        case IDC_BTN_SHOP:
            DialogBoxW(g_hInst,MAKEINTRESOURCEW(600),hWnd,DlgShop); Refresh(); break;
        case IDC_BTN_TASK:
            DialogBoxW(g_hInst,MAKEINTRESOURCEW(700),hWnd,DlgTask); Refresh(); break;
        case IDC_BTN_BATTLE:
            DialogBoxW(g_hInst,MAKEINTRESOURCEW(800),hWnd,DlgBattle); Refresh(); break;
        case IDC_BTN_SAVE:
            SaveManager::savePlayerData(g_player);
            SetStatus("存档已保存！"); break;
        case IDC_BTN_LOAD:
            if(SaveManager::loadPlayerData(g_player)){
                g_killSlacker=g_killNightOwl=g_killBoss=g_totalBattles=0;
                InitGame(); SyncGold(); Refresh();
                SetStatus("存档已加载！");
            } else MessageBoxW(hWnd, L"未找到存档文件。", L"提示", MB_OK);
            break;
        case IDC_BTN_EXIT: PostMessage(hWnd, WM_CLOSE, 0, 0); break;
        }
        break;
    case WM_CLOSE: DestroyWindow(hWnd); return 0;
    case WM_DESTROY:
        stopAutoSave();
        if(g_hFontMono) DeleteObject(g_hFontMono);
        if(g_hFontYahei) DeleteObject(g_hFontYahei);
        SkillFactory::cleanup();
        PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ==================== WinMain ====================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    srand((unsigned)time(nullptr));
    g_hInst = hInst;
    INITCOMMONCONTROLSEX icc={sizeof(icc),ICC_STANDARD_CLASSES|ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);

    WNDCLASSW wc={};
    wc.lpfnWndProc=WndProc; wc.hInstance=hInst;
    wc.lpszClassName=L"RPGAdventureWindow";
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    RegisterClassW(&wc);

    SaveManager::initDataFolder();
    g_player = Player();
    SaveManager::loadPlayerData(g_player);

    g_hWnd = CreateWindowExW(0, L"RPGAdventureWindow", L"校园RPG冒险游戏",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        100, 100, 540, 600, nullptr, nullptr, hInst, nullptr);
    if(!g_hWnd){ MessageBoxW(nullptr, L"窗口创建失败！", L"错误", MB_OK); return 1; }

    ShowWindow(g_hWnd, SW_SHOWNORMAL); UpdateWindow(g_hWnd);
    Refresh();

    MSG msg;
    while(GetMessage(&msg,nullptr,0,0)){
        if(!IsDialogMessage(g_hWnd,&msg)){ TranslateMessage(&msg); DispatchMessage(&msg); }
    }

    if (g_player.getExp() > 0) SaveManager::savePlayerData(g_player);
    stopAutoSave();
    return 0;
}



