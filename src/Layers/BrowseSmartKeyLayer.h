#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJSmartTemplate;
class GJSmartPrefab;

class BrowseSmartKeyLayer : public FLAlertLayer {
public:
    void addChanceToSelected(int chance, bool zero);
    static BrowseSmartKeyLayer* create(GJSmartTemplate* smartTemplate, std::string prefabKey);
    void createTemplateObjects();
    void deletedSelectedItems();
    cocos2d::CCArray* getAllSelectedBlocks();
    bool init(GJSmartTemplate* smartTemplate, std::string prefabKey);
    void onBack(cocos2d::CCObject* sender);
    void onButton(cocos2d::CCObject* sender);
    void onPrefabObject(cocos2d::CCObject* sender);
    void updateChanceValues();
    ~BrowseSmartKeyLayer();

private:
    GJSmartTemplate* m_template;
    std::string m_prefabKey;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCArray* m_pages;
};