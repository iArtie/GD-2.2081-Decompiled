#include "FindBPMLayer.h"
#include "MusicDownloadManager.h"
#include "SongInfoObject.h"
#include "FMODAudioEngine.h"
#include "GameToolbox.h"

void FindBPMLayer::calculateBPM() {
    if (m_beats > 1) {
        float totalTime = m_elapsed - m_startOffset;
        float timePerBeat = totalTime / (float)(m_beats - 1);
        m_beatsPerMinute = (int)llroundf(60.0f / timePerBeat);
    }
}

FindBPMLayer* FindBPMLayer::create(int songID) {
    FindBPMLayer* ret = new FindBPMLayer();
    if (ret && ret->init(songID)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FindBPMLayer::init(int songID) {
    if (!CreateGuidelinesLayer::init(0, 0)) { // BPMFinder type
        return false;
    }
    
    m_songID = songID;
    m_beats = 0;
    m_beatsPerMinute = 0;
    m_elapsed = 0.0f;
    m_startOffset = 0.0f;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    m_bpmLabel = cocos2d::CCLabelBMFont::create();
    if (m_bpmLabel) {
        m_mainLayer->addChild(m_bpmLabel, 1);
        m_bpmLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
        m_recordingObjects->addObject(m_bpmLabel);
    }
    
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    SongInfoObject* songInfo = musicManager->getSongInfoObject(m_songID);
    if (songInfo && songInfo->m_BPM > 0) {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("BPM: %i", songInfo->m_BPM);
        m_infoLabel->setString(str->getCString());
    }
    
    CreateGuidelinesLayer::toggleItems(false);
    
    return true;
}

void FindBPMLayer::onClose(cocos2d::CCObject* sender) {
    MusicDownloadManager::sharedState()->songStateChanged();
    CreateGuidelinesLayer::onClose(sender);
}

void FindBPMLayer::onInfo(cocos2d::CCObject* sender) {
    // Empty implementation
}

void FindBPMLayer::onRecord(cocos2d::CCObject* sender) {
    CreateGuidelinesLayer::onRecord(sender);
    m_beats = 0;
    m_startOffset = 0.0f;
    m_bpmLabel->setString("0");
}

void FindBPMLayer::playMusic() {
    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    
    std::string path = musicManager->pathForSong(m_songID);
    float startTimeMs = m_elapsed * 1000.0f;
    audioEngine->loadAndPlayMusic(path, (unsigned int)startTimeMs, 0);
}

void FindBPMLayer::recordingDidStop() {
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("BPM: %i", m_beatsPerMinute);
    m_infoLabel->setString(str->getCString());
    
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    SongInfoObject* songInfo = musicManager->getSongInfoObject(m_songID);
    if (songInfo) {
        songInfo->m_BPM = m_beatsPerMinute;
    }
}

void FindBPMLayer::registerTouch() {
    m_beats++;
    
    if (m_beats == 1) {
        m_startOffset = m_elapsed;
        m_bpmLabel->setString("0");
    } else {
        calculateBPM();
        std::string bpmStr = GameToolbox::intToString(m_beatsPerMinute);
        m_bpmLabel->setString(bpmStr.c_str());
    }
}

FindBPMLayer::~FindBPMLayer() {
    // Base class destructor handles cleanup
}