#pragma once
#ifdef _DEBUG
#define _XGF_DEBUG_ALLOC
#endif

#define XGF_USE_FREETYPE_STATIC

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

	class Color : public SM::Color
	{
	public:
		Color() noexcept : SM::Color(0, 0, 0, 1.f) {}
		XM_CONSTEXPR Color(float _r, float _g, float _b) : SM::Color(_r, _g, _b, 1.f) {}
		XM_CONSTEXPR Color(float _r, float _g, float _b, float _a) : SM::Color(_r, _g, _b, _a) {}
		explicit Color(const DirectX::SimpleMath::Vector3& clr) : SM::Color(clr.x, clr.y, clr.z, 1.f) {}
		explicit Color(const DirectX::SimpleMath::Vector4& clr) : SM::Color(clr.x, clr.y, clr.z, clr.w) {}
		explicit Color(_In_reads_(4) const float *pArray) : SM::Color(pArray) {}
		Color(DirectX::FXMVECTOR V) { XMStoreFloat4(this, V); }
		Color(const XMFLOAT4& c) { this->x = c.x; this->y = c.y; this->z = c.z; this->w = c.w; }
		explicit Color(const DirectX::XMVECTORF32& F) { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

		explicit Color(const DirectX::PackedVector::XMCOLOR& Packed);
		// BGRA Direct3D 9 D3DCOLOR packed color

		explicit Color(const DirectX::PackedVector::XMUBYTEN4& Packed);
		// RGBA XNA Game Studio packed color

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
		Color3() noexcept : Vector3() {}
		constexpr explicit Color3(float x) : SM::Vector3(x) {}
		constexpr Color3(float _r, float _g, float _b) : SM::Vector3(_r, _g, _b) {}
		explicit Color3(const DirectX::SimpleMath::Vector3& clr) : SM::Vector3(clr.x, clr.y, clr.z) {}
		explicit Color3(_In_reads_(3) const float *pArray) : SM::Vector3(pArray) {}
		Color3(DirectX::FXMVECTOR V) :SM::Vector3(V) {  }
		Color3(const XMFLOAT3& V) : SM::Vector3(V) {}
		explicit Color3(const DirectX::XMVECTORF32& F) : SM::Vector3(F) {}

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
