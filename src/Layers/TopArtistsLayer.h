#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/OnlineListDelegate.h"
#include <string>

class LoadingCircle;
class GJCommentListLayer;
class CCMenuItemSpriteExtra;

class TopArtistsLayer : public FLAlertLayer, public OnlineListDelegate {
public:
    static TopArtistsLayer* create();
    bool init() override;
    bool isCorrect(const char* key) override;
    void keyBackClicked() override;
    void loadListFailed(const char* key) override;
    void loadListFinished(cocos2d::CCArray* objects, const char* key) override;
    void loadPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void setupLeaderboard(cocos2d::CCArray* artists);
    void setupPageInfo(std::string info, const char* key) override;
    void show() override;
    void updateLevelsLabel();
    virtual ~TopArtistsLayer();

protected:
    LoadingCircle* m_loadingCircle = nullptr;
    GJCommentListLayer* m_listLayer = nullptr;
    cocos2d::CCArray* m_topArtists = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    cocos2d::CCLabelBMFont* m_pageInfoLabel = nullptr;
    int m_pageStartIdx = 0;
    unsigned int m_itemCount = 0;
    int m_pageEndIdx = 0;
    int m_page = 0;
};