#include <iostream>
#include <Windows.h>
using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;	// dimensions based on gameboy version
int nFieldHeight = 18;
unsigned char *pField = nullptr;
int nScreenWidth = 80;	// Console screen dimensions
int nScreenHeight = 30;


// use math functions to get pieces off of a grid
// each case is for each rotation
//
//  |  0  1  2  3
// -|-------------
// 0|  0  1  2  3
// 1|  4  5  6  7
// 2|  8  9 10 11
// 3| 12 13 14 15

int rotate(int px, int py, int r)
{
	switch (r % 4) 
	{
	case 0: return py * 4 + px;			// 0 degrees
	case 1: return 12 + py - (px * 4);	// 90 degress
	case 2: return 15 - (py * 4) - px;	// 180 degrees
	case 3: return 3 - py + (px * 4);	// 270 degrees
	}
	return 0;
}

int main()
{
	// Create assets
	// Pieces
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");


	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");


	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	// Playing field
	pField = new unsigned char[nFieldWidth * nFieldHeight]; // Array size = width * height of the board

	for(int x = 0; x < nFieldWidth; x++)		// Create play field
		for(int y = 0; y < nFieldHeight; y++)	// Board boundary
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
	// 9 = border space | 0 = empty space

	// Information for the screen printing
	// Admittedly I don't really understand how this works. Never worked with consoles like this before
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Display Frame
	WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	bool bGameOver = false;

	while (!bGameOver)
	{
		// Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		//Display frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}


