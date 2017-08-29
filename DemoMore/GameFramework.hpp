#pragma once
#include "../XGF/include/XGF.h"
class GameFramework :
    public SceneFramework
{
public:
    GameFramework();
    ~GameFramework();
    virtual void OnCreate();
    virtual void OnDestory();
    virtual bool OnClose();
};

