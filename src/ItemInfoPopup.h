#pragma once

#include "cocos2d.h"
#include <string>

class ItemInfoPopup : public FLAlertLayer {
public:
    static ItemInfoPopup* create(int id, UnlockType type);
    bool init(int id, UnlockType type) override;
    bool isUnlockedByDefault(int id, UnlockType type);
    void keyBackClicked() override;
    static std::string nameForUnlockType(int id, UnlockType type);
    void onClose(cocos2d::CCObject* sender);
    void onCredit(cocos2d::CCObject* sender);
    virtual ~ItemInfoPopup();

private:
    int m_accountID;
    int m_itemID;
    UnlockType m_unlockType;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
};