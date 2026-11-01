#include "ParticleGameObject.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "PlayLayer.h"
#include "LevelEditorLayer.h"

ParticleGameObject* ParticleGameObject::create()
{
    ParticleGameObject* returnVal = new ParticleGameObject();
    if (returnVal->init())
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool ParticleGameObject::init()
{
    bool returnVal = EnhancedGameObject::init("edit_eCParticleBtn_001.png");
    if (returnVal)
    {
        m_bUnkBool2 = false;
        m_hasCustomAnimation = true;
    }
    return returnVal;
}

void ParticleGameObject::customSetup()
{
    EnhancedGameObject::customSetup();
    m_particleOffset = cocos2d::CCPoint(0.0f, 0.0f);
    m_particleLocked = false;
    m_objectType = GameObjectType::Decoration;
    m_isParticleSpriteLocked = false;
    m_unk370 = true;
    m_ignoreFade = true;
    m_ignoreEnter = true;
    m_hasParticles = true;
    m_blackChildOpacityLocked = true;
    m_particleData = "30a-1a1a0.3a30a90a90a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a1a0a1a0a1a0a1a1a0a0a1a0a1a0a1a0a1a0a0a0a0a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0";
}

void ParticleGameObject::addMainSpriteToParent(bool reorder)
{
    GameObject::addMainSpriteToParent(reorder);

    if (m_particle)
    {
        ZLayer objectZLayer = getObjectZLayer();
        cocos2d::CCNode* parent = parentForZLayer(objectZLayer, false, 4);

        if (parent != m_particle->getParent())
        {
            m_particle->removeFromParentAndCleanup(false);
            parent->addChild(m_particle, getObjectZOrder());
        }
        else if (reorder)
        {
            parent->reorderChild(m_particle, getObjectZOrder());
        }
    }
}

void ParticleGameObject::applyParticleSettings(cocos2d::CCParticleSystemQuad* particle)
{
    bool locked = particle->getPositionType() != cocos2d::tCCPositionTypeGrouped;
    m_isParticleSpriteLocked = locked;
    m_particleLocked = locked;

    if (locked)
    {
        particle->setScale(1.0f);
    }
    else
    {
        particle->setScaleX(getScaleX());
        particle->setScaleY(getScaleY());
    }

    if (!m_particleLocked)
    {
        particle->setRotation(getRotation());

        if (!m_particleUseObjectColor)
            goto checkUniform;

        applyStartColor:
        {
            cocos2d::ccColor3B objectColor = getColor();
            cocos2d::ccColor3B finalColor = m_colorSprite ? m_colorSprite->getColor() : objectColor;

            cocos2d::ccColor4F startColor;
            startColor.r = (float)objectColor.r / 255.0f;
            startColor.g = (float)objectColor.g / 255.0f;
            startColor.b = (float)objectColor.b / 255.0f;
            startColor.a = particle->getStartColor().a;
            particle->setStartColor(startColor);

            cocos2d::ccColor4F endColor;
            endColor.r = (float)finalColor.r / 255.0f;
            endColor.g = (float)finalColor.g / 255.0f;
            endColor.b = (float)finalColor.b / 255.0f;
            endColor.a = particle->getEndColor().a;
            particle->setEndColor(endColor);

            particle->setBlendAdditive(m_baseOrDetailBlending);
        }
        return;
    }

    particle->setRotation(0.0f);
    updateParticleAngle(getRotation(), particle);

    if (m_particleUseObjectColor)
        goto applyStartColor;

checkUniform:
    if (m_hasUniformObjectColor)
    {
        cocos2d::ccColor3B objectColor = getColor();
        cocos2d::ccColor3B colorSpriteColor = m_colorSprite ? m_colorSprite->getColor() : objectColor;

        particle->m_tUniformColor.r = (float)objectColor.r / 255.0f;
        particle->m_tUniformColor.g = (float)objectColor.g / 255.0f;
        particle->m_tUniformColor.b = (float)objectColor.b / 255.0f;
        particle->m_tUniformColor.a = 1.0f;

        particle->m_tUniformDeltaColor.r = (float)colorSpriteColor.r / 255.0f;
        particle->m_tUniformDeltaColor.g = (float)colorSpriteColor.g / 255.0f;
        particle->m_tUniformDeltaColor.b = (float)colorSpriteColor.b / 255.0f;
        particle->m_tUniformDeltaColor.a = 1.0f;

        particle->setBlendAdditive(m_baseOrDetailBlending);
    }
}

void ParticleGameObject::blendModeChanged()
{
    if (m_particle && (m_particleUseObjectColor || m_hasUniformObjectColor))
        m_particle->setBlendAdditive(m_baseOrDetailBlending);
}

void ParticleGameObject::claimParticle()
{
    if (m_particle || m_notPreviewing)
        return;

    updateParticleStruct();

    if (m_editorEnabled)
    {
        LevelEditorLayer* editor = GameManager::sharedState()->m_levelEditorLayer;
        m_particle = editor->claimCustomParticle(
            &m_particleData,
            &m_particleStruct,
            (int)getObjectZLayer(),
            getObjectZOrder(),
            0,
            false);
    }
    else
    {
        PlayLayer* play = GameManager::sharedState()->m_playLayer;
        m_particle = play->claimCustomParticle(
            &m_particleData,
            &m_particleStruct,
            (int)getObjectZLayer(),
            getObjectZOrder(),
            m_isUIObject,
            false);
    }

    cocos2d::CCPoint pos = getPosition();
    setPosition(&pos);

    if (m_particle)
    {
        m_hasCustomAnimation = m_particle->getDuration() >= 0.0f;
        if (m_hasCustomAnimation)
        {
            m_particle->stopSystem();
            m_startingRespawn = true;
            m_respawnResult = -1.0f;
        }
        applyParticleSettings(m_particle);
    }
}

void ParticleGameObject::createAndAddCustomParticle()
{
    if (!m_particleData.empty())
    {
        m_hasParticles = true;
        updateParticleStruct();
        m_hasParticles = true;
    }
}

void ParticleGameObject::createParticlePreviewArt()
{
    if (getChildByTag(1))
        return;

    std::string frameName = "edit_eCParticleBtn_001.png";
    cocos2d::CCPoint offset = cocos2d::CCPoint(0.0f, 0.0f);
    cocos2d::CCSprite* sprite = addCustomChild(frameName, offset, 0);
    sprite->setTag(1);

    std::string colorFrameName = "edit_eParticleBtn_color_001.png";
    cocos2d::CCSprite* colorSprite = addInternalCustomColorChild(colorFrameName, offset);
    colorSprite->setTag(1);
}

void ParticleGameObject::deactivateObject(bool deactivate)
{
    EnhancedGameObject::deactivateObject(deactivate);
    if (m_isActivated && !m_isActivated)
        m_respawnResult = -1.0f;
}

std::string ParticleGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EnhancedGameObject::getSaveString(layer);

    result += ",";
    result += "145";
    result += ",";
    result += m_particleData;

    if (m_particleUseObjectColor)
    {
        result += ",";
        result += "146";
        result += ",";
        result += std::to_string((int)m_particleUseObjectColor);
    }

    if (m_hasUniformObjectColor)
    {
        result += ",";
        result += "147";
        result += ",";
        result += std::to_string((int)m_hasUniformObjectColor);
    }

    if (m_shouldQuickStart)
    {
        result += ",";
        result += "211";
        result += ",";
        result += std::to_string((int)m_shouldQuickStart);
    }

    return result;
}

