#pragma once
#include <vector>

struct R_Rect
{
	float left;
	float top;
	float width;
	float height;
};
class RenderTarget
{
public:
	RenderTarget() {};
	~RenderTarget(){};
	void SetRect(int x, int y, int w, int h) { mRect.left = x;mRect.top = y; mRect.width = w;mRect.height = h; };
	virtual void OnSize(int x, int y) {};
	const R_Rect * GetRect()
	{
		return &mRect;
	}
private:
	R_Rect mRect;
};
class RenderTargetList
{
public:
	RenderTargetList() : mPosition(0) {};
	void Reset() { mPosition = 0; }
	RenderTarget * Next() { mPosition++; return mTargets[mPosition];
	}
	bool HasNext() { return mTargets.size() < mPosition; }
	RenderTarget * Get(int num) { return mTargets[num]; };
	bool IsOne() {
		return mTargets.size() == 1;
	}
	void Add(RenderTarget * rt) { mTargets.push_back(rt); }
private:
	std::vector<RenderTarget * > mTargets;
	int mPosition;
};
