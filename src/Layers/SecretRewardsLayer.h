#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Delegates/DialogDelegate.h"

class BoomScrollLayer;
class GJChestSprite;
class GJRewardItem;
class DynamicScrollDelegate;
class CCMenuItemSpriteExtra;

class SecretRewardsLayer : public cocos2d::CCLayer, public DialogDelegate {
public:
    static SecretRewardsLayer* create(bool fromShop);
    static cocos2d::CCScene* scene(bool fromShop);
    bool init(bool fromShop) override;

    void dialogClosed(DialogLayer* layer) override;
    void keyBackClicked() override;
    void onExit() override;

    void createSecondaryLayer(int chestType);
    void fadeInMusic();
    void fadeInOutMusic();
    cocos2d::CCArray* generateChestItems(int chestType);
    cocos2d::ccColor3B getPageColor(int page);
    void goToPage(int page);
    void moveToMainLayer(cocos2d::CCObject* sender);
    void moveToSecondaryLayer(int page);
    void onBack(cocos2d::CCObject* sender);
    void onChestType(cocos2d::CCObject* sender);
    void onGoldChest(cocos2d::CCObject* sender);
    void onSelectItem(cocos2d::CCObject* sender);
    void onShop(cocos2d::CCObject* sender);
    void onSpecialItem(cocos2d::CCObject* sender);
    void onSwitchPage(cocos2d::CCObject* sender);
    void scrollLayerMoved(cocos2d::CCPoint position);
    void showDialog01();
    void showDialog03();
    void showDialogDiamond();
    void showDialogMechanic();
    void showLockedChest();
    void showShop(int shop);
    void switchToOpenedState(CCMenuItemSpriteExtra* button);
    void updateBackButton();
    void updateUnlockedGoldLabel();
    void updateUnlockedLabel();

    ~SecretRewardsLayer();

protected:
    bool m_inMainLayer = false;
    BoomScrollLayer* m_mainScrollLayer = nullptr;
    BoomScrollLayer* m_secondaryScrollLayer = nullptr;
    cocos2d::CCSprite* m_backgroundSprite = nullptr;
    cocos2d::CCLabelBMFont* m_keysLabel = nullptr;
    cocos2d::CCSprite* m_keySprite = nullptr;
    cocos2d::CCLabelBMFont* m_goldKeysLabel = nullptr;
    cocos2d::CCSprite* m_goldKeySprite = nullptr;
    int m_lockedDialogIndex = 0;
    cocos2d::CCSprite* m_backSprite = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCLayer* m_secondaryLayer = nullptr;
    cocos2d::CCLabelBMFont* m_chestCounter = nullptr;
    cocos2d::CCLabelBMFont* m_chestLabel = nullptr;
    int m_rewardType = 0;
    cocos2d::CCDictionary* m_chestCounters = nullptr;
    cocos2d::CCSprite* m_goldPriceSprite = nullptr;
    int m_scratchDialogIndex = 0;
    int m_potborDialogIndex = 0;
    CCMenuItemSpriteExtra* m_leftButton = nullptr;
    CCMenuItemSpriteExtra* m_rightButton = nullptr;
    int m_mechanicDialogIndex = 0;
    int m_diamondDialogIndex = 0;
};