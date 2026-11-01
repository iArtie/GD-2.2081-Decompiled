#include "Layers/VideoOptionsLayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/MenuLayer.h"
#include "Layers/MoreVideoOptionsLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Utils/PlatformToolbox.h"

VideoOptionsLayer* VideoOptionsLayer::create()
{
    VideoOptionsLayer* layer = new VideoOptionsLayer();

    if (layer->init())
    {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

bool VideoOptionsLayer::init()
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 150)))
        return false;

    if (GameManager::sharedState()->getGameVariable("0168"))
        m_noElasticity = true;

    if (GameManager::sharedState()->m_menuLayer)
        GameManager::sharedState()->m_menuLayer->videoOptionsOpened();

    incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    m_availableResolutions = cocos2d::CCArray::createWithCapacity(1);
    m_availableResolutions->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    m_fullscreen = !GameManager::sharedState()->getGameVariable("0025");
    m_borderless = GameManager::sharedState()->getGameVariable("0170");
    m_fix = GameManager::sharedState()->getGameVariable("0175");

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(360.0f, 260.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Video Options", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f - 14.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);

    PlatformToolbox::getDisplaySize();

    int currentRes = GameManager::sharedState()->m_resolution;
    m_currentResolution = -1;

    for (int i = 1; i != 28; i++)
    {
        cocos2d::CCSize resSize = GameManager::sharedState()->resolutionForKey(i);
        if (resSize.width <= winSize.width && resSize.height <= winSize.height)
        {
            cocos2d::CCString* resString = cocos2d::CCString::createWithFormat("%i", i);
            m_availableResolutions->addObject(resString);
            if (i == currentRes)
                m_currentResolution = m_availableResolutions->count() - 1;
        }
    }

    if (m_availableResolutions->count() == 0)
    {
        cocos2d::CCString* defaultRes = cocos2d::CCString::createWithFormat("%i", 3);
        m_availableResolutions->addObject(defaultRes);
    }

    if (m_currentResolution == -1)
        m_currentResolution = m_availableResolutions->count() - 1;

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCPoint toggleBase(winSize.width * 0.5f - 65.0f, winSize.height * 0.5f + 85.0f);

    bool fullscreenActive = !GameManager::sharedState()->getGameVariable("0025");
    createToggleButton("Fullscreen", menu_selector(VideoOptionsLayer::onFullscreen), fullscreenActive, m_buttonMenu, cocos2d::CCPoint(toggleBase.x, toggleBase.y), 0.7f, 100.0f, cocos2d::CCPoint(0, 0), false);

    bool borderlessActive = GameManager::sharedState()->getGameVariable("0170");
    cocos2d::CCPoint borderlessPos = toggleBase + cocos2d::CCPoint(-24.0f, -37.0f);
    m_borderlessToggle = createToggleButton("Borderless", menu_selector(VideoOptionsLayer::onBorderless), borderlessActive, m_buttonMenu, cocos2d::CCPoint(0.7f, 100.0f), borderlessPos.x, borderlessPos.y, false);

    bool fixActive = GameManager::sharedState()->getGameVariable("0175");
    cocos2d::CCPoint fixPos = toggleBase + cocos2d::CCPoint(116.0f, -37.0f);
    m_fixToggle = createToggleButton("Fix", menu_selector(VideoOptionsLayer::onBorderlessFix), fixActive, m_buttonMenu, cocos2d::CCPoint(0.7f, 100.0f), fixPos.x, fixPos.y, false);

    m_selectedResolutionLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_selectedResolutionLabel);
    m_selectedResolutionLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 8.0f));

    m_qualityLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_qualityLabel);
    m_qualityLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 66.0f));

    cocos2d::CCLabelBMFont* qualityTitleLabel = cocos2d::CCLabelBMFont::create("Texture Quality", "bigFont.fnt");
    m_mainLayer->addChild(qualityTitleLabel);
    qualityTitleLabel->setPosition(m_qualityLabel->getPosition() + cocos2d::CCPoint(0.0f, 25.0f));
    qualityTitleLabel->setScale(0.8f);

    m_windowedResolutionLabel = cocos2d::CCLabelBMFont::create("Windowed Resolution", "bigFont.fnt");
    m_mainLayer->addChild(m_windowedResolutionLabel);
    m_windowedResolutionLabel->setPosition(m_selectedResolutionLabel->getPosition() + cocos2d::CCPoint(0.0f, 25.0f));
    m_windowedResolutionLabel->setScale(0.8f);

    cocos2d::CCSprite* leftSprite1 = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    m_prevResolutionBtn = CCMenuItemSpriteExtra::create(leftSprite1, nullptr, menu_selector(VideoOptionsLayer::onResolutionPrev));
    m_buttonMenu->addChild(m_prevResolutionBtn);
    cocos2d::CCPoint prevResPos = m_selectedResolutionLabel->getPosition() + cocos2d::CCPoint(-70.0f, 0.0f);
    m_prevResolutionBtn->setPosition(m_buttonMenu->convertToNodeSpace(prevResPos));
    m_prevResolutionBtn->setSizeMult(2.0f);

    cocos2d::CCSprite* rightSprite1 = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    m_nextResolutionBtn = CCMenuItemSpriteExtra::create(rightSprite1, nullptr, menu_selector(VideoOptionsLayer::onResolutionNext));
    m_buttonMenu->addChild(m_nextResolutionBtn);
    cocos2d::CCPoint nextResPos = m_selectedResolutionLabel->getPosition() + cocos2d::CCPoint(70.0f, 0.0f);
    m_nextResolutionBtn->setPosition(m_buttonMenu->convertToNodeSpace(nextResPos));
    m_nextResolutionBtn->setSizeMult(2.0f);

    cocos2d::CCSprite* leftSprite2 = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    CCMenuItemSpriteExtra* prevQualityBtn = CCMenuItemSpriteExtra::create(leftSprite2, nullptr, menu_selector(VideoOptionsLayer::onTextureQualityPrev));
    m_buttonMenu->addChild(prevQualityBtn);
    cocos2d::CCPoint prevQualityPos = m_qualityLabel->getPosition() + cocos2d::CCPoint(-70.0f, 0.0f);
    prevQualityBtn->setPosition(m_buttonMenu->convertToNodeSpace(prevQualityPos));
    prevQualityBtn->setSizeMult(2.0f);

    cocos2d::CCSprite* rightSprite2 = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    CCMenuItemSpriteExtra* nextQualityBtn = CCMenuItemSpriteExtra::create(rightSprite2, nullptr, menu_selector(VideoOptionsLayer::onTextureQualityNext));
    m_buttonMenu->addChild(nextQualityBtn);
    cocos2d::CCPoint nextQualityPos = m_qualityLabel->getPosition() + cocos2d::CCPoint(70.0f, 0.0f);
    nextQualityBtn->setPosition(m_buttonMenu->convertToNodeSpace(nextQualityPos));
    nextQualityBtn->setSizeMult(2.0f);

    cocos2d::CCPoint buttonBase(winSize.width * 0.5f, winSize.height * 0.5f - 130.0f + 23.0f);

    cocos2d::CCNode* backSprite = ButtonSprite::create("Back", 0, 0, 1.0f, true);
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(VideoOptionsLayer::onClose));
    m_buttonMenu->addChild(backButton);

    cocos2d::CCNode* applySprite = ButtonSprite::create("Apply", 0, 0, 1.0f, true);
    CCMenuItemSpriteExtra* applyButton = CCMenuItemSpriteExtra::create(applySprite, nullptr, menu_selector(VideoOptionsLayer::onApply));
    m_buttonMenu->addChild(applyButton);

    cocos2d::CCPoint backPos = buttonBase + cocos2d::CCPoint(-55.0f, 0.0f);
    backButton->setPosition(m_buttonMenu->convertToNodeSpace(backPos));

    cocos2d::CCPoint applyPos = buttonBase + cocos2d::CCPoint(55.0f, 0.0f);
    applyButton->setPosition(m_buttonMenu->convertToNodeSpace(applyPos));

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(VideoOptionsLayer::onClose));
    closeButton->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeButton);
    cocos2d::CCPoint closePos(winSize.width * 0.5f - 180.0f + 5.0f, winSize.height * 0.5f + 130.0f - 5.0f);
    closeButton->setPosition(m_buttonMenu->convertToNodeSpace(closePos));

    updateTextureQuality(GameManager::sharedState()->m_texQuality);
    updateResolution(m_currentResolution);

    cocos2d::CCNode* advancedSprite = ButtonSprite::create("Advanced", 0, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* advancedButton = CCMenuItemSpriteExtra::create(advancedSprite, nullptr, menu_selector(VideoOptionsLayer::onAdvanced));
    m_buttonMenu->addChild(advancedButton);
    cocos2d::CCPoint advancedPos(winSize.width * 0.5f + 180.0f - 48.0f, winSize.height * 0.5f + 130.0f - 20.0f);
    advancedButton->setPosition(m_buttonMenu->convertToNodeSpace(advancedPos));

    return true;
}

