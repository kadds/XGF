#pragma once
#include "Defines.hpp"
#include "Defines.hpp"
class ActionInterface
{
public:
	virtual void OnPositionChange(const Point & p, int ID) = 0;
	virtual void OnAngleChange(const Point & p, int ID) = 0;
	virtual void OnScaleChange(const Point & p, int ID) = 0;
	virtual void OnColorChange(const Color & p, int ID) = 0;
	virtual void OnAlphaChange(float a, int ID) = 0;

	virtual void GetPosition(Point & p, int ID) const = 0;
	virtual void GetAngle(Point & p, int ID) const = 0;
	virtual void GetScale(Point & p, int ID) const = 0;
	virtual void GetColor(Color & p, int ID) const = 0;
	virtual void GetAlpha(float & p, int ID) const = 0;
};