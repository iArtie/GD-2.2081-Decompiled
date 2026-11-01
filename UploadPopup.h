#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "FLAlertLayer.h"
#include "LevelUploadDelegate.h"

class GJGameLevel;
class TextArea;
class LoadingCircle;
class CCMenuItemSpriteExtra;

class UploadPopup : public FLAlertLayer, public LevelUploadDelegate {
public:
    static UploadPopup* create(GJGameLevel* level);
    bool init(GJGameLevel* level);

    void keyBackClicked() override;
    void levelUploadFailed(GJGameLevel* level);
    void levelUploadFinished(GJGameLevel* level);
    void onBack(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onReturnToLevel(cocos2d::CCObject* sender);
    void show();

    ~UploadPopup();

protected:
    GJGameLevel* m_level = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    TextArea* m_textArea = nullptr;
    CCMenuItemSpriteExtra* m_backButton = nullptr;
};