#include "Layers/SecretGame01Layer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SecretGame01Layer* SecretGame01Layer::create()
{
    SecretGame01Layer* returnVal = new SecretGame01Layer();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool SecretGame01Layer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainMenu = cocos2d::CCMenu::create();
    addChild(m_mainMenu);
    m_mainMenu->setTouchEnabled(false);

    m_gameObjects = cocos2d::CCArray::create();
    m_gameObjects->retain();

    m_requiredObjects = cocos2d::CCArray::create();
    m_requiredObjects->retain();

    m_timeBarSprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
    m_timeBarSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 50.0f, 40.0f));
    m_timeBarSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_timeBarSprite->setScaleY(0.6f);
    m_timeBarSprite->setColor(cocos2d::ccc3(100, 100, 100));
    m_timeBarSprite->setOpacity(100);

    float barWidth = m_timeBarSprite->getContentSize().width;
    m_timeBarSprite->setScaleX(100.0f / barWidth);

    addChild(m_timeBarSprite);
    m_timeBarSprite->setVisible(false);

    scheduleUpdate();

    return true;
}

void SecretGame01Layer::didSelectCorrectObject(CCMenuItemSpriteExtra* button)
{
    if (!button)
        return;

    m_gameObjects->removeObject(button);
    button->setEnabled(false);

    cocos2d::CCNode* normalImage = button->getNormalImage();
    cocos2d::CCArray* children = normalImage->getChildren();
    cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(children->objectAtIndex(0));

    cocos2d::CCSequence* seq1 = cocos2d::CCSequence::create(
        cocos2d::CCTintTo::create(0.1f, 0, 255, 0),
        cocos2d::CCDelayTime::create(0.05f),
        cocos2d::CCFadeOut::create(0.3f),
        cocos2d::CCDelayTime::create(0.3f),
        cocos2d::CCCallFunc::create(button, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr);
    normalImage->runAction(seq1);

    cocos2d::CCSequence* seq2 = cocos2d::CCSequence::create(
        cocos2d::CCTintTo::create(0.1f, 0, 255, 0),
        cocos2d::CCDelayTime::create(0.05f),
        cocos2d::CCFadeOut::create(0.3f),
        cocos2d::CCDelayTime::create(0.3f),
        cocos2d::CCCallFunc::create(button, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr);
    child->runAction(seq2);

    cocos2d::CCSequence* seq3 = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.15f),
        cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.3f, 0.1f), 2.0f),
        nullptr);
    normalImage->runAction(seq3);
}

void SecretGame01Layer::gameStep01()
{
    int count = getCountForDifficulty(m_gameDifficulty);
    cocos2d::CCArray* pool = cocos2d::CCArray::create();
    pool->addObjectsFromArray(m_gameObjects);

    m_requiredObjects->removeAllObjects();

    for (int i = 0; i < count; i++)
    {
        int index = (int)llroundf((float)rand() * 4.6566e-10f * (pool->count() - 1));
        cocos2d::CCObject* obj = pool->objectAtIndex(index);
        pool->removeObjectAtIndex(index);
        m_requiredObjects->addObject(obj);
    }

    float totalDelay = 0.0f;

    for (unsigned int i = 0; i < m_requiredObjects->count(); i++)
    {
        CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_requiredObjects->objectAtIndex(i));
        cocos2d::CCNode* normalImage = item->getNormalImage();
        cocos2d::CCArray* children = normalImage->getChildren();
        cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(children->objectAtIndex(0));

        float delay = i * 0.2f;

        cocos2d::CCSequence* seq1 = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(delay),
            cocos2d::CCTintTo::create(0.1f, 0, 255, 0),
            cocos2d::CCDelayTime::create(0.1f),
            cocos2d::CCTintTo::create(0.2f, 255, 255, 255),
            nullptr);
        normalImage->runAction(seq1);

        cocos2d::CCSequence* seq2 = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(delay),
            cocos2d::CCTintTo::create(0.1f, 0, 255, 0),
            cocos2d::CCDelayTime::create(0.1f),
            cocos2d::CCTintTo::create(0.2f, 255, 255, 255),
            nullptr);
        child->runAction(seq2);

        totalDelay = delay + 0.1f + 0.2f + 0.2f;
    }

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(totalDelay),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretGame01Layer::gameStep02)),
        nullptr));
}