void VideoOptionsLayer::keyBackClicked()
{
    onClose(nullptr);
}

void VideoOptionsLayer::onAdvanced(cocos2d::CCObject* sender)
{
    MoreVideoOptionsLayer* layer = MoreVideoOptionsLayer::create();
    layer->show();
}

void VideoOptionsLayer::onApply(cocos2d::CCObject* sender)
{
    cocos2d::CCString* resString = m_availableResolutions->stringAtIndex(m_currentResolution);
    int newResolution = resString->intValue();
    GameManager::sharedState()->resolutionForKey(newResolution);

    bool fullscreenChanged = m_fullscreen != GameManager::sharedState()->getGameVariable("0025");
    bool borderlessUnchanged = m_borderless == GameManager::sharedState()->getGameVariable("0170");
    bool fixUnchanged = m_fix == GameManager::sharedState()->getGameVariable("0175");

    if (!fullscreenChanged || (!m_fullscreen || (borderlessUnchanged && fixUnchanged))
        && newResolution == GameManager::sharedState()->m_resolution
        && GameManager::sharedState()->m_texQuality == m_textureQuality)
    {
        FLAlertLayer::create(nullptr, "Graphics", "No changes have been made.", "OK", nullptr)->show();
    }
    else
    {
        GameManager::sharedState()->m_resolution = newResolution;
        GameManager::sharedState()->m_texQuality = m_textureQuality;

        cocos2d::CCScene* scene = cocos2d::CCScene::create();
        cocos2d::CCDirector::sharedDirector()->replaceScene(scene);

        PlatformToolbox::resizeWindow(0, 0);

        if (m_fullscreen == GameManager::sharedState()->getGameVariable("0025"))
        {
            GameManager::sharedState()->setGameVariable("0025", !m_fullscreen);
            PlatformToolbox::toggleFullScreen(m_fullscreen, m_borderless, m_fix);
        }

        cocos2d::TextureQuality oldQuality = cocos2d::CCDirector::sharedDirector()->m_eTextureQuality;
        cocos2d::CCDirector::sharedDirector()->updateContentScale((cocos2d::TextureQuality)m_textureQuality);

        if (oldQuality == cocos2d::CCDirector::sharedDirector()->m_eTextureQuality)
            GameManager::sharedState()->queueReloadMenu();
        else
            GameManager::sharedState()->reloadAll(false, false, false);
    }
}

