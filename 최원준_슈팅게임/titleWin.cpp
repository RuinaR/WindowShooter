#include "winControl.h"

HDC hdc_title;
PAINTSTRUCT ps_title;
RECT endBtnRect = { 100, 140 ,300, 180 };
RECT clientRect_title;

LRESULT CALLBACK WndProcTitle(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_CREATE:
	{
		GetClientRect(hWnd, &clientRect_title);

		endBtnRect.left = clientRect_title.right - 200;
		endBtnRect.right = clientRect_title.right;
		endBtnRect.top = clientRect_title.bottom - 40;
		endBtnRect.bottom = clientRect_title.bottom;

		WindowCount++;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (InRect(&endBtnRect, LOWORD(lParam), HIWORD(lParam)))
		{
			MessageBox(hWnd, TEXT("종료합니다..."), TEXT("종료"), MB_OK);
			DestroyWindow(hWnd);
			break;
		}
		RECT mainRect = { 0, 0, screenWidth, screenHeight };
		RECT TargetRect = { (screenWidth - targetWinSize) / 2, (screenHeight - targetWinSize) / 2, targetWinSize, targetWinSize };
		hWndMain = MakeWindow(g_hInst, &mainRect, WndProcMain, TEXT("메인"), BLACK_BRUSH, SW_SHOW);
		hWndTarget = MakeWindow(g_hInst, &TargetRect, WndProcTarget, TEXT("조준"), WHITE_BRUSH, SW_SHOW, hWndMain);
		DestroyWindow(hWnd);
		hWndTitle = NULL;
		break;
	}
	case WM_PAINT:
	{
		hdc_title = BeginPaint(hWnd, &ps_title);
		SetBkMode(hdc_title, TRANSPARENT);
		DrawBitmap(hdc_title, 0, 0, hBitBg);

		hFont = CreateFontW(60, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("휴먼옛체"));
		oldFont = (HFONT)SelectObject(hdc_title, hFont);

		SetTextColor(hdc_title, RGB(255, 255, 255));
		TextOut(hdc_title, clientRect_title.right / 2 - 200, clientRect_title.bottom / 2 - 150, TEXT("Window Shooter"), lstrlen(TEXT("Window Shooter")));
		hFont = CreateFontW(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("휴먼엑스포"));
		SelectObject(hdc_title, hFont);
		TextOut(hdc_title, clientRect_title.right / 2 - 150, clientRect_title.bottom / 2, TEXT("*시작하려면 아무곳이나 클릭하세요*"), lstrlen(TEXT("*시작하려면 아무곳이나 클릭하세요*")));
		SetTextColor(hdc_title, RGB(0, 0, 0));
		hFont = CreateFontW(30, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("휴먼엑스포"));
		SelectObject(hdc_title, hFont);
		DrawBitmap(hdc_title, endBtnRect.left, endBtnRect.top, endBtnRect.right - endBtnRect.left, endBtnRect.bottom - endBtnRect.top, hBitMenu_1, whiteColor);
		TextOut(hdc_title, endBtnRect.left + 35, (endBtnRect.top + endBtnRect.bottom) / 2 - 15, TEXT("게임 종료"), lstrlen(TEXT("게임 종료")));

		SelectObject(hdc_title, oldFont);
		DeleteObject(hFont);

		SetBkMode(hdc_title, TRANSPARENT);
		EndPaint(hWnd, &ps_title);
		break;
	}
	case WM_DESTROY:
	{
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
