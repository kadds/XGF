#pragma once
#include <DirectXMath.h>
#ifdef _DEBUG
#define _XGF_DEBUG_ALLOC
#endif

#ifdef _XGF_DEBUG_ALLOC
#define _CRTDBG_MAP_ALLOC  
#pragma warning(disable:4005)
#include <stdlib.h>  
#include <crtdbg.h>  
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#endif

namespace XGF
{
	typedef unsigned int index;
	using DirectX::XMMATRIX;
	using DirectX::FXMMATRIX;
	using DirectX::CXMMATRIX;
	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT4;
	using DirectX::XMFLOAT4X4;
	using DirectX::XMFLOAT4;

	typedef XMFLOAT2 Position;
	typedef XMFLOAT3 Point;
	typedef XMFLOAT4 Point4;
	typedef XMFLOAT4 Color;
	struct WVPMatrix
	{
		DirectX::XMMATRIX worldMatrix;

		DirectX::XMMATRIX viewMatrix;

		DirectX::XMMATRIX projMatrix;

		void Transpose()
		{
			worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
			viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
			projMatrix = DirectX::XMMatrixTranspose(projMatrix);
		}
	};
}

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)