void VideoOptionsLayer::onBorderless(cocos2d::CCObject* sender)
{
    m_borderless = !m_borderless;
    toggleResolution();
}

void VideoOptionsLayer::onBorderlessFix(cocos2d::CCObject* sender)
{
    m_fix = !m_fix;
}

void VideoOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->m_menuLayer)
        GameManager::sharedState()->m_menuLayer->videoOptionsClosed();

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void VideoOptionsLayer::onFullscreen(cocos2d::CCObject* sender)
{
    m_fullscreen = !m_fullscreen;
    toggleResolution();
}

void VideoOptionsLayer::onInfo(cocos2d::CCObject* sender)
{
    FLAlertLayer::create(nullptr, "Video Options", "No info yet")->show();
}

void VideoOptionsLayer::onResolutionNext(cocos2d::CCObject* sender)
{
    updateResolution(m_currentResolution + 1);
}

void VideoOptionsLayer::onResolutionPrev(cocos2d::CCObject* sender)
{
    updateResolution(m_currentResolution - 1);
}

void VideoOptionsLayer::onTextureQualityNext(cocos2d::CCObject* sender)
{
    updateTextureQuality(m_textureQuality + 1);
}

void VideoOptionsLayer::onTextureQualityPrev(cocos2d::CCObject* sender)
{
    updateTextureQuality(m_textureQuality - 1);
}

