#include "Popups/Setup/SetupCameraOffsetTrigger.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/CameraTriggerGameObject.h"
#include "Objects/EffectGameObject.h"
#include "Utils/GameToolbox.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Cocos/CCTextInputNode.h"
#include "Popups/ConfigureValuePopup.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"

SetupCameraOffsetTrigger* SetupCameraOffsetTrigger::create(CameraTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraOffsetTrigger* instance = new SetupCameraOffsetTrigger();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCameraOffsetTrigger::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    m_cameraEasingType = (EasingType)-99999;
    m_offsetX = -99999;
    m_offsetY = -99999;
    m_moveTime = -99999.0f;
    m_cameraEasingRate = -99999.0f;

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    float moveTime = -99999.0f;
    float easingRate = -99999.0f;
    int offsetX = -99999;
    int offsetY = -99999;
    int targetMode = -99999;
    EasingType easingType = (EasingType)-99999;

    int moveTimeConflict = 0;
    int easingRateConflict = 0;
    int offsetXConflict = 0;
    int offsetYConflict = 0;
    int targetModeConflict = 0;
    int easingConflict = 0;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!moveTimeConflict)
        {
            if (moveTime == -99999.0f)
                moveTime = obj->m_duration;
            else if (moveTime != obj->m_duration)
            {
                moveTimeConflict = 1;
                moveTime = -99999.0f;
            }
        }

        if (!easingConflict)
        {
            if (easingType == (EasingType)-99999)
                easingType = obj->m_easingType;
            else if (easingType != obj->m_easingType)
            {
                easingType = (EasingType)-99999;
                easingConflict = 1;
            }
        }

        if (!easingRateConflict)
        {
            if (easingRate == -99999.0f)
                easingRate = obj->m_easingRate;
            else if (easingRate != obj->m_easingRate)
            {
                easingRateConflict = 1;
                easingRate = -99999.0f;
            }
        }

        if (!offsetXConflict)
        {
            if (offsetX == -99999)
                offsetX = (int)obj->m_moveOffset.x;
            else if (offsetX != (int)obj->m_moveOffset.x)
            {
                offsetX = -99999;
                offsetXConflict = 1;
            }
        }

        if (!offsetYConflict)
        {
            if (offsetY == -99999)
                offsetY = (int)obj->m_moveOffset.y;
            else if (offsetY != (int)obj->m_moveOffset.y)
            {
                offsetY = -99999;
                offsetYConflict = 1;
            }
        }

        if (!targetModeConflict)
        {
            if (targetMode == -99999)
                targetMode = obj->m_moveTargetMode;
            else if (targetMode != obj->m_moveTargetMode)
            {
                targetModeConflict = 1;
                targetMode = -99999;
            }
        }
    }

    if (moveTime != -99999.0f)
        m_moveTime = moveTime;
    if (easingType != (EasingType)-99999)
        m_cameraEasingType = easingType;
    if (easingRate != -99999.0f)
        m_cameraEasingRate = easingRate;
    if (offsetX != -99999)
        m_offsetX = offsetX;
    if (offsetY != -99999)
        m_offsetY = offsetY;
    if (targetMode != -99999)
        m_targetMode = targetMode;
}

void SetupCameraOffsetTrigger::onClose(cocos2d::CCObject* sender)
{
    m_moveTimeInput->m_delegate = nullptr;
    m_moveTimeInput->onClickTrackNode(false);
    m_offsetXInput->m_delegate = nullptr;
    m_offsetXInput->onClickTrackNode(false);
    m_offsetYInput->m_delegate = nullptr;
    m_offsetYInput->onClickTrackNode(false);
    SetupTriggerPopup::onClose(sender);
}

void SetupCameraOffsetTrigger::onEasing(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 1)
        m_cameraEasingType = (EasingType)((int)m_cameraEasingType + 1);
    else
        m_cameraEasingType = (EasingType)((int)m_cameraEasingType - 1);

    if ((int)m_cameraEasingType < 0)
        m_cameraEasingType = (EasingType)0;
    if ((int)m_cameraEasingType > (int)EasingType::BackOut)
        m_cameraEasingType = EasingType::BackOut;

    updateMoveCommandEasing();
    updateEasingLabel();
    toggleEasingRateVisibility();
}

void SetupCameraOffsetTrigger::onEasingRate(cocos2d::CCObject* sender)
{
    ConfigureValuePopup* popup = ConfigureValuePopup::create(
        this, m_cameraEasingRate, 0.1f, 4.0f, "Easing Rate", "", 0);
    popup->show();
}

void SetupCameraOffsetTrigger::onTargetMode(cocos2d::CCObject* sender)
{
    if (m_targetMode == sender->getTag())
        m_targetMode = 0;
    else
        m_targetMode = sender->getTag();

    if (m_gameObject)
    {
        m_gameObject->m_moveTargetMode = m_targetMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_moveTargetMode = m_targetMode;
        }
    }

    m_xOnlyToggler->toggle(m_targetMode == 1);
    m_yOnlyToggler->toggle(m_targetMode == 2);
}

int SetupCameraOffsetTrigger::posFromSliderValue(float value)
{
    return (int)(floorf((value - 0.5f) * 200.0f) * 3.0f);
}

