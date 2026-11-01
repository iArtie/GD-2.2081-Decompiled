#include "Layers/MoreVideoOptionsLayer.h"
#include "Layers/FLAlertLayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Cocos/CCTextInputNode.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "CCScale9Sprite.h"

int MoreVideoOptionsLayer::countForPage(int page)
{
    cocos2d::CCString* str = (cocos2d::CCString*)m_values->valueForKey(pageKey(page));
    return str->intValue();
}

MoreVideoOptionsLayer* MoreVideoOptionsLayer::create()
{
    MoreVideoOptionsLayer* ret = new MoreVideoOptionsLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MoreVideoOptionsLayer::goToPage(int page)
{
    if (m_pageCount < page)
        page = 0;
    else if (page < 0)
        page = m_pageCount;

    if (m_page == page)
        return;

    cocos2d::CCArray* oldObjects = (cocos2d::CCArray*)m_values->objectForKey(objectKey(m_page));
    if (oldObjects) {
        for (unsigned int i = 0; i < oldObjects->count(); ++i) {
            cocos2d::CCNode* node = (cocos2d::CCNode*)oldObjects->objectAtIndex(i);
            node->setVisible(false);
        }
    }

    cocos2d::CCLayer* oldLayer = (cocos2d::CCLayer*)m_values->objectForKey(layerKey(m_page));
    if (oldLayer)
        oldLayer->setVisible(false);

    cocos2d::CCArray* newObjects = (cocos2d::CCArray*)m_values->objectForKey(objectKey(page));
    if (newObjects) {
        for (unsigned int i = 0; i < newObjects->count(); ++i) {
            cocos2d::CCNode* node = (cocos2d::CCNode*)newObjects->objectAtIndex(i);
            node->setVisible(true);
        }
    }

    cocos2d::CCLayer* newLayer = (cocos2d::CCLayer*)m_values->objectForKey(layerKey(page));
    if (newLayer)
        newLayer->setVisible(true);

    m_page = page;
    m_prevButton->setVisible(page > 0);
    m_nextButton->setVisible(m_page < m_pageCount);
}

void MoreVideoOptionsLayer::incrementCountForPage(int page)
{
    int count = countForPage(page);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", count + 1);
    m_values->setObject(str, pageKey(page));
}

const char* MoreVideoOptionsLayer::infoKey(int index)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("info_%i", index);
    return str->getCString();
}

bool MoreVideoOptionsLayer::init()
{
    cocos2d::ccColor4B color = ccc4(0, 0, 0, 0);
    if (!FLAlertLayer::initWithColor(color, 150))
        return false;

    GameManager* gm = GameManager::sharedState();
    if (gm->getGameVariable("0168"))
        m_noElasticity = true;

    FLAlertLayer::incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(m_mainLayer);

    cocos2d::CCRect rect(0.0f, 0.0f, 80.0f, 80.0f);
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    bg->setContentSize(cocos2d::CCSize(360.0f, 260.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Video Options", "bigFont.fnt");
    titleLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f - 16.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    m_variables = cocos2d::CCDictionary::create();
    m_variables->retain();

    m_values = cocos2d::CCDictionary::create();
    m_values->retain();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_fpsNodes = cocos2d::CCArray::create();
    m_fpsNodes->retain();

    GameToolbox::createToggleButton(
        "Smooth Fix", this, menu_selector(MoreVideoOptionsLayer::onToggle),
        0, gm->getGameVariable("0023"), m_buttonMenu, nextPosition(0),
        this, layerForPage(0));
    incrementCountForPage(0);
    m_variables->setObject(cocos2d::CCString::create("0023"), "info_0");

    GameToolbox::createToggleButton(
        "Show FPS", this, menu_selector(MoreVideoOptionsLayer::onToggle),
        0, gm->getGameVariable("0115"), m_buttonMenu, nextPosition(0),
        this, layerForPage(0));
    incrementCountForPage(0);
    m_variables->setObject(cocos2d::CCString::create("0115"), "info_1");

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(MoreVideoOptionsLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 180.0f + 5.0f, winSize.height * 0.5f + 130.0f - 5.0f)));

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(MoreVideoOptionsLayer::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f, winSize.height * 0.5f)));

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(MoreVideoOptionsLayer::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f, winSize.height * 0.5f)));

    goToPage(0);

    return true;
}

void MoreVideoOptionsLayer::keyBackClicked()
{
    onClose(nullptr);
}