void VideoOptionsLayer::reloadMenu()
{
    cocos2d::CCScene* scene = MenuLayer::scene(true);
    cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
}

void VideoOptionsLayer::toggleResolution()
{
    m_selectedResolutionLabel->setColor(m_fullscreen ? cocos2d::ccc3(128, 128, 128) : cocos2d::ccc3(255, 255, 255));
    m_windowedResolutionLabel->setColor(m_fullscreen ? cocos2d::ccc3(128, 128, 128) : cocos2d::ccc3(255, 255, 255));

    m_prevResolutionBtn->setEnabled(!m_fullscreen);
    m_nextResolutionBtn->setEnabled(!m_fullscreen);

    m_prevResolutionBtn->getNormalImage()->setColor(m_fullscreen ? cocos2d::ccc3(128, 128, 128) : cocos2d::ccc3(255, 255, 255));
    m_nextResolutionBtn->getNormalImage()->setColor(m_fullscreen ? cocos2d::ccc3(128, 128, 128) : cocos2d::ccc3(255, 255, 255));

    m_borderlessToggle->setEnabled(m_fullscreen);
    m_borderlessToggle->m_offButton->getNormalImage()->setColor(m_fullscreen ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(128, 128, 128));
    m_borderlessToggle->m_onButton->getNormalImage()->setColor(m_fullscreen ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(128, 128, 128));
    m_borderlessLabel->setColor(m_fullscreen ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(128, 128, 128));

    bool fixEnabled = m_fullscreen && m_borderless;
    m_fixToggle->setEnabled(fixEnabled);
    cocos2d::ccColor3B fixColor = fixEnabled ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(128, 128, 128);
    m_fixToggle->m_offButton->getNormalImage()->setColor(fixColor);
    m_fixToggle->m_onButton->getNormalImage()->setColor(fixColor);
    m_fixLabel->setColor(fixColor);
}

void VideoOptionsLayer::updateResolution(int resolution)
{
    int count = m_availableResolutions->count();

    if (count <= resolution)
    {
        resolution = 0;
    }
    else if (resolution < 0)
    {
        resolution = count - 1;
    }

    m_currentResolution = resolution;

    cocos2d::CCString* resString = m_availableResolutions->stringAtIndex(resolution);
    int resKey = resString->intValue();
    cocos2d::CCSize resSize = GameManager::sharedState()->resolutionForKey(resKey);

    cocos2d::CCString* displayString = cocos2d::CCString::createWithFormat("%ix%i", (int)resSize.width, (int)resSize.height);
    m_selectedResolutionLabel->setString(displayString->getCString());
    m_selectedResolutionLabel->limitLabelWidth(100.0f, 0.75f, 0.0f);

    toggleResolution();
}

void VideoOptionsLayer::updateTextureQuality(int quality)
{
    if (quality > 3)
    {
        m_textureQuality = 0;
        m_qualityLabel->setString("Auto");
    }
    else if (quality < 0)
    {
        m_textureQuality = 3;
        m_qualityLabel->setString("High");
    }
    else
    {
        m_textureQuality = quality;
        switch (quality)
        {
            case 1:
                m_qualityLabel->setString("Low");
                break;
            case 2:
                m_qualityLabel->setString("Medium");
                break;
            case 3:
                m_qualityLabel->setString("High");
                break;
            default:
                m_qualityLabel->setString("Auto");
                break;
        }
    }

    m_qualityLabel->limitLabelWidth(100.0f, 0.75f, 0.0f);
}

VideoOptionsLayer::~VideoOptionsLayer()
{
}