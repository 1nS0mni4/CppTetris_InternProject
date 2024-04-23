#pragma once
template <typename T>
class Packet abstract {
public:
	virtual void Read() = 0;
	virtual T& Write() = 0;
};

