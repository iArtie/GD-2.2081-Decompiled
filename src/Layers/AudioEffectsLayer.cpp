#include "Layers/AudioEffectsLayer.h"
#include "Managers/GameManager.h"
#include "Layers/PlayLayer.h"
#include "Platform/cocos2d.h"
#include <cstdlib>
#include <string>

void AudioEffectsLayer::audioStep(float dt)
{
    m_timeElapsed += dt;
    
    if (!m_unk1c0)
        return;
    
    if (cocos2d::CCArray::count(m_unk1c0) == 0)
        return;
    
    cocos2d::CCString* timeStr = (cocos2d::CCString*)m_unk1c0->objectAtIndex(0);
    if (timeStr->floatValue() < m_timeElapsed)
    {
        cocos2d::CCString* pulseStr = (cocos2d::CCString*)m_unk1c0->objectAtIndex(1);
        float pulse = pulseStr->floatValue();
        
        m_unk1c0->removeObjectAtIndex(0);
        m_unk1c0->removeObjectAtIndex(0);
        
        if (!GameManager::sharedState()->m_playLayer->m_isPracticeMode)
        {
            triggerEffect(pulse);
        }
    }
}

AudioEffectsLayer* AudioEffectsLayer::create(std::string audioString)
{
    AudioEffectsLayer* pRet = new AudioEffectsLayer();
    if (pRet && pRet->init(audioString))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void AudioEffectsLayer::draw()
{
    // Empty implementation
}

cocos2d::CCSprite* AudioEffectsLayer::getBGSquare()
{
    unsigned int index = (unsigned int)(((float)rand() / RAND_MAX) * 14.0f);
    float scale = 10.0f;
    cocos2d::CCPoint pos(0.0f, 0.0f);
    
    // Predefined positions for each random index
    static const cocos2d::CCPoint positions[] = {
        cocos2d::CCPoint(60.0f, 60.0f),     // 0
        cocos2d::CCPoint(170.0f, 40.0f),    // 1
        cocos2d::CCPoint(280.0f, 60.0f),    // 2
        cocos2d::CCPoint(430.0f, 77.0f),    // 3
        cocos2d::CCPoint(25.0f, 150.0f),    // 4
        cocos2d::CCPoint(90.0f, 150.0f),    // 5
        cocos2d::CCPoint(170.0f, 380.0f),   // 6
        cocos2d::CCPoint(280.0f, 177.0f),   // 7
        cocos2d::CCPoint(360.0f, 186.0f),   // 8
        cocos2d::CCPoint(360.0f, 230.0f),   // 9
        cocos2d::CCPoint(446.0f, 230.0f),   // 10
        cocos2d::CCPoint(40.0f, 234.0f),    // 11
        cocos2d::CCPoint(150.0f, 250.0f),   // 12
        cocos2d::CCPoint(300.0f, 325.0f)    // 13
    };
    
    static const float scales[] = {
        1.0f,   // 0 - size 60x60 -> scale 1.0
        1.0f,   // 1 - size 60x60 -> scale 1.0
        1.0f,   // 2 - size 60x60 -> scale 1.0
        1.0f,   // 3 - size 60x60 -> scale 1.0
        1.0f,   // 4 - size 60x60 -> scale 1.0
        1.0f,   // 5 - size 60x60 -> scale 1.0
        1.0f,   // 6 - size 60x60 -> scale 1.0
        1.0f,   // 7 - size 60x60 -> scale 1.0
        1.0f,   // 8 - size 60x60 -> scale 1.0
        1.0f,   // 9 - size 60x60 -> scale 1.0
        1.0f,   // 10 - size 60x60 -> scale 1.0
        1.0f,   // 11 - size 60x60 -> scale 1.0
        1.0f,   // 12 - size 60x60 -> scale 1.0
        1.0f    // 13 - size 60x60 -> scale 1.0
    };
    
    if (index < 14)
    {
        pos = positions[index];
        scale = scales[index];
    }
    
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    sprite->setPosition(pos);
    sprite->setScale(scale);
    
    return sprite;
}

void AudioEffectsLayer::goingDown()
{
    m_goingDown = true;
}

bool AudioEffectsLayer::init(std::string audioString)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;
    
    // Parse audio string (comma separated values)
    // Format: "time1,pulse1,time2,pulse2,..."
    std::vector<std::string> parts;
    std::string current;
    for (char c : audioString)
    {
        if (c == '~')
        {
            parts.push_back(current);
            current = "";
        }
        else
        {
            current += c;
        }
    }
    if (!current.empty())
        parts.push_back(current);
    
    m_unk1bc = cocos2d::CCArray::create();
    
    for (const std::string& part : parts)
    {
        if (!part.empty())
        {
            cocos2d::CCString* str = cocos2d::CCString::create(part.c_str());
            m_unk1bc->addObject(str);
        }
    }
    
    m_unk1bc->retain();
    
    resetAudioVars();
    
    // Create batch node
    cocos2d::CCTextureCache* cache = cocos2d::CCTextureCache::sharedTextureCache();
    cocos2d::CCTexture2D* texture = cache->addImage("GJ_GameSheet.png");
    cocos2d::CCSpriteBatchNode* batchNode = cocos2d::CCSpriteBatchNode::createWithTexture(texture, 5);
    m_batchNode = batchNode;
    this->addChild(batchNode);
    batchNode->setBlendFunc(cocos2d::ccBlendFunc(0, 0x302));
    
    m_audioScale = 0.1f;
    
    return true;
}

void AudioEffectsLayer::resetAudioVars()
{
    if (m_unk1c0)
    {
        m_unk1c0->removeAllObjects();
        m_unk1c0->release();
        m_unk1c0 = nullptr;
    }
    
    m_unk1c0 = cocos2d::CCArray::createWithArray(m_unk1bc);
    m_unk1c0->retain();
    
    m_timeElapsed = 0.0f;
}

void AudioEffectsLayer::triggerEffect(float pulse)
{
    float scale;
    
    if (pulse == 1.0f)
        scale = 0.8f;
    else if (pulse == 0.9f)
        scale = 0.6f;
    else if (pulse == 0.8f)
        scale = 0.5f;
    else
        scale = pulse * 0.2f;
    
    float newScale = scale + m_audioScale;
    if (newScale >= 1.1f)
        newScale = 1.1f;
    
    if (m_goingDown || newScale >= m_audioPulseMod)
    {
        m_audioPulseMod = newScale;
        m_goingDown = false;
        
        this->stopActionByTag(0);
        
        // Tween up
        cocos2d::CCActionTween* tweenUp = cocos2d::CCActionTween::create(
            "audioScale",
            0.05f,
            m_audioScale,
            newScale
        );
        
        // Tween down
        cocos2d::CCActionTween* tweenDown = cocos2d::CCActionTween::create(
            "audioScale",
            0.2f,
            newScale,
            0.1f
        );
        
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(
            this,
            callfunc_selector(AudioEffectsLayer::goingDown)
        );
        
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
            tweenUp,
            callFunc,
            tweenDown,
            nullptr
        );
        sequence->setTag(0);
        this->runAction(sequence);
    }
}

void AudioEffectsLayer::updateTweenAction(float value, const char* key)
{
    std::string keyStr(key);
    if (keyStr == "audioScale")
    {
        m_audioScale = value;
    }
}

AudioEffectsLayer::~AudioEffectsLayer()
{
    // Destructor
}