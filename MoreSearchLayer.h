#pragma once

#include "cocos2d.h"
#include <string>

class CCTextInputNode;
class CCMenuItemSpriteExtra;

class MoreSearchLayer : public FLAlertLayer,
                        public TextInputDelegate {
public:
    static MoreSearchLayer* create();

    bool init() override;
    void keyBackClicked() override;

    void audioNext(cocos2d::CCObject* sender);
    void audioPrevious(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCoins(cocos2d::CCObject* sender);
    void onCompleted(cocos2d::CCObject* sender);
    void onEpic(cocos2d::CCObject* sender);
    void onFeatured(cocos2d::CCObject* sender);
    void onFollowed(cocos2d::CCObject* sender);
    void onFriends(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onLegendary(cocos2d::CCObject* sender);
    void onMythic(cocos2d::CCObject* sender);
    void onNoStar(cocos2d::CCObject* sender);
    void onOriginal(cocos2d::CCObject* sender);
    void onSongFilter(cocos2d::CCObject* sender);
    void onSongMode(cocos2d::CCObject* sender);
    void onTwoPlayer(cocos2d::CCObject* sender);
    void onUncompleted(cocos2d::CCObject* sender);
    void selectSong(int songID);
    void textInputReturn(CCTextInputNode* node);
    void textInputShouldOffset(CCTextInputNode* node, float yOffset);
    void toggleSongNodes(bool custom, bool disable);
    void updateAudioLabel();

    virtual ~MoreSearchLayer();

protected:
    CCTextInputNode* m_enterSongID = nullptr;
    cocos2d::CCLabelBMFont* m_audioTrackName = nullptr;
    cocos2d::CCArray* m_normalSongNodes = nullptr;
    cocos2d::CCArray* m_customSongNodes = nullptr;
    cocos2d::CCArray* m_commonSongNodes = nullptr;
    CCMenuItemSpriteExtra* m_normalBtn = nullptr;
    CCMenuItemSpriteExtra* m_customBtn = nullptr;
    CCMenuItemSpriteExtra* m_songLeftBtn = nullptr;
    CCMenuItemSpriteExtra* m_songRightBtn = nullptr;
};