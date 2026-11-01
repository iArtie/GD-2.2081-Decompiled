#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class GJLevelList;
class CCMenuItemToggler;

class ShareListLayer : public FLAlertLayer {
public:
    static ShareListLayer* create(GJLevelList* list);
    bool init(GJLevelList* list) override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onShare(cocos2d::CCObject* sender);
    void onUnlisted(cocos2d::CCObject* sender);
    void updateUnlistedF();
    virtual ~ShareListLayer();

protected:
    GJLevelList* m_list = nullptr;
    CCMenuItemToggler* m_friendsOnlyToggler = nullptr;
    cocos2d::CCLabelBMFont* m_friendsOnlyLabel = nullptr;
};