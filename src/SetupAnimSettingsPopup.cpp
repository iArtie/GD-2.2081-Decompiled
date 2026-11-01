#include "SetupAnimSettingsPopup.h"
#include "EffectGameObject.h"
#include "EnhancedGameObject.h"

SetupAnimSettingsPopup* SetupAnimSettingsPopup::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    SetupAnimSettingsPopup* popup = new SetupAnimSettingsPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAnimSettingsPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 400.0f, 240.0f, 1)) {
        return false;
    }

    m_gameObject = object;
    m_gameObjects = objects;

    SetupTriggerPopup::preSetup();
    SetupTriggerPopup::addTitle("Edit Animation Settings");
    SetupTriggerPopup::addCloseButton("OK");
    SetupTriggerPopup::addHelpButton(
        "Help",
        "<cg>Disable Anim Shine</c> removes the white flash at the start of the animation for some animated objects.\n"
        "<cy>Use Speed</c> lets you set a custom animation speed.\n"
        "Use <cl>Animate On Trigger</c> to toggle on with Animate trigger.\n"
        "<cp>Only If Active</c> only does this if the object is active.\n"
        "<cr>Single Frame</c> lets you select a specific frame of the animation.\n"
        "<co>Offset Anim</c> will offset the animation the value of <cr>single frame</c>.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint toggleBase = center + cocos2d::CCPoint(-40.0f, 60.0f);

    SetupTriggerPopup::createToggleValueControl(
        106, "Randomize\nStart",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(-125.0f, 0.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        122, "Use\nSpeed",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(0.0f, 0.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        123, "Animate\non Trigger",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(125.0f, 0.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        126, "Disable\ndelayed\nloop",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(-125.0f, -40.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        127, "Disable\nAnimShine",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(0.0f, -40.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createToggleValueControl(
        214, "Only if\nactive",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(125.0f, -40.0f),
        false, false, 0.0f);

    SetupTriggerPopup::createValueControlAdvanced(
        107, "Speed:",
        cocos2d::CCPoint(0.8f, -2.0f), 2.0f, true,
        center + cocos2d::CCPoint(-100.0f, -30.0f),
        InputValueType::Float, 1, 6,
        false, false, 0, 0, GoldLabel, 0, 2);

    SetupTriggerPopup::createValueControlAdvanced(
        462, "Single Frame:",
        cocos2d::CCPoint(0.8f, 0.0f), 20.0f, true,
        center + cocos2d::CCPoint(100.0f, -45.0f),
        InputValueType::Float, 3, 3,
        false, false, 1, 0, GoldLabel, 0, 2);

    SetupTriggerPopup::createToggleValueControl(
        592, "Offset\nAnim",
        cocos2d::CCPoint(1.0f, 0.0f), true,
        toggleBase + cocos2d::CCPoint(125.0f, -155.0f),
        false, false, 0.0f);

    SetupTriggerPopup::postSetup();

    return true;
}

void SetupAnimSettingsPopup::onClose(cocos2d::CCObject* sender) {
    updateAnimSettings();
    SetupTriggerPopup::onClose(sender);
}

void SetupAnimSettingsPopup::updateAnimSettings() {
    if (m_gameObject) {
        m_gameObject->setupAnimationVariables();
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            EnhancedGameObject* obj = (EnhancedGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setupAnimationVariables();
        }
    }
}

SetupAnimSettingsPopup::~SetupAnimSettingsPopup() {
}