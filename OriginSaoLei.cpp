/*
���д˴���ǰ������ر�֤�Թ���Ա������У������޷�׼ȷ��λ���λ�á�
Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.

����Author: Haoyu Wang
*/

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0) //����ĳ�����ݵ��ź�

int X0, Y0, x_click, y_click;
int dif, judge = -1, flag = 1, bomb_num = 0;

int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0}, dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
char ord;
POINT p;
HWND h = GetConsoleWindow(); // ����̨���ڵľ��
HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); // ��׼������
HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE); // ��׼������
CONSOLE_FONT_INFO consoleCurrentFont; // ���ڴ洢�����С
DWORD mode;

void UserDesign() {
	/*
	�û�����Ѷ�
	*/
	SetConsoleTitle(u8"ɨ�ף�");

	printf(u8"***���д˴���ǰ������ر�֤�Թ���Ա������У������޷�׼ȷ��λ���λ�á�***\n");
	printf(u8"***Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.***\n\n\n");

	printf(u8"***��ӭʹ��ɨ��-v-��Authored by Haoyu Wang***\n");
	system("pause");
	system("cls");
sign1:
	printf(u8"�������ͼ������(��ΧΪ8~32)��\n");
	scanf(u8"%d", &X0);
	if (!(X0 >= 8 && X0 <= 32)) {
		system("cls");
		printf(u8"��⵽����������������룺\n");
		goto sign1;
	}
	system("cls");
sign2:
	printf(u8"�������ͼ������(��ΧΪ8~32)��\n");
	scanf(u8"%d", &Y0);
	if (!(Y0 >= 8 && Y0 <= 32)) {
		system(u8"cls");
		printf(u8"��⵽����������������룺\n");
		goto sign2;
	}
	system("cls");
sign3:
	printf(u8"��������Ҫ���е��Ѷ�(��ΧΪ1~50���Ƽ���ΧΪ3~25)��\n");
	scanf(u8"%d", &dif);
	if (!(dif >= 1 && dif <= 50)) {
		system("cls");
		printf(u8"��⵽����������������룺\n");
		goto sign3;
	}
	system("mode con cols=90 lines=45");
	system("cls");
}

int map[35][35] = {0}, st[35][35] = {0};

void FormMap() {
	/*
	��������γɵ�ͼ
	*/
	for (int i = 0; i < X0; i++)
		for (int j = 0; j < Y0; j++) {
			st[i][j] = rand() % 100;
			if (st[i][j] < dif) {
				st[i][j] = 1;
				bomb_num++;
			} else
				st[i][j] = 0;
		}
}

void UserInput() {
	/*
	�û����в���
	*/
	printf(u8"\n�淨��\n��������̽�����򣻵���Ҽ����������\n");
	GetConsoleMode(hStdin, &mode);
	GetCurrentConsoleFont(hOutPut, FALSE, &consoleCurrentFont);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hStdin, mode);
	while (1) {
		if (KEY_DOWN(VK_LBUTTON)) { // ���������
			GetCursorPos(&p);
			// printf(u8"���棺x=%ld, y=%ld", p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1, p.y / consoleCurrentFont.dwFontSize.Y - 1);
			ScreenToClient(h, &p);
			// printf(u8"����̨��x=%ld, y=%ld", p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1, p.y / consoleCurrentFont.dwFontSize.Y - 1);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'E';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		if (KEY_DOWN(VK_RBUTTON)) { // �������Ҽ�
			GetCursorPos(&p);
			ScreenToClient(h, &p);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'S';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		Sleep(100); //ΪCPU������������ͣ100����
	}
	if (flag) {
		if (st[x_click][y_click])
			bomb_num--;
		st[x_click][y_click] = 0;
		flag = 0;
	}
}

int CheckAround(int x, int y) {
	/*
	���ĳ������Χ�Ƿ��е��ף����ڵݹ�
	*/
	int sum = 0, tx, ty;
	for (int k = 0; k < 8; k++) {
		tx = x + dx[k];
		ty = y + dy[k];
		if (tx >= 0 && tx < X0 && ty >= 0 && ty < Y0)
			sum += st[x + dx[k]][y + dy[k]];
	}
	return sum;
}

void ExploreMap(int x, int y) {
	/*
	����Ա����ף���ֹͣ�����û���ף�����չ
	*/
	int warn = CheckAround(x, y), tx, ty;
	if (warn != 0) {
		map[x][y] = warn;
		return;
	}
	map[x][y] = -1;
	for (int k = 0; k < 8; k++) {
		tx = x + dx[k];
		ty = y + dy[k];
		if (tx >= 0 && tx < X0 && ty >= 0 && ty < Y0 && map[tx][ty] == 0)
			ExploreMap(tx, ty);
	}
}

int CheckWin() {
	/*
	����û��Ƿ�ʤ��
	*/
	for (int i = 0; i < X0; i++)
		for (int j = 0; j < Y0; j++) {
			if ((map[i][j] == 0 || map[i][j] == -2) && st[i][j] == 0)
				return 0;
		}
	return 1;
}

void DealWithMap() {
	/*
	�����û��Ĳ��������ͼ
	*/
	if (st[x_click][y_click] == 1 && ord == 'E') { //����û��ȵ����ף��и�
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (st[i][j] == 1)
					map[i][j] = -3;
			} //��������������
		judge = 0; //�и�
		return;
	}
	if (ord == 'S') { //����û���ǣ��Ͱ������λ���������
		if (map[x_click][y_click] == 0)
			map[x_click][y_click] = -2;
		return;
	} else if (ord == 'E') //����û�������ʹӵ����λ�ý���̽��
		ExploreMap(x_click, y_click);
	if (CheckWin()) { //����û�Ӯ�ˣ�������Ӯ��
		judge = 1;
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (map[i][j] == 0)
					map[i][j] = -2;
			} //���������Ա����ʽ������
	}
}

void PrintMap() {
	/*
	�����ͼ
	*/
	int i, j;
	printf("  ");
	for (j = 0; j < Y0; j++)
		printf(u8"%2d", j + 1);
	printf(u8" ��\n");
	for (i = 0; i < X0; i++) {
		printf(u8"%2d", i + 1);
		for (j = 0; j < Y0; j++)//��ͼ�ϵĲ�ͬ����Ӧ��ͬ��״
			switch(map[i][j]){
				case 0: 
					printf(u8"��");
					break;
				case -1: 
					printf(u8"  ");
					break;
				case -2: 
					printf(u8"��");
					break;
				case -3: 
					printf(u8" X");
					break;
				default: 
					printf(u8"%2d", map[i][j]);
			}
		printf("\n");
	}
	printf(u8"��\n");
	if (flag == 0)
		printf(u8"����������%d\n", bomb_num);
}

void ClearMap() {
	/*
	�����Ļ���������
	*/
	system("cls");
}

int main() {
	/*
	������
	*/
	// printf("h=%ld\n", h);
	srand(time(0));

	UserDesign();
	FormMap();
	PrintMap();

	while (1) {
		UserInput();
		DealWithMap();
		ClearMap();
		PrintMap();
		if (judge == 1) {
			printf(u8"***��ϲ������Ӯ�ˣ�***");
			system("color 0A");
			system("pause");
			break;
		} else if (judge == 0) {
			printf(u8"***���ź��������ˣ�***");
			system("color 0C");
			system("pause");
			break;
		}
	}
	
	return 0;
}