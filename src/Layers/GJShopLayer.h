#pragma once

#include "Platform/cocos2d.h"

class CCCounterLabel;
class AnimatedShopKeeper;
class GJStoreItem;
class DialogLayer;

enum class ShopType {
    Main = 0,
    Secret = 1,
    Community = 2,
    Mechanic = 3,
    Diamond = 4
};

class GJShopLayer : public cocos2d::CCLayer {
public:
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    static GJShopLayer* create(ShopType type);
    void dialogClosed(DialogLayer* layer);
    void didPurchaseItem(GJStoreItem* item);
    void exitVideoAdItems();
    bool init(ShopType type) override;
    void keyBackClicked() override;
    void onBack(cocos2d::CCObject* sender);
    void onCommunityCredits(cocos2d::CCObject* sender);
    void onExit() override;
    void onPlushies(cocos2d::CCObject* sender);
    void onSelectItem(cocos2d::CCObject* sender);
    void onVideoAd(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void rewardedVideoFinished();
    static cocos2d::CCScene* scene(ShopType type);
    void showCantAffordMessage(GJStoreItem* item);
    void showReactMessage();
    void updateCurrencyCounter();
    virtual ~GJShopLayer();

private:
    bool m_videoPlaying;
    CCCounterLabel* m_currencyLabel;
    std::string m_sheetName;
    cocos2d::CCDictionary* m_shopItems;
    ShopType m_type;
    bool m_closing;
    cocos2d::CCNode* m_unkNode1;
    cocos2d::CCNode* m_unkNode2;
    AnimatedShopKeeper* m_shopKeeper;
    int m_zolgurothDialogIndex;
    int m_affordDialogIndex;
};