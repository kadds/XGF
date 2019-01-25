#pragma once
#ifdef _DEBUG
#define _XGF_DEBUG_ALLOC
#endif

#ifdef _XGF_DEBUG_ALLOC
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#endif

#include <d3d11_1.h>
#include "SimpleMath.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#define  DEPRECATED(str) __declspec(deprecated(str))

namespace XGF
{
	typedef std::wstring string;
	typedef unsigned int Index;

	namespace SM = ::DirectX::SimpleMath;


	class Rectangle: public SM::Rectangle
	{
	public:
		using SM::Rectangle::Rectangle;
		Rectangle() = default;
		static Rectangle Intersect(const Rectangle& ra, const Rectangle& rb)
		{
			return Rectangle(SM::Rectangle::Intersect(ra, rb));
		}
		static Rectangle Union(const Rectangle& ra, const Rectangle& rb)
		{
			return Rectangle(SM::Rectangle::Union(ra, rb));
		}
	};


	class Point : public SM::Vector3
	{
	public:
		using SM::Vector3::Vector3;
		Point() = default;
		Point(const Point&) = default;
		Point& operator=(const Point&) = default;

		Point(Point&&) = default;
		Point& operator=(Point&&) = default;

		void Set(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};
	class Point4 : public SM::Vector4
	{
	public:
		using SM::Vector4::Vector4;
		Point4() = default;
		Point4(const Point & p) :SM::Vector4(p.x, p.y, p.z, 1.0) {  }
		Point4(const Point4&) = default;
		Point4& operator=(const Point4&) = default;

		Point4(Point4&&) = default;
		Point4& operator=(Point4&&) = default;

		void Set(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};

	class Point2 : public SM::Vector2
	{
	public:
		using SM::Vector2::Vector2;
		Point2() = default;
		Point2(const Point2&) = default;
		Point2& operator=(const Point2&) = default;

		Point2(Point2&&) = default;
		Point2& operator=(Point2&&) = default;

		void Set(float x, float y)
		{
			this->x = x;
			this->y = y;
		}
	};
	typedef Point2 Position;

	class Color : public SM::Color
	{
	public:
		using SM::Color::Color;
		Color() = default;

		Color(const Color&) = default;
		Color& operator=(const Color&) = default;

		Color(Color&&) = default;
		Color& operator=(Color&&) = default;

		void Set(float r, float g, float b, float a)
		{
			this->x = r;
			this->y = g;
			this->z = b;
			this->w = a;
		}
	};

	class Color3 : public SM::Vector3
	{
	public:
		
		using SM::Vector3::Vector3;

		Color3(const Color3&) = default;
		Color3& operator=(const Color3&) = default;

		Color3(Color3&&) = default;
		Color3& operator=(Color3&&) = default;
		Color3& operator+=(Color & c)
		{
			this->x += c.x;
			this->y += c.y;
			this->z += c.z;
			return *this;
		}
		void Set(float r, float g, float b)
		{
			this->x = r;
			this->y = g;
			this->z = b;
		}
	};

	struct WVPMatrix
	{
		SM::Matrix worldMatrix;
		SM::Matrix viewMatrix;
		SM::Matrix projMatrix;
		void Transpose()
		{
			worldMatrix.Transpose();
			viewMatrix.Transpose();
			projMatrix.Transpose();
		}
	};
}

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

#ifdef _DEBUG
#define XGF_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#else
#define XGF_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (assert(_CRT_WIDE(#expression)), 0) \
        )
#endif
