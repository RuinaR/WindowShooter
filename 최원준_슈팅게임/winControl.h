#pragma once
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "msimg32.lib")
#include <windows.h>		// 윈도우 헤더파일
#include <mmsystem.h>//PlaySound, mciSendCommand
#include <Digitalv.h>//mciSendCommand
#include <math.h>
#include <string.h>		
#include <list>
#include "resource.h"
using namespace std;

#define TimerUpdate 1
#define TimerCreateB 2
#define TimerMoveB 3
#define TimerCreateE 4
#define TimerMoveE 5
#define TimerColl 6
#define TimerBoom 7

typedef struct _POS
{
	double x;
	double y;
}POS;
typedef struct _CirclePos
{
	POS pos;
	int r;
}CirclePos;
typedef struct _Enemy
{
	int hpMax;
	int hpCur;
	CirclePos circlePos;
	POS dir;
}Enemy;
typedef struct _Bullet
{
	CirclePos circlePos;
	POS dir;
}Bullet;
typedef struct BOOM
{
	POS pos;
	int r;
	bool isActive;
};

enum class SFX
{
	SHOOT,
	HEATED,
	BOOM,
	ATTACK,
	MAX
};

#ifdef MAIN
//윈도우 관련
HINSTANCE g_hInst;
int WindowCount = 0;
int screenWidth;
int screenHeight;
//HWND
HWND hWndTitle = NULL;
HWND hWndMain = NULL;
HWND hWndTarget = NULL;
HWND hWndLight = NULL;

RECT titleRect = { 500,200,500,500 };
int lightWinSize = 500;
RECT lightRect = { 0,0,500,500 };
//개체별 초기 크기, 속성
int myR = 50;
int bulletR = 10;

int bulletSpeed = 10;
int enemySpeed = 3;
int enemyHpMax = 3;
int bulletLV_MAX = 5;
int bulletLV_CUR;
CirclePos my;
int myHpMax;
int myHpCur;

int targetWinSize = 500;

COLORREF myColor = RGB(255, 255, 0);
COLORREF enemyColor = RGB(255, 0, 0);
COLORREF enemyColor_dark = RGB(20, 0, 0);
COLORREF bulletColor = RGB(0, 0, 255);
COLORREF whiteColor = RGB(255, 255, 255);
//개체 관리

list<Enemy*>listEnemy;
list<Bullet*>listBullet;

POS targetPos;
bool isOver = false;

BOOM boom;

//mainTimer
int TimerUpdateUnit = 10;
int TimerCreateBUnit = 500;
int TimerMoveBUnit = 10;
int TimerCreateEUnit = 1500;
int TimerMoveEUnit = 10;
int TimerCollUnit = 10;

//포인트, 점수
int point;
int pointUnit = 30;
int score;
int scoreUnit = 1;

//비트맵
HBITMAP hBitMy;
HBITMAP hBitEnemy;
HBITMAP hBitMenu_1;
HBITMAP hBitMenu_2;
HBITMAP hBitBg;
HBITMAP hBitBullet;
HBITMAP hBitBoom;
HBITMAP hBitGameOver;

//폰트
HFONT hFont;
HFONT oldFont;

//사운드
MCI_PLAY_PARMS playBgm[(int)SFX::MAX];
MCI_OPEN_PARMS openBgm[(int)SFX::MAX];
MCI_SEEK_PARMS SeekBgm[(int)SFX::MAX];

#else
//윈도우 관련
extern HINSTANCE g_hInst;
extern int WindowCount;
extern int screenWidth;
extern int screenHeight;
//HWND
extern HWND hWndTitle;
extern HWND hWndMain;
extern HWND hWndTarget;
extern HWND hWndLight;

extern RECT titleRect;
extern int lightWinSize;
extern RECT lightRect;
//개체별 초기 크기, 속성
extern int myR;
extern int bulletR;

extern int bulletSpeed;
extern int enemySpeed;
extern int enemyHpMax;
extern int bulletLV_MAX;
extern int bulletLV_CUR;
extern CirclePos my;
extern int myHpMax;
extern int myHpCur;

extern int targetWinSize;

extern COLORREF myColor;
extern COLORREF enemyColor;
extern COLORREF enemyColor_dark;
extern COLORREF bulletColor;
extern COLORREF whiteColor;
//개체 관리

extern list<Enemy*>listEnemy;
extern list<Bullet*>listBullet;

extern POS targetPos;
extern bool isOver;

extern BOOM boom;

//mainTimer
extern int TimerUpdateUnit;
extern int TimerCreateBUnit;
extern int TimerMoveBUnit;
extern int TimerCreateEUnit;
extern int TimerMoveEUnit;
extern int TimerCollUnit;

//포인트, 점수
extern int point;
extern int pointUnit;
extern int score;
extern int scoreUnit;

//비트맵
extern HBITMAP hBitMy;
extern HBITMAP hBitEnemy;
extern HBITMAP hBitMenu_1;
extern HBITMAP hBitMenu_2;
extern HBITMAP hBitBg;
extern HBITMAP hBitBullet;
extern HBITMAP hBitBoom;
extern HBITMAP hBitGameOver;

//사운드
extern MCI_PLAY_PARMS playBgm[(int)SFX::MAX];
extern MCI_OPEN_PARMS openBgm[(int)SFX::MAX];
extern MCI_SEEK_PARMS SeekBgm[(int)SFX::MAX];

//폰트
extern HFONT hFont;
extern HFONT oldFont;
#endif

void Init(HINSTANCE hInstance, int nCmdShow);
void ClientToClient(HWND hWndOrg, HWND hWndTarget, POS* pPos);
double LengthPts(int x1, int y1, int x2, int y2);
BOOL InCircle(int x, int y, int mx, int my, int r);
BOOL InRect(const RECT* rect, int mx, int my);
void VectorNormalize(POS* vector);

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hbit);
void DrawBitmap(HDC hdc, int x, int y, int w, int h, HBITMAP hbit, UINT color);

void End(void);

BOOL SetNoIntersect(const LPRECT pHold, LPRECT pRect);
HWND MakeWindow(HINSTANCE hInstance, const RECT* rect, WNDPROC winproc, LPCTSTR name, int BackColor, int nCmdShow);
HWND MakeWindow(HINSTANCE hInstance, const RECT* rect, WNDPROC winproc, LPCTSTR name, int BackColor, int nCmdShow, HWND hP);

LRESULT CALLBACK WndProcTitle(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcMain(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcTarget(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcLight(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);