void ParticleGameObject::particleWasActivated()
{
    if (m_shouldQuickStart && m_particle && m_particle->getTimeElapsed() <= 0.0f)
    {
        float step = 0.2f;
        if (m_particle->getLife() / 10.0f < 0.2f)
            step = m_particle->getLife() / 10.0f;

        int iterations = 10;
        do
        {
            m_particle->update(step);
            iterations--;
        }
        while (iterations);
    }
}

void ParticleGameObject::resetObject()
{
    EnhancedGameObject::resetObject();
    m_startingRespawn = true;
    m_respawnResult = -1.0f;

    if (m_particle && m_particle->isActive())
    {
        m_particle->resetSystem();
        particleWasActivated();
    }
}

void ParticleGameObject::setChildColor(const cocos2d::ccColor3B* color)
{
    GameObject::setChildColor(color);

    if (!m_particle)
        return;

    if (m_particleUseObjectColor)
    {
        cocos2d::ccColor4F endColor;
        endColor.r = (float)color->r / 255.0f;
        endColor.g = (float)color->g / 255.0f;
        endColor.b = (float)color->b / 255.0f;
        endColor.a = m_particle->getEndColor().a;
        m_particle->setEndColor(endColor);
    }
    else if (m_hasUniformObjectColor)
    {
        m_particle->m_tUniformDeltaColor.r = (float)color->r / 255.0f;
        m_particle->m_tUniformDeltaColor.g = (float)color->g / 255.0f;
        m_particle->m_tUniformDeltaColor.b = (float)color->b / 255.0f;
    }
}

