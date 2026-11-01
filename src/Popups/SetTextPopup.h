#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include <string>

class CCTextInputNode;
class SetTextPopupDelegate;

class SetTextPopup : public FLAlertLayer, public TextInputDelegate {
public:
    static SetTextPopup* create(std::string value, std::string title, int maxLength, std::string button1, std::string button2, bool allowDefault, float textWidth);
    bool init(std::string value, std::string title, int maxLength, std::string button1, std::string button2, bool allowDefault, float textWidth);

    void show();

    void keyBackClicked() override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void onCancel(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void onResetValue(cocos2d::CCObject* sender);
    void updateTextInputLabel();

    ~SetTextPopup();

protected:
    __int64 m_ZOrder = 0;
    cocos2d::CCNode* m_scene = nullptr;
    bool m_disableDelegate = false;
    std::string m_value;
    CCTextInputNode* m_input = nullptr;
    SetTextPopupDelegate* m_delegate = nullptr;
    bool m_cancelled = false;
};