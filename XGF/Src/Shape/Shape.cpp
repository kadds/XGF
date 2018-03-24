#include "../../Include/Shape.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Texture.hpp"
namespace XGF
{
	namespace Shape
	{
		Shape::Shape(int n, int indexCount) :mPolygonPleIndex(indexCount), mPolygon(n)
		{
		}
		Shape::~Shape()
		{
		}
		void Shape::GetPosition(Point & p) const
		{
			p = mPolygon.mPoint[0];
		}
		const PolygonPleIndex & Shape::GetIndex() const
		{
			return mPolygonPleIndex;
		}

		void Shape::SetZ(float z)
		{
			for (int i = 0; i < mPolygon.mCount; i++)
			{
				mPolygon.mPoint[i].z = z;
			}
		}

		void Shape::Render(Batch & batch, const BindingBridge & bbrige, const Texture & tx)
		{
			Render(batch, bbrige, tx.GetRawTexture());
		}
		void Shape::Render(Batch & batch, const BindingBridge & bbrige, ID3D11ShaderResourceView * tex)
		{
			batch.GetShaderStage()->SetPSSRV(0, tex);//if srv index is 0
			Render(batch, bbrige);
		}
		void Shape::Render(Batch & batch, const BindingBridge & bbrige)
		{
			PolygonPlePoint3 ppe(mPolygon.mCount);
			PolygonPlePoint3 * pPPe;
			auto matirix = mTransform.GetMatrix();
			mPolygon.MulTo(&ppe, matirix);
			pPPe = &ppe;
			BindingBridge bbr(bbrige);
			bbr.InsertBinder(*pPPe, 0);
			batch.DrawPolygon(GetIndex(), bbr);
		}
		float triangleArea(Point a, Point b, Point c)
		{
			float result = std::abs((a.x * b.y + b.x * c.y + c.x * a.y - b.x * a.y
				- c.x * b.y - a.x * c.y) / 2.0f);
			return result;
		}
		// 判断2维点是否在图元内部
		// 使用 PNPoly 算法
		bool pInPolygon(const PolygonPlePoint3& ql, float x, float y)
		{
			// 生成最小包围盒
			Position minPosition = { ql.mPoint[0].x, ql.mPoint[0].y }, maxPosition = { ql.mPoint[0].x, ql.mPoint[0].y };
			for (int i = 1; i < ql.mCount; i++) 
			{
				if (ql.mPoint[i].x < minPosition.x)
					minPosition.x = ql.mPoint[i].x;
				if (ql.mPoint[i].y < minPosition.y)
					minPosition.y = ql.mPoint[i].y;
				if (ql.mPoint[i].x > maxPosition.x)
					maxPosition.x = ql.mPoint[i].x;
				if (ql.mPoint[i].y > maxPosition.y)
					maxPosition.y = ql.mPoint[i].y;
			}

			if (x < minPosition.x || x > maxPosition.x || y < minPosition.y || y > maxPosition.y)
				return false;
			bool k = false;
			int i = 0;
			int j = ql.mCount - 1;
			for (; i < ql.mCount; j = i++) {
				if (((ql.mPoint[i].y > y) != (ql.mPoint[j].y > y)) &&
					(x < (ql.mPoint[j].x - ql.mPoint[i].x) * (y - ql.mPoint[i].y) /
					(ql.mPoint[j].y - ql.mPoint[i].y) + ql.mPoint[i].x))
					k = !k;
			}
			return k;
		}
	}
}
	
