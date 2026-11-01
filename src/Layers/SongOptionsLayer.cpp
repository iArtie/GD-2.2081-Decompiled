#include "Layers/SongOptionsLayer.h"
#include "Delegates/CustomSongDelegate.h"
#include "Models/LevelSettingsObject.h"
#include "Utils/GameToolbox.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Audio/FMODAudioEngine.h"

SongOptionsLayer* SongOptionsLayer::create(CustomSongDelegate* delegate) {
    SongOptionsLayer* returnVal = new SongOptionsLayer();
    if (returnVal->init(delegate)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool SongOptionsLayer::init(CustomSongDelegate* delegate) {
    if (!FLAlertLayer::init(75))
        return false;

    m_delegate = delegate;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(360.0f, 220.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Song Options", "bigFont.fnt");
    titleLabel->setPosition(centerX, centerY + 110.0f - 20.0f);
    m_mainLayer->addChild(titleLabel);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCPoint fadeInPos(centerX - 140.0f, centerY - 40.0f);
    cocos2d::CCPoint fadeOutPos(centerX + 30.0f, fadeInPos.y);

    bool fadeIn = m_delegate->getLevelSettings()->m_fadeIn;
    GameToolbox::createToggleButton(
        "Fade in", menu_selector(SongOptionsLayer::onFadeIn), false,
        fadeIn, m_buttonMenu, &fadeInPos, this, m_mainLayer,
        0.8f, 0.6f, 150.0f, nullptr, "bigFont.fnt", false, 0, nullptr);

    bool fadeOut = m_delegate->getLevelSettings()->m_fadeOut;
    GameToolbox::createToggleButton(
        "Fade out", menu_selector(SongOptionsLayer::onFadeOut), false,
        fadeOut, m_buttonMenu, &fadeOutPos, this, m_mainLayer,
        0.8f, 0.6f, 150.0f, nullptr, "bigFont.fnt", false, 0, nullptr);

    cocos2d::CCPoint dontResetPos(fadeInPos.x, fadeInPos.y - 40.0f);
    bool dontReset = m_delegate->getLevelSettings()->m_dontReset;
    GameToolbox::createToggleButton(
        "Dont Reset", menu_selector(SongOptionsLayer::onSongPersistent), false,
        dontReset, m_buttonMenu, &dontResetPos, this, m_mainLayer,
        0.8f, 0.6f, 150.0f, nullptr, "bigFont.fnt", false, 0, nullptr);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(SongOptionsLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX - 180.0f + 5.0f, centerY + 110.0f - 5.0f)));

    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite, nullptr, menu_selector(SongOptionsLayer::onInfo));
    infoBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX + 180.0f - 17.0f, centerY + 110.0f - 17.0f)));

    cocos2d::CCPoint offsetPos(centerX, centerY + 25.0f);

    cocos2d::CCLabelBMFont* offsetLabel = cocos2d::CCLabelBMFont::create("Offset", "bigFont.fnt");
    offsetLabel->setScale(0.4f);
    offsetLabel->setPosition(offsetPos + cocos2d::CCPoint(0.0f, 25.0f));
    m_mainLayer->addChild(offsetLabel);

    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    inputBg->setOpacity(100);
    inputBg->setContentSize(cocos2d::CCSize(100.0f, 30.0f));
    m_mainLayer->addChild(inputBg, -1);
    inputBg->setPosition(offsetPos);

    m_offsetInput = CCTextInputNode::create(100.0f, 50.0f, "Seconds", "Thonburi");
    m_mainLayer->addChild(m_offsetInput, 20);
    m_offsetInput->setPosition(offsetPos);
    m_offsetInput->setMaxLabelScale(0.7f);
    m_offsetInput->setMaxLabelWidth(200.0f);
    m_offsetInput->m_textField->setVisible(false);
    m_offsetInput->m_maxLabelLength = 5;
    m_offsetInput->setAllowedChars("0123456789.");
    m_offsetInput->setLabelPlaceholderColor(cocos2d::ccc3(120, 170, 240));
    m_offsetInput->setLabelPlaceholderScale(0.5f);

    float songOffset = m_delegate->getLevelSettings()->m_songOffset;
    if (songOffset > 0.0f) {
        std::string offsetStr = cocos2d::CCString::createWithFormat("%0.1f", songOffset)->getCString();
        m_offsetInput->setString(offsetStr);
    }

    cocos2d::CCSprite* playbackSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png");
    m_playbackButton = CCMenuItemSpriteExtra::create(
        playbackSprite, nullptr, menu_selector(SongOptionsLayer::onPlayback));
    m_buttonMenu->addChild(m_playbackButton);
    m_playbackButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX + 100.0f, offsetPos.y)));

    return true;
}

void SongOptionsLayer::keyBackClicked() {
    onClose(nullptr);
}

void SongOptionsLayer::onClose(cocos2d::CCObject* sender) {
    m_offsetInput->onClickTrackNode(false);
    float oldOffset = m_delegate->getLevelSettings()->m_songOffset;
    LevelSettingsObject* settings = m_delegate->getLevelSettings();
    float newOffset = atof(m_offsetInput->getString().c_str());
    settings->m_songOffset = newOffset;
    if (oldOffset != m_delegate->getLevelSettings()->m_songOffset)
        m_delegate->getLevelSettings()->m_guidelinesUpdated = true;
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SongOptionsLayer::onFadeIn(cocos2d::CCObject* sender) {
    LevelSettingsObject* settings = m_delegate->getLevelSettings();
    settings->m_fadeIn = !settings->m_fadeIn;
}

void SongOptionsLayer::onFadeOut(cocos2d::CCObject* sender) {
    LevelSettingsObject* settings = m_delegate->getLevelSettings();
    settings->m_fadeOut = !settings->m_fadeOut;
}

void SongOptionsLayer::onInfo(cocos2d::CCObject* sender) {
    FLAlertLayer::create(
        nullptr,
        "Options",
        "<cl>Fade in:</c> Song starts with a fade in.\n"
        "<cy>Fade out:</c> Song fades out at the end of the level.\n"
        "<cg>Offset:</c> Change the start time of the song.")->show();
}

void SongOptionsLayer::onPlayback(cocos2d::CCObject* sender) {
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(false)) {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    } else {
        float offset = atof(m_offsetInput->getString().c_str());
        std::string songFileName = m_delegate->getSongFileName();
        FMODAudioEngine::sharedEngine()->loadAndPlayMusic(songFileName, (unsigned int)(offset * 1000.0f), false);
    }
    updatePlaybackBtn();
}

void SongOptionsLayer::onSongPersistent(cocos2d::CCObject* sender) {
    LevelSettingsObject* settings = m_delegate->getLevelSettings();
    settings->m_dontReset = !settings->m_dontReset;
}

void SongOptionsLayer::updatePlaybackBtn() {
    cocos2d::CCNode* image = m_playbackButton->getNormalImage();
    cocos2d::CCSpriteFrame* frame;
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(false)) {
        frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_stopMusicBtn_001.png");
    } else {
        frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_playMusicBtn_001.png");
    }
    ((cocos2d::CCSprite*)image)->setDisplayFrame(frame);
}

SongOptionsLayer::~SongOptionsLayer() {
}