#include "Popups/Setup/SetupGravityModPopup.h"
#include "Objects/EffectGameObject.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"

void SetupGravityModPopup::determineStartValues()
{
    if (m_gameObject)
    {
        m_gravityMod = m_gameObject->m_gravityMod;
        m_itemID = m_gameObject->m_itemID;
    }
    else if (m_gameObjects && m_gameObjects->count() > 0)
    {
        m_itemID = -1;
        float gravityValue = -999.0f;
        m_gravityMod = 1.0f;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

            if (gravityValue == -999.0f)
                gravityValue = obj->m_gravityMod;
            else if (gravityValue != obj->m_gravityMod)
                break;

            int itemID = obj->m_itemID;
            if (m_itemID == -1)
                m_itemID = itemID;
            else if (m_itemID != itemID)
                m_itemID = 0;
        }

        m_gravityMod = gravityValue;
    }
}

void SetupGravityModPopup::keyBackClicked()
{
    onClose(nullptr);
}

void SetupGravityModPopup::onClose(cocos2d::CCObject* sender)
{
    if (m_itemIDInput)
    {
        m_itemIDInput->m_delegate = nullptr;
        m_itemIDInput->onClickTrackNode(false);
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetupGravityModPopup::onItemIDArrow(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_itemID -= 1;
    else
        m_itemID += 1;

    updateItemID();
    updateTextInputLabel();
}

void SetupGravityModPopup::show()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();

    if (!m_scene)
        m_scene = runningScene;
    if (!m_ZOrder)
        m_ZOrder = 105;

    m_scene->addChild(this, m_ZOrder);
}

void SetupGravityModPopup::sliderChanged(cocos2d::CCObject* sender)
{
    float range = m_maxValue - m_minValue;
    float value = ((SliderThumb*)sender)->getValue();
    m_gravityMod = cocos2d::CCString::createWithFormat(
        "%.1f", m_minValue + range * value)->floatValue();

    updateValue();
    updateValueLabel();
}

void SetupGravityModPopup::textChanged(CCTextInputNode* node)
{
    if (m_disableDelegate)
        return;

    m_itemID = atoi(node->getString().c_str());
    updateItemID();
}

void SetupGravityModPopup::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void SetupGravityModPopup::updateItemID()
{
    if (m_itemID < 0)
        m_itemID = 0;
    if (m_itemID > 999)
        m_itemID = 999;

    if (m_gameObject)
    {
        m_gameObject->m_itemID = m_itemID;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_itemID = m_itemID;
        }
    }
}

void SetupGravityModPopup::updateTextInputLabel()
{
    m_disableDelegate = true;

    m_itemIDInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_itemID)->getCString());

    m_disableDelegate = false;
}

void SetupGravityModPopup::updateValue()
{
    if (m_gameObject)
    {
        m_gameObject->m_gravityMod = m_gravityMod;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_gravityMod = m_gravityMod;
        }
    }
}

void SetupGravityModPopup::updateValueLabel()
{
    const char* label = m_gravityTrigger ? "Gravity mod" : "Force";
    m_gravityModLabel->setString(
        cocos2d::CCString::createWithFormat("%s: %.1f", label, m_gravityMod)->getCString());
}

SetupGravityModPopup::~SetupGravityModPopup()
{
}