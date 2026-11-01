#include "Popups/Setup/SetupTeleportPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Cocos/CCMenuItemToggler.h"

void SetupTeleportPopup::onTeleportGravity(cocos2d::CCObject* sender)
{
    float currentValue = getValue(354);
    int tag = sender->getTag();
    float newValue;

    if ((int)currentValue == tag)
    {
        newValue = 0.0f;
        tag = 0;
    }
    else
    {
        newValue = (float)tag;
    }

    cocos2d::CCObject* value = new cocos2d::CCObject();
    value->m_uID = (int)newValue;
    value->autorelease();

    m_triggerValues->setObject(value, "354");
    valueChanged(354, newValue);
    updateTeleportGravityState(tag);
}

void SetupTeleportPopup::updateDefaultTriggerValues()
{
    SetupTriggerPopup::updateDefaultTriggerValues();

    float value = getValue(354);
    updateTeleportGravityState((int)value);
}

void SetupTeleportPopup::updateTeleportGravityState(int state)
{
    if (!m_teleportGravityToggles)
        return;

    for (unsigned int i = 0; i < m_teleportGravityToggles->count(); i++)
    {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_teleportGravityToggles->objectAtIndex(i);
        toggler->toggle(toggler->getTag() == state);
    }
}

void SetupTeleportPopup::valueDidChange(int tag, float value)
{
    if (tag == 345)
    {
        if (value != 0.0f)
            updateValue(347, 0.0f);
        toggleGroup(1, value != 0.0f);
    }
    else if (tag == 347)
    {
        if (value != 0.0f)
            updateValue(345, 0.0f);
        toggleGroup(2, value != 0.0f);
    }
}

SetupTeleportPopup::~SetupTeleportPopup()
{
}