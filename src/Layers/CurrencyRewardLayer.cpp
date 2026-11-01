#include "Layers/CurrencyRewardLayer.h"
#include "Sprites/CurrencySprite.h"
#include "Utils/GameToolbox.h"

CurrencyRewardLayer* CurrencyRewardLayer::create(CurrencyRewardDelegate* delegate, int rewardCount, float time, cocos2d::CCArray* objects)
{
    CurrencyRewardLayer* pRet = new CurrencyRewardLayer();
    if (pRet && pRet->init(delegate, rewardCount, time, objects))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CurrencyRewardLayer::init(CurrencyRewardDelegate* delegate, int rewardCount, float time, cocos2d::CCArray* objects)
{
    if (!cocos2d::CCLayer::init())
        return false;

    m_delegate = delegate;
    m_rewardCount = rewardCount;
    m_time = time;
    m_objects = objects;
    if (objects) objects->retain();
    m_elapsed = 0.0f;
    m_orbs = 0;
    m_stars = 0;
    m_moons = 0;
    m_diamonds = 0;
    m_keys = 0;
    m_shards = 0;

    // Initialize UI elements for each currency type
    // This would include labels and sprites for orbs, stars, moons, diamonds, keys, shards
    // positioned appropriately on screen

    // Start updating
    scheduleUpdate();

    return true;
}

void CurrencyRewardLayer::incrementCount(int count)
{
    if (m_orbsLabel)
    {
        m_orbs += count;
        pulseSprite(m_orbsSprite);
        m_orbsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_orbs)->getCString());
    }
}

void CurrencyRewardLayer::incrementDiamondsCount(int count)
{
    if (m_diamondsLabel)
    {
        m_diamonds += count;
        pulseSprite(m_diamondsSprite);
        m_diamondsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_diamonds)->getCString());
    }
}

void CurrencyRewardLayer::incrementMoonsCount(int count)
{
    if (m_moonsLabel)
    {
        m_moons += count;
        pulseSprite(m_moonsSprite);
        m_moonsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_moons)->getCString());
    }
}

void CurrencyRewardLayer::incrementSpecialCount1(int count)
{
    if (m_keysLabel)
    {
        m_keys += count;
        pulseSprite(m_keysSprite);
        m_keysLabel->setString(cocos2d::CCString::createWithFormat("%i", m_keys)->getCString());
    }
}

void CurrencyRewardLayer::incrementSpecialCount2(int count)
{
    if (m_shardsLabel)
    {
        m_shards += count;
        pulseSprite(m_shardsSprite);
        m_shardsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_shards)->getCString());
    }
}

void CurrencyRewardLayer::incrementStarsCount(int count)
{
    if (m_starsLabel)
    {
        m_stars += count;
        pulseSprite(m_starsSprite);
        m_starsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_stars)->getCString());
    }
}

void CurrencyRewardLayer::pulseSprite(cocos2d::CCSprite* sprite)
{
    if (!sprite->getActionByTag(99))
    {
        float currentScale = sprite->getScale();
        
        cocos2d::CCAction* scaleUp = cocos2d::CCEaseOut::create(
            cocos2d::CCScaleTo::create(0.08f, currentScale * 1.5f), 1.8f
        );
        cocos2d::CCAction* scaleDown = cocos2d::CCEaseIn::create(
            cocos2d::CCScaleTo::create(0.08f, currentScale), 1.8f
        );
        
        cocos2d::CCAction* sequence = cocos2d::CCSequence::create(scaleUp, scaleDown, nullptr);
        sequence->setTag(99);
        sprite->runAction(sequence);
    }
}

