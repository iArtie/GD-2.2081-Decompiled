#include "SetupSongTriggerPopup.h"
#include "SetupAudioTriggerPopup.h"
#include "EffectGameObject.h"
#include "SongSelectNode.h"
#include "SongInfoObject.h"
#include "MusicDownloadManager.h"
#include "FMODAudioEngine.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "CustomSongWidget.h"
#include "CCMenuItemSpriteExtra.h"

SetupSongTriggerPopup* SetupSongTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupSongTriggerPopup* instance = new SetupSongTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupSongTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 440.0f, 300.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Song Trigger");
    addHelpButton(
        "Help",
        "Lets you start a specified <cg>song</c>.\n"
        "A single track can be active in each <cy>Channel</c> at a time.\n"
        "<cl>Start, End, FadeIn, and FadeOut</c> are in milliseconds.\n"
        "<cp>Prep</c> is used to ensure exact sync to a specified time as there may be some delay loading it normally. Use "
        "this option on a trigger to prepare the song before you want it to start, and then use <cr>Load Prep</c> when you "
        "want it to start. Note that only one Prep can be active at a time per channel.",
        1.0f);
    addCloseButton("OK");

    createMultiTriggerItemsDefaultHorizontal();

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint prepPos = center + cocos2d::CCPoint(m_width * 0.5f - 130.0f, m_height * 0.5f - 20.0f);
    createToggleValueControl(399, "Prep", cocos2d::CCPoint(1.0f, 0.0f), &prepPos, 0, 0, 0.0f);

    cocos2d::CCPoint loadPrepPos = center + cocos2d::CCPoint(m_width * 0.5f - 60.0f, m_height * 0.5f - 20.0f);
    createToggleValueControl(400, "Load\nPrep", cocos2d::CCPoint(1.0f, 0.0f), &loadPrepPos, 0, 0, 0.0f);

    float songID = getValue(392);
    cocos2d::CCPoint selectNodePos(winSize.width * 0.5f, winSize.height * 0.5f + 55.0f);
    m_selectNode = SongSelectNode::create(
        (int)songID, 1, 0, SongSelectType::Custom, nullptr, &selectNodePos, m_buttonMenu, true);
    m_mainLayer->addChild(m_selectNode, 5);
    m_selectNode->m_songWidget->m_playbackDelegate = this;

    updateLength();

    addObjectsToGroup(m_selectNode->m_selectSongObjects, 1);
    addObjectsToGroup(m_selectNode->m_changeSongObjects, 1);

    cocos2d::CCPoint audioBase(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);

    cocos2d::CCPoint volumePos = audioBase + cocos2d::CCPoint(-91.0f, -39.0f);
    createValueControlAdvanced(
        406, "Volume:", cocos2d::CCPoint(0.65f, 0.0f), 2.0f, &volumePos,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint speedPos = audioBase + cocos2d::CCPoint(91.0f, -39.0f);
    createValueControlAdvanced(
        404, "Speed:", cocos2d::CCPoint(0.65f, -12.0f), 12.0f, &speedPos,
        (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    toggleLimitValue(404, true);
    toggleLimitValue(406, true);

    cocos2d::CCPoint loopPos = audioBase + cocos2d::CCPoint(38.0f, -93.0f);
    createToggleValueControl(413, "Loop", cocos2d::CCPoint(1.0f, 0.0f), &loopPos, 0, 0, 0.0f);

    cocos2d::CCPoint dontResetPos = audioBase + cocos2d::CCPoint(118.0f, -93.0f);
    createToggleValueControl(595, "Dont\nReset", cocos2d::CCPoint(1.0f, 0.0f), &dontResetPos, 0, 0, 0.0f);

    cocos2d::CCPoint channelPos = audioBase + cocos2d::CCPoint(-108.0f, -93.0f);
    createValueControlAdvanced(
        432, "Channel:", cocos2d::CCPoint(0.65f, 0.0f), 4.0f, &channelPos,
        Float, 3, 10, 0.0f, 0.0f, 1, 0, (GJInputStyle)2, 0, 2);

    toggleLimitValue(432, true);

    addTimeControls(1, -25.0f);

    cocos2d::CCSprite* saveStartIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png");
    saveStartIcon->setScale(0.6f);
    CCMenuItemSpriteExtra* saveStartItem = CCMenuItemSpriteExtra::create(
        saveStartIcon, nullptr, menu_selector(SetupSongTriggerPopup::onSavePlaybackPos));
    saveStartItem->setTag(0);
    m_buttonMenu->addChild(saveStartItem);
    saveStartItem->setPosition(m_buttonMenu->convertToNodeSpace(center + cocos2d::CCPoint(-185.0f, -25.0f)));
    addObjectToPage(saveStartItem, 1);

    cocos2d::CCSprite* saveEndIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseEditorBtn_001.png");
    saveEndIcon->setScale(0.6f);
    CCMenuItemSpriteExtra* saveEndItem = CCMenuItemSpriteExtra::create(
        saveEndIcon, nullptr, menu_selector(SetupSongTriggerPopup::onSavePlaybackPos));
    saveEndItem->setTag(1);
    m_buttonMenu->addChild(saveEndItem);
    saveEndItem->setPosition(m_buttonMenu->convertToNodeSpace(center + cocos2d::CCPoint(-185.0f, -64.0f)));
    addObjectToPage(saveEndItem, 1);

    MusicDownloadManager::sharedState()->addMusicDownloadDelegate(this);

    updateSongTimeSlider();

    createPageButtons(m_width * 0.5f + 15.0f, 1);

    if (getValue(400) == 0.0f)
        goToPage((int)getValue(427), false);
    else
        updateApplyPrepare(true);

    postSetup();

    return true;
}

void SetupSongTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(0))
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);

    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;

    if (m_selectNode->m_songChanged)
        updateValue(392, (float)m_selectNode->m_selectedSongID);

    SetupTriggerPopup::onClose(sender);
}

