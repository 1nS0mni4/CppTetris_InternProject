﻿#ifdef _DEBUG
#pragma comment(lib, "/Debug/ServerCore.lib")
#else
#pragma comment(lib, "/Release/ServerCore.lib")
#endif
#include "CorePch.h"
#include "Defines.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"
#include "ServerSession.h"
#include "Packet.h"

#include <Windows.h>
#include <chrono>

void CreateAssets();
int Rotate(int pieceX, int pieceY, int degree);
void DrawField(wchar_t* screen);
bool DoesPieceIft(int pieceId, int rotation, int poxX, int posY);
void ClearField(unsigned char* Field, unsigned char* otherField);

using namespace std;

wstring Pieces[7];

int FieldWidth = 12, FieldHeight = 18;
int ScreenWidth = 120, ScreenHeight = 30;
unsigned char Field[FIELD_LEN];
bool Keys[4];
extern int myWin = 0, myLose = 0, maxScore = 0;

atomic<bool> isLogined = false;
atomic<bool> isRunning = false;

extern unsigned char otherField[FIELD_LEN] = "";
extern wchar_t otherName[NAME_LEN] = L"";
extern int otherCurX = 0;
extern int otherCurY = 0;
extern int otherRotation = 0;
extern int otherCurPiece = 0;
extern int otherScore = 0;

CtS_NotifyFieldPacket* fieldNoti;
CtS_NotifyCurrentPiecePacket* pieceNoti;
CtS_NotifyScorePacket* scoreNoti;

int wmain() {
	CreateAssets();

	ClearField(Field, otherField);

	int screenBufferSize = ScreenWidth * ScreenHeight;

	wchar_t* screen = new wchar_t[screenBufferSize];

	for (int i = 0; i < screenBufferSize; i++)
		screen[i] = L' ';

	HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, NULL, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(consoleHandle);

	DWORD bytesWritten = 0;

	int currentPiece = 0;
	int currentRotation = 0;
	int currentX = (FieldWidth / 2) - 2;
	int currentY = 0;

	bool rotateButtonHeld = false;

	int gameSpeed = 20;
	int speedCount = 0;
	bool forceDown = false;
	vector<int> lines;
	int score = 0;

	CtS_LoginRequestPacket* packet = new CtS_LoginRequestPacket();

	memcpy(packet->name, L"Hell", 10);

	SessionManager<ServerSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
	if (session == nullptr)
		return -1;

	session->Send(packet);

	while (isLogined == false) {
		if (isLogined)
			break;
	}

	while (true) {

		while (isRunning) {
			// Game Timing
			this_thread::sleep_for(50ms);
			speedCount++;
			forceDown = (speedCount == gameSpeed);

			// Input
			for (size_t keyIndex = 0; keyIndex < 4; keyIndex++) {
				// Right arrow virtual key code // Left arrow virtual key code // Down arrow virtual key code // Z virtual key code key
				Keys[keyIndex] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[keyIndex]))) != 0;
			}

			// LOGIC

			// Can move right?
			currentX += (Keys[0] && DoesPieceIft(currentPiece, currentRotation, currentX + 1, currentY));

			// Can move left?
			currentX -= (Keys[1] && DoesPieceIft(currentPiece, currentRotation, currentX - 1, currentY));

			// Can move down?
			currentY += Keys[2] && DoesPieceIft(currentPiece, currentRotation, currentX, currentY + 1);

			if (Keys[3]) {
				currentRotation += !rotateButtonHeld && DoesPieceIft(currentPiece, currentRotation + 1, currentX, currentY) ? 1 : 0;
				rotateButtonHeld = true;
			}
			else {
				rotateButtonHeld = false;
			}

			if (forceDown) {
				if (DoesPieceIft(currentPiece, currentRotation, currentX, currentY + 1))
					currentY++;
				else {
					// Lock the current piece into the field,
					for (size_t pieceX = 0; pieceX < 4; pieceX++)
						for (size_t pieceY = 0; pieceY < 4; pieceY++)
							if (Pieces[currentPiece][Rotate(pieceX, pieceY, currentRotation)] == L'X')
								Field[(currentY + pieceY) * FieldWidth + (currentX + pieceX)] = currentPiece + 1;

					// Check have we created full horizontal line
					for (size_t pieceY = 0; pieceY < 4; pieceY++)
						if (currentY + pieceY < FieldHeight - 1) {
							bool isLine = true;
							for (size_t pieceX = 1; pieceX < FieldWidth - 1; pieceX++)
								isLine &= (Field[(currentY + pieceY) * FieldWidth + pieceX]) != 0;

							if (isLine) {
								// Set line to equal symbol
								for (size_t pieceX = 0; pieceX < FieldWidth; pieceX++)
									Field[(currentY + pieceY) * FieldWidth + pieceX] = 8;

								lines.push_back(currentY + pieceY);
							}
						}

					score += 25;

					if ((!lines.empty()))
						score += (1 << lines.size()) * 100;

					// choose the next piece
					currentPiece = 0;
					currentRotation = 0;
					currentX = FieldWidth / 2;
					currentY = 0;
					currentPiece = rand() % 7;

					// If we cant fit next piece then its game over
					isRunning = DoesPieceIft(currentPiece, currentRotation, currentX, currentY);
				}

				speedCount = 0;
			}

			// Draw Field
			DrawField(screen);

			// Draw current piece
			{
				for (size_t pieceX = 0; pieceX < 4; pieceX++) {
					for (size_t pieceY = 0; pieceY < 4; pieceY++) {
						if (Pieces[currentPiece][Rotate(pieceX, pieceY, currentRotation)] == L'X') {
							screen[(currentY + pieceY + 2) * ScreenWidth + (currentX + pieceX + 2)] = currentPiece + 65;
						}
					}
				}

				for (size_t pieceX = 0; pieceX < 4; pieceX++) {
					for (size_t pieceY = 0; pieceY < 4; pieceY++) {
						if (Pieces[otherCurPiece][Rotate(pieceX, pieceY, otherRotation)] == L'X') {
							screen[(otherCurY + pieceY + 2) * ScreenWidth + (FieldWidth + otherCurX + pieceX + 8)] = otherCurPiece + 65;
						}
					}
				}
			}


			// Draw score
			swprintf_s(&screen[21 * ScreenWidth + 2], sizeof(L"YOU") / 2, L"YOU");
			swprintf_s(&screen[22 * ScreenWidth + 2], 16, L"Score: %8d", score);
			swprintf_s(&screen[21 * ScreenWidth + FieldWidth + 7], NAME_LEN, L"%s", otherName);
			swprintf_s(&screen[22 * ScreenWidth + FieldWidth + 8], 16, L"Score: %8d", otherScore);

			{
				pieceNoti = new CtS_NotifyCurrentPiecePacket();
				scoreNoti = new CtS_NotifyScorePacket();
				fieldNoti = new CtS_NotifyFieldPacket();

				pieceNoti->currentPiece = currentPiece;
				pieceNoti->rotation = currentRotation;
				pieceNoti->currentX = currentX;
				pieceNoti->currentY = currentY;

				scoreNoti->score = score;

				memcpy(fieldNoti->field, Field, FIELD_LEN);
			}
			{
				session->Send(fieldNoti);
				session->Send(scoreNoti);
				session->Send(pieceNoti);
			}

			// Draw finish lines
			if (!lines.empty()) {
				// Display frame
				WriteConsoleOutputCharacter(consoleHandle, screen, screenBufferSize, { 0, 0 }, &bytesWritten);
				//this_thread::sleep_for(400ms);

				for (auto& line : lines) {
					for (size_t pieceX = 1; pieceX < FieldWidth - 1; pieceX++) {
						for (size_t pieceY = line; pieceY > 0; pieceY--)
							Field[pieceY * FieldWidth + pieceX] = Field[(pieceY - 1) * FieldWidth + pieceX];

						Field[pieceX] = 0;
					}
				}

				lines.clear();
			}

			WriteConsoleOutputCharacter(consoleHandle, screen, screenBufferSize, { 0, 0 }, &bytesWritten);
		}
	}

	

	CloseHandle(consoleHandle);

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	packetFetchLoop.join();

	cout << "Game over!! :(, your score: " << score << endl;

	return 0;
}

