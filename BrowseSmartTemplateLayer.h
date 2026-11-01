#pragma once

#include "cocos2d.h"
#include <string>

class GJSmartTemplate;
class SmartPrefabResult;

class BrowseSmartTemplateLayer : public FLAlertLayer {
public:
    void addObjectToPage(cocos2d::CCObject* object, int page);
    void addPrefabMenuItem(SmartPrefabResult* smartPrefabResult, int index);
    void baseSetup();
    void createDots();
    void createPrefab(std::string key, int id);
    void goToPage(int page);
    void keyBackClicked();
    void onBack(cocos2d::CCObject* sender);
    void onClick(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onTemplateObject(cocos2d::CCObject* sender);
    void updateDots();
    ~BrowseSmartTemplateLayer();

private:
    cocos2d::CCArray* m_pages;
    cocos2d::CCMenu* m_buttonMenu;
    GJSmartTemplate* m_template;
    bool m_noElasticity;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_prevPageBtn;
    cocos2d::CCNode* m_nextPageBtn;
    cocos2d::CCArray* m_dotsArray;
    int m_page;
};