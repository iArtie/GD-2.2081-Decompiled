#include "Layers/SetItemIDLayer.h"
#include "Objects/EffectGameObject.h"
#include "Objects/LabelGameObject.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"

SetItemIDLayer* SetItemIDLayer::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    SetItemIDLayer* layer = new SetItemIDLayer();
    if (layer && layer->init(object, objects)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SetItemIDLayer::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 260.0f, 1)) {
        return false;
    }

    SetupTriggerPopup::preSetup();
    SetupTriggerPopup::addTitle("Counter Label");
    SetupTriggerPopup::addCloseButton("OK");
    SetupTriggerPopup::addHelpButton(
        "Help",
        "Displays a set <cg>counter</c> value depending on the setting. By default it displays <cy>Item ID</c>.\n"
        "Use <cl>Time Counter</c> to make it function as a timer.\n"
        "<cp>MainTime</c> shows the exact game time.\n"
        "Use <cr>Points</c> to show linked collectables.\n"
        "<co>Attempts</c> displays the current attempt.\n"
        "<ca>Left and Right Align</c> changes the alignment of the display.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f);

    SetupTriggerPopup::createValueControlAdvanced(
        80,
        "ItemID",
        cocos2d::CCPoint(0.8f, 0.0f),
        100.0f,
        true,
        center + cocos2d::CCPoint(-60.0f, 50.0f),
        GameObjectValueType::Float,
        3,
        6,
        false,
        false,
        1,
        0,
        GoldLabel,
        0,
        2);

    SetupTriggerPopup::createToggleValueControl(
        466,
        "Time\nCounter",
        cocos2d::CCPoint(1.0f, 1.0f),
        true,
        center + cocos2d::CCPoint(-60.0f, 0.0f),
        false,
        false,
        0.0f);

    SetupTriggerPopup::createToggleValueControl(
        389,
        "Seconds\nOnly",
        cocos2d::CCPoint(1.0f, 1.0f),
        true,
        center + cocos2d::CCPoint(60.0f, 0.0f),
        false,
        false,
        0.0f);

    int alignValue = (int)SetupTriggerPopup::getValue(391);

    SetupTriggerPopup::createCustomToggleValueControl(
        1,
        alignValue == 1,
        true,
        "Left\nAlign",
        cocos2d::CCPoint(0.0f, 0.0f),
        center + cocos2d::CCPoint(60.0f, 80.0f),
        false,
        0);

    SetupTriggerPopup::createCustomToggleValueControl(
        2,
        alignValue == 2,
        true,
        "Right\nAlign",
        cocos2d::CCPoint(0.0f, 0.0f),
        center + cocos2d::CCPoint(60.0f, 40.0f),
        false,
        0);

    int counterMode = (int)SetupTriggerPopup::getValue(390);

    SetupTriggerPopup::createCustomToggleValueControl(
        -1,
        counterMode == -1,
        true,
        "MainTime",
        cocos2d::CCPoint(0.0f, 0.0f),
        center + cocos2d::CCPoint(-90.0f, -60.0f),
        true,
        0);

    SetupTriggerPopup::createCustomToggleValueControl(
        -2,
        counterMode == -2,
        true,
        "Points",
        cocos2d::CCPoint(0.0f, 0.0f),
        center + cocos2d::CCPoint(-15.0f, -60.0f),
        true,
        0);

    SetupTriggerPopup::createCustomToggleValueControl(
        -3,
        counterMode == -3,
        true,
        "Attempts",
        cocos2d::CCPoint(0.0f, 0.0f),
        center + cocos2d::CCPoint(60.0f, -60.0f),
        true,
        0);

    SetupTriggerPopup::postSetup();

    return true;
}

void SetItemIDLayer::onClose(cocos2d::CCObject* sender) {
    updateEditorLabel();
    SetupTriggerPopup::onClose(sender);
}

void SetItemIDLayer::onCustomToggleTriggerValue(cocos2d::CCObject* sender) {
    int tag = sender->getTag();

    if (tag >= 1 && tag <= 2) {
        int currentAlign = (int)SetupTriggerPopup::getValue(391);

        float newValue;
        bool setLeft = false;
        bool setRight = false;

        if (tag == currentAlign) {
            newValue = 0.0f;
        } else {
            newValue = (float)tag;
            setLeft = (tag == 1);
            setRight = (tag == 2);
        }

        SetupTriggerPopup::updateValue(391, newValue);
        SetupTriggerPopup::updateCustomToggleTrigger(1, setLeft);
        SetupTriggerPopup::updateCustomToggleTrigger(2, setRight);
    } else {
        int currentMode = (int)SetupTriggerPopup::getValue(390);

        float newValue;
        bool setMainTime = false;
        bool setPoints = false;
        bool setAttempts = false;

        if (tag == currentMode) {
            newValue = 0.0f;
        } else {
            newValue = (float)tag;
            setMainTime = (tag == -1);
            setPoints = (tag == -2);
            setAttempts = (tag == -3);
        }

        SetupTriggerPopup::updateValue(390, newValue);
        SetupTriggerPopup::updateCustomToggleTrigger(-1, setMainTime);
        SetupTriggerPopup::updateCustomToggleTrigger(-2, setPoints);
        SetupTriggerPopup::updateCustomToggleTrigger(-3, setAttempts);
        SetupTriggerPopup::updateValue(80, 0.0f);
    }
}

void SetItemIDLayer::updateEditorLabel() {
    cocos2d::CCArray* objects = SetupTriggerPopup::getObjects();
    for (unsigned int i = 0; i < objects->count(); i++) {
        LabelGameObject* labelObject = (LabelGameObject*)objects->objectAtIndex(i);
        labelObject->updatePreviewLabel();
    }
}

void SetItemIDLayer::valueDidChange(int tag, float value) {
    if (tag == 391) {
        cocos2d::CCArray* objects = SetupTriggerPopup::getObjects();
        for (unsigned int i = 0; i < objects->count(); i++) {
            LabelGameObject* labelObject = (LabelGameObject*)objects->objectAtIndex(i);
            labelObject->updateLabelAlign((int)value);
        }
    } else if (tag == 390) {
        SetupTriggerPopup::toggleGroup(this);
    }
}

SetItemIDLayer::~SetItemIDLayer() {
}