void SetupSongTriggerPopup::onPlayback(SongInfoObject* object)
{
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(0))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        return;
    }

    if (!GameManager::sharedState()->m_levelEditorLayer)
        return;

    int startTime = (int)getValue(408);
    int endTime = (int)getValue(410);
    int fadeIn = (int)getValue(409);
    int fadeOut = (int)getValue(411);
    bool loop = getValue(413) != 0.0f;
    float volume = getValue(406);
    int pitchIdx = (int)getValue(404);

    std::string path = MusicDownloadManager::sharedState()->pathForSong(object->m_songID);
    float pitch = FMODAudioEngine::sharedEngine()->pitchForIdx(pitchIdx);

    FMODAudioEngine::sharedEngine()->loadMusic(path, pitch, 0.0f, volume, loop, false, false, false);
    FMODAudioEngine::sharedEngine()->setMusicTimeMS(startTime, false, false);
    FMODAudioEngine::sharedEngine()->startMusic(startTime, endTime, fadeIn, fadeOut, loop, false, false, false);

    MusicDownloadManager::sharedState()->songStateChanged();
}

void SetupSongTriggerPopup::onResetSongTime(cocos2d::CCObject* sender)
{
    updateValue(408, 0.0f);
}

void SetupSongTriggerPopup::onSavePlaybackPos(cocos2d::CCObject* sender)
{
    int timeMS = 0;

    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(0))
    {
        int musicTime = FMODAudioEngine::sharedEngine()->getMusicTimeMS(0);
        timeMS = musicTime < 0 ? 0 : musicTime;
    }
    else if (GameManager::sharedState()->m_levelEditorLayer && m_gameObject)
    {
        LevelEditorLayer* editorLayer = GameManager::sharedState()->m_levelEditorLayer;
        cocos2d::CCPoint pos = m_gameObject->getPosition();
        timeMS = (int)(editorLayer->timeForPos(pos, 0, 0, false, 0) * 1000.0f);
        if (timeMS < 0) timeMS = 0;
    }

    if (sender->getTag() == 0)
        updateValue(408, (float)timeMS);
    else if (sender->getTag() == 1)
        updateValue(410, (float)timeMS);
}

void SetupSongTriggerPopup::pageChanged()
{
    updateValue(427, (float)m_page);

    if (m_selectNode)
    {
        if (m_hideAll || m_page > 1)
        {
            toggleGroup(1, false);
        }
        else
        {
            toggleGroup(1, true);
            m_selectNode->updateWidgetVisibility();
        }
    }
}

void SetupSongTriggerPopup::updateApplyPrepare(bool hideAll)
{
    if (hideAll)
        hideAll();
    else
        goToPage(0, false);

    toggleGroup(2, true);
}

void SetupSongTriggerPopup::updateLength()
{
    CustomSongWidget* songWidget = m_selectNode->m_songWidget;
    if (songWidget)
    {
        float pitch = FMODAudioEngine::sharedEngine()->pitchForIdx((int)getValue(404));
        songWidget->updateLengthMod(pitch);
    }
}

void SetupSongTriggerPopup::updateSongTimeSlider()
{
    if (m_selectNode->m_selectedSongID <= 0)
        return;

    std::string path = MusicDownloadManager::sharedState()->pathForSong(m_selectNode->m_selectedSongID);
    FMODAudioEngine::sharedEngine()->loadMusic(path);

    unsigned int lengthMS = FMODAudioEngine::sharedEngine()->getMusicLengthMS(0);
    if (lengthMS)
    {
        setMaxSliderValue((float)lengthMS, 408);
        setMaxSliderValue((float)lengthMS, 410);
        updateSlider(408);
        updateSlider(410);
    }
}

void SetupSongTriggerPopup::valueDidChange(int tag, float value)
{
    SetupAudioTriggerPopup::valueDidChange(tag, value);

    if (tag == 400)
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        updateApplyPrepare(value != 0.0f);

        if (value != 0.0f)
            updateValue(399, 0.0f);
    }
    else if (tag == 404)
    {
        updateLength();
    }
}

SetupSongTriggerPopup::~SetupSongTriggerPopup()
{
}