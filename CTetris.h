#pragma once
#include <windows.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include "utils.h"


class CTetris {
public:
	void Play();
	int GetScore() const;
	CTetris();
	~CTetris();
private:
	static constexpr int m_nScreenWidth{ 80 };
	static constexpr int m_nScreenHeight{ 30 };
	static constexpr int m_nFieldWidth{ 12 };
	static constexpr int m_nFieldHeight{ 18 };
	int m_score{};
	HANDLE m_hConsole{ INVALID_HANDLE_VALUE };
	static constexpr std::array<wchar_t, 11> m_aCharacters = { L" ABCDEFG=#" };
	const std::array<std::wstring, 7> m_tetrominos = {
		L"..X...X...X...X.",
		L"..X..XX...X.....",
		L".....XX..XX.....",
		L"..X..XX..X......",
		L".X...XX...X.....",
		L".X...X...XX.....",
		L"..X...X..XX....."
	};
	std::array<wchar_t, m_nScreenWidth* m_nScreenHeight > m_aScreen{};// Screen Buffer
	std::array<char, m_nFieldWidth* m_nFieldHeight> m_aField{};// Play Field buffer
	std::vector<int>m_lines{};

	void InitPlayField();
	void ProcessCompletedLines();
	void DrawFieldToScreen();
	void DrawCurrentPieceToScreen(int nCurrentPiece, int nCurrentRotation, int nCurrentX, int nCurrentY);
	int RotateCell(int px, int py, int rotation)const;
	bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY);
	void CheckForCompletedLines(int nCurrentY);
	void CloseScreenHandle();
};
