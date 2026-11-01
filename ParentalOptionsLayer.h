#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class CCMenuItemSpriteExtra;

class ParentalOptionsLayer : public FLAlertLayer {
public:
    static ParentalOptionsLayer* create();

    bool init() override;

    int countForPage(int page);
    void goToPage(int page);
    void incrementCountForPage(int page);
    const char* infoKey(int index);
    cocos2d::CCLayer* layerForPage(int page);
    const char* layerKey(int page);
    cocos2d::CCPoint nextPosition(int page);
    const char* objectKey(int page);
    cocos2d::CCArray* objectsForPage(int page);
    const char* pageKey(int page);

    virtual void keyBackClicked() override;

    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);

    virtual ~ParentalOptionsLayer();

protected:
    cocos2d::CCDictionary* m_variables = nullptr;
    cocos2d::CCDictionary* m_values = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    int m_maxPage = 0;
    int m_page = -1;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    int m_toggleCount = 0;
};