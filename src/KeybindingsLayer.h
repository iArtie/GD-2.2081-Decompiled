#pragma once

#include "cocos2d.h"

class KeybindingsLayer : public FLAlertLayer {
public:
    cocos2d::CCLabelBMFont* addKeyPair(const char* action, const char* key);
    int countForPage(int page);
    static KeybindingsLayer* create();
    void goToPage(int page);
    void incrementCountForPage(int page);
    const char* infoKey(int index);
    bool init() override;
    void keyBackClicked() override;
    cocos2d::CCLayer* layerForPage(int page);
    const char* layerKey(int page);
    cocos2d::CCPoint nextPosition(int page);
    const char* objectKey(int page);
    cocos2d::CCArray* objectsForPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    const char* pageKey(int page);
    virtual ~KeybindingsLayer();

private:
    cocos2d::CCDictionary* m_values;
    cocos2d::CCLayer* m_mainLayer;
    int m_keyCount;
    int m_maxPage;
    int m_page;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCDictionary* m_variables;
    cocos2d::CCMenu* m_buttonMenu;
};