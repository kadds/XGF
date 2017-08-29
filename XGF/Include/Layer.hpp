#pragma once
#include "Defines.hpp"
#include <vector>
#include "Batches.hpp"
class Scene;
class Actor;
class Layer
{
public:
	Layer();
	virtual ~Layer();
    void SetParent(Scene * parent);
	virtual void OnCreate() = 0;
	virtual void OnDestory() = 0;
    virtual void Updata(float deltaTime) = 0;
	virtual void Render(Batches & bs) = 0;
	virtual void OnSize(int ClientX, int ClientY) = 0;
	virtual void OnActivate(bool isActivate) = 0;

	virtual void Add(const Actor * actor);
    void Delete(const Actor * actor);
	Scene * GetParent() { return mParent; }
protected:
    std::vector<const Actor *> mActors;
    Scene* mParent;
};

