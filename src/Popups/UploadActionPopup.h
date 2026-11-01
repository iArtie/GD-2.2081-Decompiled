#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include <string>

class UploadPopupDelegate;
class TextArea;
class LoadingCircle;
class CCMenuItemSpriteExtra;

class UploadActionPopup : public FLAlertLayer {
public:
    static UploadActionPopup* create(UploadPopupDelegate* delegate, std::string str);
    bool init(UploadPopupDelegate* delegate, std::string str);

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void closePopup();
    void showFailMessage(std::string message);
    void showSuccessMessage(std::string message);

    ~UploadActionPopup();

protected:
    UploadPopupDelegate* m_delegate = nullptr;
    TextArea* m_textArea = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_closeButton = nullptr;
    bool m_succeeded = false;
};