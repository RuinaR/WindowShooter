#include "winControl.h"
#include <time.h>

void Init(HINSTANCE hInstance, int nCmdShow)
{
	srand((unsigned int)time(NULL));

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	hBitMy = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
	hBitEnemy = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	hBitBullet = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	hBitBg = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	hBitMenu_1 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
	hBitMenu_2 = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	hBitBoom = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
	hBitGameOver = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP8));

	hWndTitle = MakeWindow(hInstance, &titleRect, WndProcTitle, TEXT("Ÿ��Ʋ"), WHITE_BRUSH, nCmdShow);	
}

void ClientToClient(HWND hWndOrg, HWND hWndTarget, POS* pPos)
{
	POINT point = { pPos->x, pPos->y };
	ClientToScreen(hWndOrg, &point);
	ScreenToClient(hWndTarget, &point);
	pPos->x = point.x;
	pPos->y = point.y;
}

double LengthPts(int x1, int y1, int x2, int y2)
{
	return(sqrt((float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))));
}

BOOL InCircle(int x, int y, int mx, int my, int r)
{
	if (LengthPts(x, y, mx, my) < r)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL InRect(const RECT* rect, int mx, int my)
{
	if (rect->left < mx &&
		rect->right > mx &&
		rect->top < my &&
		rect->bottom > my)
	{
		return TRUE;
	}
	return FALSE;
}

void VectorNormalize(POS* vector)
{
	double d = sqrt((vector->x * vector->x) + (vector->y * vector->y));
	vector->x = vector->x / d;
	vector->y = vector->y / d;
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hbit)
{
	HDC MemoryDC;
	int BitmapX, BitmapY;
	BITMAP Bitmap_;
	HBITMAP OldBitmap;
	MemoryDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemoryDC, hbit);
	GetObject(hbit, sizeof(BITMAP), &Bitmap_);
	BitmapX = Bitmap_.bmWidth;
	BitmapY = Bitmap_.bmHeight;
	BitBlt(hdc, x, y, BitmapX, BitmapY, MemoryDC, 0, 0, SRCCOPY);
	SelectObject(MemoryDC, OldBitmap);
	DeleteDC(MemoryDC);
}

void DrawBitmap(HDC hdc, int x, int y, int w, int h, HBITMAP hbit, UINT color)
{
	HDC MemoryDC;
	int BitmapX, BitmapY;
	BITMAP Bitmap_;
	HBITMAP OldBitmap;
	MemoryDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemoryDC, hbit);
	GetObject(hbit, sizeof(BITMAP), &Bitmap_);
	BitmapX = Bitmap_.bmWidth;
	BitmapY = Bitmap_.bmHeight;
	TransparentBlt(hdc, x, y, w, h, MemoryDC, 0, 0, BitmapX, BitmapY, color);
	SelectObject(MemoryDC, OldBitmap);
	DeleteDC(MemoryDC);
}

void End(void)
{
	DeleteObject(hBitMy);
	DeleteObject(hBitEnemy);
	DeleteObject(hBitBg);
	DeleteObject(hBitBullet);
	DeleteObject(hBitMenu_1);
	DeleteObject(hBitMenu_2);
	DeleteObject(hBitBoom);
	DeleteObject(hBitGameOver);
}

BOOL SetNoIntersect(const LPRECT pHold, LPRECT pRect)
{
	RECT rcInter = { 0 };
	if (IntersectRect(&rcInter, pHold, pRect))
	{
		int nW = rcInter.right - rcInter.left;
		int nH = rcInter.bottom - rcInter.top;

		if (nW > nH)
		{
			if (rcInter.top == pHold->top)
			{
				pRect->top -= nH;
				pRect->bottom -= nH;
			}
			else if (rcInter.bottom == pHold->bottom)
			{
				pRect->top += nH;
				pRect->bottom += nH;
			}
		}
		else
		{
			if (rcInter.left == pHold->left)
			{
				pRect->left -= nW;
				pRect->right -= nW;
			}
			else if (rcInter.right == pHold->right)
			{
				pRect->left += nW;
				pRect->right += nW;
			}
		}
		return TRUE;
	}
	return FALSE;
}

HWND MakeWindow(HINSTANCE hInstance, const RECT* rect, WNDPROC winproc, LPCTSTR name, int BackColor, int nCmdShow)
{										//������ �ڵ� ����
	MSG Message;										//�޼��� ����ü ���� ����
	WNDCLASS WndClass;									//Windows Class ����ü ���� ����
	g_hInst = hInstance;								//hInstance���� �ܺο����� ����� �� �ֵ��� ���������� ���� ����

	WndClass.cbClsExtra = 0;							//���� ����. ������ ���X
	WndClass.cbWndExtra = 0;							//���� ����
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BackColor);	// �������� ��� ������ ����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//�������� ���콺������ ����� ����
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//�������� Ŀ�� ������ ��� ����
	WndClass.hInstance = hInstance;						//������ Ŭ������ ����ϴ� ���α׷� ��ȣ
	WndClass.lpfnWndProc = winproc;						//������ �޼��� ó�� �Լ� ����
	WndClass.lpszClassName = name;					//������ Ŭ������ �̸� ����
	WndClass.lpszMenuName = NULL;						//�޴� ����
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;			//�������� ��Ÿ���� ����

	RegisterClass(&WndClass);							//WNDCLASS ����ü�� ������ ����

	HWND hWnd = CreateWindow(name, name,			//�����츦 ����
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		rect->left, rect->top, rect->right, rect->bottom, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}

HWND MakeWindow(HINSTANCE hInstance, const RECT* rect, WNDPROC winproc, LPCTSTR name, int BackColor, int nCmdShow, HWND hP)
{										//������ �ڵ� ����
	MSG Message;										//�޼��� ����ü ���� ����
	WNDCLASS WndClass;									//Windows Class ����ü ���� ����
	g_hInst = hInstance;								//hInstance���� �ܺο����� ����� �� �ֵ��� ���������� ���� ����

	WndClass.cbClsExtra = 0;							//���� ����. ������ ���X
	WndClass.cbWndExtra = 0;							//���� ����
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BackColor);	// �������� ��� ������ ����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//�������� ���콺������ ����� ����
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//�������� Ŀ�� ������ ��� ����
	WndClass.hInstance = hInstance;						//������ Ŭ������ ����ϴ� ���α׷� ��ȣ
	WndClass.lpfnWndProc = winproc;						//������ �޼��� ó�� �Լ� ����
	WndClass.lpszClassName = name;					//������ Ŭ������ �̸� ����
	WndClass.lpszMenuName = NULL;						//�޴� ����
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;			//�������� ��Ÿ���� ����

	RegisterClass(&WndClass);							//WNDCLASS ����ü�� ������ ����

	HWND hWnd = CreateWindow(name, name,			//�����츦 ����
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		rect->left, rect->top, rect->right, rect->bottom, hP, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}


