#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"

class CCTextInputNode;
class SetIDPopupDelegate;

class SetIDPopup : public FLAlertLayer, public TextInputDelegate {
public:
    bool init(int value, int minimum, int maximum, std::string title, std::string button, bool allowArrows, bool allowDefault, float textWidth, bool noElasticity, bool cancelled);
    void show();

    void keyBackClicked() override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void onCancel(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void onItemIDArrow(cocos2d::CCObject* sender);
    void onResetValue(cocos2d::CCObject* sender);
    void updateTextInputLabel();
    virtual void valueChanged();

    ~SetIDPopup();

protected:
    CCTextInputNode* m_inputNode = nullptr;
    int m_value = 0;
    int m_minimum = 0;
    SetIDPopupDelegate* m_delegate = nullptr;
    int m_maximum = 0;
    bool m_cancelled = false;
    cocos2d::CCNode* m_scene = nullptr;
    int m_ZOrder = 0;
    bool m_disableDelegate = false;
    int m_default = 0;
};