void ParticleGameObject::setObjectColor(const cocos2d::ccColor3B* color)
{
    GameObject::setObjectColor(color);

    if (!m_particle)
        return;

    if (m_particleUseObjectColor)
    {
        cocos2d::ccColor4F startColor;
        startColor.r = (float)color->r / 255.0f;
        startColor.g = (float)color->g / 255.0f;
        startColor.b = (float)color->b / 255.0f;
        startColor.a = m_particle->getStartColor().a;
        m_particle->setStartColor(startColor);
    }
    else if (m_hasUniformObjectColor)
    {
        m_particle->m_tUniformColor.r = (float)color->r / 255.0f;
        m_particle->m_tUniformColor.g = (float)color->g / 255.0f;
        m_particle->m_tUniformColor.b = (float)color->b / 255.0f;
    }
}

void ParticleGameObject::setParticleString(std::string str)
{
    if (m_particleData != str)
    {
        m_particleData = str;
        m_updatedParticleData = true;
    }
}

void ParticleGameObject::setRotation(float rotation)
{
    GameObject::setRotation(rotation);
    if (m_particleLocked)
        updateParticleAngle(getRotation(), m_particle);
}

void ParticleGameObject::setRotationX(float rotationX)
{
    GameObject::setRotationX(rotationX);
    if (m_particleLocked)
        updateParticleAngle(getRotation(), m_particle);
}

void ParticleGameObject::setRotationY(float rotationY)
{
    GameObject::setRotationY(rotationY);
    if (m_particleLocked)
        updateParticleAngle(getRotation(), m_particle);
}

void ParticleGameObject::setScale(float scale)
{
    GameObject::setScale(scale);
    if (m_isParticleSpriteLocked)
        updateParticleScale(scale);
}

void ParticleGameObject::setScaleX(float scaleX)
{
    GameObject::setScaleX(scaleX);
    if (m_isParticleSpriteLocked)
        updateParticleScale(scaleX);
}

void ParticleGameObject::setScaleY(float scaleY)
{
    GameObject::setScaleY(scaleY);
    if (m_isParticleSpriteLocked)
        updateParticleScale(scaleY);
}

void ParticleGameObject::unclaimParticle()
{
    if (m_particleData.empty() || !m_particle)
        return;

    if (m_editorEnabled)
    {
        LevelEditorLayer* editor = GameManager::sharedState()->m_levelEditorLayer;
        editor->unclaimCustomParticle(&m_particleData, m_particle);
    }
    else
    {
        PlayLayer* play = GameManager::sharedState()->m_playLayer;
        play->unclaimCustomParticle(&m_particleData, m_particle);
    }
    m_particle = nullptr;
}

void ParticleGameObject::updateAnimateOnTrigger(bool animate)
{
    if (m_hasCustomAnimation && m_animateOnTrigger)
    {
        waitForAnimationTrigger();
        if (m_particle)
        {
            m_particle->resetSystem();
            m_particle->stopSystem();
        }
    }
}

