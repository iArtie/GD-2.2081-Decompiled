#include "Popups/Setup/SetupObjectOptionsPopup.h"
#include "Objects/GameObject.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"
#include "Layers/SetGroupIDLayer.h"

void SetupObjectOptionsPopup::determineStartValues()
{
    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_effectObject = false;
    m_iceBlock = true;
    m_dontFade = true;
    m_dontEnter = true;
    m_noEffects = true;
    m_noParticle = true;
    m_groupParent = true;
    m_areaParent = true;
    m_highDetail = true;
    m_noTouch = true;
    m_passable = true;
    m_hide = true;
    m_nonStickX = true;
    m_nonStickY = true;
    m_extraSticky = true;
    m_dontBoostY = true;
    m_dontBoostX = true;
    m_scaleStick = true;
    m_gripSlope = true;
    m_noGlow = true;
    m_reverse = true;
    m_singlePlayerTouch = true;
    m_centerEffect = true;
    m_extendedCollision = true;
    m_noAudioScale = true;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_isIceBlock)
            m_iceBlock = false;
        if (!obj->m_isDontFade)
            m_dontFade = false;
        if (!obj->m_isDontEnter)
            m_dontEnter = false;
        if (!obj->m_hasGroupParent)
            m_groupParent = false;
        if (!obj->m_hasAreaParent)
            m_areaParent = false;
        if (!obj->m_isHighDetail)
            m_highDetail = false;
        if (!obj->m_hasNoEffects)
            m_noEffects = false;
        if (!obj->m_hasNoParticles)
            m_noParticle = false;
        if (!obj->m_isNoTouch)
            m_noTouch = false;
        if (!obj->m_isPassable)
            m_passable = false;
        if (!obj->m_isHide)
            m_hide = false;
        if (!obj->m_isExtendedCollision)
            m_extendedCollision = false;
        if (!obj->m_hasNoAudioScale)
            m_noAudioScale = false;
        if (!obj->m_isNonStickX)
            m_nonStickX = false;
        if (!obj->m_isNonStickY)
            m_nonStickY = false;
        if (!obj->m_isExtraSticky)
            m_extraSticky = false;
        if (!obj->m_isDontBoostY)
            m_dontBoostY = false;
        if (!obj->m_isDontBoostX)
            m_dontBoostX = false;
        if (!obj->m_isScaleStick)
            m_scaleStick = false;
        if (!obj->m_isGripSlope)
            m_gripSlope = false;
        if (!obj->m_hasNoGlow)
            m_noGlow = false;

        if (obj->m_classType == GameObjectClassType::Effect)
        {
            m_effectObject = true;
            if (!obj->m_reverse)
                m_reverse = false;
            if (!obj->m_singlePlayerTouch)
                m_singlePlayerTouch = false;
            if (!obj->m_centerEffect)
                m_centerEffect = false;
        }
    }
}

void SetupObjectOptionsPopup::keyBackClicked()
{
    onClose(nullptr);
}

void SetupObjectOptionsPopup::onAlwaysHide(cocos2d::CCObject* sender)
{
    m_hide = !m_hide;

    if (m_gameObject)
    {
        m_gameObject->m_isHide = m_hide;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isHide = m_hide;
        }
    }
}

void SetupObjectOptionsPopup::onApplyScaleStick(cocos2d::CCObject* sender)
{
    m_scaleStick = !m_scaleStick;

    if (m_gameObject)
    {
        m_gameObject->m_isScaleStick = m_scaleStick;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isScaleStick = m_scaleStick;
        }
    }
}

void SetupObjectOptionsPopup::onCenterEffect(cocos2d::CCObject* sender)
{
    m_centerEffect = !m_centerEffect;

    if (m_gameObject)
    {
        if (m_gameObject->m_classType == GameObjectClassType::Effect)
            m_gameObject->m_centerEffect = m_centerEffect;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            if (obj->m_classType == GameObjectClassType::Effect)
                obj->m_centerEffect = m_centerEffect;
        }
    }
}

