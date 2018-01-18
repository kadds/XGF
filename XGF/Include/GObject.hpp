#include "Event.hpp"
#include <functional>
#include <map>
#include "EventDispatcher.hpp"
namespace XGF {
	
	/*
	* 游戏基本对象，提供事件处理函数
	*/
	class GObject
	{
	public:
		GObject();
		~GObject();
		void _OnEvent(Event & event);
		virtual void OnEvent(Event & event) {};
	public:
		
	private:
		std::map<std::any, EventCallBack> mEventListener;
	};

}