void SetupCameraOffsetTrigger::sliderChanged(cocos2d::CCObject* sender)
{
    m_moveTime = ((SliderThumb*)sender)->getValue() * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

float SetupCameraOffsetTrigger::sliderValueFromPos(int pos)
{
    float value = floorf((float)pos / 3.0f) / 200.0f + 0.5f;
    if (value >= 1.0f)
        return 1.0f;
    if (value <= 0.0f)
        return 0.0f;
    return value;
}

void SetupCameraOffsetTrigger::sliderXChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();
    m_offsetX = posFromSliderValue(value);
    updateValueXLabel();
    updateMoveCommandPosX();
}

void SetupCameraOffsetTrigger::sliderYChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();
    m_offsetY = posFromSliderValue(value);
    updateValueYLabel();
    updateMoveCommandPosY();
}

void SetupCameraOffsetTrigger::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    int tag = node->getTag();

    if (tag == 1)
    {
        m_offsetY = (int)((float)atoi(node->getString().c_str()) * 3.0f);
        updateMoveCommandPosY();
        float value = sliderValueFromPos(m_offsetY);
        m_offsetYSlider->setValue(value < 1.0f ? value : 1.0f);
    }
    else if (tag == 3)
    {
        m_moveTime = cocos2d::CCString::createWithFormat("%.02f", atof(node->getString().c_str()))->floatValue();
        updateDuration();
        float value = m_moveTime / 10.0f;
        if (value >= 1.0f)
            value = 1.0f;
        m_moveTimeSlider->setValue(value);
    }
    else if (tag == 0)
    {
        m_offsetX = (int)((float)atoi(node->getString().c_str()) * 3.0f);
        updateMoveCommandPosX();
        float value = sliderValueFromPos(m_offsetX);
        m_offsetXSlider->setValue(value < 1.0f ? value : 1.0f);
    }
}

void SetupCameraOffsetTrigger::toggleEasingRateVisibility()
{
    bool visible = GameToolbox::isRateEasing(m_cameraEasingType);

    if (!visible && m_gameObjects && m_gameObjects->count() > 0)
    {
        visible = true;
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            if (!GameToolbox::isRateEasing(obj->m_easingType))
            {
                visible = false;
                break;
            }
        }
    }

    m_cameraEasingRateButton->setVisible(visible);
}

void SetupCameraOffsetTrigger::updateDurLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_moveTime == -99999.0f)
        text = "Mixed";
    else if (!decimals && m_moveTime - (float)(int)m_moveTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_moveTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_moveTime)->getCString();

    m_moveTimeInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupCameraOffsetTrigger::updateDuration()
{
    if (m_gameObject)
    {
        m_gameObject->m_duration = m_moveTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_moveTime;
        }
    }
}

void SetupCameraOffsetTrigger::updateEasingLabel()
{
    std::string text = "Mixed";
    if (m_cameraEasingType != (EasingType)-99999)
        text = GameToolbox::easeToText(m_cameraEasingType);

    m_cameraEasingLabel->setString(text.c_str());
    m_cameraEasingLabel->limitLabelWidth(150.0f, 0.7f, 0.0f);
}

void SetupCameraOffsetTrigger::updateEasingRateLabel()
{
    std::string text = cocos2d::CCString::createWithFormat("%.02f", m_cameraEasingRate)->getCString();
    if (m_cameraEasingRate == -99999.0f)
        text = "";

    m_cameraEasingRateLabel->setString(text.c_str());
}

void SetupCameraOffsetTrigger::updateMoveCommandEasing()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingType = m_cameraEasingType;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingType = m_cameraEasingType;
        }
    }
}

void SetupCameraOffsetTrigger::updateMoveCommandEasingRate()
{
    if (m_gameObject)
    {
        m_gameObject->m_easingRate = m_cameraEasingRate;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_easingRate = m_cameraEasingRate;
        }
    }
}

void SetupCameraOffsetTrigger::updateMoveCommandPosX()
{
    if (m_gameObject)
    {
        m_gameObject->m_moveOffset = cocos2d::CCPoint((float)m_offsetX, m_gameObject->m_moveOffset.y);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_moveOffset = cocos2d::CCPoint((float)m_offsetX, obj->m_moveOffset.y);
        }
    }
}

void SetupCameraOffsetTrigger::updateMoveCommandPosY()
{
    if (m_gameObject)
    {
        m_gameObject->m_moveOffset = cocos2d::CCPoint(m_gameObject->m_moveOffset.x, (float)m_offsetY);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_moveOffset = cocos2d::CCPoint(obj->m_moveOffset.x, (float)m_offsetY);
        }
    }
}

void SetupCameraOffsetTrigger::updateValueXLabel()
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_offsetX == -99999)
        text = "";
    else
        text = cocos2d::CCString::createWithFormat("%i", (int)floorf((float)m_offsetX / 3.0f))->getCString();

    m_offsetXInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupCameraOffsetTrigger::updateValueYLabel()
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_offsetY == -99999)
        text = "";
    else
        text = cocos2d::CCString::createWithFormat("%i", (int)floorf((float)m_offsetY / 3.0f))->getCString();

    m_offsetYInput->setString(text);
    m_disableTextDelegate = false;
}

SetupCameraOffsetTrigger::~SetupCameraOffsetTrigger()
{
}