cocos2d::CCLayer* MoreVideoOptionsLayer::layerForPage(int page)
{
    cocos2d::CCLayer* layer = (cocos2d::CCLayer*)m_values->objectForKey(layerKey(page));
    if (layer)
        return layer;

    layer = cocos2d::CCLayer::create();
    m_mainLayer->addChild(layer);
    m_values->setObject(layer, layerKey(page));
    return layer;
}

const char* MoreVideoOptionsLayer::layerKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("layer_%i", page);
    return str->getCString();
}

cocos2d::CCPoint MoreVideoOptionsLayer::nextPosition(int page)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint posA(winSize.width * 0.5f - 135.0f, winSize.height * 0.5f + 60.0f);
    cocos2d::CCPoint posB(winSize.width * 0.5f + 35.0f, posA.y);

    int count = countForPage(page);
    cocos2d::CCPoint result = ((count + 1) & 1) ? posA : posB;

    if (count + 1 > 2) {
        float rows = (float)count * 0.5f;
        result.y -= (float)(int)(48 * (int)rows);
    }
    return result;
}

const char* MoreVideoOptionsLayer::objectKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("object_%i", page);
    return str->getCString();
}

cocos2d::CCArray* MoreVideoOptionsLayer::objectsForPage(int page)
{
    cocos2d::CCArray* arr = (cocos2d::CCArray*)m_values->objectForKey(objectKey(page));
    if (arr)
        return arr;

    arr = cocos2d::CCArray::create();
    m_values->setObject(arr, objectKey(page));
    return arr;
}

void MoreVideoOptionsLayer::onApplyFPS(cocos2d::CCObject* sender)
{
    float value = (float)atof(m_fpsInput->getString().c_str());

    if (value < 60.0f)
        value = 60.0f;

    float flooredValue = floorf(value);
    std::string result;
    if (value == flooredValue) {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", (int)flooredValue);
        result = str->getCString();
    } else {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%.03f", value);
        result = str->getCString();
    }

    m_fpsInput->setString(result);

    GameManager::sharedState()->m_customFPSTarget = value;
    GameManager::sharedState()->updateCustomFPS();
}

void MoreVideoOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_fpsInput)
        m_fpsInput->onClickTrackNode(false);

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MoreVideoOptionsLayer::onInfo(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCString* str = (cocos2d::CCString*)m_values->valueForKey(infoKey(tag));
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Info", str->getCString());
    alert->show();
}

void MoreVideoOptionsLayer::onNextPage(cocos2d::CCObject* sender)
{
    goToPage(m_page + 1);
}

void MoreVideoOptionsLayer::onPrevPage(cocos2d::CCObject* sender)
{
    goToPage(m_page - 1);
}

void MoreVideoOptionsLayer::onToggle(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCString* str = (cocos2d::CCString*)m_variables->objectForKey(tag);
    if (!str)
        return;

    const char* key = str->getCString();
    GameManager* gm = GameManager::sharedState();
    gm->toggleGameVariable(key);

    std::string keyStr = key;

    if (keyStr == "0023") {
        gm = GameManager::sharedState();
        PlatformToolbox::toggleSmoothFix(gm->getGameVariable("0023"));
        return;
    }

    if (keyStr == "0115") {
        gm = GameManager::sharedState();
        bool show = gm->getGameVariable("0115");
        cocos2d::CCDirector::sharedDirector()->toggleShowFPS(show, "chatFont.fnt", ccp(0.0f, 0.0f));
        return;
    }

    if (keyStr == "0116") {
        gm = GameManager::sharedState();
        if (gm->getGameVariable("0116")) {
            gm->updateCustomFPS();
        } else {
            cocos2d::CCDirector::sharedDirector()->setAnimationInterval(0.0166666675f);
        }
        updateFPSButtons();
        return;
    }
}

const char* MoreVideoOptionsLayer::pageKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("page_%i", page);
    return str->getCString();
}

void MoreVideoOptionsLayer::updateFPSButtons()
{
    GameManager* gm = GameManager::sharedState();
    bool enabled = gm->getGameVariable("0116");

    for (unsigned int i = 0; i < m_fpsNodes->count(); ++i) {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_fpsNodes->objectAtIndex(i);
        node->setVisible(enabled);
    }
    m_fpsInput->setTouchEnabled(enabled);
}

MoreVideoOptionsLayer::~MoreVideoOptionsLayer()
{
    if (m_variables)
        m_variables->release();
    if (m_values)
        m_values->release();
    if (m_fpsNodes)
        m_fpsNodes->release();
}