#include "Popups/UIObjectSettingsPopup.h"
#include "Objects/UISettingsGameObject.h"
#include "Utils/GameToolbox.h"

UIObjectSettingsPopup* UIObjectSettingsPopup::create(UISettingsGameObject* object, cocos2d::CCArray* objects)
{
    UIObjectSettingsPopup* popup = new UIObjectSettingsPopup();

    if (popup->init(object, objects))
    {
        popup->autorelease();
        return popup;
    }

    delete popup;
    return nullptr;
}

bool UIObjectSettingsPopup::init(UISettingsGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    addTitle("UI Object Settings");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Group ID</c> specify the objects to use for the UI. This is permanent from the start and cannot be edited.\n"
        "<cy>UI Target</c> is a single object used as a reference for the camera center. Camera Guide is a recommended tool"
        ".\n"
        "<cl>XRef and YRef</c> are reference points for aligning the object.\n"
        "<cp>Relative</c> makes UI positions scale with aspect ratio.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    createValueControlAdvanced(
        51,
        "Group ID:",
        cocos2d::CCPoint(0.8f, 0.0f),
        9999.0f,
        center + cocos2d::CCPoint(-70.0f, 60.0f),
        Float,
        2,
        10,
        false,
        false,
        1,
        0,
        GoldLabel,
        0,
        0);

    createValueControlAdvanced(
        71,
        "UI Target:",
        cocos2d::CCPoint(0.8f, 0.0f),
        9999.0f,
        center + cocos2d::CCPoint(70.0f, 60.0f),
        Float,
        2,
        10,
        false,
        false,
        1,
        0,
        GoldLabel,
        0,
        0);

    cocos2d::CCPoint rowOrigin(winSize.width * 0.5f - 25.0f, winSize.height * 0.5f - 10.0f);

    int xRefValue = (int)getValue(385);
    int yRefValue = (int)getValue(386);

    addInfoLabel("XRef:", 0.5f, cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-125.0f, 0.0f), 0);

    createCustomToggleValueControl(1, xRefValue <= 1, 1, "Auto", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-75.0f, 0.0f), true, 0);
    createCustomToggleValueControl(2, xRefValue == 2, 1, "Center", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-25.0f, 0.0f), true, 0);
    createCustomToggleValueControl(3, xRefValue == 3, 1, "Left", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(25.0f, 0.0f), true, 0);
    createCustomToggleValueControl(4, xRefValue == 4, 1, "Right", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(75.0f, 0.0f), true, 0);
    createToggleValueControl(387, "Relative", cocos2d::CCPoint(1.0f, 0.0f), rowOrigin + cocos2d::CCPoint(150.0f, 0.0f), true, 0, 0.0f);

    rowOrigin.y -= 60.0f;

    addInfoLabel("YRef:", 0.5f, cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-125.0f, 0.0f), 0);

    createCustomToggleValueControl(5, yRefValue == 0 || yRefValue == 5, 1, "Auto", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-75.0f, 0.0f), true, 0);
    createCustomToggleValueControl(6, yRefValue == 6, 1, "Center", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(-25.0f, 0.0f), true, 0);
    createCustomToggleValueControl(7, yRefValue == 7, 1, "Bottom", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(25.0f, 0.0f), true, 0);
    createCustomToggleValueControl(8, yRefValue == 8, 1, "Top", cocos2d::CCPoint(0, 0), rowOrigin + cocos2d::CCPoint(75.0f, 0.0f), true, 0);
    createToggleValueControl(388, "Relative", cocos2d::CCPoint(1.0f, 0.0f), rowOrigin + cocos2d::CCPoint(150.0f, 0.0f), true, 0, 0.0f);

    postSetup();

    return true;
}

void UIObjectSettingsPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    float value = (float)tag;

    if (tag <= 4)
    {
        updateValue(385, value);
        updateCustomToggleTrigger(1, tag == 1);
        updateCustomToggleTrigger(2, tag == 2);
        updateCustomToggleTrigger(3, tag == 3);
        updateCustomToggleTrigger(4, tag == 4);
    }
    else
    {
        updateValue(386, value);
        updateCustomToggleTrigger(5, tag == 5);
        updateCustomToggleTrigger(6, tag == 6);
        updateCustomToggleTrigger(7, tag == 7);
        updateCustomToggleTrigger(8, tag == 8);
    }
}

UIObjectSettingsPopup::~UIObjectSettingsPopup()
{
}