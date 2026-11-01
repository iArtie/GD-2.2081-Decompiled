#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>

class CCTextInputNode;
class UploadActionPopup;
class ShareCommentDelegate;

class ShareCommentLayer : public FLAlertLayer, public UploadActionDelegate, public UploadPopupDelegate, public TextInputDelegate {
public:
    static ShareCommentLayer* create(std::string desc, int itemID, int charLimit, CommentType commentType);
    bool init(std::string desc, int itemID, int charLimit, CommentType commentType);
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;
    void onClear(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onPercent(cocos2d::CCObject* sender);
    void onShare(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void textInputOpened(CCTextInputNode* node);
    void updateCharCountLabel();
    void updateDescText(std::string desc);
    void updatePercentLabel();
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;
    virtual ~ShareCommentLayer();

protected:
    CCTextInputNode* m_commentInput = nullptr;
    UploadActionPopup* m_uploadPopup = nullptr;
    bool m_uploadSuccess = false;
    int m_itemID = 0;
    CommentType m_commentType = CommentType::Level;
    std::string m_descText;
    bool m_percentEnabled = false;
    int m_percent = 0;
    ShareCommentDelegate* m_delegate = nullptr;
    cocos2d::CCLabelBMFont* m_percentLabel = nullptr;
    int m_charLimit = 0;
    cocos2d::CCLabelBMFont* m_charCountLabel = nullptr;
};