#include "Event.hpp"
#include <functional>
#include <map>
#include "EventDispatcher.hpp"
namespace XGF {
	
	/*
	* ��Ϸ���������ṩ�¼�������
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