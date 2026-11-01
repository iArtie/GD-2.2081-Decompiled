#include "SongSelectNode.h"
#include "LevelSettingsObject.h"
#include "LevelTools.h"
#include "GameManager.h"
#include "MusicDownloadManager.h"
#include "CustomSongWidget.h"
#include "CustomSongLayer.h"
#include "GJGameLevel.h"

void SongSelectNode::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    int tag = layer->getTag();
    if (tag == 1 && btn2) {
        GameManager::sharedState()->setGameVariable("0016", true);
        showCustomSongSelect();
    }
}

void SongSelectNode::audioNext(cocos2d::CCObject* sender) {
    if (m_selectedSongID != 21)
        selectSong(m_selectedSongID + 1);
}

void SongSelectNode::audioPrevious(cocos2d::CCObject* sender) {
    if (m_selectedSongID)
        selectSong(m_selectedSongID - 1);
}

void SongSelectNode::customSongLayerClosed() {
    onSongMode(m_customButton);
}

int SongSelectNode::getActiveSongID() {
    return m_selectedSongID;
}

LevelSettingsObject* SongSelectNode::getLevelSettings() {
    return m_settingsObject;
}

std::string SongSelectNode::getSongFileName() {
    if (m_selectedSongID) {
        return MusicDownloadManager::sharedState()->pathForSong(m_selectedSongID);
    }
    return m_settingsObject->m_level->getAudioFileName();
}

void SongSelectNode::onOpenCustomSong(cocos2d::CCObject* sender) {
    if (GameManager::sharedState()->getGameVariable("0016")) {
        showCustomSongSelect();
    } else {
        MusicDownloadManager::sharedState()->showTOS(this);
    }
}

void SongSelectNode::onSongMode(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    onSongMode(tag);
}

void SongSelectNode::selectSong(int id) {
    if (id > 21)
        id = 21;
    m_songChanged = true;
    if (id < 0)
        id = 0;
    m_selectedSongID = id;
    updateAudioLabel();
}

void SongSelectNode::showCustomSongSelect() {
    CustomSongLayer* layer = CustomSongLayer::create(this);
    layer->m_delegate = this;
    layer->show();
}

void SongSelectNode::songIDChanged(int id) {
    m_selectedSongID = id;
    m_songChanged = true;
}

void SongSelectNode::updateAudioLabel() {
    std::string title = LevelTools::getAudioTitle(m_selectedSongID);
    std::string labelText = cocos2d::CCString::createWithFormat("%02d: %s", m_selectedSongID + 1, title.c_str())->getCString();
    m_audioLabel->setString(labelText.c_str());
    float scale = 0.8f;
    if ((200.0f / m_audioLabel->getContentSize().width) < 0.8f)
        scale = 200.0f / m_audioLabel->getContentSize().width;
    m_audioLabel->setScale(scale);
}

void SongSelectNode::updateWidgetVisibility() {
    if (m_isCustomSong) {
        m_isCustomSong = m_selectedSongID != 0;
        m_songWidget->setVisible(m_selectedSongID != 0);
    } else {
        m_songWidget->setVisible(m_isCustomSong);
    }
    m_selectButton->setVisible(!m_isCustomSong && m_isCustomSong);
    m_changeButton->setVisible(m_isCustomSong && m_isCustomSong);
}

SongSelectNode::~SongSelectNode() {
}