void ParticleGameObject::updateMainParticleOpacity(unsigned char opacity)
{
    if (!m_particle)
        return;

    if (m_particleUseObjectColor)
    {
        m_particle->m_uOpacity = opacity;
        m_particle->m_tStartColor.a = (float)opacity / 255.0f;
    }
    else if (m_hasUniformObjectColor)
    {
        m_particle->m_uOpacity = opacity;
        m_particle->m_tUniformColor.a = (float)opacity / 255.0f;
    }
    else
    {
        m_particle->m_uOpacity = opacity;
    }
}

void ParticleGameObject::updateParticle()
{
    if (!m_particle)
        return;

    m_particle->stopSystem();
    GameToolbox::particleFromString(&m_particleData, m_particle, false);

    float duration = m_particle->getDuration();
    m_animateOnTrigger = true;

    if (duration < 0.0f)
        m_animateOnTrigger = false;

    m_hasCustomAnimation = m_animateOnTrigger;

    bool locked = m_particle->getPositionType() != cocos2d::tCCPositionTypeGrouped;
    m_isParticleSpriteLocked = locked;
    m_particleLocked = locked;

    if (locked)
    {
        updateParticleAngle(getRotation(), m_particle);
        if (m_isParticleSpriteLocked)
            updateParticleScale(getScaleX());
    }

    if (m_particle && (m_particleUseObjectColor || m_hasUniformObjectColor))
        updateObjectEditorColor();
}

void ParticleGameObject::updateParticleColor(const cocos2d::ccColor3B* color)
{
}

void ParticleGameObject::updateParticleOpacity(unsigned char opacity)
{
}

void ParticleGameObject::updateParticlePreviewArtOpacity(float opacity)
{
    cocos2d::CCNode* art = getChildByTag(1);
    if (art)
        art->setOpacity((unsigned char)opacity);

    if (m_colorSprite)
    {
        cocos2d::CCNode* colorArt = m_colorSprite->getChildByTag(1);
        if (colorArt)
            colorArt->setOpacity((unsigned char)opacity);
    }
}

void ParticleGameObject::updateParticleScale(float scale)
{
    if (!m_particle)
        return;

    updateParticleAngle(getRotation(), m_particle);
    m_particle->loadScaledDefaults(fabsf(scale));
}

void ParticleGameObject::updateParticleStruct()
{
    if (m_updatedParticleData)
    {
        m_updatedParticleData = false;
        GameToolbox::particleStringToStruct(&m_particleData, &m_particleStruct);
    }
}

void ParticleGameObject::updateSecondaryParticleOpacity(unsigned char opacity)
{
    if (!m_particle)
        return;

    if (m_particleUseObjectColor)
    {
        m_particle->m_tEndColor.a = (float)opacity / 255.0f;
    }
    else if (m_hasUniformObjectColor)
    {
        m_particle->m_tUniformDeltaColor.a = (float)opacity / 255.0f;
    }
}

void ParticleGameObject::updateSyncedAnimation(float totalTime, int frameIndex)
{
    if (!m_particle)
        return;

    if (m_animateOnTrigger)
    {
        if (m_animationTriggered)
        {
            m_particle->resetSystem();
            m_animationTriggered = false;
        }
        return;
    }

    if (m_respawnResult < 0.0f)
    {
        float life = m_particle->getLife();
        float lifeVar = m_particle->getLifeVar();
        float respawn = m_particle->m_fRespawn;
        float respawnVar = m_particle->m_fRespawnVar;

        uint64_t seed = 214013 * (uint64_t)GameToolbox::getfast_srand() + 2531011;
        float rand = (float)((seed >> 16) & 0x7FFF) / 32767.0f;
        float value = respawn + ((rand * 2.0f) - 1.0f) * respawnVar;

        if (value <= 0.0f)
            value = 0.0f;

        if (m_startingRespawn)
        {
            m_respawnResult = value + totalTime;
            m_startingRespawn = false;
        }
        else
        {
            m_respawnResult = value + (totalTime + life + lifeVar);
        }
    }

    if (totalTime > m_respawnResult)
    {
        m_particle->resetSystem();
        m_respawnResult = -1.0f;
    }
}

ParticleGameObject::~ParticleGameObject()
{
}