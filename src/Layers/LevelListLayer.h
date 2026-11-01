#pragma once

#include "Platform/cocos2d.h"
#include "Layers/LevelBrowserLayer.h"
#include "Delegates/LikeItemDelegate.h"
#include "Delegates/LevelManagerDelegate.h"
#include "Delegates/LevelListDeleteDelegate.h"
#include "Delegates/SelectListIconDelegate.h"
#include "Delegates/TextInputDelegate.h"
#include "Utils/GJEnums.h"

class GJLevelList;
class GJSearchObject;
class GJListLayer;
class CCMenuItemSpriteExtra;
class TextArea;
class CCTextInputNode;
class SetIDPopup;
class SelectListIconLayer;
class ShareCommentLayer;
class CurrencyRewardLayer;

class LevelListLayer : public LevelBrowserLayer,
                       public LikeItemDelegate,
                       public LevelManagerDelegate,
                       public LevelListDeleteDelegate,
                       public SelectListIconDelegate,
                       public TextInputDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void cloneList();
    void confirmClone(cocos2d::CCObject* sender);
    void confirmDelete(cocos2d::CCObject* sender);
    void confirmOwnerDelete(cocos2d::CCObject* sender);
    static LevelListLayer* create(GJLevelList* list);
    void iconSelectClosed(SelectListIconLayer* layer);
    bool init(GJLevelList* list);
    void levelListDeleteFailed(int id) override;
    void levelListDeleteFinished(int id) override;
    void likedItem(LikeItemType type, int id, bool liked) override;
    void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type) override;
    void onBack(cocos2d::CCObject* sender) override;
    void onClaimReward(cocos2d::CCObject* sender);
    void onDelete();
    void onDescription(cocos2d::CCObject* sender);
    void onEnter() override;
    void onExit() override;
    void onFavorite(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onLike(cocos2d::CCObject* sender);
    void onListInfo(cocos2d::CCObject* sender);
    void onRefreshLevelList(cocos2d::CCObject* sender);
    void onSelectIcon(cocos2d::CCObject* sender);
    void onShare(cocos2d::CCObject* sender);
    void onToggleEditMode(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void ownerDelete();
    static cocos2d::CCScene* scene(GJLevelList* list);
    void setIDPopupClosed(SetIDPopup* popup, int value) override;
    void shareCommentClosed(std::string text, ShareCommentLayer* layer) override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void updateEditMode();
    cocos2d::CCArray* updateResultArray(cocos2d::CCArray* results) override;
    void updateSideButtons();
    void updateStatsArt();
    void verifyListName();
    ~LevelListLayer() override;

private:
    GJLevelList* m_levelList = nullptr;
    bool m_exited = false;
    bool m_exiting = false;
    std::string m_searchKey2;
    cocos2d::CCPoint m_rewardPosition;
    cocos2d::CCArray* m_objects = nullptr;
    cocos2d::CCSprite* m_diffSprite = nullptr;
    cocos2d::CCSprite* m_featureSprite = nullptr;
    CCMenuItemSpriteExtra* m_likeButton = nullptr;
    std::string m_searchKey1;
    int m_editMode = 0;
    CCTextInputNode* m_titleInput = nullptr;
    bool m_unk2 = false;
};