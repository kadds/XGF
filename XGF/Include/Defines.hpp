#pragma once
#ifdef _DEBUG
#define _XGF_DEBUG_ALLOC
#define XGF_DEBUG_SHADER
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
		Color3() = default;
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
	struct LightShadowMatrix
	{
		SM::Matrix viewProj;
	};

	// hash helper

	template<typename T>
	inline void hash_combine(std::size_t& seed, const T& val)
	{
		seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline void hash_combine_value(std::size_t& seed, std::size_t v)
	{
		seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename T>
	inline void hash_val(std::size_t& seed, const T& val)
	{
		hash_combine(seed, val);
	}

	template<typename T, typename... Types>
	inline void hash_val(std::size_t& seed, const T& val, const Types&... args)
	{
		hash_combine(seed, val);
		hash_val(seed, args...);
	}

	template<typename... Types>
	inline std::size_t hash_val(const Types& ...args)
	{
		std::size_t seed = 0;
		hash_val(seed, args...);
		return seed;
	}
}

#define DisableCopyAndAssign(TypeName) \
private:\
TypeName(const TypeName&) = delete; \
TypeName & operator=(const TypeName&) = delete;

#define XGF_ASSERT(expression) assert(expression)

#define BaseClassSettersAndGetters(name, type) \
	public:\
		type Get##name() const{\
			return m##name;\
		};\
		type & Get##name(){\
			return m##name;\
		};\
		void Set##name(type const & name){\
			m##name = name;\
		}; 
	
#define NotifyClassSettersAndGetters(name, type, func) \
	public:\
		type Get##name() const{\
			return m##name;\
		};\
		type & Get##name(){\
			return m##name;\
		};\
		void Set##name(const type & name){\
			if(name != m##name){m##name = name;func(); return;}\
		}; 
	
#define BaseClassArraySettersAndGetters(name, type, count) \
	public:\
	type Get##name(size_t index) const{\
		XGF_ASSERT(index < count && index  >= 0);\
		return m##name[index];\
	};\
	type & Get##name(size_t index){\
		XGF_ASSERT(index < count && index  >= 0);\
		return m##name[index];\
	};\
	void Set##name(size_t index, const type & name){\
		XGF_ASSERT(index < count && index  >= 0);\
		m##name[index] = name;\
	}; \
	size_t Get##name##Count() const{\
		return count;\
	};
	

#define ClassProperty(name, type)\
	BaseClassSettersAndGetters(name, type)\
	private: \
	type m##name;
 

#define ClassArrayProperty(name, type, count)\
	BaseClassArraySettersAndGetters(name, type, count)\
	private: \
	type m##name[count];
 


#define ClassPropertyWithInit(name, type, initialize)\
	BaseClassSettersAndGetters(name, type)\
	private: \
	type m##name = initialize;
 

#define ClassPropertyWithChanged(name, type, func)\
	NotifyClassSettersAndGetters(name, type, func)\
	private: \
	type m##name;
 
#define ClassPropertyWithInitAndChanged(name, type, initialize, func)\
	NotifyClassSettersAndGetters(name, type, func)\
	private: \
	type m##name = initialize;
 
