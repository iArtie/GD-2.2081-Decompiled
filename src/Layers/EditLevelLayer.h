#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/UploadActionDelegate.h"
#include "Delegates/SetIDPopupDelegate.h"
#include "Delegates/UploadPopupDelegate.h"

class GJGameLevel;
class UploadActionPopup;
class SetIDPopup;
class CCTextInputNode;
class GJGameLevel;

class EditLevelLayer : public cocos2d::CCLayer, public FLAlertLayerProtocol, public TextInputDelegate, public UploadActionDelegate, public SetIDPopupDelegate, public UploadPopupDelegate {
public:
    static EditLevelLayer* create(GJGameLevel* level);
    static cocos2d::CCScene* scene(GJGameLevel* level);
    bool init(GJGameLevel* level);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    
    void closeTextInputs();
    void confirmClone(cocos2d::CCObject* sender);
    void confirmDelete(cocos2d::CCObject* sender);
    void confirmMoveToTop(cocos2d::CCObject* sender);
    void onBack(cocos2d::CCObject* sender);
    void onClone();
    void onClosePopup(UploadActionPopup* popup) override;
    void onDelete();
    void onEdit(cocos2d::CCObject* sender);
    void onGuidelines(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onLevelInfo(cocos2d::CCObject* sender);
    void onLevelLeaderboard(cocos2d::CCObject* sender);
    void onLevelOptions(cocos2d::CCObject* sender);
    void onMoveToTop();
    void onPlay(cocos2d::CCObject* sender);
    void onSetFolder(cocos2d::CCObject* sender);
    void onShare(cocos2d::CCObject* sender);
    void onTest(cocos2d::CCObject* sender);
    void onUpdateDescription(cocos2d::CCObject* sender);
    void playStep2();
    void playStep3();
    void setIDPopupClosed(SetIDPopup* popup, int value) override;
    void setupLevelInfo();
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void updateDescText(const char* text);
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;
    void verifyLevelName();
    
    virtual ~EditLevelLayer();

protected:
    GJGameLevel* m_level;
    cocos2d::CCLabelBMFont* m_folderLabel;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCArray* m_textInputs;
    UploadActionPopup* m_descriptionPopup;
    std::string m_levelName;
    int m_levelType;
    bool m_exiting;
};