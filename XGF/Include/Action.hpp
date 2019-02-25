#pragma once
#include "Defines.hpp"
#include "Interpolator\Interpolator.hpp"
namespace XGF
{
	typedef std::function<void()> NotificationCallback;
	class Action
	{
	public:
		Action(float duration);
		virtual ~Action() = default;
		bool Tick(float deltaTime);
		void Reset();
		virtual void OnReset() = 0;
	protected:
		virtual bool OnTick(float k, float deltaTime) = 0;
	private:
		const float mDuration;
		float mPassedTime;
	};
	class DataBaseAction : public Action
	{
	public:
		DataBaseAction(float duration, std::shared_ptr<Interpolator> interpolator, NotificationCallback callback);

		void SetNotificationCallback(NotificationCallback callback);
	protected:
		virtual bool OnTick(float k, float deltaTime) override;
		virtual void Calc(float deltaK) = 0;
		void OnReset() override;
		const std::shared_ptr<Interpolator> mInterpolator;
		float mLastK;
		NotificationCallback mCallback;
	};
	template<typename DataType>
	class DataAction : public DataBaseAction
	{
	public:
		DataAction(const DataType& delta, float duration, std::shared_ptr<Interpolator> interpolator, DataType * target = nullptr, NotificationCallback callback = nullptr): 
			DataBaseAction(duration, interpolator, callback), mTarget(target),mDelta(delta)
		{
			
		}
		void SetTarget(DataType * target)
		{
			mTarget = target;
			mLastK = 0.f;
		}
		void Calc(float deltaK) override
		{
			*mTarget = *mTarget + mDelta * deltaK;
		}
	protected:
		const DataType mDelta;
		DataType * mTarget;
	};
	class RepeatAction final : public Action
	{
	public:
		// count < 0 is infinite
		RepeatAction(std::unique_ptr<Action> action, int count);
	protected:
		bool OnTick(float k, float deltaTime) override;

		void OnReset() override;
	private:
		int mRestCount;
		int mRawCount;
		const bool mInfinite;
		const std::unique_ptr<Action> mAction;
	};
	class CompositeAction : public Action
	{
	protected:
		std::vector<std::unique_ptr<Action>> mActions;
		void OnReset() override
		{
			for (auto & action : mActions)
			{
				action->Reset();
			}
		}
	public:
		CompositeAction(std::vector<std::unique_ptr<Action>>&& actions);
	};

	class ParallelAction final : public CompositeAction
	{
	public:
		ParallelAction(std::vector<std::unique_ptr<Action>>&& actions);
	protected:
		virtual bool OnTick(float k, float deltaTime) override;
	};

	class SequenceAction final : public CompositeAction
	{
	private:
		size_t mIndex;
	public:
		SequenceAction(std::vector<std::unique_ptr<Action>>&& actions);
	protected:
		virtual bool OnTick(float k, float deltaTime) override;
		void OnReset() override;
	};


	class DelayAction : public Action
	{
	public:
		DelayAction(float duration);
	protected:
		virtual bool OnTick(float k, float deltaTime) override;
		void OnReset() override;
	};

}
