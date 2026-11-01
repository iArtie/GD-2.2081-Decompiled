#include "SetupGameplayOffsetPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"

SetupGameplayOffsetPopup* SetupGameplayOffsetPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupGameplayOffsetPopup* instance = new SetupGameplayOffsetPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupGameplayOffsetPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    bool toggle1 = false;
    bool toggle2 = false;

    if (tag == (int)getValue(101))
    {
        tag = 0;
    }
    else
    {
        toggle1 = (tag == 1);
        toggle2 = (tag == 2);
    }

    updateCustomToggleTrigger(1, toggle1);
    updateCustomToggleTrigger(2, toggle2);
    updateValue(101, (float)tag);
}

void SetupGameplayOffsetPopup::onDefaultValues(cocos2d::CCObject* sender)
{
    updateValue(28, 75.0f);
    updateValue(29, 75.0f);
}

float SetupGameplayOffsetPopup::triggerSliderValueFromValue(int tag, float value)
{
    if (shouldModifyValueGPO(tag))
        value /= 3.0f;
    return SetupTriggerPopup::triggerSliderValueFromValue(tag, value);
}

float SetupGameplayOffsetPopup::triggerValueFromSliderValue(int tag, float value)
{
    float result = SetupTriggerPopup::triggerValueFromSliderValue(tag, value);
    if (shouldModifyValueGPO(tag))
        return floorf(result) * 3.0f;
    return result;
}

void SetupGameplayOffsetPopup::updateInputNode(int tag, float value)
{
    if (shouldModifyValueGPO(tag))
        value /= 3.0f;
    SetupTriggerPopup::updateInputNode(tag, value);
}

void SetupGameplayOffsetPopup::updateInputValue(int tag, float* value)
{
    if (shouldModifyValueGPO(tag))
        *value *= 3.0f;
}

SetupGameplayOffsetPopup::~SetupGameplayOffsetPopup()
{
}