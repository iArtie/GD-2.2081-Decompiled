#include "ObjectControlGameObject.h"
#include "GJBaseGameLayer.h"

ObjectControlGameObject* ObjectControlGameObject::create()
{
    ObjectControlGameObject* returnVal = new ObjectControlGameObject();
    if (returnVal->init())
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool ObjectControlGameObject::init()
{
    return EffectGameObject::init("edit_eCollisionStateBtn_001.png");
}

std::string ObjectControlGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_targetGroupID)
    {
        result += ",";
        result += "51";
        result += ",";
        result += std::to_string(m_targetGroupID);
    }

    return result;
}

ObjectControlGameObject::~ObjectControlGameObject()
{
}