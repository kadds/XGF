#pragma once
#include "Batches.hpp"
#include "Defines.hpp"
#include <vector>
using std::vector;
/*
* ≥°æ∞¿‡
*/
class Layer;
class XGFramework;
class Scene
{
public:
	Scene();
	virtual ~Scene();
    virtual void UpdataLayer(float dt);
	virtual void RenderLayer(Batches & bts);

	virtual void Render(float deltaTime) = 0;
	virtual void Updata(float deltaTime) = 0;
	void OnSwitchIn();
	void OnSwitchOut();
	
	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnSize(int ClientX, int ClientY) = 0;
	virtual void OnActivate(bool isActivate) = 0;
	int GetWidth();
	int GetHeight();
	XGFramework * GetFramework()
	{ 
		return mFramework; 
	}
	void SetFramework(XGFramework * framework);
	void AddLayer(Layer * layer);
	void SwitchScene(Scene * scene);
	void Clear(Color & c);
	void ClearDepthStencilBuffer();

protected:
	
	vector<Layer *> mLayerList;
	XGFramework *mFramework;
private:
	DISALLOW_COPY_AND_ASSIGN(Scene);
};