void SecretGame01Layer::gameStep02()
{
    m_mainMenu->setTouchEnabled(true);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_gameObjects->objectAtIndex(i));
            cocos2d::CCNode* normalImage = item->getNormalImage();
            cocos2d::CCArray* children = normalImage->getChildren();
            cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(children->objectAtIndex(0));

            normalImage->runAction(cocos2d::CCTintTo::create(0.2f, 125, 125, 125));
            child->runAction(cocos2d::CCTintTo::create(0.2f, 90, 90, 90));
        }
    }

    resetGameTimer();
}

int SecretGame01Layer::getCountForDifficulty(int difficulty)
{
    static const int counts[] = {
        2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8
    };

    if (difficulty >= 1 && difficulty <= 12)
        return counts[difficulty - 1];

    return 2;
}

std::string SecretGame01Layer::getFrameForDifficulty(int difficulty)
{
    switch (difficulty)
    {
        case 0: return "player_01_001.png";
        case 4:
        case 5:
        case 6:
        case 7: return "player_12_001.png";
        case 8:
        case 9:
        case 10:
        case 11:
        case 12: return "player_73_001.png";
        default: return "player_04_001.png";
    }
}

int SecretGame01Layer::getRowsForDifficulty(int difficulty)
{
    static const int rows[] = {
        3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9
    };

    if (difficulty >= 0 && difficulty <= 12)
        return rows[difficulty];

    return 3;
}

int SecretGame01Layer::getTimeForDifficulty(int difficulty)
{
    static const int times[] = {
        10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34
    };

    if (difficulty >= 0 && difficulty <= 12)
        return times[difficulty];

    return 10;
}

void SecretGame01Layer::onSelectButton(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    CCMenuItemSpriteExtra* expected = nullptr;

    if (m_selectedObject < m_requiredObjects->count())
        expected = static_cast<CCMenuItemSpriteExtra*>(m_requiredObjects->objectAtIndex(m_selectedObject));

    if (expected == nullptr || expected->getTag() != tag)
    {
        showGameOver();
        return;
    }

    m_selectedObject++;

    if (m_selectedObject >= m_requiredObjects->count())
        showGameWon();
    else
        didSelectCorrectObject(expected);
}

void SecretGame01Layer::resetGame()
{
    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_gameObjects->objectAtIndex(i));
            item->removeFromParentAndCleanup(true);
        }
    }

    m_gameObjects->removeAllObjects();

    int rows = getRowsForDifficulty(m_gameDifficulty);
    int totalCells = rows * rows;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    float halfGrid = rows * 0.5f;
    float halfInner = (rows - 1) * 0.5f;
    float topY = center.y + halfGrid * 30.0f + halfInner * 8.0f;
    float leftX = center.x - halfGrid * 30.0f - halfInner * 8.0f;

    std::string frame = getFrameForDifficulty(0);

    size_t pos = frame.find("_001.png");
    while (pos != std::string::npos)
    {
        frame.replace(pos, 8, "_2_001.png");
        pos = frame.find("_001.png", pos + 10);
    }

    for (int i = 0; i < totalCells; i++)
    {
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(getFrameForDifficulty(0).c_str());
        sprite->setColor(cocos2d::ccc3(75, 75, 75));

        cocos2d::CCSprite* innerSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
        sprite->addChild(innerSprite, -1);
        innerSprite->setPosition(cocos2d::CCNode::convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f)));
        innerSprite->setColor(cocos2d::ccc3(60, 60, 60));

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(sprite, nullptr, menu_selector(SecretGame01Layer::onSelectButton));
        item->setSizeMult(1.2f);
        item->setTag(i);
        m_gameObjects->addObject(item);
        m_mainMenu->addChild(item);

        int col = i % rows;
        int row = i / rows;

        cocos2d::CCPoint gridPos(
            leftX + 15.0f + col * 38.0f,
            topY - row * 38.0f);
        item->setPosition(m_mainMenu->convertToNodeSpace(gridPos));

        sprite->setScale(0.1f);
        sprite->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCScaleTo::create(0.6f, 1.0f), 0.6f));
    }

    m_mainMenu->setTouchEnabled(false);
    m_selectedObject = 0;

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.8f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretGame01Layer::gameStep01)),
        nullptr));
}

