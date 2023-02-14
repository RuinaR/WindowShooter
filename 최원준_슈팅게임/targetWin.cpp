#include "winControl.h"

HDC hdc_tg;
PAINTSTRUCT ps_tg;
//더블버퍼링용 변수
HDC hdcBuff_tg;
HBITMAP hBmpBuff_tg;
HBITMAP hBmpBuffOld_tg;
RECT clientRect_tg = { 0 };

//좌표계 변환용 변수
POS tmpPos_tg = { 0 };
POINT screenBgPos_tg = { 0, 0 };
//
HBRUSH brush_tg;
HPEN pen_tg;
const int targetSize = 20;

void Init_tg(HWND hWnd)
{
	WindowCount++;
	GetClientRect(hWnd, &clientRect_tg);
	SetTimer(hWnd, TimerUpdate, TimerUpdateUnit, NULL);

	hdc_tg = GetDC(hWnd);
	hBmpBuff_tg = CreateCompatibleBitmap(hdc_tg, clientRect_tg.right, clientRect_tg.bottom);
	ReleaseDC(hWnd, hdc_tg);
}

void Paint_tg(HWND hWnd)
{
	hdc_tg = BeginPaint(hWnd, &ps_tg);

	//더블 버퍼링용 DC, BMP
	hdcBuff_tg = CreateCompatibleDC(hdc_tg);
	hBmpBuffOld_tg = (HBITMAP)SelectObject(hdcBuff_tg, hBmpBuff_tg);
	PatBlt(hdcBuff_tg, 0, 0, clientRect_tg.right, clientRect_tg.bottom, WHITENESS);

	//버퍼에 그리기
	//배경 
	screenBgPos_tg = { 0,0 };
	ScreenToClient(hWnd, &screenBgPos_tg);
	DrawBitmap(hdcBuff_tg, screenBgPos_tg.x, screenBgPos_tg.y, screenWidth, screenHeight, hBitBg, whiteColor);
	//enemy
	for (list<Enemy*>::iterator iter = listEnemy.begin(); iter != listEnemy.end(); iter++)
	{
		tmpPos_tg = (*iter)->circlePos.pos;
		ClientToClient(hWndMain, hWnd, &tmpPos_tg);

		//적 개체 그리기
		DrawBitmap(hdcBuff_tg, tmpPos_tg.x - (*iter)->circlePos.r, tmpPos_tg.y - (*iter)->circlePos.r,
			(*iter)->circlePos.r * 2, (*iter)->circlePos.r * 2, hBitEnemy, whiteColor);

		//적 HP바 그리기
		//HpMax
		brush_tg = CreateSolidBrush(RGB(45, 45, 45));
		SelectObject(hdcBuff_tg, brush_tg);
		Rectangle(hdcBuff_tg, tmpPos_tg.x - (*iter)->circlePos.r, tmpPos_tg.y - 10,
			tmpPos_tg.x + (*iter)->circlePos.r, (tmpPos_tg.y + 10));
		DeleteObject(brush_tg);
		//HpCur
		brush_tg = CreateSolidBrush(RGB(0, 255, 0));
		SelectObject(hdcBuff_tg, brush_tg);
		Rectangle(hdcBuff_tg,
			tmpPos_tg.x - (*iter)->circlePos.r,
			tmpPos_tg.y - 10,
			tmpPos_tg.x - (*iter)->circlePos.r + ((*iter)->circlePos.r * 2.0) * ((double)(*iter)->hpCur / (double)(*iter)->hpMax),
			(tmpPos_tg.y + 10));
		DeleteObject(brush_tg);
	}

	//bullet
	for (list<Bullet*>::iterator iter = listBullet.begin(); iter != listBullet.end(); iter++)
	{
		tmpPos_tg = (*iter)->circlePos.pos;
		ClientToClient(hWndMain, hWnd, &tmpPos_tg);
		DrawBitmap(hdcBuff_tg, tmpPos_tg.x - (*iter)->circlePos.r, tmpPos_tg.y - (*iter)->circlePos.r,
			(*iter)->circlePos.r * 2, (*iter)->circlePos.r * 2, hBitBullet, whiteColor);
	}
	//my
	tmpPos_tg = my.pos;
	ClientToClient(hWndMain, hWnd, &tmpPos_tg);
	DrawBitmap(hdcBuff_tg, tmpPos_tg.x - my.r, tmpPos_tg.y - my.r, my.r * 2, my.r * 2, hBitMy, whiteColor);
	//myhp
	//HpMax
	brush_tg = CreateSolidBrush(RGB(45, 45, 45));
	SelectObject(hdcBuff_tg, brush_tg);
	Rectangle(hdcBuff_tg, tmpPos_tg.x - my.r, tmpPos_tg.y - (my.r / 5.0) + 50,
		tmpPos_tg.x + my.r, (tmpPos_tg.y + my.r / 5.0) + 50);
	DeleteObject(brush_tg);
	//HpCur
	brush_tg = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdcBuff_tg, brush_tg);
	Rectangle(hdcBuff_tg,
		tmpPos_tg.x - my.r,
		tmpPos_tg.y - my.r / 5.0 + 50,
		tmpPos_tg.x - my.r + (my.r * 2.0) * ((double)myHpCur / (double)myHpMax),
		(tmpPos_tg.y + my.r / 5.0) + 50);
	DeleteObject(brush_tg);

	//boom
	if (boom.isActive)
	{
		tmpPos_tg = boom.pos;
		ClientToClient(hWndMain, hWndTarget, &tmpPos_tg);
		DrawBitmap(hdcBuff_tg, tmpPos_tg.x - boom.r, tmpPos_tg.y - boom.r, boom.r * 2, boom.r * 2, hBitEnemy, whiteColor);
		DrawBitmap(hdcBuff_tg, tmpPos_tg.x - boom.r * 2, tmpPos_tg.y - boom.r * 2, boom.r * 4, boom.r * 4, hBitBoom, RGB(0, 0, 0));
	}

	//target
	brush_tg = CreateSolidBrush(RGB(255, 0, 0));
	pen_tg = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	SelectObject(hdcBuff_tg, brush_tg);
	SelectObject(hdcBuff_tg, pen_tg);

	Ellipse(hdcBuff_tg, clientRect_tg.right / 2 - targetSize / 2, clientRect_tg.bottom / 2 - targetSize / 2, clientRect_tg.right / 2 + targetSize / 2, clientRect_tg.bottom / 2 + targetSize / 2);
	MoveToEx(hdcBuff_tg, clientRect_tg.right / 2, clientRect_tg.bottom / 2 - targetSize, NULL);
	LineTo(hdcBuff_tg, clientRect_tg.right / 2, clientRect_tg.bottom / 2 + targetSize);
	MoveToEx(hdcBuff_tg, clientRect_tg.right / 2 - targetSize, clientRect_tg.bottom / 2, NULL);
	LineTo(hdcBuff_tg, clientRect_tg.right / 2 + targetSize, clientRect_tg.bottom / 2);

	DeleteObject(brush_tg);
	DeleteObject(pen_tg);

	//더블 버퍼링 쓰기
	BitBlt(hdc_tg, 0, 0, clientRect_tg.right, clientRect_tg.bottom, hdcBuff_tg, 0, 0, SRCCOPY);
	SelectObject(hdcBuff_tg, hBmpBuffOld_tg);
	//delete
	DeleteDC(hdcBuff_tg);
	EndPaint(hWnd, &ps_tg);
}

LRESULT CALLBACK WndProcTarget(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	switch (iMessage)
	{
	case WM_CREATE:
	{
		Init_tg(hWnd);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = targetWinSize;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = targetWinSize;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = targetWinSize;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = targetWinSize;
		break;
	}
	case WM_WINDOWPOSCHANGED:
	{
		targetPos.x = clientRect_tg.right / 2;
		targetPos.y = clientRect_tg.bottom / 2;
		ClientToClient(hWnd, hWndMain, &targetPos);
		break;
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case TimerUpdate:
		{
			InvalidateRect(hWnd, NULL, false);
			break;
		}
		}
		break;
	}
	case WM_KEYDOWN:
		//테스트용
		if (wParam == VK_ESCAPE)
			isOver = true;
		break;
	case WM_PAINT:
	{
		Paint_tg(hWnd);
		break;
	}

	case WM_DESTROY:
	{
		DeleteObject(hBmpBuff_tg);
		KillTimer(hWnd, TimerUpdate);

		hWndTarget = NULL;

		WindowCount--;
		if (WindowCount <= 0)
		{
			End();
			PostQuitMessage(0);
		}
		break;
	}
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}