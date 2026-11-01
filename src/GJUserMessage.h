#pragma once

#include "cocos2d.h"
#include <string>

class GJUserMessage : public cocos2d::CCObject {
public:
    static GJUserMessage* create();
    virtual ~GJUserMessage();

    int m_messageID;
    int m_accountID;
    int m_userID;
    std::string m_title;
    std::string m_content;
    std::string m_username;
    std::string m_uploadDate;
    bool m_read;
    bool m_outgoing;
    bool m_toggled;
};