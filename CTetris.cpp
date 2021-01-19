#include "CTetris.h"

#pragma comment (lib,"User32.lib")

void CTetris::Play() {
	using namespace std::chrono_literals;
	int nCurrentPiece = 0;
	int nCurrentX = m_nFieldWidth / 2;
	int nCurrentY = 1;
	int nCurrentRotation = 1;
	int nSpeed = 20;
	int nSpeedCount = 0;
	bool bRotateHold = true;
	bool bForceDown = false;
	bool bGameOver = false;
	bool bWrongWindowSize = false;

	m_hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(m_hConsole, { m_nScreenWidth,  m_nScreenHeight });
	CONSOLE_SCREEN_BUFFER_INFO sbi{};
	GetConsoleScreenBufferInfo(m_hConsole, &sbi);
	auto width = (sbi.srWindow.Right - sbi.srWindow.Left) + 1;
	auto height = (sbi.srWindow.Bottom - sbi.srWindow.Top) + 1;
	if (width != 80 || height != 30) {
		bGameOver = true;
		bWrongWindowSize = true;
	}

	SetConsoleActiveScreenBuffer(m_hConsole);
	InitPlayField();
	m_score = 0;
	while (!bGameOver) {
		DWORD dwBytesWritten;
		std::this_thread::sleep_for(50ms);
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) nCurrentX -= 1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) nCurrentX += 1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) nCurrentY += 1;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			bGameOver = true; continue;
		}
		if (GetAsyncKeyState(0x5A) & 0x8000) {
			if (bRotateHold)
				if (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) {
					nCurrentRotation++;
					bRotateHold = false;
				}
		}
		else bRotateHold = true;


		if (bForceDown) {
			nSpeedCount = 0;
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))nCurrentY++;
			else {
				// It can't! Lock the piece in place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (m_tetrominos[nCurrentPiece][RotateCell(px, py, nCurrentRotation)] != L'.') {
							auto posY = (nCurrentY + py) * m_nFieldWidth;
							auto posX = nCurrentX + px;
							m_aField[posY + posX] = static_cast<char>(nCurrentPiece) + 1;
						}
				// Check for lines
				CheckForCompletedLines(nCurrentY);
				m_score += 25; //25 for a new piece
				if (!m_lines.empty())m_score += static_cast<int>(m_lines.size()) * 100; // 100 for each line
				// Pick New Piece
				nCurrentX = m_nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}
		DrawFieldToScreen();
		DrawCurrentPieceToScreen(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
		swprintf_s(&m_aScreen[(2 * m_nScreenWidth) + m_nFieldWidth + 16], 16, L"SCORE: %8d", m_score);
		swprintf_s(&m_aScreen[(15 * m_nScreenWidth) + m_nFieldWidth + 16], 19, L"Arrow keys to move");
		swprintf_s(&m_aScreen[(16 * m_nScreenWidth) + m_nFieldWidth + 16], 16, L"Z key to rotate"); 
			ProcessCompletedLines();
		WriteConsoleOutputCharacter(m_hConsole, m_aScreen.data(), m_nScreenWidth * m_nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	CloseScreenHandle();
	if (bWrongWindowSize) {
		std::cout << "To play please set the window's size to 80 * 30\n";
	}
}

int CTetris::GetScore() const {
	return m_score;
}

CTetris::CTetris() {
	m_aScreen.fill(L' ');
}

CTetris::~CTetris() {
	CloseScreenHandle();
}
/*
Clear the playfield and draw bounding lines
*/
void CTetris::InitPlayField() {
	for (int index = 0; index < m_aField.size(); index++) {
		if (
			((index + 1) % m_nFieldWidth) == 0 // Left
			|| (index % m_nFieldWidth) == 0 // Right
			|| (index >= (m_nFieldWidth * m_nFieldHeight) - m_nFieldWidth))	//Bottom
			m_aField[index] = 9;
		else m_aField[index] = 0;
	}
}

void CTetris::CheckForCompletedLines(int nCurrentY) {
	// The tetromino could have potencially completed 4 lines
	for (int line = 0; line < 4; line++) {
		if ((nCurrentY + line) < (m_nFieldHeight - 1)) {
			//Check all cells of this line
			auto start = ((nCurrentY + line) * m_nFieldWidth) + 1;
			auto completeLine = std::all_of(&m_aField[start], &m_aField[start + (m_nFieldWidth - 2)],
				[](char cell) {return cell != 0; });
			if (completeLine) {
				// Mark line as completed
				for (int x = 0; x < m_nFieldWidth - 2; x++)
					m_aField[start + x] = 8;//8='='
				m_lines.push_back(nCurrentY + line);
			}
		}
	}
}

void CTetris::ProcessCompletedLines() {
	using namespace std::chrono_literals;
	if (!m_lines.empty()) {
		std::this_thread::sleep_for(400ms);
		for (auto& line : m_lines) {
			for (int y = line; y > 0; y--) {
				auto begin = ((y - 1) * m_nFieldWidth) + 1;
				auto end = begin + (m_nFieldWidth - 2);
				auto result = (y * m_nFieldWidth) + 1;
				std::copy(&m_aField[begin],
					&m_aField[end],
					&m_aField[result]);
				// Deletes the line's content
				std::fill(&m_aField[begin], &m_aField[end], static_cast<char>(0));
			}
		}
		m_lines.clear();
	}
}

void CTetris::DrawFieldToScreen() {
	// Draw Field from left to right, top to bottom
	const int padding = 2;
	for (int line = 0; line < m_nFieldHeight; line++)
		for (int cell = 0; cell < m_nFieldWidth; cell++) {
			auto start = (line + padding) * m_nScreenWidth;
			auto offset = cell + padding;
			m_aScreen[start + offset] = m_aCharacters[m_aField[(line * m_nFieldWidth) + cell]];
		}
}

void CTetris::DrawCurrentPieceToScreen(int nCurrentPiece, int nCurrentRotation, int nCurrentX, int nCurrentY) {

	// Draw Current Piece
	for (int py = 0; py < 4; py++)
		for (int px = 0; px < 4; px++)
			if (m_tetrominos[nCurrentPiece][RotateCell(px, py, nCurrentRotation)] != L'.') {
				auto posY = nCurrentY + py + 2;
				auto posX = nCurrentX + px + 2;
				auto posBufferScreen = (posY * m_nScreenWidth) + posX;
				m_aScreen[posBufferScreen] = m_aCharacters[nCurrentPiece + 1];
			}
}

int CTetris::RotateCell(int px, int py, int rotation)const
{
	int pi = 0;
	switch (rotation % 4)
	{
	case 0: // 0 degrees			// 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
									//12 13 14 15

	case 1: // 90 degrees			//12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
									//15 11  7  3

	case 2: // 180 degrees			//15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
									// 3  2  1  0

	case 3: // 270 degrees			// 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}

bool CTetris::DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
	for (int py = 0; py < 4; py++) {
		for (int px = 0; px < 4; px++) {
			int pi = RotateCell(px, py, nRotation);
			if (m_tetrominos[nTetromino][pi] == L'.') continue; // If empty then is not necessary further validation
			int fieldIndex = (nPosY + py) * m_nFieldWidth + (nPosX + px);
			// If it's in bounds
			if ((nPosX + px >= 0 && nPosX + px < m_nFieldWidth) &&
				(nPosY + py >= 0 && nPosY + py < m_nFieldHeight)) {
				if (m_aField[fieldIndex] != 0) {
					TRACE(L"Colision:%d\n", fieldIndex);
					return false;
				}
			}
			else {
				TRACE(L"Out of bounds %d,%d\n", nPosX, nPosY);
				return false;
			}
		}
	}
	return true;
}

void CTetris::CloseScreenHandle() {
	if (m_hConsole != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hConsole);
		m_hConsole = INVALID_HANDLE_VALUE;
	}
}