void SetupObjectOptionsPopup::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetupObjectOptionsPopup::onDisableGlow(cocos2d::CCObject* sender)
{
    m_noGlow = !m_noGlow;

    if (m_gameObject)
    {
        m_gameObject->m_hasNoGlow = m_noGlow;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasNoGlow = m_noGlow;
        }
    }
}

void SetupObjectOptionsPopup::onDisableObject(cocos2d::CCObject* sender)
{
    m_noTouch = !m_noTouch;

    LevelEditorLayer* editorLayer = GameManager::sharedState()->m_levelEditorLayer;

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (!m_gameObjects)
        return;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);

        if (editorLayer)
        {
            editorLayer->removeObjectFromSection(obj);
            GameObjectType savedType = obj->m_savedObjectType;
            obj->m_isNoTouch = m_noTouch;
            obj->setType(savedType);
            obj->saveActiveColors();
            editorLayer->addToSection(obj);
        }
        else
        {
            obj->m_isNoTouch = m_noTouch;
            obj->setType(obj->m_savedObjectType);
            obj->saveActiveColors();
        }
    }
}

void SetupObjectOptionsPopup::onDontBoost(cocos2d::CCObject* sender)
{
    m_dontBoostY = !m_dontBoostY;

    if (m_gameObject)
    {
        m_gameObject->m_isDontBoostY = m_dontBoostY;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isDontBoostY = m_dontBoostY;
        }
    }
}

void SetupObjectOptionsPopup::onDontBoostX(cocos2d::CCObject* sender)
{
    m_dontBoostX = !m_dontBoostX;

    if (m_gameObject)
    {
        m_gameObject->m_isDontBoostX = m_dontBoostX;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isDontBoostX = m_dontBoostX;
        }
    }
}

void SetupObjectOptionsPopup::onDontEnter(cocos2d::CCObject* sender)
{
    m_dontEnter = !m_dontEnter;

    if (m_gameObject)
    {
        m_gameObject->m_isDontEnter = m_dontEnter;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isDontEnter = m_dontEnter;
        }
    }
}

void SetupObjectOptionsPopup::onDontFade(cocos2d::CCObject* sender)
{
    m_dontFade = !m_dontFade;

    if (m_gameObject)
    {
        m_gameObject->m_isDontFade = m_dontFade;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isDontFade = m_dontFade;
        }
    }
}

void SetupObjectOptionsPopup::onExtendedCollision(cocos2d::CCObject* sender)
{
    LevelEditorLayer* editorLayer = GameManager::sharedState()->m_levelEditorLayer;
    if (!editorLayer)
        return;

    m_extendedCollision = !m_extendedCollision;

    if (m_gameObject)
    {
        editorLayer->updateExtendedCollision(m_gameObject, m_extendedCollision);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            editorLayer->updateExtendedCollision(obj, m_extendedCollision);
        }
    }
}

void SetupObjectOptionsPopup::onExtraSticky(cocos2d::CCObject* sender)
{
    m_extraSticky = !m_extraSticky;

    if (m_gameObject)
    {
        m_gameObject->m_isExtraSticky = m_extraSticky;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isExtraSticky = m_extraSticky;
        }
    }
}

void SetupObjectOptionsPopup::onGripSlope(cocos2d::CCObject* sender)
{
    m_gripSlope = !m_gripSlope;

    if (m_gameObject)
    {
        m_gameObject->m_isGripSlope = m_gripSlope;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isGripSlope = m_gripSlope;
        }
    }
}

void SetupObjectOptionsPopup::onHideEffects(cocos2d::CCObject* sender)
{
    m_noEffects = !m_noEffects;

    if (m_gameObject)
    {
        m_gameObject->m_hasNoEffects = m_noEffects;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasNoEffects = m_noEffects;
        }
    }
}

