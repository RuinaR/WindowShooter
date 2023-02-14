#include "winControl.h"

 HDC hdc_main;
 PAINTSTRUCT ps_main;

//상점 관련 변수
const RECT btnHP = { 10,850,180,880 };
const RECT btnCBTime = { 10,890,180,920 };
const RECT btnLight = { 10,930,180,960 };
const POINT pointPos = { 10, 970 };
const int price_hp = 100;
const int price_CBTime = 150;
const int price_light = 200;
//더블버퍼링용 변수
HDC hdcBuff_main;
HBITMAP hBmpBuff_main;
HBITMAP hBmpBuffOld_main;
RECT clientRect_main = { 0 };
//
HBRUSH brush_main;

void Init_main(HWND hWnd)
{
	WindowCount++;
	boom.isActive = false;
	isOver = false;
	GetClientRect(hWnd, &clientRect_main);
	my.pos.x = clientRect_main.right / 2;
	my.pos.y = clientRect_main.bottom - 100;
	my.r = myR;
	myHpMax = 5;
	myHpCur = myHpMax;
	bulletLV_CUR = 0;
	//
	targetPos.x = clientRect_main.right / 2;
	targetPos.y = clientRect_main.top;
	point = 0;
	//
	SetTimer(hWnd, TimerUpdate, TimerUpdateUnit, NULL);
	SetTimer(hWnd, TimerCreateB, TimerCreateBUnit, NULL);
	SetTimer(hWnd, TimerMoveB, TimerMoveBUnit, NULL);
	SetTimer(hWnd, TimerCreateE, TimerCreateEUnit, NULL);
	SetTimer(hWnd, TimerMoveE, TimerMoveEUnit, NULL);
	SetTimer(hWnd, TimerColl, TimerCollUnit, NULL);

}

void LBtnDown_main(LPARAM lParam, HWND hWnd)
{
	if (isOver)
	{
		hWndTitle = MakeWindow(g_hInst, &titleRect, WndProcTitle, TEXT("타이틀"), BLACK_BRUSH, SW_SHOW);
		DestroyWindow(hWndMain);
		hWndMain = NULL;	
	}
	if (InRect(&btnHP, LOWORD(lParam), HIWORD(lParam)))
	{
		if (myHpCur + 1 <= myHpMax && point >= price_hp)
		{
			myHpCur++;
			point -= price_hp;
		}
	}
	if (InRect(&btnCBTime, LOWORD(lParam), HIWORD(lParam)))
	{
		if (bulletLV_CUR + 1 <= bulletLV_MAX && point >= price_CBTime)
		{
			bulletLV_CUR++;
			point -= price_CBTime;
			KillTimer(hWndMain, TimerCreateB);
			SetTimer(hWndMain, TimerCreateB, TimerCreateBUnit - (bulletLV_CUR * 70), NULL);
		}
	}
	if (InRect(&btnLight, LOWORD(lParam), HIWORD(lParam)))
	{
		if ((!hWndLight) && point >= price_light)
		{
			point -= price_light;
			hWndLight = MakeWindow(g_hInst, &lightRect, WndProcLight, TEXT("빛!!!"), WHITE_BRUSH, SW_SHOW, hWnd);
		}
	}
	if (hWndLight != NULL)
	{
		SetActiveWindow(hWndLight);
	}
	if (hWndTarget != NULL)
	{
		SetActiveWindow(hWndTarget);
	}
}

void Timer_Update_main(HWND hWnd)
{
	score += scoreUnit;
	if (isOver)
	{
		KillTimer(hWnd, TimerUpdate);
		KillTimer(hWnd, TimerCreateB);
		KillTimer(hWnd, TimerMoveB);
		KillTimer(hWnd, TimerCreateE);
		KillTimer(hWnd, TimerMoveE);
		KillTimer(hWnd, TimerColl);
		KillTimer(hWnd, TimerBoom);

		DestroyWindow(hWndTarget);
		hWndTarget = NULL;
		if (hWndLight)
		{
			DestroyWindow(hWndLight);
			hWndLight = NULL;
		}
		//
		InvalidateRect(hWnd, NULL, false);
		//
	}
	InvalidateRect(hWnd, NULL, false);
}

