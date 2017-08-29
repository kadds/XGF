#include "../../Include/Layer.hpp"



Layer::Layer()
{
}


Layer::~Layer()
{
}

void Layer::SetParent(Scene * parent)
{
    mParent = parent;
}

void Layer::Add(const Actor * actor)
{
    mActors.push_back(actor);
}

void Layer::Delete(const Actor * actor)
{
    //mActors.erase(actor);
}
