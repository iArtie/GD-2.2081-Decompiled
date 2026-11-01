#include "Managers/SpriteAnimationManager.h"
#include "Cocos/CCAnimatedSprite.h"
#include "Managers/ObjectManager.h"
#include "Utils/GameToolbox.h"

void SpriteAnimationManager::animationFinished() {
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(SpriteAnimationManager::callAnimationFinished));
    m_sprite->runAction(cocos2d::CCSequence::create(delay, callFunc, nullptr));
}

void SpriteAnimationManager::callAnimationFinished() {
    std::string anim = (m_currentAnimation == "not_used") ? m_nextAnimation : m_currentAnimation;
    m_sprite->animationFinished(anim);
}

void SpriteAnimationManager::createAnimations(std::string definition) {
    ObjectManager* om = ObjectManager::instance();
    if (om->animLoaded(definition))
        return;

    om->setLoaded(definition);

    cocos2d::CCAnimationCache* animCache = cocos2d::CCAnimationCache::sharedAnimationCache();
    cocos2d::CCDictionary* dict = ObjectManager::instance()->getDefinition(definition);
    cocos2d::CCDictionary* animDict = (cocos2d::CCDictionary*)dict->objectForKey("animations");

    cocos2d::CCArray* keys = animDict->allKeys();
    for (unsigned int i = 0; i < keys->count(); i++) {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        std::string animName = key->getCString();
        cocos2d::CCDictionary* animDef = (cocos2d::CCDictionary*)animDict->objectForKey(animName);

        float delay = animDef->objectForKey("delay") ? ((cocos2d::CCString*)animDef->objectForKey("delay"))->floatValue() : 0.0f;
        bool usesParts = animDef->objectForKey("usesParts") && ((cocos2d::CCString*)animDef->objectForKey("usesParts"))->boolValue();

        cocos2d::CCString* frameNameStr = (cocos2d::CCString*)animDef->objectForKey("animFrameName");
        std::string frameName = frameNameStr ? frameNameStr->getCString() : (definition + "_" + animName);

        cocos2d::CCObject* singleFrame = animDef->objectForKey("singleFrame");
        if (singleFrame) {
            std::string frameStr = ((cocos2d::CCString*)singleFrame)->getCString();
            cocos2d::CCAnimation* anim;
            if (usesParts) {
                anim = (cocos2d::CCAnimation*)cocos2d::CCString::create(frameStr);
                anim->m_eObjType = 1;
            } else {
                anim = (cocos2d::CCAnimation*)cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameStr.c_str());
                anim->m_eObjType = 2;
            }
            cocos2d::CCAnimationCache::sharedAnimationCache()->addAnimation(anim, (definition + "_" + animName).c_str());
            continue;
        }

        cocos2d::CCArray* frames = cocos2d::CCArray::create();
        bool other = animDef->objectForKey("other") && ((cocos2d::CCString*)animDef->objectForKey("other"))->boolValue();

        if (other) {
            std::string framesStr = ((cocos2d::CCString*)animDef->objectForKey("frames"))->getCString();
            std::vector<std::string> parts = GameToolbox::splitString(framesStr, " ");
            for (auto& part : parts) {
                int frameIdx = atoi(part.c_str());
                cocos2d::CCObject* frame;
                if (usesParts)
                    frame = cocos2d::CCString::createWithFormat("%s_%03d.png", frameName.c_str(), frameIdx);
                else
                    frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                        cocos2d::CCString::createWithFormat("%s_%03d.png", frameName.c_str(), frameIdx)->getCString());
                frames->addObject(frame);
            }
        } else {
            int numFrames = ((cocos2d::CCString*)animDef->objectForKey("frames"))->intValue();
            for (int f = 1; f <= numFrames; f++) {
                cocos2d::CCObject* frame;
                if (usesParts)
                    frame = cocos2d::CCString::createWithFormat("%s_%03d.png", frameName.c_str(), f);
                else
                    frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                        cocos2d::CCString::createWithFormat("%s_%03d.png", frameName.c_str(), f)->getCString());
                frames->addObject(frame);
            }
        }

        cocos2d::CCAnimation* animation = cocos2d::CCAnimation::createWithSpriteFrames(frames, delay);
        cocos2d::CCAnimationCache::sharedAnimationCache()->addAnimation(animation, (definition + "_" + animName).c_str());
    }
}

