#include "Popups/Setup/SetupRotatePopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EnhancedGameObject.h"
#include "Objects/EffectGameObject.h"

SetupRotatePopup* SetupRotatePopup::create(EnhancedGameObject* object, cocos2d::CCArray* objects)
{
    SetupRotatePopup* instance = new SetupRotatePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupRotatePopup::init(EnhancedGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init((EffectGameObject*)object, objects, 300.0f, 240.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Edit Rotate Action");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Default</c> uses the default rotation values.\n"
        "<cy>Custom</c> lets you specify degrees per second.\n"
        "<cl>Disable</c> stops rotation completely.",
        1.0f);

    cocos2d::CCPoint toggleBase(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    bool disabled = getValue(98) != 0.0f;
    float degrees = getValue(97);

    cocos2d::CCPoint defaultPos = toggleBase + cocos2d::CCPoint(-70.0f, 0.0f);
    createCustomToggleValueControl(0, !disabled && degrees == 0.0f, 1, "Default",
        cocos2d::CCPoint(1.0f, 0.0f), &defaultPos, 0, 0);

    cocos2d::CCPoint customPos = toggleBase;
    createCustomToggleValueControl(2, !disabled && degrees != 0.0f, 1, "Custom",
        cocos2d::CCPoint(1.0f, 0.0f), &customPos, 0, 0);

    cocos2d::CCPoint disablePos = toggleBase + cocos2d::CCPoint(70.0f, 0.0f);
    createCustomToggleValueControl(1, disabled, 1, "Disable",
        cocos2d::CCPoint(1.0f, 0.0f), &disablePos, 0, 0);

    cocos2d::CCPoint degreesPos = toggleBase + cocos2d::CCPoint(0.0f, -60.0f);
    createValueControlAdvanced(
        97, "Degrees/s:", cocos2d::CCPoint(0.8f, -360.0f), 360.0f, &degreesPos,
        (InputValueType)0, 2, 4, 0.0f, 0.0f, 0, 0, (GJInputStyle)2, 0, 3);

    toggleGroup(2, !disabled && degrees != 0.0f);

    postSetup();

    return true;
}

void SetupRotatePopup::onClose(cocos2d::CCObject* sender)
{
    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EnhancedGameObject* obj = (EnhancedGameObject*)m_gameObjects->objectAtIndex(i);
            obj->refreshRotateAction();
        }
    }

    SetupTriggerPopup::onClose(sender);
}

void SetupRotatePopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    updateValue(97, 0.0f);

    if (tag == 1)
        updateValue(98, 1.0f);
    else if (tag == 2 || tag == 0)
        updateValue(98, 0.0f);

    toggleGroup(2, tag == 2);
    updateCustomToggleTrigger(0, tag == 0);
    updateCustomToggleTrigger(2, tag == 2);
    updateCustomToggleTrigger(1, tag == 1);
}

SetupRotatePopup::~SetupRotatePopup()
{
}