void DrawField(wchar_t* screen) {
	for (int x = 0; x < FieldWidth; x++)
		for (int y = 0; y < FieldHeight; y++) {
			screen[(y + 2) * ScreenWidth + (x + 2)] = L" ABCDEFG=#"[Field[y * FieldWidth + x]];
			screen[(y + 2) * ScreenWidth + (FieldWidth + x + 8)] = L" ABCDEFG=#"[otherField[y * FieldWidth + x]];
		}
			
}

bool DoesPieceIft(int pieceId, int rotation, int posX, int posY) {
	for (size_t x = 0; x < 4; x++)
		for (size_t y = 0; y < 4; y++) {
			// Get piece index
			int pieceIndex = Rotate(x, y, rotation);

			// Get index into field
			int fieldIndex = (posY + y) * FieldWidth + (posX + x);

			if (posX + x >= 0 && posX + x < FieldWidth) {
				if (posY + y >= 0 && posY + y < FieldHeight) {
					if (Pieces[pieceId][pieceIndex] == L'X' && Field[fieldIndex] != 0) {
						return false;
					}
				}
			}
		}

	return true;
}

void CreateAssets() {
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");

	Pieces[1].append(L".XX.");
	Pieces[1].append(L"..X.");
	Pieces[1].append(L"..X.");
	Pieces[1].append(L"....");

	Pieces[2].append(L".XX.");
	Pieces[2].append(L".X..");
	Pieces[2].append(L".X..");
	Pieces[2].append(L"....");

	Pieces[3].append(L"....");
	Pieces[3].append(L".XX.");
	Pieces[3].append(L".XX.");
	Pieces[3].append(L"....");

	Pieces[4].append(L".X..");
	Pieces[4].append(L".XX.");
	Pieces[4].append(L".X..");
	Pieces[4].append(L"....");

	Pieces[5].append(L".X..");
	Pieces[5].append(L".XX.");
	Pieces[5].append(L"..X.");
	Pieces[5].append(L"....");

	Pieces[6].append(L"..X.");
	Pieces[6].append(L".XX.");
	Pieces[6].append(L".X..");
	Pieces[6].append(L"....");
}

int Rotate(int pieceX, int pieceY, int degree) {
	switch (degree % 4) {
		case 0: return pieceY * 4 + pieceX;				// 0 Degrees
		case 1: return 12 + pieceY - (4 * pieceX);		// 90 Degrees
		case 2: return 15 - (pieceY * 4) - pieceX;		// 180 Degrees
		case 3: return 3 - pieceY + (4 * pieceX);		// 270 Degrees
		default: return 0;
	}
}

void ClearField(unsigned char* Field, unsigned char* otherField) {

	for (int x = 0; x < FieldWidth; x++)
		for (int y = 0; y < FieldHeight; y++) {
			bool isLeftWall = x == 0;
			bool isRightWall = x == (FieldWidth - 1);
			bool isBottomWall = y == (FieldHeight - 1);
			int value = (isLeftWall || isRightWall || isBottomWall) ? 9 : 0;
			Field[y * FieldWidth + x] = value;
			otherField[y * FieldWidth + x] = value;
		}
}