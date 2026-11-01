#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class CustomSongDelegate;
class CCTextInputNode;
class CCMenuItemSpriteExtra;

class SongOptionsLayer : public FLAlertLayer {
public:
    static SongOptionsLayer* create(CustomSongDelegate* delegate);
    bool init(CustomSongDelegate* delegate) override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onFadeIn(cocos2d::CCObject* sender);
    void onFadeOut(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onPlayback(cocos2d::CCObject* sender);
    void onSongPersistent(cocos2d::CCObject* sender);
    void updatePlaybackBtn();
    virtual ~SongOptionsLayer();

protected:
    CustomSongDelegate* m_delegate = nullptr;
    CCTextInputNode* m_offsetInput = nullptr;
    CCMenuItemSpriteExtra* m_playbackButton = nullptr;
};