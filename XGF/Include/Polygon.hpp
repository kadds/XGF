#pragma once
#include "Defines.hpp"
#include <vector>
namespace XGF
{
	template<typename T>
	struct BindingResourceTypeTraits {
		constexpr static bool IsBindingType() {
			return false;
		}
	};
	template<>
	struct BindingResourceTypeTraits<Point2> {
		constexpr static bool IsBindingType() {
			return true;
		}
	};
	template<>
	struct BindingResourceTypeTraits<Point> {
		constexpr static bool IsBindingType() {
			return true;
		}
	};
	template<>
	struct BindingResourceTypeTraits<Point4> {
		constexpr static bool IsBindingType() {
			return true;
		}
	};
	template<>
	struct BindingResourceTypeTraits<Color> {
		constexpr static bool IsBindingType() {
			return true;
		}
	};


	class Texture;
	/**
	* 可绑定资源基类
	*/
	class PolygonPleBinder
	{
	public:
		PolygonPleBinder() = default;
		virtual ~PolygonPleBinder() = default;
		// 把数据复制到 Des 处 且长度为chunk
		virtual void CopyTo(void * Des, unsigned int chunk) const = 0;
		int Count() const
		{
			return mCount;
		}
	protected:
		void SetCount(int c)
		{
			mCount = c;
		}
	private:
		/**
		 * 数据个数
		 */
		int mCount;
		
	};
	/**
	* 数据绑定器
	*/

	template<typename Vector>
	class PolygonPleDataBinder : public PolygonPleBinder
	{
	protected:
		PolygonPleDataBinder()
		{
			static_assert(BindingResourceTypeTraits<Vector>::IsBindingType(), "Vector type must be Point Point2 Point4 Color");
		};
		std::unique_ptr<Vector[]> mData;
	public:
		Vector * GetData()
		{
			return mData.get();
		}
		Vector & GetData(int n)
		{ 
			XGF_ASSERT(n >= 0 && n < Count());

			return mData[n];
		}
		PolygonPleDataBinder(int count)
		{
			SetCount(count);
			mData = std::make_unique<Vector[]>(count);
		}
		~PolygonPleDataBinder()
		{
		}

		PolygonPleDataBinder(const PolygonPleDataBinder & pcb)
		{
			mData = std::move(pcb.mData);
			SetCount(pcb.Count());
		}
		PolygonPleDataBinder & operator =(const PolygonPleDataBinder & pcb)
		{
			mData = std::move(pcb.mData);
			SetCount(pcb.Count());
			return *this;
		}
		virtual void PolygonPleDataBinder::CopyTo(void * Des, unsigned int chunk) const
		{
			unsigned char* desc = (unsigned char*)Des;
			for(int i = 0; i < Count(); i ++)
			{
				auto * des = reinterpret_cast<Vector *>(desc);
				*(des) = mData[i];
				desc += chunk;
			}
		}

		virtual bool operator ==(const PolygonPleDataBinder &rx) const {
			if (&rx == this)
				return true;
			if (rx.Count() != Count()) return false;
			for (int i = 0; i < Count(); i++)
			{
				if (mData[i] != rx.mData[i])
					return false;
			}
			return  true;
		}
		virtual void ExpandAll(std::function<void(const Vector & src, Vector & dsc)> func)
		{
			for (int i = 0; i < Count(); i ++){
				func(GetData(i), GetData(i));
			}
		}
		virtual void ExpandAllTo(PolygonPleDataBinder& pdb, std::function<void(const Vector & src, Vector & dsc)> func)
		{
			for (int i = 0; i < Count(); i++) {
				func(GetData(i), pdb.GetData(i));
			}
		}
	};
	
	template<typename Vector>
	class PolygonPleConstantDataBinder : public PolygonPleDataBinder<Vector>
	{
	private:
		int mDataCount;
		std::vector<int> tgLayer;
	public:
		PolygonPleConstantDataBinder(int count, const Vector & v): tgLayer(1)
		{
			this->SetCount(count);
			this->mData = std::make_unique<Vector[]>(1);
			mDataCount = 1;
			tgLayer[0] = count;
			this->mData[0] = v;
		}
		void Set(int layer, const Vector & vec)
		{
			XGF_ASSERT(layer < mDataCount && layer >= 0);
			this->GetData(layer) = vec;
		}
		/**
		 * \brief 常量数据
		 * \param count 顶点个数
		 * \param data 每一组数据对应的 ConstantData 索引开始处
		 */
		PolygonPleConstantDataBinder(int count, std::vector<std::pair<int, Vector>> data):tgLayer(data.size())
		{
			XGF_ASSERT(data.size() != 0);
			this->SetCount(count);
			this->mData = std::make_unique<Vector[]>(data.size());
			mDataCount = data.size();
			int i = 0;
			this->mData[0] = data[0].second;
			for (; i < data.size() - 1; i++)
			{
				tgLayer[i] = data[i + 1].first;
				this->mData[i + 1] = data[i + 1].second;
			}
			tgLayer[i] = this->Count();
			//XGF_ASSERT(tgLayer.back() == this->Count());
		}
		virtual void CopyTo(void * Des, unsigned int chunk) const
		{
			unsigned char* desc = (unsigned char*)Des;

			int j = 0, sumStartIndex = tgLayer[0];
			for (int i = 0; i < this->Count(); i++)
			{
				Vector * des = reinterpret_cast<Vector *>(desc);
				if (i >= sumStartIndex)
				{
					sumStartIndex = tgLayer[++j];
				}
				*(des) = this->mData[j];
				desc += chunk;
			}
		}

	};
	template<typename Vector>
	class PolygonPleFunctionalDataBinder : public PolygonPleDataBinder<Vector>
	{
	private:
		std::function<void(int index, Vector* desc)> func;
	public:
		PolygonPleFunctionalDataBinder(int count, std::function<void(int index, Vector* desc)> func)
		{
			this->mCount = count;
			this->func = func;
			this->mData = nullptr;
		}
		virtual void CopyTo(void * Des, unsigned int chunk) const
		{
			unsigned char* desc = (unsigned char*)Des;

			for (int i = 0; i < this->Count(); i++)
			{
				auto * des = reinterpret_cast<Vector *>(desc);
				func(i, des);
				desc += chunk;
			}
		}
	};
	/**
	* 索引结构
	*/
	class PolygonPleIndex
	{
	public:
		// 起始地址
		Index *mIndex;
		// 索引个数
		int mCount;
		PolygonPleIndex(int n);
		~PolygonPleIndex();
		PolygonPleIndex(const PolygonPleIndex& tb);
		void ShrinkTo(int count);
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
		Index Get(int n) const;
	};



