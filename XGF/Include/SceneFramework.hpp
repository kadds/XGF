#pragma once
#include "XGFramework.hpp"
#include "Defines.hpp"
class Scene;
/*
*����Scene��Ӧ�ó�����
*��̳�ʵ��OnCreate��OnDestory��������
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
	//�л�Scene��ע�⣬�ú���ֻ������Ϣ�������������Ϣ����һ֡�Ż��л���ʵ���л�������ISwithScene��
	//��ȷ����Click�¼��е��ò����ƻ�������
	void SwitchScene(Scene * scene);
	void AddScene(Scene * scene);
	//��ȾScene
	void RenderScene();
	//���rederTaget buffer
	void Clear(Color & c);

protected:
	
	Scene * mScene;
	void ISwitchScene(Scene * scene);
private:
	DISALLOW_COPY_AND_ASSIGN(SceneFramework);
};

