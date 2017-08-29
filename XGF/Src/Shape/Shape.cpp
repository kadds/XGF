#include "../../Include/Shape.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Texture.hpp"
namespace Shape {

Shape::Shape(int n, int indexCount):mPolygonPleIndex(indexCount),mIsWriteToVertex(false), mPolygon(n)
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
	for(int i = 0; i < mPolygon.mCount; i++)
	{
		mPolygon.mPoint[i].z = z;
	}
}

void Shape::Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige, const Texture & tx)
{
	Render(batch, matirix, bbrige, tx.GetShaderResourceView());
}
void Shape::Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige, ID3D11ShaderResourceView * tex)
{
	batch.SetTexture(tex);
	PolygonPle ppe(mPolygon.mCount);
	if (matirix != nullptr)
	{
		mPolygon.MulTo(&ppe, *matirix);
	}
	else
		mPolygon.CopyTo(ppe);
	ppe.Transform(static_cast<float>(batch.GetClientWidth() / 2), static_cast<float>(batch.GetClientHeight() / 2));
	batch.DrawPolygon(ppe, GetIndex(), bbrige);
}
void Shape::Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige)
{
	PolygonPle ppe(mPolygon.mCount);
	if (matirix != nullptr)
	{
		mPolygon.MulTo(&ppe, *matirix);
	}
	else
		mPolygon.CopyTo(ppe);
	ppe.Transform(static_cast<float>(batch.GetClientWidth() / 2), static_cast<float>(batch.GetClientHeight() / 2));
	batch.DrawPolygon(ppe, GetIndex(), bbrige);
}
float triangleArea(Point a, Point b, Point c)
{
	float result = std::abs((a.x * b.y + b.x * c.y + c.x * a.y - b.x * a.y
		- c.x * b.y - a.x * c.y) / 2.0f);
	return result;
}
bool pInPolygon(const PolygonPle& ql, int x, int y)
{
	Point p = Point(static_cast<float>(x), static_cast<float>(y), 0.f);
	float t = 0.00f, rf = 0.00f;
	for (int i = 0; i < ql.GetPointCount() - 1; i++)
	{
		t += triangleArea(ql.mPoint[i], ql.mPoint[i + 1], p);
	}
	t += triangleArea(ql.mPoint[ql.GetPointCount() - 1], ql.mPoint[0], p);
	for (int i = 0; i < ql.GetPointCount() - 2; i++)
	{
		rf += triangleArea(ql.mPoint[i], ql.mPoint[i + 1], ql.mPoint[i + 2]);
	}
	if (abs(t - rf) < 0.1f)
		return true;
	return false;
}
}
