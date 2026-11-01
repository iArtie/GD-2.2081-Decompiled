#include "FindObjectPopup.h"
#include "cocos2d.h"

FindObjectPopup* FindObjectPopup::create() {
    FindObjectPopup* ret = new FindObjectPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FindObjectPopup::init() {
    std::string title = "Find GroupID";
    std::string okButton = "OK";
    
    if (!SetIDPopup::init(0, 0, 9999999, title, okButton, false, false, 0.0f, true, false)) {
        return false;
    }
    
    // Additional initialization if needed
    m_unknownBool = false;
    
    return true;
}

void FindObjectPopup::onFindObjectID(cocos2d::CCObject* sender) {
    m_unknownBool = !m_unknownBool;
}

FindObjectPopup::~FindObjectPopup() {
    // Base class destructor handles cleanup
}