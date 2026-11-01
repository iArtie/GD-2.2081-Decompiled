#include "SongInfoLayer.h"
#include "LevelTools.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "FMODAudioEngine.h"
#include "FindBPMLayer.h"
#include "DialogObject.h"
#include "DialogLayer.h"

SongInfoLayer* SongInfoLayer::create(int songID) {
    int artistID = LevelTools::artistForAudio(songID);
    std::string title = LevelTools::getAudioTitle(songID);
    std::string artistName = LevelTools::nameForArtist(artistID);
    std::string downloadLink = LevelTools::urlForAudio(songID);
    std::string ngLink = LevelTools::ngURLForArtist(artistID);
    std::string ytLink = LevelTools::ytURLForArtist(artistID);
    std::string fbLink = LevelTools::fbURLForArtist(artistID);

    SongInfoLayer* layer = new SongInfoLayer();
    layer->m_songID = songID;
    layer->m_downloadLink = downloadLink;
    layer->m_newgroundsLink = ngLink;
    layer->m_youtubeLink = ytLink;
    layer->m_facebookLink = fbLink;

    if (layer->init(200)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void SongInfoLayer::keyBackClicked() {
    onClose(nullptr);
}

void SongInfoLayer::onBPM(cocos2d::CCObject* sender) {
    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    FindBPMLayer* layer = FindBPMLayer::create(m_songID);
    layer->show();
}

void SongInfoLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SongInfoLayer::onDownload(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL(m_downloadLink.c_str());
}

void SongInfoLayer::onFB(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL(m_facebookLink.c_str());
}

void SongInfoLayer::onMenuMusic(cocos2d::CCObject* sender) {
    if (GameStatsManager::sharedState()->isItemUnlocked(GJItemType::GJItem, 16)) {
        if (m_songID > 0) {
            if (GameManager::sharedState()->m_customMenuSongID == m_songID)
                GameManager::sharedState()->m_customMenuSongID = 0;
            else
                GameManager::sharedState()->m_customMenuSongID = m_songID;
            GameManager::sharedState()->fadeInMenuMusic();
        }
    } else {
        showCustomMusicUnlockInfo();
    }
}

void SongInfoLayer::onNG(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL(m_newgroundsLink.c_str());
}

void SongInfoLayer::onPracticeMusic(cocos2d::CCObject* sender) {
    if (GameStatsManager::sharedState()->isItemUnlocked(GJItemType::GJItem, 16)) {
        if (m_songID > 0) {
            if (GameManager::sharedState()->m_customPracticeSongID == m_songID)
                GameManager::sharedState()->m_customPracticeSongID = 0;
            else
                GameManager::sharedState()->m_customPracticeSongID = m_songID;
        }
    } else {
        showCustomMusicUnlockInfo();
    }
}

void SongInfoLayer::onYT(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL(m_youtubeLink.c_str());
}

void SongInfoLayer::showCustomMusicUnlockInfo() {
    std::string dialogText;
    int dialogChar;

    switch (m_musicCustomizerMessageIndex) {
        case 0:
            dialogText = "You need a <cg>Music Customizer</c> to change menu and practice music.";
            dialogChar = 37;
            break;
        case 1:
            dialogText = "Lucky for you I have one in my <cg>shop</c>.\n<cl>But they are very expensive.</c>";
            dialogChar = 38;
            break;
        case 2:
            dialogText = "Find me when you can <cy>afford</c> it.\nI need the <cl>Mana Orbs</c><d030>.<d030>.<d030>.<d030>";
            dialogChar = 37;
            break;
        case 3:
            dialogText = "All this <cg>clicking</c> makes me want to give you the item for <cy>free</c>!";
            dialogChar = 39;
            break;
        case 4:
            dialogText = ".<d020>.<d020>.<d020> is something I would never say.";
            dialogChar = 37;
            break;
        case 5:
            dialogText = "<cl>You really are persistent, I will give you that.</c>";
            dialogChar = 38;
            break;
        case 6:
            dialogText = "I am <cp>The Mechanic</c>, not <cr>Santa Claus</c>. I don't work for free.";
            dialogChar = 39;
            break;
        default:
            dialogChar = 37;
            break;
    }

    DialogObject* dialogObject = DialogObject::create("The Mechanic", dialogText, dialogChar, 1.0f, false, {255, 255, 255});
    DialogLayer* dialogLayer = DialogLayer::create(dialogObject, 0);
    dialogLayer->addToMainScene();
    dialogLayer->updateChatPlacement(ChatPlacement::Center);
    dialogLayer->animateIn(DialogAnimationType::FromCenter);

    m_musicCustomizerMessageIndex++;
    if (m_musicCustomizerMessageIndex > 6)
        m_musicCustomizerMessageIndex = 0;
}

SongInfoLayer::~SongInfoLayer() {
}