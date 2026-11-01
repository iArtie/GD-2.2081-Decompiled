#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "DialogDelegate.h"
#include "TextInputDelegate.h"
#include "GJOnlineRewardDelegate.h"

class TextArea;
class CCSpriteWithHue;
class CCSpriteGrayscale;
class LoadingCircleSprite;
class RewardUnlockLayer;
class GJRewardItem;
class DialogLayer;
class CCTextInputNode;

class SecretLayer5 : public cocos2d::CCLayer,
                     public DialogDelegate,
                     public TextInputDelegate,
                     public GJOnlineRewardDelegate {
public:
    static SecretLayer5* create();
    static cocos2d::CCScene* scene();
    bool init() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void dialogClosed(DialogLayer* layer) override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void onlineRewardStatusFailed() override;
    void onlineRewardStatusFinished(std::string key) override;

    void animateHead();
    void claimOnlineReward();
    void fadeInMessage();
    void fadeInSubmitMessage();
    void fadeOutMessage();
    void finishLoadingState();
    std::string getMessage();
    void handleSecretResponse();
    void hideTextInput();
    void incrementChatIdx();
    void keyBackClicked() override;
    void onBack(cocos2d::CCObject* sender);
    void onExit() override;
    void onSubmit(cocos2d::CCObject* sender);
    void playWinSFX();
    void showDialog(int index);
    void showFailAnimation();
    void showFirstDialog();
    void showSuccessAnimation();
    void showTextInput();
    void unlockUI();
    void updateMessageLabel(std::string text);
    void updateSearchLabel(const char* text);
    void updateTweenActionInt(float value, int tag);

    ~SecretLayer5();

protected:
    TextArea* m_messageArea = nullptr;
    CCSpriteWithHue* m_wraithSprite = nullptr;
    bool m_uiLocked = false;
    CCTextInputNode* m_textInput = nullptr;
    bool m_exiting = false;
    std::vector<int> m_soundEffects;
    int m_chatIndex = 0;
    cocos2d::CCSprite* m_eyesSprite = nullptr;
    CCSpriteGrayscale* m_wraithGraySprite = nullptr;
    cocos2d::CCSprite* m_lockSprite = nullptr;
    cocos2d::CCSprite* m_background = nullptr;
    cocos2d::CCArray* m_torchFires = nullptr;
    cocos2d::extension::CCScale9Sprite* m_inputBackground = nullptr;
    std::string m_chestID;
    LoadingCircleSprite* m_circleSprite = nullptr;
    CCMenuItemSpriteExtra* m_wraithButton = nullptr;
    int m_rewardStatus = 0;
    bool m_loading = false;
    cocos2d::CCLabelBMFont* m_messageLabel = nullptr;
};