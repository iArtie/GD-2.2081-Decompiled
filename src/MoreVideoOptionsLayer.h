#pragma once

#include "cocos2d.h"

class CCTextInputNode;
class CCMenuItemSpriteExtra;

class MoreVideoOptionsLayer : public FLAlertLayer,
                              public TextInputDelegate {
public:
    static MoreVideoOptionsLayer* create();

    bool init() override;
    void keyBackClicked() override;

    int countForPage(int page);
    void goToPage(int page);
    void incrementCountForPage(int page);
    const char* infoKey(int index);
    cocos2d::CCLayer* layerForPage(int page);
    const char* layerKey(int page);
    cocos2d::CCPoint nextPosition(int page);
    const char* objectKey(int page);
    cocos2d::CCArray* objectsForPage(int page);
    void onApplyFPS(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    const char* pageKey(int page);
    void updateFPSButtons();

    virtual ~MoreVideoOptionsLayer();

protected:
    CCTextInputNode* m_fpsInput = nullptr;
    cocos2d::CCDictionary* m_values = nullptr;
    int m_pageCount = 0;
    int m_page = -1;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    cocos2d::CCDictionary* m_variables = nullptr;
    cocos2d::CCArray* m_fpsNodes = nullptr;
};