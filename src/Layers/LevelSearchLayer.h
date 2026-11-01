#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/DemonFilterDelegate.h"
#include <string>

class FLAlertLayer;
class GJSearchObject;
class GameLevelManager;
class SearchButton;
class MoreSearchLayer;
class DemonFilterSelectLayer;
class GJGameLevel;
class GJAccountManager;
class CCTextInputNode;
class CCMenuItemSpriteExtra;

class LevelSearchLayer : public cocos2d::CCLayer,
                         public FLAlertLayerProtocol,
                         public TextInputDelegate,
                         public DemonFilterDelegate {
public:
    static LevelSearchLayer* create(int type);
    static cocos2d::CCScene* scene(int type);

    bool init(int type) override;
    void keyBackClicked() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    bool checkDiff(int diff);
    bool checkTime(int time);
    void clearFilters();
    void confirmClearFilters(cocos2d::CCObject* sender);
    void demonFilterSelectClosed(int filter);
    const char* getDiffKey(int diff);
    std::string getLevelLenKey();
    std::string getSearchDiffKey();
    GJSearchObject* getSearchObject(SearchType type, std::string query);
    const char* getTimeKey(int time);
    void onBack(cocos2d::CCObject* sender);
    void onClearFreeSearch(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onFollowed(cocos2d::CCObject* sender);
    void onFriends(cocos2d::CCObject* sender);
    void onLatestStars(cocos2d::CCObject* sender);
    void onMagic(cocos2d::CCObject* sender);
    void onMoreOptions(cocos2d::CCObject* sender);
    void onMostDownloaded(cocos2d::CCObject* sender);
    void onMostLikes(cocos2d::CCObject* sender);
    void onMostRecent(cocos2d::CCObject* sender);
    void onPasteClipboard(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onSearchMode(cocos2d::CCObject* sender);
    void onSearchUser(cocos2d::CCObject* sender);
    void onSpecialDemon(cocos2d::CCObject* sender);
    void onStarAward(cocos2d::CCObject* sender);
    void onSuggested(cocos2d::CCObject* sender);
    void onTrending(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void textInputOpened(CCTextInputNode* node);
    void toggleDifficulty(cocos2d::CCObject* sender);
    void toggleDifficultyNum(int diff, bool enabled);
    void toggleStar(cocos2d::CCObject* sender);
    void toggleTime(cocos2d::CCObject* sender);
    void toggleTimeNum(int time, bool enabled);
    void updateSearchLabel(const char* label);

    virtual ~LevelSearchLayer();

protected:
    cocos2d::CCSprite* m_starsSprite = nullptr;
    CCTextInputNode* m_searchInput = nullptr;
    cocos2d::CCSprite* m_lastDifficultySprite = nullptr;
    int m_type = 0;
    cocos2d::CCDictionary* m_difficultyDict = nullptr;
    cocos2d::CCArray* m_difficultySprites = nullptr;
    CCMenuItemSpriteExtra* m_demonTypeButton = nullptr;
    cocos2d::CCArray* m_lengthSprites = nullptr;
};