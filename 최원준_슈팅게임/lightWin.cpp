#include "winControl.h"

HDC hdc_light;
PAINTSTRUCT ps_light;
//더블버퍼링용 변수
HDC hdcBuff_light;
HBITMAP hBmpBuff_light;
HBITMAP hBmpBuffOld_light;
RECT clientRect_light = { 0 };

//좌표계 변환용 변수
POS tmpPos = { 0 };
POINT screenBgPos_light;
//
HBRUSH brush_light;
HPEN pen_light;
HPEN oldPen_light;

void Init_light(HWND hWnd)
{
	WindowCount++;
	GetClientRect(hWnd, &clientRect_light);
	SetTimer(hWnd, TimerUpdate, TimerUpdateUnit, NULL);
}
void Timer_light(HWND hWnd, WPARAM wParam)
{
	switch (wParam)
	{
	case TimerUpdate:
	{
		InvalidateRect(hWnd, NULL, false);
		break;
	}
	}
}

void Paint_light(HWND hWnd)
{
	hdc_light = BeginPaint(hWnd, &ps_light);

	//더블 버퍼링용 DC, BMP
	hdcBuff_light = CreateCompatibleDC(hdc_light);
	hBmpBuff_light = CreateCompatibleBitmap(hdc_light, clientRect_light.right, clientRect_light.bottom);
	hBmpBuffOld_light = (HBITMAP)SelectObject(hdcBuff_light, hBmpBuff_light);
	PatBlt(hdcBuff_light, 0, 0, clientRect_light.right, clientRect_light.bottom, WHITENESS);

	//버퍼에 그리기
	//배경 
	screenBgPos_light = { 0,0 };
	ScreenToClient(hWnd, &screenBgPos_light);
	DrawBitmap(hdcBuff_light, screenBgPos_light.x, screenBgPos_light.y, screenWidth, screenHeight, hBitBg, whiteColor);
	//enemy
	for (list<Enemy*>::iterator iter = listEnemy.begin(); iter != listEnemy.end(); iter++)
	{
		tmpPos = (*iter)->circlePos.pos;
		ClientToClient(hWndMain, hWnd, &tmpPos);

		//적 개체 그리기
		DrawBitmap(hdcBuff_light, tmpPos.x - (*iter)->circlePos.r, tmpPos.y - (*iter)->circlePos.r,
			(*iter)->circlePos.r * 2, (*iter)->circlePos.r * 2, hBitEnemy, whiteColor);

		//적 HP바 그리기
		//HpMax
		brush_light = CreateSolidBrush(RGB(45, 45, 45));
		SelectObject(hdcBuff_light, brush_light);
		Rectangle(hdcBuff_light, tmpPos.x - (*iter)->circlePos.r, tmpPos.y - 10,
			tmpPos.x + (*iter)->circlePos.r, (tmpPos.y + 10));
		DeleteObject(brush_light);
		//HpCur
		brush_light = CreateSolidBrush(RGB(0, 255, 0));
		SelectObject(hdcBuff_light, brush_light);
		Rectangle(hdcBuff_light,
			tmpPos.x - (*iter)->circlePos.r,
			tmpPos.y -10,
			tmpPos.x - (*iter)->circlePos.r + ((*iter)->circlePos.r * 2.0) * ((double)(*iter)->hpCur / (double)(*iter)->hpMax),
			(tmpPos.y + 10));
		DeleteObject(brush_light);

		//boom
		if (boom.isActive)
		{
			tmpPos = boom.pos;
			ClientToClient(hWndMain, hWndLight, &tmpPos);
			DrawBitmap(hdcBuff_light, tmpPos.x - boom.r, tmpPos.y - boom.r, boom.r * 2, boom.r * 2, hBitEnemy, whiteColor);
			DrawBitmap(hdcBuff_light, tmpPos.x - boom.r * 2, tmpPos.y - boom.r * 2, boom.r * 4, boom.r * 4, hBitBoom, RGB(0, 0, 0));
		}
	}

	//bullet
	for (list<Bullet*>::iterator iter = listBullet.begin(); iter != listBullet.end(); iter++)
	{
		tmpPos = (*iter)->circlePos.pos;
		ClientToClient(hWndMain, hWnd, &tmpPos);
		DrawBitmap(hdcBuff_light, tmpPos.x - (*iter)->circlePos.r, tmpPos.y - (*iter)->circlePos.r,
			(*iter)->circlePos.r * 2, (*iter)->circlePos.r * 2, hBitBullet, whiteColor);
	}
	//my
	tmpPos = my.pos;
	ClientToClient(hWndMain, hWnd, &tmpPos);
	DrawBitmap(hdcBuff_light, tmpPos.x - my.r, tmpPos.y - my.r, my.r * 2, my.r * 2, hBitMy, whiteColor);
	//myhp
	//HpMax
	brush_light = CreateSolidBrush(RGB(45, 45, 45));
	SelectObject(hdcBuff_light, brush_light);
	Rectangle(hdcBuff_light, tmpPos.x - my.r, tmpPos.y - (my.r / 5.0) + 50,
		tmpPos.x + my.r, (tmpPos.y + my.r / 5.0) + 50);
	DeleteObject(brush_light);
	//HpCur
	brush_light = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdcBuff_light, brush_light);
	Rectangle(hdcBuff_light,
		tmpPos.x - my.r,
		tmpPos.y - my.r / 5.0 + 50,
		tmpPos.x - my.r + (my.r * 2.0) * ((double)myHpCur / (double)myHpMax),
		(tmpPos.y + my.r / 5.0) + 50);
	DeleteObject(brush_light);

	//더블 버퍼링 쓰기
	BitBlt(hdc_light, 0, 0, clientRect_light.right, clientRect_light.bottom, hdcBuff_light, 0, 0, SRCCOPY);
	SelectObject(hdcBuff_light, hBmpBuffOld_light);
	//delete
	DeleteObject(hBmpBuff_light);
	DeleteDC(hdcBuff_light);
	EndPaint(hWnd, &ps_light);
}

LRESULT CALLBACK WndProcLight(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_CREATE:
	{
		Init_light(hWnd);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = lightWinSize;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = lightWinSize;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = lightWinSize;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = lightWinSize;
		break;
	}
	case WM_TIMER:
	{
		Timer_light(hWnd, wParam);
		break;
	}
	case WM_PAINT:
	{
		Paint_light(hWnd);
		break;
	}
	case WM_DESTROY:
	{
		KillTimer(hWnd, TimerUpdate);
		hWndLight = NULL;
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
