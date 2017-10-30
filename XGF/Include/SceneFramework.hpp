#pragma once
#include "XGFramework.hpp"
#include "Defines.hpp"
class Scene;
/*
*包含Scene的应用程序框架
*需继承实现OnCreate和OnDestory两个方法
*/
class SceneFramework :
	public XGFramework
{
public:
	SceneFramework();
	virtual ~SceneFramework();
	virtual void OnCreate() override = 0;
	virtual void OnDestory() override = 0;
	virtual void OnSize(int ClientX, int ClientY) override;
	virtual void OnActivate(bool isActivate) override;
	virtual void Render() override;
	virtual void Update(float time) override;
	virtual void OnMessage(const Event& ev) override;
	//切换Scene。注意，该函数只是在消息队列中添加了消息，下一帧才会切换，实际切换代码在ISwithScene中
	//可确保在Click事件中调用不会破坏迭代器
	void SwitchScene(Scene * scene);
	void AddScene(Scene * scene);
	//渲染Scene
	void RenderScene();
	//清除rederTaget buffer
	void Clear(Color & c);

protected:
	
	Scene * mScene;
	void ISwitchScene(Scene * scene);
private:
	DISALLOW_COPY_AND_ASSIGN(SceneFramework);
};