void SetupObjectOptionsPopup::onHighDetail(cocos2d::CCObject* sender)
{
    m_highDetail = !m_highDetail;

    if (m_gameObject)
    {
        m_gameObject->m_isHighDetail = m_highDetail;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isHighDetail = m_highDetail;
        }
    }
}

void SetupObjectOptionsPopup::onIceBlock(cocos2d::CCObject* sender)
{
    m_iceBlock = !m_iceBlock;

    if (m_gameObject)
    {
        m_gameObject->m_isIceBlock = m_iceBlock;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isIceBlock = m_iceBlock;
        }
    }
}

void SetupObjectOptionsPopup::onNoAudioScale(cocos2d::CCObject* sender)
{
    m_noAudioScale = !m_noAudioScale;

    if (m_gameObject)
    {
        m_gameObject->m_hasNoAudioScale = m_noAudioScale;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasNoAudioScale = m_noAudioScale;
        }
    }
}

void SetupObjectOptionsPopup::onNoParticle(cocos2d::CCObject* sender)
{
    m_noParticle = !m_noParticle;

    if (m_gameObject)
    {
        m_gameObject->m_hasNoParticles = m_noParticle;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasNoParticles = m_noParticle;
        }
    }
}

void SetupObjectOptionsPopup::onNonStickX(cocos2d::CCObject* sender)
{
    m_nonStickX = !m_nonStickX;

    if (m_gameObject)
    {
        m_gameObject->m_isNonStickX = m_nonStickX;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isNonStickX = m_nonStickX;
        }
    }
}

void SetupObjectOptionsPopup::onNonStickY(cocos2d::CCObject* sender)
{
    m_nonStickY = !m_nonStickY;

    if (m_gameObject)
    {
        m_gameObject->m_isNonStickY = m_nonStickY;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isNonStickY = m_nonStickY;
        }
    }
}

void SetupObjectOptionsPopup::onPassable(cocos2d::CCObject* sender)
{
    m_passable = !m_passable;

    if (m_gameObject)
    {
        m_gameObject->m_isPassable = m_passable;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_isPassable = m_passable;
        }
    }
}

void SetupObjectOptionsPopup::onReverse(cocos2d::CCObject* sender)
{
    if (m_groupIDLayer)
        m_groupIDLayer->m_reverseChanged = true;

    m_reverse = !m_reverse;

    if (m_gameObject)
    {
        if (m_gameObject->m_classType == GameObjectClassType::Effect)
            m_gameObject->m_reverse = m_reverse;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            if (obj->m_classType == GameObjectClassType::Effect)
                obj->m_reverse = m_reverse;
        }
    }
}

void SetupObjectOptionsPopup::onSinglePlayerTouch(cocos2d::CCObject* sender)
{
    m_singlePlayerTouch = !m_singlePlayerTouch;

    if (m_gameObject)
    {
        if (m_gameObject->m_classType == GameObjectClassType::Effect)
            m_gameObject->m_singlePlayerTouch = m_singlePlayerTouch;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            if (obj->m_classType == GameObjectClassType::Effect)
                obj->m_singlePlayerTouch = m_singlePlayerTouch;
        }
    }
}

void SetupObjectOptionsPopup::onToggleAreaParent(cocos2d::CCObject* sender)
{
    m_areaParent = !m_areaParent;

    if (m_gameObject)
    {
        m_gameObject->m_hasAreaParent = m_areaParent;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasAreaParent = m_areaParent;
        }
    }
}

void SetupObjectOptionsPopup::onToggleGroupParent(cocos2d::CCObject* sender)
{
    m_groupParent = !m_groupParent;

    if (m_gameObject)
    {
        m_gameObject->m_hasGroupParent = m_groupParent;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hasGroupParent = m_groupParent;
        }
    }
}

void SetupObjectOptionsPopup::show()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();

    if (!m_scene)
        m_scene = runningScene;
    if (!m_ZOrder)
        m_ZOrder = 105;

    m_scene->addChild(this, m_ZOrder);
}

SetupObjectOptionsPopup::~SetupObjectOptionsPopup()
{
}