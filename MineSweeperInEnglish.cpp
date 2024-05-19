/*
1.Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.

2.This code is encoded by UTF-8. After opening it by VSCode, be sure to check if the encoding type is consistent.

——Author: Haoyu Wang
*/

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0) //signal transmitted by pressing a certain key

int X0, Y0, x_click, y_click;
int dif, judge = -1, flag = 1, bomb_num = 0;

int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0}, dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
char ord;
POINT p;
HWND h = GetConsoleWindow(); // handle of console window
HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); // handle of standard input
HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE); // handle of standard output
CONSOLE_FONT_INFO consoleCurrentFont; // to store the fontsize
DWORD mode;

VOID ManagerRun(LPCSTR exe,LPCSTR param,INT nShow=SW_SHOW){
	/*
	Obtain administrator privileges. This code block is borrowed from https://blog.csdn.net/cjz2005/article/details/104513305
	*/
 SHELLEXECUTEINFO ShExecInfo; 
 ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);  
 ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;  
 ShExecInfo.hwnd = NULL;  
 ShExecInfo.lpVerb = "runas";  
 ShExecInfo.lpFile = exe; 
 ShExecInfo.lpParameters = param;   
 ShExecInfo.lpDirectory = NULL;  
 ShExecInfo.nShow = nShow;  
 ShExecInfo.hInstApp = NULL;   
 ShellExecuteEx(&ShExecInfo);  
 CloseHandle(ShExecInfo.hProcess);
 return;
}

void UserDesign() {
	/*
	用户设计难度
	*/
	SetConsoleTitle(u8"MineSweeping!");

	printf(u8"\n***Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.***\n\n");

	printf(u8"*** Welcome to MineSweeping!! ^v^        [Authored by Haoyu Wang] ***\n");
	system("pause");
	system("cls");
sign1:
	printf(u8"Please enter how many rows of the map you want to set (range: 8~32):\n");
	scanf(u8"%d", &X0);
	if (!(X0 >= 8 && X0 <= 32)) {
		system("cls");
		printf(u8"Input error detected, please re-enter:\n");
		goto sign1;
	}
	system("cls");
sign2:
	printf(u8"Please enter how many columns of the map you want to set (range: 8~32):\n");
	scanf(u8"%d", &Y0);
	if (!(Y0 >= 8 && Y0 <= 32)) {
		system(u8"cls");
		printf(u8"Input error detected, please re-enter:\n");
		goto sign2;
	}
	system("cls");
sign3:
	printf(u8"Please enter the difficulty level you want to perform (range: 1~50; recommended range: 3~25):\n");
	scanf(u8"%d", &dif);
	if (!(dif >= 1 && dif <= 50)) {
		system("cls");
		printf(u8"Input error detected, please re-enter:\n");
		goto sign3;
	}
	system("mode con cols=90 lines=45");
	system("cls");
}

int map[35][35] = {0}, st[35][35] = {0};

void FormMap() {
	/*
	Create the origin map by random numbers.
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
	user operation
	*/
	printf(u8"\nGameplay:\nPress the left button to explore a certain point, and press the right to mark a certain point.\n");
	GetConsoleMode(hStdin, &mode);
	GetCurrentConsoleFont(hOutPut, FALSE, &consoleCurrentFont);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hStdin, mode);
	while (1) {
		if (KEY_DOWN(VK_LBUTTON)) { // when pressing the left button
			GetCursorPos(&p);
			ScreenToClient(h, &p);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'E';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		if (KEY_DOWN(VK_RBUTTON)) { // when pressing the right button
			GetCursorPos(&p);
			ScreenToClient(h, &p);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'S';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		Sleep(100); //Reduce CPU load, pause program for 100 milliseconds.
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
	Detect whether there are landmines nearby. Used for recursion.
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
	If there's a landmine nearby, then stop. If not, continue to explore.
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
	Check whether the user has won.
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
	Deal with the map according to the operation of user.
	*/
	if (st[x_click][y_click] == 1 && ord == 'E') { //If the user steps on a mine, it will be judged as lost.
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (st[i][j] == 1)
					map[i][j] = -3;
			} //show all the landmines in X form
		judge = 0; //lose
		return;
	}
	if (ord == 'S') { //If marked by the user, replace the square with a triangle.
		if (map[x_click][y_click] == 0)
			map[x_click][y_click] = -2;
		return;
	} else if (ord == 'E') //If clicked, explore from the location of the click.
		ExploreMap(x_click, y_click);
	if (CheckWin()) { //If the user wins, then it wins.
		judge = 1;
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (map[i][j] == 0)
					map[i][j] = -2;
			} //show all the landmines in triangle form
	}
}

void PrintMap() {
	/*
	Print the current map.
	*/
	int i, j;
	printf("  ");
	for (j = 0; j < Y0; j++)
		printf(u8"%2d", j + 1);
	printf(u8" Columns\n");
	for (i = 0; i < X0; i++) {
		printf(u8"%2d", i + 1);
		for (j = 0; j < Y0; j++)//different shapes for different kinds of points
			switch(map[i][j]){
				case 0: 
					printf(u8"■");
					break;
				case -1: 
					printf(u8"  ");
					break;
				case -2: 
					printf(u8"▲");
					break;
				case -3: 
					printf(u8" X");
					break;
				default: 
					printf(u8"%2d", map[i][j]);
			}
		printf("\n");
	}
	printf(u8"Rows\n");
	if (flag == 0)
		printf(u8"Number of landmines: %d\n", bomb_num);
}

void ClearMap() {
	/*
	Clear all outputs on the console.
	*/
	system("cls");
}

int main(int argc,char *argv[]) {
	/*
	main function
	*/

	SetConsoleOutputCP(65001);
	srand(time(0));

    if(argc == 1){
       ShowWindow(GetConsoleWindow(),SW_HIDE);
       ManagerRun(argv[0],"2");
       return 1;
    }else if(argc == 2){
		UserDesign();
		FormMap();
		PrintMap();

		while (1) {
			UserInput();
			DealWithMap();
			ClearMap();
			PrintMap();
			if (judge == 1) {
				printf(u8"***Congratulations, you won! ***");
				system("color 0A");
				system("pause");
				break;
			} else if (judge == 0) {
				printf(u8"***Unfortunately, you lost! ***");
				system("color 0C");
				system("pause");
				break;
			}
		}
    }
	return 0;
}
