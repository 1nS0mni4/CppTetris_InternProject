#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <iostream>

#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include "Types.h"
#include "Defines.h"
#include "Network.h"
#include <WinSock2.h>
#include "CommModel.h"

using namespace std;