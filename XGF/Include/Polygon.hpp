#pragma once
#include "Defines.hpp"
#include <vector>
namespace XGF
{
	class Texture;
	/**
	* 可绑定资源基类
	*/
	class PolygonPleBinder
	{
	public:
		PolygonPleBinder() {}
		virtual ~PolygonPleBinder() {}
		int mCount;
		// 把数据复制到 Des 处 且长度为chunk
		virtual void CopyTo(void * Des, unsigned int chunk) const = 0;
	};
	/**
	* 通用的 Float4 数据绑定器
	*/
	class PolygonPleDataBinder : public PolygonPleBinder
	{
	public:
		PolygonPleDataBinder();
		~PolygonPleDataBinder();
		PolygonPleDataBinder(const PolygonPleDataBinder& pcb);
		XMFLOAT4 mData;
		//chunk is byte
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		bool operator ==(const PolygonPleDataBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (!(mData.x == rx.mData.x && mData.y == rx.mData.y && mData.x == rx.mData.x &&
					mData.y == rx.mData.y && mData.z == rx.mData.z && mData.w == rx.mData.w))
					return false;
			}
			return  true;
		}
	};
	/**
	* 索引结构
	*/
	class PolygonPleIndex
	{
	public:
		// 起始地址
		index *mIndex;
		// 索引个数
		int mCount;
		PolygonPleIndex(int n);
		~PolygonPleIndex();
		PolygonPleIndex(const PolygonPleIndex& tb);

		void CopyTo(void * Des, int offset) const;
		bool operator ==(const PolygonPleIndex &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (mIndex[i] != rx.mIndex[i])
					return false;
			}
			return  true;
		}
		index Get(int n) const;
	};

	/**
	* 纹理UV绑定器
	*/
	class PolygonPleTextureBinder : public PolygonPleBinder
	{
	public:
		Position *mPoint;
		PolygonPleTextureBinder(int n);
		~PolygonPleTextureBinder();
		PolygonPleTextureBinder(const PolygonPleTextureBinder& tb);
		void FromTexture(const Texture * tx);
		bool operator ==(const PolygonPleTextureBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (!(mPoint[i].x == rx.mPoint[i].x && mPoint[i].y == rx.mPoint[i].y))
					return false;
			}
			return  true;
		}
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		void SetPosition(float left, float right, float top, float bottom);
	};

	/**
	* 颜色数据绑定器
	*/
	class PolygonPleColorBinder : public PolygonPleBinder
	{
	public:
		PolygonPleColorBinder(int n);
		~PolygonPleColorBinder();
		PolygonPleColorBinder(const PolygonPleColorBinder& pcb);
		Color * mColor;
		void Set(int start, int count, const Color & c);
		virtual Color Get(int n);
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		bool operator ==(const PolygonPleColorBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (!(mColor[i].x == rx.mColor[i].x && mColor[i].y == rx.mColor[i].y && mColor[i].x == rx.mColor[i].x && mColor[i].y == rx.mColor[i].y && mColor[i].z == rx.mColor[i].z && mColor[i].w == rx.mColor[i].w))
					return false;
			}
			return  true;
		}
	};
	/**
	* 恒定颜色结构
	*/
	class PolygonPleConstantColorBinder : public PolygonPleColorBinder
	{
	public:
		PolygonPleConstantColorBinder(const Color & c, int Maxcount);
		~PolygonPleConstantColorBinder();
		PolygonPleConstantColorBinder(const PolygonPleConstantColorBinder& pcb);
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		virtual Color Get(int n);

		bool operator ==(const PolygonPleConstantColorBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			return  mColor == rx.mColor;
		}
		int mAllCount;
	};
	/*
	恒定多颜色结构
	*/
	class PolygonPleConstantExColorBinder : public PolygonPleColorBinder
	{
	public:
		PolygonPleConstantExColorBinder(int CountInEachLayer[], int LayerCount);
		~PolygonPleConstantExColorBinder();
		PolygonPleConstantExColorBinder(const PolygonPleConstantExColorBinder& pcb);
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		virtual Color Get(int n);
		void SetLayerColor(int layer, const Color & color);
		bool operator ==(const PolygonPleConstantExColorBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			return  mColor == rx.mColor;
		}
		int mAllCount;
		int *mLayer;
		int mLayerCount;
	};

	/**
	* 顶点数据 - 桥
	*/
	class BindingBridge
	{
	public:
		BindingBridge();
		~BindingBridge();
		int mCount;
		int GetBinderCount() const {
			return mCount;
		};
		void SetBinder(const  PolygonPleBinder & c, int pos);
		void InsertBinder(const  PolygonPleBinder & c, int pos);
		void AddBinder(const  PolygonPleBinder & c);
		void AddPlaceHolder();
		const PolygonPleBinder * GetBinder(int tp) const;
	private:
		const PolygonPleBinder* binders[16];//max is 16
	};


	/*
	* 几何顶点结构
	*/
	class PolygonPlePoint : public PolygonPleBinder
	{
	public:
		virtual void CopyTo(void * Des, unsigned int chunk) const = 0;
	};
	class PolygonPlePoint3 : public PolygonPleBinder
	{
	public:
		Point *mPoint;
		PolygonPlePoint3(int n);
		PolygonPlePoint3(const PolygonPlePoint3 & p);
		~PolygonPlePoint3();
		bool operator ==(const PolygonPlePoint3 &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (mPoint[i].x != rx.mPoint[i].x || mPoint[i].y != rx.mPoint[i].y || mPoint[i].z != rx.mPoint[i].z)
					return false;
			}
			return true;
		}
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		void CopyTo(PolygonPlePoint3 & ppe);
	public:
		void Transform(float centerXD2, float centerYD2);
		void TransformTo(PolygonPlePoint3 * pol, float centerXD2, float centerYD2) const;
		void Translation(float x, float y, float z);
		void Mul(const DirectX::CXMMATRIX matrix);
		void MulTo(PolygonPlePoint3 * pol, const DirectX::CXMMATRIX matrix) const;
	};

	class PolygonPlePoint4 : public PolygonPleBinder
	{
	public:
		Point4 *mPoint;
		PolygonPlePoint4(int n);
		PolygonPlePoint4(const PolygonPlePoint4 & p);
		~PolygonPlePoint4();
		bool operator ==(const PolygonPlePoint4 &rx) const {
			if (&rx == this)
				return true;
			if (rx.mCount != mCount) return false;
			for (int i = 0; i < mCount; i++)
			{
				if (mPoint[i].x != rx.mPoint[i].x || mPoint[i].y != rx.mPoint[i].y || mPoint[i].z != rx.mPoint[i].z)
					return false;
			}
			return true;
		}
		virtual void CopyTo(void * Des, unsigned int chunk) const override;
		void CopyTo(PolygonPlePoint4 & ppe);
	public:
		void Transform(float centerXD2, float centerYD2);
		void TransformTo(PolygonPlePoint4 * pol, float centerXD2, float centerYD2) const;
		void Translation(float x, float y, float z);
		void Mul(const DirectX::CXMMATRIX matrix);
		void MulTo(PolygonPlePoint4 * pol, const DirectX::CXMMATRIX matrix) const;
	};

}