void Timer_CreateB()
{
	Bullet* newBullet = new Bullet;
	newBullet->circlePos.pos = my.pos;
	newBullet->circlePos.r = bulletR;
	newBullet->dir.x = targetPos.x - my.pos.x;
	newBullet->dir.y = targetPos.y - my.pos.y;
	VectorNormalize(&(newBullet->dir));

	listBullet.push_back(newBullet);

	//효과음
	mciSendCommand(openBgm[(int)SFX::SHOOT].wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID) &(SeekBgm[(int)SFX::SHOOT]));
	mciSendCommand(openBgm[(int)SFX::SHOOT].wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&(playBgm[(int)SFX::SHOOT]));
}

void Timer_MoveB()
{
	list<Bullet*>::iterator iter = listBullet.begin();
	while (iter != listBullet.end())
	{
		(*iter)->circlePos.pos.x += (*iter)->dir.x * bulletSpeed;
		(*iter)->circlePos.pos.y += (*iter)->dir.y * bulletSpeed;

		//화면 밖 충돌
		if ((*iter)->circlePos.pos.x <= -200 ||
			(*iter)->circlePos.pos.x >= clientRect_main.right + 200 ||
			(*iter)->circlePos.pos.y <= -200 ||
			(*iter)->circlePos.pos.y >= clientRect_main.bottom + 200)
		{
			delete (*iter);
			list<Bullet*>::iterator deleteIter = iter;
			iter++;
			listBullet.erase(deleteIter);
		}
		else
		{
			iter++;
		}
	}
}

void Timer_CreateE()
{
	Enemy* newEnemy = new Enemy;
	newEnemy->circlePos.pos.x = rand() % screenWidth;
	newEnemy->circlePos.pos.y = clientRect_main.top;
	newEnemy->circlePos.r = 50 + (rand() % 50);
	newEnemy->dir.x = my.pos.x - newEnemy->circlePos.pos.x;
	newEnemy->dir.y = my.pos.y - newEnemy->circlePos.pos.y;
	newEnemy->hpMax = enemyHpMax;
	newEnemy->hpCur = newEnemy->hpMax;
	VectorNormalize(&(newEnemy->dir));
	listEnemy.push_back(newEnemy);
}

void Timer_MoveE()
{
	list<Enemy*>::iterator iter = listEnemy.begin();
	while (iter != listEnemy.end())
	{
		(*iter)->circlePos.pos.x += (*iter)->dir.x * enemySpeed;
		(*iter)->circlePos.pos.y += (*iter)->dir.y * enemySpeed;
		//나와 충돌
		if (InCircle((*iter)->circlePos.pos.x, (*iter)->circlePos.pos.y, my.pos.x, my.pos.y, (*iter)->circlePos.r + my.r))
		{
			delete (*iter);
			list<Enemy*>::iterator deleteIter = iter;
			iter++;
			listEnemy.erase(deleteIter);

			//효과음
			mciSendCommand(openBgm[(int)SFX::HEATED].wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID) & (SeekBgm[(int)SFX::HEATED]));
			mciSendCommand(openBgm[(int)SFX::HEATED].wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&(playBgm[(int)SFX::HEATED]));
			//추가처리
			myHpCur--;
			if (myHpCur <= 0)	isOver = true;
		}
		//화면 밖 충돌 (안전장치)
		else if ((*iter)->circlePos.pos.x <= -500 ||
			(*iter)->circlePos.pos.x >= screenWidth + 500 ||
			(*iter)->circlePos.pos.y <= -500 ||
			(*iter)->circlePos.pos.x >= screenHeight + 500)
		{
			delete (*iter);
			list<Enemy*>::iterator deleteIter = iter;
			iter++;
			listEnemy.erase(deleteIter);
		}
		else
		{
			iter++;
		}
	}
}

