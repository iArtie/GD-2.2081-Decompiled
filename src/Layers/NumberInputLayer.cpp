#include "Layers/NumberInputLayer.h"
#include "Delegates/NumberInputDelegate.h"
#include "Layers/FLAlertLayer.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "cocos2d-ext/CCScale9Sprite.h"

NumberInputLayer* NumberInputLayer::create()
{
    NumberInputLayer* returnVal = new NumberInputLayer();
    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool NumberInputLayer::init()
{
    if (!FLAlertLayer::init(150))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(270.0f, 270.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 135.0f - 16.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(NumberInputLayer::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f - 135.0f + 10.0f, winSize.height * 0.5f + 135.0f - 10.0f));
    closeBtn->setPosition(closePos);

    float baseY = winSize.height * 0.5f + 50.0f;

    for (int i = 1; i <= 12; i++)
    {
        CCMenuItemSpriteExtra* button;
        float posX;
        float posY;

        if (i <= 9)
        {
            cocos2d::CCString* numStr = cocos2d::CCString::createWithFormat("%i", i);
            cocos2d::CCNode* sprite = ButtonSprite::create(
                numStr->getCString(), 0, 0, 1.0f, true, "bigFont.fnt", "GJ_button_01.png", 40.0f);
            button = CCMenuItemSpriteExtra::create(
                sprite, nullptr, menu_selector(NumberInputLayer::onNumber));
            button->setTag(i);
            m_buttonMenu->addChild(button);

            if (i <= 3)
                posY = winSize.height * 0.5f + 50.0f;
            else if (i <= 6)
                posY = baseY - 50.0f;
            else
                posY = baseY - 100.0f;
        }
        else
        {
            const char* label;
            int tag;
            if (i == 10)
            {
                label = "<";
                tag = 10;
            }
            else if (i == 11)
            {
                label = "0";
                tag = 0;
            }
            else
            {
                label = "OK";
                tag = 12;
            }
            cocos2d::CCNode* sprite = ButtonSprite::create(
                label, 0, 0, 1.0f, true, "bigFont.fnt", "GJ_button_01.png", 40.0f);
            button = CCMenuItemSpriteExtra::create(
                sprite, nullptr, menu_selector(NumberInputLayer::onNumber));
            button->setTag(tag);
            m_buttonMenu->addChild(button);
            posY = baseY - 150.0f;
        }

        posX = winSize.width * 0.5f;
        if (i % 3 == 0)
            posX += 60.0f;
        else if (i % 3 == 1)
            posX -= 60.0f;

        cocos2d::CCPoint pos = cocos2d::CCNode::convertToNodeSpace(
            m_buttonMenu, cocos2d::CCPoint(posX, posY));
        button->setPosition(pos);

        if (i == 12)
            m_okButton = button;
    }

    m_inputLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_inputLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f));
    m_mainLayer->addChild(m_inputLabel);
    m_inputLabel->setScale(0.8f);

    updateNumberState();

    return true;
}

void NumberInputLayer::deleteLast()
{
    if (!m_inputString.empty())
    {
        m_inputString = m_inputString.substr(0, m_inputString.size() - 1);
        updateNumberState();
    }
}

void NumberInputLayer::inputNumber(int num)
{
    if ((int)m_inputString.size() < m_maximum)
    {
        cocos2d::CCString* numStr = cocos2d::CCString::createWithFormat("%i", num);
        m_inputString += numStr->getCString();
        updateNumberState();
    }
}

void NumberInputLayer::keyBackClicked()
{
    onClose(nullptr);
}

void NumberInputLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void NumberInputLayer::onDone(cocos2d::CCObject* sender)
{
    if (m_delegate)
        m_delegate->numberInputClosed(this);
    onClose(nullptr);
}

void NumberInputLayer::onNumber(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    if (tag == 10)
    {
        deleteLast();
    }
    else if (tag == 12)
    {
        onDone(nullptr);
    }
    else
    {
        inputNumber(tag);
    }
}

void NumberInputLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -501, true);
}

void NumberInputLayer::updateNumberState()
{
    int len = (int)m_inputString.size();
    bool valid = (len >= m_minimum && len <= m_maximum);

    ButtonSprite* sprite = (ButtonSprite*)m_okButton->getNormalImage();
    if (valid)
        sprite->setColor(cocos2d::ccc3(0, 255, 0));
    else
        sprite->setColor(cocos2d::ccc3(255, 0, 0));

    m_okButton->setEnabled(valid);
    m_inputLabel->setString(m_inputString.c_str());
}

NumberInputLayer::~NumberInputLayer()
{
}