#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "ServerSession.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"
//#include <WinSock2.h>

void ErrorHandling(const char* message);

static ServerSession* session = nullptr;


typedef struct {
	SOCKET hServSock;
	char buf[1024];
	WSABUF wsaBuf;
}PER_IO_DATA, * LPPER_IO_DATA;

void CreateAssets();
int Rotate(int pieceX, int pieceY, int degree);
void DrawField(wchar_t* screen);
bool DoesPieceIft(int pieceId, int rotation, int poxX, int posY);
void ClearField(unsigned char* Field, unsigned char* otherField);

wstring Pieces[7];

int FieldWidth = 12, FieldHeight = 18;
int ScreenWidth = 120, ScreenHeight = 30;
unsigned char Field[FIELD_LEN];
bool Keys[4];
extern int myWin = 0, myLose = 0, maxScore = 0;

extern atomic<bool> isLogined = false;
extern atomic<bool> isRunning = false;

mutex otherMtx;

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

int main(int argc, char* argv[]) {
	cout << "HEllo" << '\n';
	CreateAssets();

	ClearField(Field, otherField);

	int screenBufferSize = ScreenWidth * ScreenHeight;

	wchar_t* screen = new wchar_t[screenBufferSize];

	for (int i = 0; i < screenBufferSize; i++)
		screen[i] = L' ';

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
	atomic<bool> isLose = false;

	SessionManager<ServerSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
	if (session == nullptr)
		return -1;

	session->Recv();

	CtS_LoginRequestPacket* packet = new CtS_LoginRequestPacket();

	session->Send(packet);
	
	while (session->isLogined == false) { SleepEx(10, TRUE); }

	CtS_MatchingRequestPacket* match = new CtS_MatchingRequestPacket();
	session->Send(match);

	HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, NULL, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(consoleHandle);

	while (session->isLogined) {
		SleepEx(10, TRUE);

		while (session->isRunning) {
			int prevScore = score;
			// Game Timing
			SleepEx(1, TRUE);
			this_thread::sleep_for(50ms);
			speedCount++;
			forceDown = (speedCount == gameSpeed);

			//Input
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

					fieldNoti = new CtS_NotifyFieldPacket();
					memcpy(fieldNoti->field, Field, FIELD_LEN);
					session->Send(fieldNoti);

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

					{
						// choose the next piece
						currentPiece = 0;
						currentRotation = 0;
						currentX = FieldWidth / 2;
						currentY = 0;
						currentPiece = rand() % 7;
					}


					// If we cant fit next piece then its game over
					session->isRunning = DoesPieceIft(currentPiece, currentRotation, currentX, currentY);
					if (session->isRunning == false) {
						CtS_NotifyLosePacket* lose = new CtS_NotifyLosePacket();
						session->Send(lose);
					}
				}

				speedCount = 0;
			}


			{
				pieceNoti = new CtS_NotifyCurrentPiecePacket();

				pieceNoti->currentPiece = currentPiece;
				pieceNoti->rotation = currentRotation;
				pieceNoti->currentX = currentX;
				pieceNoti->currentY = currentY;

				session->Send(pieceNoti);
			}
			{
				if (score != prevScore) {
					scoreNoti = new CtS_NotifyScorePacket();
					scoreNoti->score = score;
					session->Send(scoreNoti);
				}
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

				lock_guard<mutex> guard(otherMtx);
				for (size_t pieceX = 0; pieceX < 4; pieceX++) {
					for (size_t pieceY = 0; pieceY < 4; pieceY++) {
						if (Pieces[otherCurPiece][Rotate(pieceX, pieceY, otherRotation)] == L'X') {
							screen[(otherCurY + pieceY + 2) * ScreenWidth + (FieldWidth + otherCurX + pieceX + 8)] = otherCurPiece + 65;
						}
					}
				}
			}

			int test = 22 * ScreenWidth + FieldWidth + 8;
			int test2 = lstrlenW(L"Score: %4d");
			int test3 = lstrlenW(L"YOU");
			int test4 = sizeof(L"YOU");
			// Draw score
			swprintf_s(&screen[21 * ScreenWidth + 2], sizeof(L"YOU") / 2, L"YOU");
			swprintf_s(&screen[22 * ScreenWidth + 2], sizeof(L"Score: %4d"), L"Score: %4d", score);
			{
				lock_guard<mutex> guard(otherMtx);
				swprintf_s(&screen[21 * ScreenWidth + FieldWidth + 7], sizeof(otherName) / 2, L"%s", otherName);
				swprintf_s(&screen[22 * ScreenWidth + FieldWidth + 8], sizeof(L"Score: %4d"), L"Score: %4d", otherScore);
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

	swprintf_s(&screen[15 * ScreenWidth + 4], sizeof(session->isLose ? L"YOU LOSE" : L"YOU WIN") / 2, session->isLose ? L"YOU LOSE" : L"YOU WIN");
	swprintf_s(&screen[15 * ScreenWidth + FieldWidth + 9], sizeof(!session->isLose ? L"YOU LOSE" : L"YOU WIN") / 2, !session->isLose ? L"YOU LOSE" : L"YOU WIN");


	CloseHandle(consoleHandle);

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	packetFetchLoop.join();

	cout << "Game over!! :(, your score: " << score << endl;
	std::cout << "Server Closed" << std::endl;

	return 0;

	{
		/*SessionManager<ServerSession>::GetInstance().Init();
		std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

		if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
			return -1;

		ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
		if (session == nullptr)
			return -1;

		TestPacket* packet;
		int i = 1;

		std::string input;

		while (true) {
			packet = new TestPacket();
			packet->data = i;
			session->Send(packet);
			Sleep(10);
			i++;
		}

		OvlpCallback::GetInstance().Close();
		PacketQueue::GetInstance().Close();

		packetFetchLoop.join();

		std::cout << "Server Closed" << std::endl;

		return 0;*/
	}
}
void ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(-1);
}

void DrawField(wchar_t* screen) {
	lock_guard<mutex> guard(otherMtx);
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
	lock_guard<mutex> guard(otherMtx);
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