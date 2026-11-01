#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/DialogDelegate.h"

class CCMenuItemSpriteExtra;
class DialogLayer;
class DungeonBarsSprite;

class SecretLayer3 : public cocos2d::CCLayer, public DialogDelegate {
public:
    static SecretLayer3* create();
    static cocos2d::CCScene* scene();

    bool init() override;

    void dialogClosed(DialogLayer* layer) override;

    void animateEyes();
    void firstInteractionStep1();
    void firstInteractionStep2();
    void firstInteractionStep3();
    void firstInteractionStep4();

    void keyBackClicked() override;

    void onBack(cocos2d::CCObject* sender);
    void onChest01(cocos2d::CCObject* sender);
    void onChest02(cocos2d::CCObject* sender);
    void onClick(cocos2d::CCObject* sender);
    void onExit() override;

    void revealStep1();
    void revealStep2();
    void revealStep3();
    void revealStep4();
    void revealStep5();

    void showUnlockDialog();

    virtual ~SecretLayer3();

protected:
    cocos2d::CCSprite* m_demonEyes = nullptr;
    bool m_lockInput = false;
    cocos2d::CCArray* m_locksArray = nullptr;
    cocos2d::CCSprite* m_demonBody = nullptr;
    CCMenuItemSpriteExtra* m_secretChest = nullptr;
    DungeonBarsSprite* m_dungeonBars = nullptr;
};