void Timer_Coll(HWND hWnd)
{
	list<Bullet*>::iterator iterB = listBullet.begin();
	list<Enemy*>::iterator iterE;
	bool isDeleteE = false;
	bool isDeleteB = false;
	while (iterB != listBullet.end())
	{
		isDeleteE = false;
		isDeleteB = false;

		iterE = listEnemy.begin();
		while (iterE != listEnemy.end())
		{
			//enemy - bullet 충돌
			if (InCircle((*iterB)->circlePos.pos.x, (*iterB)->circlePos.pos.y,
				(*iterE)->circlePos.pos.x, (*iterE)->circlePos.pos.y,
				(*iterB)->circlePos.r + (*iterE)->circlePos.r))
			{
				delete (*iterB);
				list<Bullet*>::iterator deleteIterB = iterB;
				iterB++;
				listBullet.erase(deleteIterB);
				isDeleteB = true;

				(*iterE)->hpCur--;
				if ((*iterE)->hpCur <= 0)
				{
					point += pointUnit;
					//			
					KillTimer(hWnd, TimerBoom);
					boom.pos = (*iterE)->circlePos.pos;
					boom.r = (*iterE)->circlePos.r;
					boom.isActive = true;
					SetTimer(hWnd, TimerBoom, 300, NULL);
					mciSendCommand(openBgm[(int)SFX::BOOM].wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID) & (SeekBgm[(int)SFX::BOOM]));
					mciSendCommand(openBgm[(int)SFX::BOOM].wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID) & (playBgm[(int)SFX::BOOM]));
					//
					delete (*iterE);
					list<Enemy*>::iterator deleteIterE = iterE;
					iterE++;
					listEnemy.erase(deleteIterE);
					isDeleteE = true;
				}
				else
				{
					mciSendCommand(openBgm[(int)SFX::ATTACK].wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID) & (SeekBgm[(int)SFX::ATTACK]));
					mciSendCommand(openBgm[(int)SFX::ATTACK].wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID) & (playBgm[(int)SFX::ATTACK]));
				}
				break;
			}
			if (!isDeleteE)	iterE++;
		}
		if (!isDeleteB)	iterB++;
	}
}