void CurrencyRewardLayer::update(float dt)
{
    float speed = dt * 60.0f;
    m_elapsed += dt;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float maxX = winSize.width - 6.0f;
    float maxY = winSize.height - 6.0f;
    float minPos = 6.0f;

    for (unsigned int i = 0; i < m_objects->count(); )
    {
        cocos2d::CCObject* obj = m_objects->objectAtIndex(i);
        
        // obj[11] contains physics data:
        // - m_nTag: remaining life
        // - m_uID: velocity Y
        // - __vftable[1]: velocity X
        // - m_uIndexInArray: currency type
        // - m_uReference: count value
        // - m_uAutoReleaseCount: particle system reference
        // - m_uOrderOfArrival: scale reference
        // - m_nLuaID: attraction strength

        float remainingLife = obj[11].m_nTag - dt;
        obj[11].m_nTag = remainingLife;

        if (remainingLife > 0.0f)
        {
            // Update position with physics
            float* velX = (float*)&obj[11].__vftable[1];
            float* velY = (float*)&obj[11].m_uID;
            cocos2d::CCPoint pos = obj->getPosition();

            *velX -= speed * 0.4f;
            *velY -= speed * *velX;

            pos.x += speed * *velX;
            pos.y += speed * *velY;

            // Bounce off walls
            int type = obj[11].m_uIndexInArray;
            if (pos.y < minPos)
            {
                pos.y = minPos;
                if (type == 15 || type == 2) // Moons or Stars
                    *velY *= -0.9f;
                else
                    *velY *= -0.6f - ((float)rand() * 4.6566e-10f) * 0.3f;
                
                if (*velY < 4.0f)
                    *velY = ((float)rand() * 4.6566e-10f) * 2.0f + 4.0f;
            }
            else if (pos.y > maxY)
            {
                pos.y = maxY;
                if (type == 15 || type == 2)
                    *velY *= -0.9f;
                else
                    *velY *= -0.6f - ((float)rand() * 4.6566e-10f) * 0.3f;
            }

            if (pos.x < minPos)
            {
                pos.x = minPos;
                *velX *= -0.9f;
            }
            else if (pos.x > maxX)
            {
                pos.x = maxX;
                *velX *= -0.9f;
            }

            obj->setPosition(pos);

            // Attraction to target
            void* target = *(void**)&obj[11].m_uAutoReleaseCount;
            if (target)
            {
                cocos2d::CCPoint targetPos = ((cocos2d::CCNode*)target)->getPosition();
                float dist = cocos2d::ccpDistance(pos, targetPos);
                
                if (dist <= 5.0f)
                {
                    // Collect currency
                    int currencyType = obj[11].m_uIndexInArray;
                    int count = obj[11].m_uReference;

                    switch (currencyType)
                    {
                        case 1:  // Orbs
                            incrementCount(count);
                            break;
                        case 2:  // Stars
                            incrementStarsCount(count);
                            break;
                        case 15: // Moons
                            incrementMoonsCount(count);
                            break;
                        case 3:  // Diamonds
                            incrementDiamondsCount(count);
                            break;
                        default:
                            if (m_shardsLabel && m_shardsLabel->getTag() == currencyType)
                                incrementSpecialCount2(count);
                            else
                                incrementSpecialCount1(count);
                            break;
                    }

                    // Play collection animation
                    obj->runAction(cocos2d::CCSequence::create(
                        cocos2d::CCScaleTo::create(0.1f, 0.1f),
                        cocos2d::CCCallFunc::create(obj, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup), 1),
                        nullptr
                    ));

                    if (target)
                    {
                        ((cocos2d::CCNode*)target)->runAction(cocos2d::CCSequence::create(
                            cocos2d::CCScaleTo::create(0.1f, 0.1f),
                            cocos2d::CCCallFunc::create(target, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup), 1),
                            nullptr
                        ));
                    }

                    m_objects->removeObjectAtIndex(i);
                    continue;
                }

                // Move toward target
                float strength = obj[11].m_nLuaID;
                float factor = (dist / 10.0f + 10.0f) / strength;
                float dx = (targetPos.x - pos.x) / factor * speed;
                float dy = (targetPos.y - pos.y) / factor * speed;

                if ((dx >= 0 && pos.x + dx > targetPos.x) || (dx < 0 && pos.x + dx < targetPos.x))
                    dx = targetPos.x - pos.x;
                if ((dy >= 0 && pos.y + dy > targetPos.y) || (dy < 0 && pos.y + dy < targetPos.y))
                    dy = targetPos.y - pos.y;

                pos.x += dx;
                pos.y += dy;
                obj->setPosition(pos);

                obj[11].m_nLuaID += (speed * 0.1f) / m_time;
            }
        }
        else
        {
            // Object expired, remove it
            m_objects->removeObjectAtIndex(i);
            continue;
        }

        i++;
    }

    // If all objects collected, finish animation
    if (m_objects->count() == 0)
    {
        unscheduleUpdate();

        int count = m_rewardCount - 1;
        float duration = ((count * 0.1f + 0.5f) * m_time);
        cocos2d::CCPoint offset = cocos2d::CCPoint(0.0f, (float)(30 * count + 50));

        cocos2d::CCAction* anim = cocos2d::CCSequence::create(
            cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.2f, 1.2f), 2.0f),
            cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.2f, 1.0f), 2.0f),
            cocos2d::CCEaseInOut::create(cocos2d::CCMoveBy::create(duration, offset), 2.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(CurrencyRewardLayer::removeFromParentAndCleanup), 1),
            nullptr
        );
        m_mainNode->runAction(anim);

        if (m_delegate)
            m_delegate->currencyWillExit(this);
    }
}

CurrencyRewardLayer::~CurrencyRewardLayer()
{
    CC_SAFE_RELEASE(m_objects);
}