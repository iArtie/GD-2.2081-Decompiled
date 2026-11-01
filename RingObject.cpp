#include "RingObject.h"
#include "GameManager.h"
#include "PlayLayer.h"
#include "CCCircleWave.h"

RingObject* RingObject::create(const char* frame)
{
    RingObject* returnVal = new RingObject();

    if (returnVal->init(frame))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RingObject::init(const char* frame)
{
    if (!EffectGameObject::init(frame))
        return false;

    m_customGlowColor = true;
    m_isTouchTriggered = true;
    return true;
}

void RingObject::customObjectSetup(std::vector<std::string>* values, std::vector<void*>* exists)
{
    EffectGameObject::customObjectSetup(values, exists);

    if (m_objectType != GameObjectType::CustomRing)
        return;

    if (exists->at(51))
        m_targetGroupID = atoi(values->at(51).c_str());
    else
        m_targetGroupID = 0;

    if (exists->at(56))
        m_activateGroup = atoi(values->at(56).c_str());
    else
        m_activateGroup = 0;

    if (exists->at(445))
        m_claimTouch = atoi(values->at(445).c_str()) != 0;
    else
        m_claimTouch = false;

    if (exists->at(504))
        m_isSpawnOnly = atoi(values->at(504).c_str()) != 0;
    else
        m_isSpawnOnly = false;
}

std::string RingObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_objectType != GameObjectType::CustomRing)
        return result;

    if (m_targetGroupID)
    {
        result += ",";
        result += std::to_string(51);
        result += ",";
        result += std::to_string(m_targetGroupID);
    }

    if (m_activateGroup)
    {
        result += ",";
        result += std::to_string(56);
        result += ",";
        result += std::to_string(m_activateGroup);
    }

    if (m_claimTouch)
    {
        result += ",";
        result += std::to_string(445);
        result += ",";
        result += std::to_string(m_claimTouch);
    }

    if (m_isSpawnOnly)
    {
        result += ",";
        result += std::to_string(504);
        result += ",";
        result += std::to_string(m_isSpawnOnly);
    }

    return result;
}

void RingObject::powerOnObject(int state)
{
    EnhancedGameObject::powerOnObject(state);

    if (!m_isRingPoweredOn
        && !GameManager::sharedState()->m_performanceMode
        && !GameManager::sharedState()->m_editorEnabled
        && !m_hasNoEffects)
    {
        spawnCircle();
    }
}

void RingObject::resetObject()
{
    EnhancedGameObject::resetObject();
    powerOffObject();
}

void RingObject::setRScale(float scale)
{
    if (!m_editorEnabled)
    {
        float adjusted = scale + 0.3f;
        if (adjusted >= 1.2f)
            adjusted = 1.2f;
        scale = adjusted;
    }

    GameObject::setRScale(scale);
}

void RingObject::setScale(float scale)
{
    if (!m_editorEnabled)
    {
        float adjusted = scale + 0.3f;
        if (adjusted >= 1.2f)
            adjusted = 1.2f;
        scale = adjusted;
    }

    GameObject::setScale(scale);
}

bool RingObject::shouldDrawEditorHitbox()
{
    return m_objectID == 3643;
}

void RingObject::spawnCircle()
{
    if (!GameManager::sharedState()->m_playLayer || m_hasNoEffects || m_objectID == 3643)
        return;

    CCCircleWave* wave = CCCircleWave::create(5.0f, 55.0f, 0.25f, 0);
    wave->setPosition(getPosition());

    PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;
    playLayer->m_objectLayer->addChild(wave, 0);
    wave->m_circleMode = CircleMode::Outline;

    CCCircleWave::followObject(wave, this, false);
    wave->m_delegate = &playLayer->CCCircleWaveDelegate;
    playLayer->addCircle(wave);
}

void RingObject::triggerActivated(float xPosition)
{
    powerOffObject();
}

RingObject::~RingObject()
{
}