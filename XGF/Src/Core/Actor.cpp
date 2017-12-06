#include "../../Include/Actor.hpp"

#include "../../Include/Batch.hpp"
namespace XGF{
	Actor::Actor() : mScale(1.f, 1.f, 1.f), mRotation(0.0f, 0.0f, 0.0f), mTranslation(0.f, 0.f, 0.f),
		mRotationOrigin(0.f, 0.f, 0.f), mScaleOrigin(0.f, 0.f, 0.f), mIsChange(true), mClickable(false), mCanGetMouseEvent(false)
		, onMouseDowmListener(nullptr), onMouseMoveListener(nullptr), onMouseUpListener(nullptr), mColor(1.f, 1.f, 1.f, 1.f)
	{
		DirectX::XMStoreFloat4x4(&mMatrix, DirectX::XMMatrixIdentity());
		mActions.SetOnActionCompletedListener(std::bind(&Actor::OnActionCompleted, this));
	}

	void Actor::OnMouseUp(const MousePoint & mp, int c, bool isIn)
	{
		if (mClickable)
			mClickHelper.OnMouseUp(mp, c, isIn);
		if (onMouseUpListener && isIn)
			onMouseUpListener(mp, c);
	}
	void Actor::OnMouseMove(const MousePoint & mp, int cp, bool isIn)
	{
		if (mClickable)
			mClickHelper.OnMouseMove(isIn);
		if (onMouseMoveListener && isIn)
			onMouseMoveListener(mp, cp);
	}
	void Actor::OnMouseDowm(const MousePoint & mp, int c, bool isIn)
	{
		if (mClickable)
			mClickHelper.OnMouseDowm(mp, c, isIn);
		if (onMouseDowmListener && isIn)
			onMouseDowmListener(mp, c);
	}
	Actor::~Actor()
	{
	}
	void Actor::_Updata(float deltaTime)
	{
		mActions.Updata(deltaTime);
	}
	void Actor::_Render(Batches & bs)
	{
		DirectX::XMMATRIX matrix;
		RefreshMatrix();
		matrix = DirectX::XMLoadFloat4x4(&mMatrix);
		Render(&matrix, bs);
	}
	void Actor::SetAction(std::unique_ptr<Action> action)
	{
		mActions.SetAction(std::move(action), this);
	}
	void Actor::StartAction()
	{
		mActions.Start();
	}
	void Actor::StopAction()
	{
		mActions.Stop();
	}
	void Actor::SetRotationAngle(Point angle)
	{
		mRotation = angle;
		mIsChange = true;
	}
	void Actor::SetScale(Point p)
	{
		mScale = p;
		mIsChange = true;
	}

	Point Actor::GetOrigin() const
	{
		return mRotationOrigin;
	}
	Point Actor::GetAngle() const
	{
		return mRotation;
	}
	Point Actor::GetScale() const
	{
		return mScale;
	}
	void Actor::ChangeState()
	{
		mIsChange = true;
	}
	void Actor::OnActionCompleted()
	{
		mIsChange = true;
		mTranslation = Point(0, 0, 0);
	}
	inline void Actor::OnPositionChange(const Point & p, int ID)
	{
		GetShape()->SetPosition(p);
		mIsChange = true;
	}
	inline void Actor::OnAngleChange(const Point & p, int ID)
	{
		mRotation = p;
		mIsChange = true;
	}
	inline void Actor::OnScaleChange(const Point & p, int ID)
	{
		mScale = p;
		mIsChange = true;
	}
	inline void Actor::OnColorChange(const Color & p, int ID)
	{
		mColor = p;
		mIsChange = true;
	}
	inline void Actor::OnAlphaChange(float a, int ID)
	{
		mColor.w = a;
		mIsChange = true;
	}
	inline void Actor::GetPosition(Point & p, int ID) const
	{
		GetShape()->GetPosition(p);
	}
	inline void Actor::GetAngle(Point & p, int ID) const
	{
		p = mRotation;
	}
	inline void Actor::GetScale(Point & p, int ID) const
	{
		p = mScale;
	}
	inline void Actor::GetColor(Color & p, int ID) const
	{
		p = mColor;
	}
	inline void Actor::GetAlpha(float & p, int ID)const
	{
		p = mColor.w;
	}
	void Actor::RefreshMatrix()
	{
		if (mIsChange)
		{
			DirectX::XMStoreFloat4x4(&mMatrix, CalculateMatrix());
			mIsChange = false;
		}
	}
	void Actor::SetTranslation(Point translation) {
		mTranslation = translation;
		mIsChange = true;
	}

	bool Actor::IsInBBox(Point p)
	{
		if (mRotation.x == 0.0f && mRotation.y == 0.0f && mRotation.z == 0.0f && mScale.x == 1.0f && mScale.y == 1.0f && mScale.z == 1.0f)//无缩放旋转变换
		{
			if (mTranslation.x == 0.0f && mTranslation.y == 0.0f && mTranslation.z == 0.0f)
				return GetShape()->IsInBBox(p, nullptr);
			else
			{
				RefreshMatrix();
				return  GetShape()->IsInBBox(p, &mMatrix);
			}
		}
		else
		{
			RefreshMatrix();
			return  GetShape()->IsInBBox(p, &mMatrix);
		}
		return false;
	}

	XMMATRIX Actor::CalculateMatrix() const
	{
		//3D 矩阵计算
		Point mx;
		GetShape()->GetPosition(mx);
		Point op = mx;
		op.x += mRotationOrigin.x;
		op.y += mRotationOrigin.y;
		op.z += mRotationOrigin.z;
		return DirectX::XMMatrixTransformation(
			DirectX::XMLoadFloat3(&mx),
			DirectX::XMLoadFloat3(&mRotation),
			DirectX::XMLoadFloat3(&mScale),
			DirectX::XMLoadFloat3(&op),
			DirectX::XMLoadFloat3(&mRotation),
			DirectX::XMLoadFloat3(&mTranslation)
		);
	}


}