	typedef PolygonPleDataBinder<Position> PolygonPleTextureBinder;

	typedef PolygonPleDataBinder<Point> PolygonPlePointBinder;

	
	
	typedef PolygonPleDataBinder<Point4> PolygonPlePoint4Binder;

	typedef PolygonPleDataBinder<Point2> PolygonPlePoint2Binder;

	typedef PolygonPleDataBinder<Color> PolygonPleColorBinder;

	typedef PolygonPleConstantDataBinder<Point> PolygonPleConstantPointBinder;

	typedef PolygonPleConstantDataBinder<Point4> PolygonPleConstantPoint4Binder;

	typedef PolygonPleConstantDataBinder<Point2> PolygonPleConstantPoint2Binder;

	typedef PolygonPleConstantDataBinder<Color> PolygonPleConstantColorBinder;

	typedef PolygonPleDataBinder<Point> PolygonPleNormalBinder;
	typedef PolygonPleConstantDataBinder<Point> PolygonPleConstantNormalBinder;
	/**
	* 顶点数据 - 桥
	*/
	class BindingBridge
	{
	public:
		BindingBridge();
		~BindingBridge();
		BindingBridge(const BindingBridge & c);

		void SetBinder(const std::shared_ptr<PolygonPleBinder> c, int pos);
		void InsertBinder(const std::shared_ptr<PolygonPleBinder> c, int pos);
		void InsertBinder(const BindingBridge & c, int pos);

		void AddBinder(const std::shared_ptr<PolygonPleBinder> c);
		void AddBinder(const BindingBridge & c);
		void AddBinder(const std::vector<std::shared_ptr<PolygonPleBinder>> & c);
		void AddPlaceHolder();
		void RemoveBinder(int index);
		void RemoveFrom(int indexStart);
		void Clear();
		std::shared_ptr<PolygonPleBinder> GetBinder(int i) const { return binders[i]; };
		const std::vector<std::shared_ptr<PolygonPleBinder>> & GetBinders() { return binders; };
		size_t Count() const { return binders.size(); }
	private:
		std::vector<std::shared_ptr<PolygonPleBinder>> binders;
	};

	namespace Operator
	{
		struct Transfrom
		{
			float xd2, yd2;
			Transfrom(float centerXD2, float centerYD2) : xd2(centerXD2), yd2(centerYD2) {  }
			void operator ()(const Point2 & vec, Point2 & out) const
			{
				out.x = vec.x - xd2;
				out.y = -vec.y + yd2;
			}
		};
		struct Translation
		{
			float x, y, z, w;
			Translation(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {  }
			void operator ()(const Point & vec, Point & out) const
			{
				out.x = vec.x + x;
				out.y = vec.y + y;
				out.z = vec.z + z;
			}
			void operator ()(const Point2 & vec, Point2 & out) const
			{
				out.x = vec.x + x;
				out.y = vec.y + y;
			}
			void operator ()(const Point4 & vec, Point4 & out) const
			{
				out.x = vec.x + x;
				out.y = vec.y + y;
				out.z = vec.z + z;
				out.w = vec.w + w;
			}
		};
		struct Multiply
		{
			DirectX::XMMATRIX matrix;
			Multiply(DirectX::CXMMATRIX m) : matrix(m) {  }
			void operator ()(const Point & vec, Point & out) const
			{
				DirectX::XMStoreFloat3(&out, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&vec), matrix));
			}
			void operator ()(const Point2 & vec, Point2 & out) const
			{
				DirectX::XMStoreFloat2(&out, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat2(&vec), matrix));
			}
			void operator ()(const Point4 & vec, Point4 & out) const
			{
				DirectX::XMStoreFloat4(&out, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&vec), matrix));
			}
		};
	}
}
