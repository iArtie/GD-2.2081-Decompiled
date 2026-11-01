#include "SetupReverbPopup.h"
#include "SetupTriggerPopup.h"
#include "SFXTriggerGameObject.h"
#include "FMODAudioEngine.h"
#include "GameManager.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"

SetupReverbPopup* SetupReverbPopup::create(SFXTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupReverbPopup* instance = new SetupReverbPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupReverbPopup::init(SFXTriggerGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 140.0f, 2))
        return false;

    preSetup();

    addTitle("Edit Global Reverb");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Lets you change the <cg>Reverb</c> settings.\n"
        "The sound engine has one global reverb channel. This means that two sounds with different reverb settings cannot b"
        "e played simultaneously.",
        1.0f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint enablePos(m_width * 0.5f - 78.0f + winSize.width * 0.5f,
                               24.0f - m_height * 0.5f + winSize.height * 0.5f);
    createToggleValueControl(503, "Enable", cocos2d::CCPoint(1.0f, 0.0f), &enablePos, 0, 0, 0.0f);

    m_reverbLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_reverbLabel, 1);
    m_reverbLabel->setPosition(center + cocos2d::CCPoint(0.0f, 5.0f));

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setFlipX(true);
    leftArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupReverbPopup::onReverb));
    m_buttonMenu->addChild(leftItem);
    leftItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_reverbLabel->getPosition() + cocos2d::CCPoint(100.0f, 0.0f)));
    leftItem->setSizeMult(1.4f);
    leftItem->setTag(1);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupReverbPopup::onReverb));
    m_buttonMenu->addChild(rightItem);
    rightItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_reverbLabel->getPosition() + cocos2d::CCPoint(-100.0f, 0.0f)));
    rightItem->setSizeMult(1.4f);
    rightItem->setTag(0);

    updateReverbLabel();

    cocos2d::CCSprite* playIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png");
    playIcon->setScale(0.9f);
    CCMenuItemSpriteExtra* playItem = CCMenuItemSpriteExtra::create(
        playIcon, nullptr, menu_selector(SetupReverbPopup::onPlay));
    m_buttonMenu->addChild(playItem);
    playItem->setPosition(m_buttonMenu->convertToNodeSpace(
        center + cocos2d::CCPoint(m_width * 0.5f - 24.0f, m_height * 0.5f - 24.0f)));

    postSetup();

    return true;
}

void SetupReverbPopup::onClose(cocos2d::CCObject* sender)
{
    FMODAudioEngine::sharedEngine()->stopAllEffects();
    SetupTriggerPopup::onClose(sender);
}

void SetupReverbPopup::onPlay(cocos2d::CCObject* sender)
{
    FMODAudioEngine::sharedEngine()->stopAllEffects();

    if (m_gameObject)
    {
        FMODAudioEngine::sharedEngine()->updateReverb(m_gameObject->m_reverbPreset, 0);
        GameManager::sharedState()->playSFXTrigger(m_gameObject);
    }
}

void SetupReverbPopup::onReverb(cocos2d::CCObject* sender)
{
    int currentValue = (int)getValue(502);
    int newValue = (sender->getTag() == 1) ? currentValue + 1 : currentValue - 1;

    if (newValue > 22) newValue = 22;
    if (newValue < 0) newValue = 0;

    updateValue(502, (float)newValue);
    updateReverbLabel();
}

void SetupReverbPopup::updateReverbLabel()
{
    int value = (int)getValue(502);
    std::string text = FMODAudioEngine::sharedEngine()->reverbToString((FMODReverbPreset)value);
    m_reverbLabel->setString(text.c_str());
    m_reverbLabel->limitLabelWidth(150.0f, 0.8f, 0.0f);
}

SetupReverbPopup::~SetupReverbPopup()
{
}