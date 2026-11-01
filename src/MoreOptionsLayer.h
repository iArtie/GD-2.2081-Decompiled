#pragma once

#include "cocos2d.h"
#include "GJEnums.h"

class GJDropDownLayer;
class CCTextInputNode;
class CCMenuItemSpriteExtra;
class CCMenuItemToggler;

class MoreOptionsLayer : public FLAlertLayer,
                         public TextInputDelegate,
                         public GooglePlayDelegate,
                         public GJDropDownLayerDelegate {
public:
    static MoreOptionsLayer* create();

    bool init() override;
    void keyBackClicked() override;

    void addToggle(const char* label, const char* key, const char* description);
    int countForPage(int page);
    void dropDownLayerWillClose(GJDropDownLayer* layer);
    void goToPage(int page);
    void incrementCountForPage(int page);
    const char* infoKey(int index);
    cocos2d::CCLayer* layerForPage(int page);
    const char* layerKey(int page);
    cocos2d::CCPoint nextPosition(int page);
    const char* objectKey(int page);
    cocos2d::CCArray* objectsForPage(int page);
    void offsetToNextPage();
    void onClose(cocos2d::CCObject* sender);
    void onFMODDebug(cocos2d::CCObject* sender);
    void onGPSignIn(cocos2d::CCObject* sender);
    void onGPSignOut(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onKeybindings(cocos2d::CCObject* sender);
    void onNextPage(cocos2d::CCObject* sender);
    void onParental(cocos2d::CCObject* sender);
    void onPrevPage(cocos2d::CCObject* sender);
    void onSongBrowser(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    const char* pageKey(int page);
    void textInputReturn(CCTextInputNode* node);
    void textInputShouldOffset(CCTextInputNode* node, float yOffset);
    void toggleGP();

    virtual ~MoreOptionsLayer();

protected:
    cocos2d::CCDictionary* m_objects = nullptr;
    int m_toggleCount = 0;
    cocos2d::CCDictionary* m_variables = nullptr;
    int m_pageCount = 0;
    int m_page = -1;
    CCMenuItemSpriteExtra* m_leftBtn = nullptr;
    CCMenuItemSpriteExtra* m_rightBtn = nullptr;
    cocos2d::CCLabelBMFont* m_categoryLabel = nullptr;
    CCTextInputNode* m_offsetInput = nullptr;
    CCMenuItemSpriteExtra* m_gpSignInBtn = nullptr;
    CCMenuItemSpriteExtra* m_gpSignOutBtn = nullptr;
};