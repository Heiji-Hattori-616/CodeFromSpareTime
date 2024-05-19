/*
1.运行此代码前，请务必保证以管理员身份运行，否则无法准确定位鼠标位置。
1.Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.

2.本代码用UTF-8编码。使用VSCode等打开后，注意检查编码类型是否一致。
2.This code is encoded by UTF-8. After opening it by VSCode, be sure to check if the encoding type is consistent.

——Author: Haoyu Wang
*/

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0) //按下某键传递的信号

int X0, Y0, x_click, y_click;
int dif, judge = -1, flag = 1, bomb_num = 0;

int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0}, dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
char ord;
POINT p;
HWND h = GetConsoleWindow(); // 控制台窗口的句柄
HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); // 标准输入句柄
HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE); // 标准输出句柄
CONSOLE_FONT_INFO consoleCurrentFont; // 用于存储字体大小
DWORD mode;

VOID ManagerRun(LPCSTR exe,LPCSTR param,INT nShow=SW_SHOW){
	/*
	获取管理员权限，本段代码借鉴自 https://blog.csdn.net/cjz2005/article/details/104513305
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
	SetConsoleTitle(u8"扫雷！");

	printf(u8"***运行此代码前，请务必保证以管理员身份运行，否则无法准确定位鼠标位置。***\n");
	printf(u8"***Please make sure you run this program as an administrator, or the position of mouse may NOT be accurately located.***\n\n\n");

	printf(u8"***欢迎使用扫雷-v-！Authored by Haoyu Wang***\n");
	system("pause");
	system("cls");
sign1:
	printf(u8"请输入地图的行数(范围为8~32)：\n");
	scanf(u8"%d", &X0);
	if (!(X0 >= 8 && X0 <= 32)) {
		system("cls");
		printf(u8"检测到输入错误，请重新输入：\n");
		goto sign1;
	}
	system("cls");
sign2:
	printf(u8"请输入地图的列数(范围为8~32)：\n");
	scanf(u8"%d", &Y0);
	if (!(Y0 >= 8 && Y0 <= 32)) {
		system(u8"cls");
		printf(u8"检测到输入错误，请重新输入：\n");
		goto sign2;
	}
	system("cls");
sign3:
	printf(u8"请输入您要进行的难度(范围为1~50，推荐范围为3~25)：\n");
	scanf(u8"%d", &dif);
	if (!(dif >= 1 && dif <= 50)) {
		system("cls");
		printf(u8"检测到输入错误，请重新输入：\n");
		goto sign3;
	}
	system("mode con cols=90 lines=45");
	system("cls");
}

int map[35][35] = {0}, st[35][35] = {0};

void FormMap() {
	/*
	用随机数形成地图
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
	用户进行操作
	*/
	printf(u8"\n玩法：\n点击左键来探索区域；点击右键来标记区域。\n");
	GetConsoleMode(hStdin, &mode);
	GetCurrentConsoleFont(hOutPut, FALSE, &consoleCurrentFont);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hStdin, mode);
	while (1) {
		if (KEY_DOWN(VK_LBUTTON)) { // 当按下左键
			GetCursorPos(&p);
			// printf(u8"桌面：x=%ld, y=%ld", p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1, p.y / consoleCurrentFont.dwFontSize.Y - 1);
			ScreenToClient(h, &p);
			// printf(u8"控制台：x=%ld, y=%ld", p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1, p.y / consoleCurrentFont.dwFontSize.Y - 1);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'E';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		if (KEY_DOWN(VK_RBUTTON)) { // 当按下右键
			GetCursorPos(&p);
			ScreenToClient(h, &p);
			y_click = p.x / (2 * consoleCurrentFont.dwFontSize.X) - 1;
			x_click = p.y / consoleCurrentFont.dwFontSize.Y - 1;
			ord = 'S';
			if (x_click >= 0 && x_click < X0 && y_click >= 0 && y_click < Y0)
				break;
		}
		Sleep(100); //为CPU减负，程序暂停100毫秒
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
	检测某个点周围是否有地雷，用于递归
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
	如果旁边有雷，就停止；如果没有雷，就拓展
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
	检测用户是否胜利
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
	根据用户的操作处理地图
	*/
	if (st[x_click][y_click] == 1 && ord == 'E') { //如果用户踩到了雷，判负
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (st[i][j] == 1)
					map[i][j] = -3;
			} //把所有雷亮出来
		judge = 0; //判负
		return;
	}
	if (ord == 'S') { //如果用户标记，就把正方形换成三角形
		if (map[x_click][y_click] == 0)
			map[x_click][y_click] = -2;
		return;
	} else if (ord == 'E') //如果用户点击，就从点击的位置进行探索
		ExploreMap(x_click, y_click);
	if (CheckWin()) { //如果用户赢了，那他就赢了
		judge = 1;
		for (int i = 0; i < X0; i++)
			for (int j = 0; j < Y0; j++) {
				if (map[i][j] == 0)
					map[i][j] = -2;
			} //把所有雷以标记形式亮出来
	}
}

void PrintMap() {
	/*
	输出地图
	*/
	int i, j;
	printf("  ");
	for (j = 0; j < Y0; j++)
		printf(u8"%2d", j + 1);
	printf(u8" 列\n");
	for (i = 0; i < X0; i++) {
		printf(u8"%2d", i + 1);
		for (j = 0; j < Y0; j++)//地图上的不同格点对应不同形状
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
	printf(u8"行\n");
	if (flag == 0)
		printf(u8"地雷总数：%d\n", bomb_num);
}

void ClearMap() {
	/*
	清除屏幕上所有输出
	*/
	system("cls");
}

int main(int argc,char *argv[]) {
	/*
	主函数
	*/

	SetConsoleOutputCP(65001);
	srand(time(0));

    if(argc == 1){
       ShowWindow(GetConsoleWindow(),SW_HIDE);
       ManagerRun(argv[0],"2");
       return 1;
    }
	else if(argc == 2){
		UserDesign();
		FormMap();
		PrintMap();

		while (1) {
			UserInput();
			DealWithMap();
			ClearMap();
			PrintMap();
			if (judge == 1) {
				printf(u8"***恭喜您，您赢了！***");
				system("color 0A");
				system("pause");
				break;
			} else if (judge == 0) {
				printf(u8"***很遗憾，您输了！***");
				system("color 0C");
				system("pause");
				break;
			}
		}
	}
	return 0;
}
