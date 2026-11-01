#include "Popups/Setup/SetupOptionsTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/GameOptionsTrigger.h"
#include "CCLabelBMFont.h"

void SetupOptionsTriggerPopup::addOption(int property, std::string label)
{
    int state = (int)getValue(property);

    cocos2d::CCLabelBMFont* labelNode = cocos2d::CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    labelNode->setScale(0.3f);
    labelNode->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(labelNode);
    labelNode->setPosition(m_togglePosition + cocos2d::CCPoint(-100.0f, 0.0f));
    addObjectToPage(labelNode, m_togglePage);

    createCustomToggleValueControl(
        property, state == 1, 1, "On",
        cocos2d::CCPoint(1.0f, 0.0f), &(m_togglePosition + cocos2d::CCPoint(40.0f, 0.0f)), 0, m_togglePage);

    createCustomToggleValueControl(
        -property, state == -1, 1, "Off",
        cocos2d::CCPoint(1.0f, 0.0f), &(m_togglePosition + cocos2d::CCPoint(100.0f, 0.0f)), 0, m_togglePage);

    m_togglePosition.y -= 30.0f;
}

SetupOptionsTriggerPopup* SetupOptionsTriggerPopup::create(GameOptionsTrigger* object, cocos2d::CCArray* objects)
{
    SetupOptionsTriggerPopup* instance = new SetupOptionsTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupOptionsTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    int property = abs(tag);
    int sign = (tag <= 0) ? -1 : 1;

    float value = 0.0f;
    bool toggleOn = false;
    bool toggleOff = false;

    if (sign != (int)getValue(property))
    {
        value = (float)sign;
        toggleOn = (sign == 1);
        toggleOff = (sign == -1);
    }

    updateValue(property, value);
    updateCustomToggleTrigger(property, toggleOn);
    updateCustomToggleTrigger(-property, toggleOff);
}

SetupOptionsTriggerPopup::~SetupOptionsTriggerPopup()
{
}