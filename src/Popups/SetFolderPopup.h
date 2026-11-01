#pragma once

#include "Platform/cocos2d.h"
#include "Popups/SetIDPopup.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/SetTextPopupDelegate.h"
#include <string>

class CCTextInputNode;
class SetTextPopup;

class SetFolderPopup : public SetIDPopup, public TextInputDelegate, public SetTextPopupDelegate {
public:
    static SetFolderPopup* create(int value, bool isCreated, std::string title);
    bool init(int value, bool isCreated, std::string title) override;

    void onSetFolderName(cocos2d::CCObject* sender);
    void setTextPopupClosed(SetTextPopup* popup, std::string text) override;
    void valueChanged();

    ~SetFolderPopup();

protected:
    bool m_isCreated = false;
    cocos2d::CCLabelBMFont* m_titleLabel = nullptr;
};