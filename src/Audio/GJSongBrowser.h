#pragma once

#include "Platform/cocos2d.h"

class CustomListView;
class GJListLayer;
class CCMenuItemSpriteExtra;
class FLAlertLayer;

class GJSongBrowser : public GJDropDownLayer {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    static GJSongBrowser* create();
    void customSetup() override;
    void exitLayer(cocos2d::CCObject* sender);
    int getSelectedCellIdx();
    bool init() override;
    void loadPage(int page);
    void onDeleteAll(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void setupPageInfo(int itemCount, int pageStartIdx, int pageEndIdx);
    void setupSongBrowser(cocos2d::CCArray* songs);
    virtual ~GJSongBrowser();

private:
    int m_songID;
    CCMenuItemSpriteExtra* m_leftArrow;
    CCMenuItemSpriteExtra* m_rightArrow;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLabelBMFont* m_countText;
    cocos2d::CCArray* m_downloadedSongs;
    GJListLayer* m_listLayer;
    CustomListView* m_listView;
    int m_page;
};