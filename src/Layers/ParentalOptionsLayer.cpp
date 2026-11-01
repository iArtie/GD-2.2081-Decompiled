#include "Layers/ParentalOptionsLayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "cocos2d-ext/CCScale9Sprite.h"
#include <cmath>

ParentalOptionsLayer* ParentalOptionsLayer::create()
{
    ParentalOptionsLayer* returnVal = new ParentalOptionsLayer();
    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool ParentalOptionsLayer::init()
{
    if (!FLAlertLayer::init(75))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(360.0f, 260.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Parental Controls", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f - 16.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    m_variables = cocos2d::CCDictionary::create();
    m_variables->retain();
    m_values = cocos2d::CCDictionary::create();
    m_values->retain();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(ParentalOptionsLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f - 180.0f + 5.0f, winSize.height * 0.5f + 130.0f - 5.0f));
    closeBtn->setPosition(closePos);

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, menu_selector(ParentalOptionsLayer::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    cocos2d::CCPoint prevPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f, winSize.height * 0.5f));
    m_prevButton->setPosition(prevPos);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, menu_selector(ParentalOptionsLayer::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    cocos2d::CCPoint nextPos = cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu,
        cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f, winSize.height * 0.5f));
    m_nextButton->setPosition(nextPos);

    goToPage(0);

    return true;
}

int ParentalOptionsLayer::countForPage(int page)
{
    std::string key = pageKey(page);
    cocos2d::CCString* value = (cocos2d::CCString*)m_values->valueForKey(key);
    return value->intValue();
}

void ParentalOptionsLayer::goToPage(int page)
{
    int targetPage = page;
    if (m_maxPage < page)
        targetPage = 0;
    else if (page < 0)
        targetPage = m_maxPage;

    if (m_page == targetPage)
        return;

    std::string oldObjKey = objectKey(m_page);
    cocos2d::CCArray* oldObjects = (cocos2d::CCArray*)m_values->objectForKey(oldObjKey);
    if (oldObjects)
    {
        for (unsigned int i = 0; i < oldObjects->count(); i++)
        {
            cocos2d::CCObject* obj = oldObjects->objectAtIndex(i);
            cocos2d::CCNode* node = (cocos2d::CCNode*)obj;
            node->setVisible(false);
        }
    }

    std::string oldLayerKey = layerKey(m_page);
    cocos2d::CCLayer* oldLayer = (cocos2d::CCLayer*)m_values->objectForKey(oldLayerKey);
    if (oldLayer)
        oldLayer->setVisible(false);

    std::string newObjKey = objectKey(targetPage);
    cocos2d::CCArray* newObjects = (cocos2d::CCArray*)m_values->objectForKey(newObjKey);
    if (newObjects)
    {
        for (unsigned int i = 0; i < newObjects->count(); i++)
        {
            cocos2d::CCObject* obj = newObjects->objectAtIndex(i);
            cocos2d::CCNode* node = (cocos2d::CCNode*)obj;
            node->setVisible(true);
        }
    }

    std::string newLayerKey = layerKey(targetPage);
    cocos2d::CCLayer* newLayer = (cocos2d::CCLayer*)m_values->objectForKey(newLayerKey);
    if (newLayer)
        newLayer->setVisible(true);

    m_page = targetPage;
    m_prevButton->setVisible(targetPage > 0);
    m_nextButton->setVisible(m_page < m_maxPage);
}

void ParentalOptionsLayer::incrementCountForPage(int page)
{
    int count = countForPage(page);
    cocos2d::CCString* value = cocos2d::CCString::createWithFormat("%i", count + 1);
    std::string key = pageKey(page);
    m_values->setObject(value, key);
}

const char* ParentalOptionsLayer::infoKey(int index)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("info_%i", index);
    return str->getCString();
}

cocos2d::CCLayer* ParentalOptionsLayer::layerForPage(int page)
{
    std::string key = layerKey(page);
    cocos2d::CCLayer* layer = (cocos2d::CCLayer*)m_values->objectForKey(key);
    if (layer)
        return layer;

    layer = cocos2d::CCLayer::create();
    m_mainLayer->addChild(layer);
    m_values->setObject(layer, key);
    return layer;
}

const char* ParentalOptionsLayer::layerKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("layer_%i", page);
    return str->getCString();
}

cocos2d::CCPoint ParentalOptionsLayer::nextPosition(int page)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint leftPos(winSize.width * 0.5f - 135.0f, winSize.height * 0.5f + 60.0f);
    cocos2d::CCPoint rightPos(winSize.width * 0.5f + 35.0f, leftPos.y);

    int count = countForPage(page);
    cocos2d::CCPoint result = ((count + 1) & 1) ? leftPos : rightPos;

    if (count + 1 > 2)
    {
        float rowOffset = (float)(int)(48 * std::floor((float)count * 0.5f));
        result.y -= rowOffset;
    }

    return result;
}

const char* ParentalOptionsLayer::objectKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("object_%i", page);
    return str->getCString();
}

cocos2d::CCArray* ParentalOptionsLayer::objectsForPage(int page)
{
    std::string key = objectKey(page);
    cocos2d::CCArray* array = (cocos2d::CCArray*)m_values->objectForKey(key);
    if (array)
        return array;

    array = cocos2d::CCArray::create();
    m_values->setObject(array, key);
    return array;
}

void ParentalOptionsLayer::keyBackClicked()
{
    onClose(nullptr);
}

void ParentalOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ParentalOptionsLayer::onInfo(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    std::string infoKeyStr = infoKey(tag);
    cocos2d::CCString* infoValue = (cocos2d::CCString*)m_values->valueForKey(infoKeyStr);
    std::string infoText = infoValue->getCString();

    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Info", infoText);
    alert->show();
}

void ParentalOptionsLayer::onNextPage(cocos2d::CCObject* sender)
{
    goToPage(m_page + 1);
}

void ParentalOptionsLayer::onPrevPage(cocos2d::CCObject* sender)
{
    goToPage(m_page - 1);
}

void ParentalOptionsLayer::onToggle(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCString* varKey = (cocos2d::CCString*)m_variables->objectForKey(std::to_string(tag));
    if (varKey)
    {
        GameManager::sharedState()->toggleGameVariable(varKey->getCString());
    }
}

const char* ParentalOptionsLayer::pageKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("page_%i", page);
    return str->getCString();
}

ParentalOptionsLayer::~ParentalOptionsLayer()
{
}