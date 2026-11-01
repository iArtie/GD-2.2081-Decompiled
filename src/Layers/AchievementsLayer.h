#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJDropDownLayer.h"
#include "UI/CustomListView.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

class AchievementsLayer : public GJDropDownLayer
{
public:
    static AchievementsLayer* create();

    void customSetup();
    void loadPage(int page);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void setupLevelBrowser(cocos2d::CCArray* arr);
    void setupPageInfo(int itemCount, int pageStartIdx, int pageEndIdx);

    ~AchievementsLayer();

private:
    cocos2d::CCLayer* m_listLayer;
    CCMenuItemSpriteExtra* m_prevPageButton;
    CCMenuItemSpriteExtra* m_nextPageButton;
    cocos2d::CCLabelBMFont* m_pageLabel;
    int m_currentPage;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCPoint m_unkPoint;
};