SpriteAnimationManager* SpriteAnimationManager::createWithOwner(CCAnimatedSprite* sprite, std::string definition) {
    SpriteAnimationManager* returnVal = new SpriteAnimationManager();
    if (returnVal->initWithOwner(sprite, definition)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void SpriteAnimationManager::doCleanup() {
    m_frameDict->removeAllObjects();
    release();
}

void SpriteAnimationManager::executeAnimation(std::string animation) {
    if (m_currentAnimation == animation)
        return;

    m_sprite->m_sprite->stopAllActions();
    m_currentAnimation = animation;
    playSoundForAnimation(animation);

    cocos2d::CCAction* action = (cocos2d::CCAction*)m_frameDict->objectForKey(animation);
    if (!action->m_eObjType) {
        spriteMode mode = m_sprite->m_spriteMode;
        spriteMode newMode = getAnimType(animation);
        m_sprite->willPlayAnimation();
        m_sprite->switchToMode(newMode);
        if (mode != m_sprite->m_spriteMode || animation.find("spawn") != std::string::npos) {
            std::string firstFrame = animation + "_first";
            cocos2d::CCObject* frame = m_frameDict->objectForKey(firstFrame);
            if (frame && frame->m_eObjType >= 1 && frame->m_eObjType <= 2)
                m_sprite->m_sprite->setDisplayFrame((cocos2d::CCSpriteFrame*)frame);
        }
        action->setTag(1);
        m_sprite->m_sprite->runAction(action);
        return;
    }

    spriteMode type = getAnimType(animation);
    m_sprite->willPlayAnimation();
    m_sprite->switchToMode(type);
    m_sprite->m_sprite->setDisplayFrame((cocos2d::CCSpriteFrame*)action);
}

void SpriteAnimationManager::finishAnimation(std::string animation) {
    if (m_queuedAnimation == animation) {
        m_queuedAnimation = "not_used";
    }
    if (m_currentAnimation == animation) {
        runQueuedAnimation();
    }
}

spriteMode SpriteAnimationManager::getAnimType(std::string animation) {
    cocos2d::CCString* str = (cocos2d::CCString*)m_typeDict->valueForKey(animation);
    return (spriteMode)str->intValue();
}

int SpriteAnimationManager::getPrio(std::string animation) {
    cocos2d::CCString* str = (cocos2d::CCString*)m_priorityDict->objectForKey(animation);
    return str ? str->intValue() : 99;
}

void SpriteAnimationManager::loadAnimations(std::string definition) {
    cocos2d::CCAnimationCache* animCache = cocos2d::CCAnimationCache::sharedAnimationCache();
    cocos2d::CCDictionary* dict = ObjectManager::instance()->getDefinition(definition);
    cocos2d::CCDictionary* animDict = (cocos2d::CCDictionary*)dict->objectForKey("animations");

    cocos2d::CCArray* keys = animDict->allKeys();
    for (unsigned int i = 0; i < keys->count(); i++) {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        std::string animName = key->getCString();
        cocos2d::CCDictionary* animDef = (cocos2d::CCDictionary*)animDict->objectForKey(animName);

        int priority = ((cocos2d::CCString*)animDef->valueForKey("prio"))->intValue();
        bool looped = ((cocos2d::CCString*)animDef->valueForKey("looped"))->boolValue();
        bool usesParts = ((cocos2d::CCString*)animDef->valueForKey("usesParts"))->boolValue();
        bool dontPlayNext = ((cocos2d::CCString*)animDef->valueForKey("dontPlayNext"))->boolValue();
        cocos2d::CCString* sound = (cocos2d::CCString*)animDef->objectForKey("sound");

        if (sound) {
            std::string soundName = sound->getCString();
            if (soundName != "0") {
                float soundDelay = ((cocos2d::CCString*)animDef->objectForKey("soundDelay"))->floatValue();
                storeSoundForAnimation(sound, animName, soundDelay);
            }
        }

        spriteMode mode = usesParts ? PartAnim : FrameByFrame;
        std::string fullName = definition + "_" + animName;
        cocos2d::CCAnimation* anim = cocos2d::CCAnimationCache::animationByName(animCache, fullName.c_str());

        cocos2d::CCObject* singleFrame = animDef->objectForKey("singleFrame");
        cocos2d::CCAction* action;
        cocos2d::CCSpriteFrame* firstFrame;

        if (singleFrame) {
            anim->m_eObjType = 2;
            action = (cocos2d::CCAction*)anim;
            firstFrame = (cocos2d::CCSpriteFrame*)anim;
        } else {
            cocos2d::CCObject* frameObj = anim->m_pFrames->objectAtIndex(0);
            if (frameObj->m_eObjType == 8)
                frameObj = (cocos2d::CCObject*)frameObj->__vftable;
            cocos2d::CCAnimate* animate = cocos2d::CCAnimate::create(anim);
            animate->m_eObjType = 0;
            if (looped) {
                action = cocos2d::CCRepeatForever::create(animate);
            } else if (dontPlayNext) {
                action = animate;
            } else {
                cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(SpriteAnimationManager::animationFinished));
                action = cocos2d::CCSequence::create(animate, callFunc, nullptr);
            }
            action->m_eObjType = 0;
            firstFrame = (cocos2d::CCSpriteFrame*)frameObj;
        }

        storeAnimation(action, action, animName, priority, mode, firstFrame);
    }
}

void SpriteAnimationManager::offsetCurrentAnimation(float dt) {
    cocos2d::CCAction* action = m_sprite->m_sprite->getActionByTag(1);
    if (action) {
        if (action[1].m_uID)
            action->step(0.0f);
        action->step(dt);
    }
}

void SpriteAnimationManager::overridePrio() {
    if (m_currentAnimation != "not_used") {
        m_nextAnimation = m_currentAnimation;
        m_currentAnimation = "not_used";
    }
}

void SpriteAnimationManager::playSound(std::string sound) {
}

void SpriteAnimationManager::playSoundForAnimation(std::string animation) {
    cocos2d::CCString* sound = (cocos2d::CCString*)m_soundDict->objectForKey(animation);
    if (!sound)
        return;

    std::string delayKey = animation + "delay";
    cocos2d::CCString* delayStr = (cocos2d::CCString*)m_soundDict->objectForKey(delayKey);
    float delay = delayStr ? delayStr->floatValue() : 0.0f;

    if (delay == 0.0f)
        playSound(sound->getCString());
}

void SpriteAnimationManager::queueAnimation(std::string animation) {
    int prio = getPrio(animation);
    int queuedPrio = getPrio(m_queuedAnimation);
    if (prio <= queuedPrio)
        m_queuedAnimation = animation;
}

void SpriteAnimationManager::resetAnimState() {
    m_currentAnimation = "not_used";
    m_queuedAnimation = "not_used";
}

void SpriteAnimationManager::runAnimation(std::string animation) {
    if (m_currentAnimation == animation)
        return;

    int prio = getPrio(animation);
    int currentPrio = getPrio(m_currentAnimation);

    if (prio > currentPrio) {
        queueAnimation(animation);
        return;
    }

    queueAnimation(m_currentAnimation);
    executeAnimation(animation);
}

void SpriteAnimationManager::runQueuedAnimation() {
    if (m_queuedAnimation != "not_used") {
        executeAnimation(m_queuedAnimation);
        m_queuedAnimation = "not_used";
    }
}

void SpriteAnimationManager::stopAnimations() {
    overridePrio();
    if (m_sprite->m_paSprite)
        m_sprite->m_paSprite->stopAllActions();
    if (m_sprite->m_fbfSprite)
        m_sprite->m_fbfSprite->stopAllActions();
}

void SpriteAnimationManager::switchToFirstFrameOfAnimation(std::string animation) {
    cocos2d::CCObject* action = m_frameDict->objectForKey(animation);
    if (!action->m_eObjType) {
        spriteMode type = getAnimType(animation);
        m_sprite->switchToMode(type);
        std::string firstFrameKey = animation + "_first";
        cocos2d::CCObject* frame = m_frameDict->objectForKey(firstFrameKey);
        if (frame && frame->m_eObjType >= 1 && frame->m_eObjType <= 2)
            m_sprite->m_sprite->setDisplayFrame((cocos2d::CCSpriteFrame*)frame);
    }
}

void SpriteAnimationManager::updateAnimationSpeed(float speed) {
    if (m_speed == speed)
        return;
    m_speed = speed;
    if (m_animateDict) {
        cocos2d::CCDictElement* element = m_animateDict->m_pElements;
        while (element) {
            cocos2d::CCObject* obj = element->m_pObject;
            if (!obj->m_eObjType)
                *(float*)&obj[1].m_uReference = speed;
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
}

SpriteAnimationManager::~SpriteAnimationManager() {
}