#pragma once

#include "cocos2d.h"
#include "GJEnums.h"

class FLAlertLayer;
class UploadActionPopup;
class CCTextInputNode;
class BoomListView;

class MPLobbyLayer : public cocos2d::CCLayer,
                     public FLAlertLayerProtocol,
                     public TextInputDelegate,
                     public UploadActionDelegate,
                     public UploadPopupDelegate {
public:
    static MPLobbyLayer* create(int type);
    static cocos2d::CCScene* scene(int type);

    bool init(int type);
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void didUploadMPComment(int id);
    void joinLobbyFailed(int id, GJMPErrorCode errorType);
    void joinLobbyFinished(int id);
    void onBack(cocos2d::CCObject* sender);
    void onBtn1(cocos2d::CCObject* sender);
    void onBtn2(cocos2d::CCObject* sender);
    void onBtn3(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onComment(cocos2d::CCObject* sender);
    void onUpdateLobby();
    void postComment();
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void textInputOpened(CCTextInputNode* node);
    void tryExitLobby();
    void updateComments();
    void updateLobby(float delay);
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);

    virtual ~MPLobbyLayer();

protected:
    float m_lobbyDelay = 0.0f;
    int m_lobbyID = 0;
    BoomListView* m_listLayer = nullptr;
    bool m_awaitingLobby = false;
    bool m_lobbyJoined = false;
    UploadActionPopup* m_uploadPopup = nullptr;
    cocos2d::CCLabelBMFont* m_lobbyLabel = nullptr;
    CCTextInputNode* m_commentInput = nullptr;
};