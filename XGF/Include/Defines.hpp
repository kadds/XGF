#pragma once
#include <d3d11.h>
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
	typedef unsigned int index;

	namespace SM = DirectX::SimpleMath;
	typedef SM::Vector3 Point;
	typedef SM::Vector2 Position;
	typedef SM::Vector4 Point4;
	
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
