#pragma once
#include <d3d11_1.h>
#include "SimpleMath.h"
#include <string>
#include <vector>
#include <memory>
#ifdef _DEBUG
#define _XGF_DEBUG_ALLOC
#endif
//freetype 使用静态库时定义此宏 动态库请自行编译
#define XGF_USE_FREETYPE_STATIC

#ifdef _XGF_DEBUG_ALLOC
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#endif
#define  DEPRECATED(str) __declspec(deprecated(str))

namespace XGF
{
	typedef std::wstring string;
	typedef unsigned int Index;

	namespace SM = DirectX::SimpleMath;

	class Point : public SM::Vector3
	{
	public:
		Point() noexcept : Vector3() {}
		constexpr explicit Point(float x) : SM::Vector3(x) {}
		constexpr Point(float _x, float _y, float _z) : SM::Vector3(_x, _y, _z) {}
		explicit Point(_In_reads_(3) const float *pArray) : SM::Vector3(pArray) {}
		Point(DirectX::FXMVECTOR V) :SM::Vector3(V){  }
		Point(const XMFLOAT3& V): SM::Vector3(V){}
		explicit Point(const DirectX::XMVECTORF32& F): SM::Vector3(F){}

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
		Point4() noexcept : Vector4() {}
		constexpr explicit Point4(float x) : SM::Vector4(x) {}
		constexpr Point4(float _x, float _y, float _z, float _w) : SM::Vector4(_x, _y, _z, _w) {}
		explicit Point4(_In_reads_(4) const float *pArray) : SM::Vector4(pArray) {}
		Point4(DirectX::FXMVECTOR V) :SM::Vector4(V) {  }
		Point4(const DirectX::XMFLOAT4& V) : SM::Vector4(V) {}
		explicit Point4(const DirectX::XMVECTORF32& F) : SM::Vector4(F) {}

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
		Point2() noexcept : Vector2() {}
		constexpr explicit Point2(float x) : SM::Vector2(x) {}
		constexpr Point2(float _x, float _y) : SM::Vector2(_x, _y) {}
		explicit Point2(_In_reads_(2) const float *pArray) : SM::Vector2(pArray) {}
		Point2(DirectX::FXMVECTOR V) :SM::Vector2(V) {  }
		Point2(const DirectX::XMFLOAT2& V) : SM::Vector2(V) {}
		explicit Point2(const DirectX::XMVECTORF32& F) : SM::Vector2(F) {}

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
