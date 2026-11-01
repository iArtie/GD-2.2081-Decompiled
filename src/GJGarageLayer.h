// GJGarageLayer.h
#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "GameRateDelegate.h"
#include "CharacterColorDelegate.h"
#include "RewardedVideoDelegate.h"
#include "DialogDelegate.h"
#include "TextInputDelegate.h"

class SimplePlayer;
class ListButtonBar;
class CCMenuItemSpriteExtra;
class CCTextInputNode;
class FLAlertLayer;
class DialogLayer;
class IconType;

class GJGarageLayer : public cocos2d::CCLayer, public FLAlertLayerProtocol, public GameRateDelegate, public CharacterColorDelegate, public RewardedVideoDelegate, public DialogDelegate, public TextInputDelegate {
public:
    static cocos2d::CCScene* scene();
    bool init();
    virtual ~GJGarageLayer();

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    static std::string achievementForUnlock(int id, UnlockType type);
    static std::string descriptionForUnlock(int id, UnlockType type);
    void dialogClosed(DialogLayer* layer) override;
    cocos2d::CCArray* getItems(IconType type);
    std::string getLockFrame(int id, UnlockType type);
    void keyBackClicked() override;
    void listButtonBarSwitchedPage(ListButtonBar* bar, int page);
    void onArrow(cocos2d::CCObject* sender);
    void onBack(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNavigate(cocos2d::CCObject* sender);
    void onPaint(cocos2d::CCObject* sender);
    void onRewardedVideo(cocos2d::CCObject* sender);
    void onSelect(cocos2d::CCObject* sender);
    void onSelectTab(cocos2d::CCObject* sender);
    void onShards(cocos2d::CCObject* sender);
    void onShop(cocos2d::CCObject* sender);
    void onSpecial(cocos2d::CCObject* sender);
    void onToggleItem(cocos2d::CCObject* sender);
    void playRainbowEffect();
    void playShadowEffect();
    void playerColorChanged() override;
    void rewardedVideoFinished() override;
    void selectTab(IconType type);
    void setupIconSelect();
    void setupPage(int page, IconType type);
    void setupSpecialPage();
    void showUnlockPopup(int id, UnlockType type);
    void showUnlockPopupNew(int id, UnlockType type);
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    static std::string titleForUnlock(int id, UnlockType type);
    void toggleGlow();
    void updatePlayerColors();
    void updatePlayerName(const char* name);
    void updateRate();

private:
    cocos2d::CCSprite* m_supporter;
    cocos2d::CCNode* m_unknown;
    ListButtonBar* m_iconSelection;
    bool m_hasClosed;
    SimplePlayer* m_playerObject;
    cocos2d::CCSprite* m_cursor1;
    cocos2d::CCSprite* m_cursor2;
    IconType m_iconType;
    int m_iconID;
    IconType m_selectedIconType;
    CCMenuItemSpriteExtra* m_currentIcon;
    CCMenuItemSpriteExtra* m_leftArrow;
    CCMenuItemSpriteExtra* m_rightArrow;
    cocos2d::CCMenu* m_navDotMenu;
    cocos2d::CCArray* m_pageButtons;
    cocos2d::CCArray* m_tabButtons;
    CCTextInputNode* m_usernameInput;
    bool m_videoPlaying;
    std::map<IconType, int> m_iconPages;
};