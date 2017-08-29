
#include "GameFramework.hpp"
#include "FirstScene.hpp"

GameFramework::GameFramework()
{
}


GameFramework::~GameFramework()
{
}

void GameFramework::OnCreate()
{
    AddScene(new FirstScene());
}

void GameFramework::OnDestory()
{
    mScene->OnDestory();
}

bool GameFramework::OnClose()
{
    return true;
}


