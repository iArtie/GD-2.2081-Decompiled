#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/ListUploadDelegate.h"

class GJLevelList;
class TextArea;
class LoadingCircle;
class CCMenuItemSpriteExtra;

class UploadListPopup : public FLAlertLayer, public ListUploadDelegate {
public:
    static UploadListPopup* create(GJLevelList* list);
    bool init(GJLevelList* list);

    void keyBackClicked() override;
    void listUploadFailed(GJLevelList* list, int response);
    void listUploadFinished(GJLevelList* list);
    void onBack(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onReturnToList(cocos2d::CCObject* sender);
    void show();

    ~UploadListPopup();

protected:
    LoadingCircle* m_loadingCircle = nullptr;
    TextArea* m_textArea = nullptr;
    CCMenuItemSpriteExtra* m_backButton = nullptr;
    GJLevelList* m_levelList = nullptr;
};