#pragma once
#include "Defines.hpp"
#include <vector>
using std::vector;

class Animation
{
public:
	Animation();
	Animation(int ID) { mID = ID; }
	~Animation();
	bool IsStop() { return mIsStop; }
	void Stop() { mIsStop = true; }
private:
	float passTime;
	bool mIsStop;
	int mID;
};

