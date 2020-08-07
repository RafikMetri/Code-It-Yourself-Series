#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

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

// checks if a piece fits in a certain position
// return true by default, return false in any case where the piece doesn't fit
bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) 
{
	for (int px = 0; px < 4; px++)
		for(int py = 0; py < 4; py++) 
		{
			// get index into piece (pi = piece index)
			int pi = rotate(px, py, nRotation);

			// get index into field (fi = field index)
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosY + py < nFieldHeight)
			{
				if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
					return false;
			}
		}
	
	return true;
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

	// Game logic stuff
	bool bGameOver = false;

	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	int nSpeed = 20;
	int nSpeedCounter = 0;

	int nPieceCount = 0;
	int nScore = 0;
	
	bool bForceDown = false;
	bool bKey[4];
	bool bRotateHold = false;

	vector<int> vLines;

	// GAME LOOP
	while (!bGameOver) 
	{
		// GAME TIMING ===================================
		this_thread::sleep_for(50ms); // game ticks
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// INPUT =========================================
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		// GAME LOGIC ====================================

		// press right, add 1 to x
		nCurrentX += bKey[0] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY) ? 1 : 0;
		// press left substract 1 from x
		nCurrentX -= bKey[1] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY) ? 1 : 0;
		// press down add 1 to y
		nCurrentY += bKey[2] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1) ? 1 : 0;

		// press z, check if button is held, then add to rotation by 1 (if button is already held, don't rotate)
		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY) ? 1 : 0);
			bRotateHold = true;
		}
		else
			bRotateHold = false;

		// forcing a piece down based on game speed
		if (bForceDown)
		{
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++; // if there is room below the piece, move it

			else
			{
				// Lock current piece in the field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
				
				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;

				//Check have we got any lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine) 
						{
							//set to '=' and remove line
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

				// Choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// if piece does not fit, game over man
				bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
			nSpeedCounter = 0; // reset counter
		}

		// RENDER OUTPUT =================================

		// ========================================================
		// The above 4 steps are used in almost every computer game
		// ========================================================

		// Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		// Draw current piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; // add by 65 for correct ASCII character (65 = A), (66 = B), ...

		// Draw score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty())
		{
			// Display Frame (cheekily to draw lines)
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); //delay

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}

		// Display frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	// Oh Dear
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");

	return 0;
}


