#include "OptionsLayer.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "FMODAudioEngine.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "FLAlertLayer.h"
#include "MoreOptionsLayer.h"
#include "VideoOptionsLayer.h"
#include "TutorialLayer.h"
#include "SupportLayer.h"
#include "AccountLayer.h"
#include "SongsLayer.h"
#include "SecretLayer.h"
#include "DialogObject.h"
#include "DialogLayer.h"

OptionsLayer* OptionsLayer::create()
{
    OptionsLayer* returnVal = new OptionsLayer();
    if (returnVal->GJDropDownLayer::init("Settings"))
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void OptionsLayer::customSetup()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_optionsMenu = cocos2d::CCMenu::create();

    cocos2d::CCNode* accountBtnSprite = ButtonSprite::create("Account");
    CCMenuItemSpriteExtra* accountBtn = CCMenuItemSpriteExtra::create(
        accountBtnSprite, nullptr, menu_selector(OptionsLayer::onAccount));
    m_optionsMenu->addChild(accountBtn);

    cocos2d::CCNode* helpBtnSprite = ButtonSprite::create("How To Play");
    CCMenuItemSpriteExtra* helpBtn = CCMenuItemSpriteExtra::create(
        helpBtnSprite, nullptr, menu_selector(OptionsLayer::onHelp));
    m_optionsMenu->addChild(helpBtn);

    cocos2d::CCNode* optionsBtnSprite = ButtonSprite::create("Options");
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(
        optionsBtnSprite, nullptr, menu_selector(OptionsLayer::onOptions));
    m_optionsMenu->addChild(optionsBtn);

    cocos2d::CCNode* rateBtnSprite = ButtonSprite::create("Rate");
    CCMenuItemSpriteExtra* rateBtn = CCMenuItemSpriteExtra::create(
        rateBtnSprite, nullptr, menu_selector(OptionsLayer::onRate));
    m_optionsMenu->addChild(rateBtn);

    cocos2d::CCNode* songsBtnSprite = ButtonSprite::create("Songs");
    CCMenuItemSpriteExtra* songsBtn = CCMenuItemSpriteExtra::create(
        songsBtnSprite, nullptr, menu_selector(OptionsLayer::onSoundtracks));
    m_optionsMenu->addChild(songsBtn);

    cocos2d::CCNode* supportBtnSprite = ButtonSprite::create("Help");
    CCMenuItemSpriteExtra* supportBtn = CCMenuItemSpriteExtra::create(
        supportBtnSprite, nullptr, menu_selector(OptionsLayer::onSupport));
    m_optionsMenu->addChild(supportBtn);

    accountBtn->setPosition(cocos2d::CCPoint(-79.0f, 0.0f));
    helpBtn->setPosition(cocos2d::CCPoint(79.0f, 0.0f));
    optionsBtn->setPosition(cocos2d::CCPoint(0.0f, -80.0f));
    rateBtn->setPosition(cocos2d::CCPoint(-105.0f, -40.0f));
    songsBtn->setPosition(cocos2d::CCPoint(0.0f, -40.0f));
    supportBtn->setPosition(cocos2d::CCPoint(105.0f, -40.0f));

    m_mainLayer->addChild(m_optionsMenu, 1);

    cocos2d::CCMenu* vaultMenu = cocos2d::CCMenu::create();

    bool vaultUnlocked = GameStatsManager::sharedState()->getStat("12") > 9;
    const char* vaultSpriteName = vaultUnlocked ? "GJ_lock_open_001.png" : "GJ_lockGray_001.png";
    cocos2d::CCSprite* vaultSprite = cocos2d::CCSprite::createWithSpriteFrameName(vaultSpriteName);

    CCMenuItemSpriteExtra* vaultBtn = CCMenuItemSpriteExtra::create(
        vaultSprite, nullptr, menu_selector(OptionsLayer::onSecretVault));
    vaultMenu->addChild(vaultBtn);

    cocos2d::CCPoint vaultPos = cocos2d::CCNode::convertToNodeSpace(
        vaultMenu, cocos2d::CCPoint(winSize.width - 20.0f, winSize.height - 18.0f));
    vaultBtn->setPosition(vaultPos);

    if (!vaultUnlocked)
    {
        float halfWidth = vaultBtn->getContentSize().width * 0.5f;
        float halfHeight = vaultBtn->getContentSize().height * 0.5f;
        cocos2d::CCPoint iconBasePos(halfWidth, halfHeight - 21.0f);

        cocos2d::CCSprite* coinsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
        vaultSprite->addChild(coinsIcon);
        coinsIcon->setPosition(iconBasePos + cocos2d::CCPoint(7.5f, 0.0f));
        coinsIcon->setScale(0.5f);

        cocos2d::CCLabelBMFont* countLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        vaultSprite->addChild(countLabel);
        countLabel->setPosition(iconBasePos + cocos2d::CCPoint(-7.5f, 0.0f));
        countLabel->setScale(0.4f);
    }

    vaultBtn->setSizeMult(1.5f);

    cocos2d::CCLabelBMFont* musicLabel = cocos2d::CCLabelBMFont::create("Music", "bigFont.fnt");
    m_mainLayer->addChild(musicLabel);
    musicLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 34.0f));
    musicLabel->setScale(0.6f);

    m_musicSlider = Slider::create(this, menu_selector(OptionsLayer::musicSliderChanged));
    m_musicSlider->setBarVisibility(true);
    m_mainLayer->addChild(m_musicSlider);
    m_musicSlider->setPosition(musicLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    m_musicSlider->setTag(0);
    m_musicSlider->setValue(FMODAudioEngine::sharedEngine()->getBackgroundMusicVolume());

    cocos2d::CCLabelBMFont* sfxLabel = cocos2d::CCLabelBMFont::create("SFX", "bigFont.fnt");
    m_mainLayer->addChild(sfxLabel);
    sfxLabel->setPosition(musicLabel->getPosition() + cocos2d::CCPoint(0.0f, -40.0f));
    sfxLabel->setScale(0.6f);

    m_sfxSlider = Slider::create(this, menu_selector(OptionsLayer::sfxSliderChanged));
    m_sfxSlider->setBarVisibility(true);
    m_mainLayer->addChild(m_sfxSlider);
    m_sfxSlider->setPosition(sfxLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    m_sfxSlider->setTag(1);
    m_sfxSlider->setValue(FMODAudioEngine::sharedEngine()->getEffectsVolume());

    std::string menuMusicLabel = "Menu\nMusic";
    bool menuMusicEnabled = GameManager::sharedState()->getGameVariable("0122");

    cocos2d::CCPoint togglePos = musicLabel->getPosition() + cocos2d::CCPoint(145.0f, -35.0f);
    GameToolbox::createToggleButton(
        menuMusicLabel,
        menu_selector(OptionsLayer::onMenuMusic),
        0,
        !menuMusicEnabled,
        vaultMenu,
        togglePos,
        this,
        m_mainLayer,
        0.6f,
        0.35f,
        0.0f,
        cocos2d::CCPoint(0.0f, 2.0f),
        "bigFont.fnt",
        true,
        false,
        nullptr);

    m_mainLayer->addChild(vaultMenu);
}

void OptionsLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
}