void SecretGame01Layer::resetGameTimer()
{
    m_gameActive = true;
    m_timeElapsed = 0.0f;
    m_gameTime = (float)getTimeForDifficulty(m_gameDifficulty);
    update(0.0f);
    m_timeBarSprite->setVisible(true);
}

void SecretGame01Layer::scaleOutGame(bool won)
{
    m_timeBarSprite->setVisible(false);
    m_gameActive = false;
    m_mainMenu->setTouchEnabled(false);

    int correctTag = -1;

    if (m_selectedObject < m_requiredObjects->count())
    {
        CCMenuItemSpriteExtra* obj = static_cast<CCMenuItemSpriteExtra*>(m_requiredObjects->objectAtIndex(m_selectedObject));
        correctTag = obj->getTag();
    }

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_gameObjects->objectAtIndex(i));
            cocos2d::CCNode* normalImage = item->getNormalImage();
            cocos2d::CCArray* children = normalImage->getChildren();
            cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(children->objectAtIndex(0));

            int tag = item->getTag();

            cocos2d::ccColor3B outerColor;
            cocos2d::ccColor3B innerColor;

            if (won)
            {
                bool alternate = (tag % 2) != 0;
                outerColor = alternate ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(150, 150, 150);
                innerColor = alternate ? cocos2d::ccc3(100, 255, 100) : cocos2d::ccc3(100, 100, 100);
            }
            else
            {
                if (tag == correctTag)
                {
                    outerColor = cocos2d::ccc3(175, 175, 175);
                    innerColor = cocos2d::ccc3(0, 0, 0);
                }
                else
                {
                    outerColor = cocos2d::ccc3(150, 150, 150);
                    innerColor = cocos2d::ccc3(100, 100, 100);
                }
            }

            normalImage->runAction(cocos2d::CCSequence::create(
                cocos2d::CCTintTo::create(0.1f, outerColor.r, outerColor.g, outerColor.b),
                cocos2d::CCDelayTime::create(0.2f),
                cocos2d::CCFadeOut::create(0.5f),
                nullptr));

            child->runAction(cocos2d::CCSequence::create(
                cocos2d::CCTintTo::create(0.1f, innerColor.r, innerColor.g, innerColor.b),
                cocos2d::CCDelayTime::create(0.2f),
                cocos2d::CCFadeOut::create(0.5f),
                nullptr));

            normalImage->runAction(cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.3f),
                cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.5f, 0.1f), 2.0f),
                nullptr));
        }
    }

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretGame01Layer::resetGame)),
        nullptr));
}

void SecretGame01Layer::showGameOver()
{
    m_gameDifficulty = 0;
    scaleOutGame(false);
}

void SecretGame01Layer::showGameWon()
{
    m_gameDifficulty++;
    scaleOutGame(true);
}

void SecretGame01Layer::update(float dt)
{
    if (!m_gameActive)
        return;

    m_timeElapsed += dt;

    float barWidth = m_timeBarSprite->getContentSize().width;
    float scale = (100.0f / barWidth) * (1.0f - (m_timeElapsed / m_gameTime));

    if (scale > 0.01f)
        m_timeBarSprite->setScaleX(scale);

    if (m_timeElapsed >= m_gameTime)
        showGameOver();
}

SecretGame01Layer::~SecretGame01Layer()
{
}