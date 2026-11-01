#include "DialogLayer.h"
#include "TextArea.h"
#include "extension/CCScale9Sprite.h"
#include "GameToolbox.h"
#include <time.h>

DialogLayer* DialogLayer::create(DialogObject* object, int background)
{
    return DialogLayer::createDialogLayer(object, nullptr, background);
}

DialogLayer* DialogLayer::createWithObjects(cocos2d::CCArray* objects, int background)
{
    return DialogLayer::createDialogLayer(nullptr, objects, background);
}

DialogLayer* DialogLayer::createDialogLayer(DialogObject* object, cocos2d::CCArray* objects, int background)
{
    DialogLayer* pRet = new DialogLayer();
    if (pRet && pRet->init(object, objects, background))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DialogLayer::init(DialogObject* object, cocos2d::CCArray* objects, int background)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 150)))
        return false;
    
    m_delegate = nullptr;
    m_touchID = -1;
    m_animating = false;
    m_skippable = false;
    m_handleTap = false;
    m_noRemove = false;
    m_animateTime = 0.0f;
    m_animationType = DialogAnimationType::FromCenter;
    
    if (objects)
    {
        m_dialogObjects = objects;
        objects->retain();
    }
    else
    {
        m_dialogObjects = cocos2d::CCArray::create();
        m_dialogObjects->retain();
        if (object)
            m_dialogObjects->addObject(object);
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->registerForcePrio(this, 2);
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    m_mainLayer->setContentSize(cocos2d::CCSize(380, 100));
    m_mainLayer->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        cocos2d::CCString::createWithFormat("GJ_square%02d.png", background)->getCString(),
        cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(380, 100));
    m_mainLayer->addChild(bg, -2);
    
    m_characterLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_characterLabel, 2);
    m_characterLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    m_characterLabel->setPosition(cocos2d::CCPoint(-93.0f, 36.0f));
    m_characterLabel->limitLabelWidth(200.0f, 0.5f, 0.0f);
    
    cocos2d::extension::CCScale9Sprite* textBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0, 0, 80, 80));
    textBg->setContentSize(cocos2d::CCSize(280, 80));
    textBg->setOpacity(50);
    m_mainLayer->addChild(textBg, -1);
    textBg->setPosition(cocos2d::CCPoint(40.0f, 0.0f));
    textBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    
    m_navButtonSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
    m_mainLayer->addChild(m_navButtonSprite, 3);
    m_navButtonSprite->setPosition(cocos2d::CCPoint(0.0f, -50.0f));
    m_navButtonSprite->setOpacity(0);
    
    if (m_dialogObjects && m_dialogObjects->count() > 0)
        displayNextObject();
    else
        displayDialogObject(object);
    
    updateNavButtonFrame();
    
    return true;
}

void DialogLayer::addToMainScene()
{
    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    int zOrder = scene->getHighestChildZ() + 1;
    if (zOrder < 1000) zOrder = 1000;
    scene->addChild(this, zOrder);
}

void DialogLayer::animateIn(DialogAnimationType type)
{
    setVisible(true);
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    m_animateTime = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    
    if (type == DialogAnimationType::FromCenter)
    {
        m_mainLayer->setScale(0.1f);
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.6f));
    }
    else if (type != DialogAnimationType::Instant)
    {
        cocos2d::CCPoint targetPos = m_mainLayer->getPosition();
        
        if (type == DialogAnimationType::FromLeft)
        {
            m_mainLayer->setPosition(cocos2d::CCPoint(-191.0f, targetPos.y));
        }
        else if (type == DialogAnimationType::FromRight)
        {
            m_mainLayer->setPosition(cocos2d::CCPoint(
                cocos2d::CCDirector::sharedDirector()->getScreenRight() + 190.0f + 1.0f,
                targetPos.y));
        }
        else if (type == DialogAnimationType::FromTop)
        {
            m_mainLayer->setPosition(cocos2d::CCPoint(
                targetPos.x,
                cocos2d::CCDirector::sharedDirector()->getScreenTop() + 50.0f + 1.0f));
        }
        else // FromTop2
        {
            m_mainLayer->setPosition(cocos2d::CCPoint(
                targetPos.x,
                cocos2d::CCDirector::sharedDirector()->getScreenTop() - 50.0f - 1.0f));
        }
        
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCMoveTo::create(0.5f, targetPos), 0.6f));
    }
    
    unsigned char opacity = getOpacity();
    if (opacity > 0)
    {
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
    }
}

void DialogLayer::animateInDialog()
{
    animateIn(m_animationType);
}

void DialogLayer::animateInRandomSide()
{
    float random = (float)rand() * 4.6566e-10f;
    if (random > 0.66f)
        animateIn(DialogAnimationType::FromLeft);
    else if (random > 0.33f)
        animateIn(DialogAnimationType::FromRight);
    else
        animateIn(DialogAnimationType::FromCenter);
}

bool DialogLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == -1)
    {
        m_touchID = touch->m_nId;
        handleDialogTap();
        return true;
    }
    return false;
}

void DialogLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == touch->m_nId)
        ccTouchEnded(touch, event);
}

void DialogLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == touch->m_nId)
        m_touchID = -1;
}

void DialogLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void DialogLayer::displayDialogObject(DialogObject* object)
{
    if (!object)
        return;
    
    m_animating = true;
    m_skippable = object->m_unskippable;
    
    m_navButtonSprite->stopAllActions();
    m_navButtonSprite->setOpacity(0);
    
    if (m_characterSprite)
    {
        m_characterSprite->removeMeAndCleanup();
        m_characterSprite = nullptr;
    }
    
    if (m_textArea)
    {
        m_textArea->setDelegate(nullptr);
        m_textArea->removeMeAndCleanup();
        m_textArea = nullptr;
    }
    
    m_characterLabel->setString(object->m_character.c_str());
    m_characterLabel->setColor(object->m_nameColor);
    
    if (object->m_characterFrame > 0)
    {
        int frame = object->m_characterFrame;
        if (frame > 56) frame = 56;
        
        m_characterSprite = cocos2d::CCSprite::create(
            cocos2d::CCString::createWithFormat("dialogIcon_%03d.png", frame)->getCString());
        m_mainLayer->addChild(m_characterSprite, 2);
        m_characterSprite->setPosition(cocos2d::CCPoint(-143.0f, 0.0f));
    }
    
    float textScale = object->m_textScale;
    m_textArea = TextArea::create(
        object->m_text,
        "chatFont.fnt",
        1.0f,
        220.0f / textScale,
        cocos2d::CCPoint(20.0f, 0.0f),
        0.0f,
        0
    );
    m_textArea->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_textArea->setScale(textScale);
    m_textArea->setDelegate(this);
    m_mainLayer->addChild(m_textArea, 3);
    m_textArea->setPosition(cocos2d::CCPoint(-92.0f, 0.0f));
    m_textArea->fadeInCharacters(0.2f, 0.02f, 0, TextFadeInStyle::Fade);
}

void DialogLayer::displayNextObject()
{
    if (m_dialogObjects && m_dialogObjects->count() > 0)
    {
        DialogObject* obj = (DialogObject*)m_dialogObjects->objectAtIndex(0);
        displayDialogObject(obj);
        m_dialogObjects->removeObjectAtIndex(0);
        updateNavButtonFrame();
    }
}

void DialogLayer::fadeInTextFinished(TextArea* textArea)
{
    m_animating = false;
    m_navButtonSprite->stopAllActions();
    m_navButtonSprite->setOpacity(0);
    
    if (m_dialogObjects && m_dialogObjects->count() > 0)
    {
        m_navButtonSprite->runAction(cocos2d::CCFadeIn::create(0.3f));
    }
    else
    {
        m_navButtonSprite->runAction(cocos2d::CCFadeIn::create(0.5f));
    }
}

void DialogLayer::finishCurrentAnimation()
{
    m_animating = false;
    if (m_textArea)
    {
        m_textArea->setDelegate(nullptr);
        m_textArea->stopAllCharacterActions();
        m_textArea->showAll();
    }
    m_navButtonSprite->stopAllActions();
    m_navButtonSprite->setOpacity(255);
    updateNavButtonFrame();
}

void DialogLayer::handleDialogTap()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    float elapsed = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0 - m_animateTime;
    
    if (elapsed >= 0.2f || m_animateTime <= 0.0f)
    {
        if (m_animating)
        {
            if (!m_skippable)
                finishCurrentAnimation();
        }
        else
        {
            if (m_dialogObjects && m_dialogObjects->count() > 0)
                displayNextObject();
            else
                onClose();
        }
    }
}

void DialogLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == cocos2d::KEY_Up || key == cocos2d::KEY_Escape || key == cocos2d::KEY_Space ||
        key == cocos2d::CONTROLLER_RB || key == cocos2d::CONTROLLER_LB || key == cocos2d::CONTROLLER_A)
    {
        handleDialogTap();
    }
}

void DialogLayer::onClose()
{
    if (m_delegate)
        m_delegate->dialogClosed(this);
    m_delegate = nullptr;
    
    if (m_textArea)
        m_textArea->setDelegate(nullptr);
    
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    
    if (!m_noRemove)
        removeFromParentAndCleanup(true);
}

void DialogLayer::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addPrioTargetedDelegate(this, -501, true);
}

void DialogLayer::updateChatPlacement(DialogChatPlacement placement)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    if (placement == DialogChatPlacement::Center)
    {
        m_mainLayer->setPosition(cocos2d::CCPoint(centerX, centerY));
    }
    else if (placement == DialogChatPlacement::Top)
    {
        m_mainLayer->setPosition(cocos2d::CCPoint(centerX, winSize.height - 50.0f - 20.0f));
    }
    else if (placement == DialogChatPlacement::Bottom)
    {
        m_mainLayer->setPosition(cocos2d::CCPoint(centerX, 70.0f));
    }
}

void DialogLayer::updateNavButtonFrame()
{
    if (m_dialogObjects && m_dialogObjects->count() > 0)
    {
        m_navButtonSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_chatBtn_01_001.png"));
    }
    else
    {
        m_navButtonSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_chatBtn_02_001.png"));
    }
}

DialogLayer::~DialogLayer()
{
    CC_SAFE_RELEASE(m_dialogObjects);
    if (m_textArea)
        m_textArea->setDelegate(nullptr);
}