void OptionsLayer::exitLayer()
{
    GJDropDownLayer::exitLayer();
}

void OptionsLayer::layerHidden()
{
    switch (m_layerChoice)
    {
        case 2:
        {
            SupportLayer* layer = SupportLayer::create();
            getParent()->addChild(layer, 100);
            layer->show();
            break;
        }
        case 3:
        {
            AccountLayer* layer = AccountLayer::create();
            getParent()->addChild(layer, 100);
            layer->show();
            break;
        }
        case 1:
        {
            SongsLayer* layer = SongsLayer::create();
            getParent()->addChild(layer, 100);
            layer->show();
            break;
        }
    }
    GJDropDownLayer::layerHidden();
}

void OptionsLayer::musicSliderChanged(cocos2d::CCObject* sender)
{
    float value = SliderThumb::getValue((SliderThumb*)sender);

    if (value < 0.03f)
    {
        FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
    }
    else
    {
        float previousVolume = FMODAudioEngine::sharedEngine()->getBackgroundMusicVolume();
        FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(value);

        if (previousVolume <= 0.0f && value > 0.0f)
        {
            GameManager::sharedState()->playMenuMusic();
        }
    }
}

void OptionsLayer::onAccount(cocos2d::CCObject* sender)
{
    m_layerChoice = 3;
    exitLayer();
}

