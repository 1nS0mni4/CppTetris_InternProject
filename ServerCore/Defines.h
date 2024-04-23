#pragma once

#define SINGLETON(x) private:\
	x();\
	x(const x& ref) = delete;\
	x(x&&) = delete;\
	x& operator=(const x& ref) = delete;\
	x& operator=(const x&& otr) = delete;\
	~x();\
\
public:\
	static x& GetInstance() {\
		static x s;\
		return s;\
	}