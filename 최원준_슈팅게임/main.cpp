#define MAIN
#include "winControl.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	MSG Message;
	Init(hInstance, nCmdShow);
	//�޼��� ������ ����
	while (GetMessage(&Message, NULL, 0, 0)) {			//Queue�� �ִ� �޼����� �о���δ�
		TranslateMessage(&Message);								//Ű���� �Է� �޼����� �����Ͽ� ���α׷����� ���� 
																				//  ����� �� �ֵ���
		DispatchMessage(&Message);								//�޼����� �������� �޼��� ó�� �Լ� WndProc�� ����
	}

	return (int)Message.wParam;							//Ż�� �ڵ�. ���α׷� ����
}

