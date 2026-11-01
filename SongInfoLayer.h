#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include <string>

class SongInfoLayer : public FLAlertLayer {
public:
    static SongInfoLayer* create(int songID);
    void keyBackClicked() override;
    void onBPM(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onDownload(cocos2d::CCObject* sender);
    void onFB(cocos2d::CCObject* sender);
    void onMenuMusic(cocos2d::CCObject* sender);
    void onNG(cocos2d::CCObject* sender);
    void onPracticeMusic(cocos2d::CCObject* sender);
    void onYT(cocos2d::CCObject* sender);
    void showCustomMusicUnlockInfo();
    virtual ~SongInfoLayer();

protected:
    std::string m_downloadLink;
    std::string m_newgroundsLink;
    std::string m_youtubeLink;
    std::string m_facebookLink;
    int m_songID = 0;
    int m_musicCustomizerMessageIndex = 0;
};