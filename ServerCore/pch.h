#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef _DOMAIN_
#define _DOMAIN_
#define _SERVER_
#define _CLIENT_
#endif

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <thread>
#include <WinSock2.h>

#include <thread>

#include <mutex>
#include <atomic>

using namespace std;