void Paint_main(HWND hWnd)
{
	hdc_main = BeginPaint(hWnd, &ps_main);
	
	//더블 버퍼링용 DC, BMP
	hdcBuff_main = CreateCompatibleDC(hdc_main);
	hBmpBuff_main = CreateCompatibleBitmap(hdc_main, clientRect_main.right, clientRect_main.bottom);
	hBmpBuffOld_main = (HBITMAP)SelectObject(hdcBuff_main, hBmpBuff_main);
	PatBlt(hdcBuff_main, 0, 0, clientRect_main.right, clientRect_main.bottom, BLACKNESS);

	hFont = CreateFontW(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
		VARIABLE_PITCH | FF_ROMAN, TEXT("휴먼엑스포"));
	oldFont = (HFONT)SelectObject(hdcBuff_main, hFont);
	SetBkMode(hdcBuff_main, TRANSPARENT);
	//버퍼에 그리기
	//enemy
	brush_main = CreateSolidBrush(enemyColor_dark);
	SelectObject(hdcBuff_main, brush_main);

	for (list<Enemy*>::iterator iter = listEnemy.begin(); iter != listEnemy.end(); iter++)
	{
		Ellipse(hdcBuff_main, (*iter)->circlePos.pos.x - (*iter)->circlePos.r, (*iter)->circlePos.pos.y - (*iter)->circlePos.r,
			(*iter)->circlePos.pos.x + (*iter)->circlePos.r, (*iter)->circlePos.pos.y + (*iter)->circlePos.r);
	}

	DeleteObject(brush_main);
	//bullet
	for (list<Bullet*>::iterator iter = listBullet.begin(); iter != listBullet.end(); iter++)
	{
		DrawBitmap(hdcBuff_main, (*iter)->circlePos.pos.x - (*iter)->circlePos.r, (*iter)->circlePos.pos.y - (*iter)->circlePos.r,
			(*iter)->circlePos.r * 2, (*iter)->circlePos.r * 2, hBitBullet, whiteColor);
	}
	//my
	DrawBitmap(hdcBuff_main, my.pos.x - my.r, my.pos.y - my.r, my.r * 2, my.r * 2, hBitMy, whiteColor);
	//myhp
	//HpMax
	brush_main= CreateSolidBrush(RGB(45, 45, 45));
	SelectObject(hdcBuff_main, brush_main);
	Rectangle(hdcBuff_main, my.pos.x - my.r, my.pos.y - (my.r / 5.0) + 50,
		my.pos.x + my.r, (my.pos.y + my.r / 5.0) + 50);
	DeleteObject(brush_main);
	//HpCur
	brush_main = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdcBuff_main, brush_main);
	Rectangle(hdcBuff_main,
		my.pos.x - my.r,
		my.pos.y - my.r / 5.0 + 50,
		my.pos.x - my.r + (my.r * 2.0) * ((double)myHpCur / (double)myHpMax),
		(my.pos.y + my.r / 5.0) + 50);
	DeleteObject(brush_main);
	//상점
	{
		TCHAR str[20];
		HBITMAP rect;
		if (point >= price_hp)
			rect = hBitMenu_1;
		else
			rect = hBitMenu_2;
		DrawBitmap(hdcBuff_main, btnHP.left, btnHP.top, btnHP.right - btnHP.left, btnHP.bottom - btnHP.top, rect, whiteColor);
		wsprintf(str, TEXT("HP구매: %dp"), price_hp);
		TextOut(hdcBuff_main, btnHP.left + 10, (btnHP.top + btnHP.bottom) / 2 - 10, str, lstrlen(str));

		if (point >= price_CBTime && bulletLV_CUR + 1 <= bulletLV_MAX)
			rect = hBitMenu_1;
		else
			rect = hBitMenu_2;
		DrawBitmap(hdcBuff_main, btnCBTime.left, btnCBTime.top, btnCBTime.right - btnCBTime.left, btnCBTime.bottom - btnCBTime.top, rect, whiteColor);
		if (bulletLV_CUR + 1 <= bulletLV_MAX)	wsprintf(str, TEXT("연사속도UP: %dp"), price_CBTime);
		else wsprintf(str, TEXT("LV MAX"));
		TextOut(hdcBuff_main, btnCBTime.left + 10, (btnCBTime.top + btnCBTime.bottom) / 2 - 10, str, lstrlen(str));

		if (point >= price_light && !hWndLight)
			rect = hBitMenu_1;
		else
			rect = hBitMenu_2;
		DrawBitmap(hdcBuff_main, btnLight.left, btnLight.top, btnLight.right - btnLight.left, btnLight.bottom - btnLight.top, rect, whiteColor);
		if (!hWndLight) wsprintf(str, TEXT("빛 구매: %dp"), price_light);
		else wsprintf(str, TEXT("Sold out"));
		TextOut(hdcBuff_main, btnLight.left + 10, (btnLight.top + btnLight.bottom) / 2 - 10, str, lstrlen(str));

		SetTextColor(hdcBuff_main, RGB(255, 255, 255));
		wsprintf(str, TEXT("POINT: %d"), point);
		TextOut(hdcBuff_main, pointPos.x, pointPos.y, str, lstrlen(str));
	}
	// UI
	TCHAR str[50] = { 0 };
	wsprintf(str, TEXT("현재 적 수: %d"), listEnemy.size());
	TextOut(hdcBuff_main, pointPos.x + 300, pointPos.y, str, lstrlen(str));
	wsprintf(str, TEXT("SCORE: %d"), score);
	TextOut(hdcBuff_main, pointPos.x + 125, pointPos.y, str, lstrlen(str));

	if (isOver)
	{
		DrawBitmap(hdcBuff_main, 400,50, clientRect_main.right - 800, clientRect_main.bottom -150 , hBitGameOver, RGB(0, 0, 0));
		wsprintf(str, TEXT("* 아무키나 입력하세요 *"), score);
		TextOut(hdcBuff_main, clientRect_main.right / 2 - 100, clientRect_main.bottom / 2 + 200, str, lstrlen(str));
	}
	SetBkMode(hdcBuff_main, 0);
	SelectObject(hdcBuff_main, oldFont);
	DeleteObject(hFont);

	//더블 버퍼링 쓰기
	BitBlt(hdc_main, 0, 0, clientRect_main.right, clientRect_main.bottom, hdcBuff_main, 0, 0, SRCCOPY);
	SelectObject(hdcBuff_main, hBmpBuffOld_main);
	//delete
	DeleteObject(hBmpBuff_main);
	DeleteDC(hdcBuff_main);

	EndPaint(hWnd, &ps_main);
}

