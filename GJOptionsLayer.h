#pragma once

#include "cocos2d.h"
#include <string>
#include <unordered_map>

class CCMenuItemToggler;
class CCMenuItemSpriteExtra;
class GJOptionsLayer : public SetupTriggerPopup {
public:
    int countForPage(int page);
    static GJOptionsLayer* create(int background);
    virtual void didToggle(int toggle);
    virtual void didToggleGV(const std::string& variable);
    CCMenuItemToggler* getToggleButton(int id);
    void goToPage(int page);
    void incrementCountForPage(int page);
    const char* infoKey(int index);
    bool init(int background) override;
    cocos2d::CCLayer* layerForPage(int page);
    const char* layerKey(int page);
    cocos2d::CCPoint nextPosition(int page);
    const char* objectKey(int page);
    cocos2d::CCArray* objectsForPage(int page);
    void offsetToNextPage();
    void onInfo(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    const char* pageKey(int page);
    virtual void setupOptions();
    virtual ~GJOptionsLayer();

private:
    cocos2d::CCDictionary* m_variables;
    int m_toggleCount;
    int m_togglesPerPage;
    std::unordered_map<int, CCMenuItemToggler*> m_toggleButtons;
    cocos2d::CCDictionary* m_values;
    float m_offset;
    float m_gap;
    cocos2d::CCLayer* m_mainLayer;
    int m_maxPage;
    int m_page;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCMenu* m_buttonMenu;
    float m_width;
    float m_height;
};