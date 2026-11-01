#include "Objects/UISettingsGameObject.h"
#include "Layers/GJBaseGameLayer.h"

UISettingsGameObject* UISettingsGameObject::create()
{
    UISettingsGameObject* object = new UISettingsGameObject();

    if (object->init())
    {
        object->autorelease();
        return object;
    }

    delete object;
    return nullptr;
}

std::string UISettingsGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveString = EffectGameObject::getSaveString(layer);

    if (m_targetGroupID)
    {
        saveString += fmt::format(",51,{}", m_targetGroupID);
    }

    if (m_centerGroupID)
    {
        saveString += fmt::format(",71,{}", m_centerGroupID);
    }

    if (m_xRef)
    {
        saveString += fmt::format(",385,{}", m_xRef);
    }

    if (m_yRef)
    {
        saveString += fmt::format(",386,{}", m_yRef);
    }

    if (m_xRelative)
    {
        saveString += fmt::format(",387,{}", m_xRelative);
    }

    if (m_yRelative)
    {
        saveString += fmt::format(",388,{}", m_yRelative);
    }

    return saveString;
}

bool UISettingsGameObject::init()
{
    return EffectGameObject::init("edit_eUISettingsBtn_001.png");
}

UISettingsGameObject::~UISettingsGameObject()
{
}