//임시
MCI_PLAY_PARMS playB;
MCI_OPEN_PARMS openB;
MCI_SEEK_PARMS SeekB;

LRESULT CALLBACK WndProcMain(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_CREATE:
	{
		Init_main(hWnd);
		//
		openBgm[(int)SFX::SHOOT].lpstrDeviceType = TEXT("waveaudio");
		openBgm[(int)SFX::SHOOT].lpstrElementName = TEXT("SHOOT.wav");
		mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)(&(openBgm[(int)SFX::SHOOT])));

		openBgm[(int)SFX::HEATED].lpstrDeviceType = TEXT("waveaudio");
		openBgm[(int)SFX::HEATED].lpstrElementName = TEXT("HEATED.wav");
		mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)(&(openBgm[(int)SFX::HEATED])));

		openBgm[(int)SFX::BOOM].lpstrDeviceType = TEXT("waveaudio");
		openBgm[(int)SFX::BOOM].lpstrElementName = TEXT("boom.wav");
		mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)(&(openBgm[(int)SFX::BOOM])));

		openBgm[(int)SFX::ATTACK].lpstrDeviceType = TEXT("waveaudio");
		openBgm[(int)SFX::ATTACK].lpstrElementName = TEXT("ATTACK.wav");
		mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)(&(openBgm[(int)SFX::ATTACK])));
		//
		PlaySound(TEXT("BGM.wav"), NULL, SND_ASYNC | SND_LOOP);
		//
		break;
	}

	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = screenWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = screenHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = screenWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = screenHeight;
		break;
	}
	case WM_LBUTTONDOWN:
		LBtnDown_main(lParam, hWnd);
		break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case TimerUpdate:
		{
			Timer_Update_main(hWnd);
			break;
		}
		case TimerCreateB:
		{
			Timer_CreateB();
			break;
		}
		case TimerMoveB:
		{
			Timer_MoveB();
			break;
		}
		case TimerCreateE:
		{
			Timer_CreateE();
			break;
		}
		case TimerMoveE:
		{
			Timer_MoveE();
			break;
		}
		case TimerColl:
		{
			Timer_Coll(hWnd);
			break;
		}
		case TimerBoom:
		{
			boom.isActive = false;
			KillTimer(hWnd, TimerBoom);
			break;
		}
		}
	}
	break;
	case WM_PAINT:
	{
		Paint_main(hWnd);
		break;
	}
	case WM_DESTROY:
	{
		PlaySound(NULL, NULL, 0);
		mciSendCommand(openBgm[(int)SFX::SHOOT].wDeviceID, MCI_CLOSE, 0, NULL);
		mciSendCommand(openBgm[(int)SFX::HEATED].wDeviceID, MCI_CLOSE, 0, NULL);
		mciSendCommand(openBgm[(int)SFX::BOOM].wDeviceID, MCI_CLOSE, 0, NULL);
		mciSendCommand(openBgm[(int)SFX::ATTACK].wDeviceID, MCI_CLOSE, 0, NULL);

		for (list<Bullet*>::iterator iter = listBullet.begin(); iter != listBullet.end(); iter++)
		{
			delete (*iter);
		}
		listBullet.clear();
		for (list<Enemy*>::iterator iter = listEnemy.begin(); iter != listEnemy.end(); iter++)
		{
			delete (*iter);
		}
		listEnemy.clear();

		KillTimer(hWnd, TimerUpdate);
		KillTimer(hWnd, TimerCreateB);
		KillTimer(hWnd, TimerMoveB);
		KillTimer(hWnd, TimerCreateE);
		KillTimer(hWnd, TimerMoveE);
		KillTimer(hWnd, TimerColl);
		KillTimer(hWnd, TimerBoom);

		hWndMain = NULL;

		WindowCount--;
		if (WindowCount <= 0)
		{
			End();
			PostQuitMessage(0);
		}
		break;
	}
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}