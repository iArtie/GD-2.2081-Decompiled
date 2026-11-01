#include "SetupObjectOptions2Popup.h"
#include "SetupTriggerPopup.h"
#include "GameObject.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"

SetupObjectOptions2Popup* SetupObjectOptions2Popup::create(GameObject* object, cocos2d::CCArray* objects)
{
    SetupObjectOptions2Popup* instance = new SetupObjectOptions2Popup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupObjectOptions2Popup::init(GameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init((EffectGameObject*)object, objects, 320.0f, 200.0f, 1))
        return false;

    preSetup();
    m_disableTextDelegate = true;

    addTitle("Config 2");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Enter Channel</c> is used to set a specific enter effect.\n"
        "<cy>Material</c> is used to link with specific events.\n"
        "<cl>ControlID</c> allows you to reference specific objects when using remapping.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f + 30.0f);

    cocos2d::CCPoint enterChannelPos = center + cocos2d::CCPoint(-80.0f, 0.0f);
    createValueControlAdvanced(
        343, "Enter Channel:", cocos2d::CCPoint(0.7f, 0.0f), 0.0f, &enterChannelPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint plusPos = center + cocos2d::CCPoint(-5.0f, 0.0f);
    createPlusButton(343, cocos2d::CCPoint(0.8f, 0.0f), 0.0f, "", &plusPos, 0);

    cocos2d::CCPoint materialPos = center + cocos2d::CCPoint(80.0f, 0.0f);
    createValueControlAdvanced(
        446, "Material:", cocos2d::CCPoint(0.7f, 0.0f), 0.0f, &materialPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    if (m_effectObjects)
    {
        cocos2d::CCPoint controlIDPos = center + cocos2d::CCPoint(80.0f, -55.0f);
        createValueControlAdvanced(
            534, "ControlID:", cocos2d::CCPoint(0.7f, 0.0f), 0.0f, &controlIDPos,
            Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);
    }

    postSetup();

    return true;
}

void SetupObjectOptions2Popup::onPlusButton(cocos2d::CCObject* sender)
{
    std::unordered_set<int> usedChannels;
    int nextChannel = GameManager::sharedState()->m_levelEditorLayer->getNextFreeEnterChannel(&usedChannels);
    updateValue(343, (float)nextChannel);
}

SetupObjectOptions2Popup::~SetupObjectOptions2Popup()
{
}