void OptionsLayer::onHelp(cocos2d::CCObject* sender)
{
    TutorialLayer* layer = TutorialLayer::create();
    layer->show();
}

void OptionsLayer::onMenuMusic(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->toggleGameVariable("0122"))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    }
    else
    {
        GameManager::sharedState()->fadeInMenuMusic();
    }
}

void OptionsLayer::onOptions(cocos2d::CCObject* sender)
{
    MoreOptionsLayer* layer = MoreOptionsLayer::create();
    layer->show();
}

void OptionsLayer::onProgressBar(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_showProgressBar = !GameManager::sharedState()->m_showProgressBar;
}

void OptionsLayer::onRate(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->rateGame();
}

void OptionsLayer::onRecordReplays(cocos2d::CCObject* sender)
{
    if (!m_recordReplays)
    {
        if (GameManager::sharedState()->m_everyPlaySetup)
        {
            tryEnableRecord();
        }
        else
        {
            GameManager::sharedState()->m_everyPlaySetup = true;
            m_recordReplays = true;

            cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(1.0f);
            cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(OptionsLayer::tryEnableRecord));
            cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
            runAction(seq);
        }
    }
}

void OptionsLayer::onSecretVault(cocos2d::CCObject* sender)
{
    if (GameStatsManager::sharedState()->getStat("12") <= 9)
    {
        std::string message;

        int dialogIndex;
        if (m_lastVaultDialog < 0)
            dialogIndex = (int)((float)rand() * 4.6566e-10f * 10.0f);
        else
            dialogIndex = m_lastVaultDialog + 1;

        m_lastVaultDialog = dialogIndex;

        switch (dialogIndex)
        {
            case 0:
                message = "Go away!";
                break;
            case 1:
                message = "No!";
                break;
            case 2:
                message = "It's a secret...";
                break;
            case 3:
                message = "Stop it!";
                break;
            case 4:
                message = "Not allowed!";
                break;
            case 5:
                message = "Why u click?";
                break;
            case 6:
                message = "Nothing to see here...";
                break;
            case 7:
                message = "Don't touch!";
                break;
            case 8:
                message = "Just, stop.";
                break;
            case 9:
                message = "No one seems to be home.";
                break;
            case 10:
                message = "zzzZZZ...";
                break;
            default:
                m_lastVaultDialog = 0;
                message = "Go away!";
                break;
        }

        DialogObject* dialogObj = DialogObject::create("The Guard", message, 1, 1.0f, false, cocos2d::ccc3(0, 0, 0));
        DialogLayer* dialogLayer = DialogLayer::create(dialogObj, 1);
        addChild(dialogLayer, 100);
        dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
        dialogLayer->animateInRandomSide();
    }
    else
    {
        cocos2d::CCScene* scene = SecretLayer::scene();
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
}

void OptionsLayer::onSoundtracks(cocos2d::CCObject* sender)
{
    m_layerChoice = 1;
    exitLayer();
}

void OptionsLayer::onSupport(cocos2d::CCObject* sender)
{
    m_layerChoice = 2;
    exitLayer();
}

void OptionsLayer::onVideo(cocos2d::CCObject* sender)
{
    VideoOptionsLayer* layer = VideoOptionsLayer::create();
    layer->show();
}

void OptionsLayer::sfxSliderChanged(cocos2d::CCObject* sender)
{
    float value = SliderThumb::getValue((SliderThumb*)sender);

    if (value < 0.03f)
        value = 0.0f;

    FMODAudioEngine::sharedEngine()->setEffectsVolume(value);
}

void OptionsLayer::tryEnableRecord()
{
    m_recordReplays = false;
    std::string text = "Gameplay recording is not supported on this device.";
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Unavailable", text);
    alert->show();
}

OptionsLayer::~OptionsLayer()
{
}