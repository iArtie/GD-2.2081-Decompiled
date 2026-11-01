// GJComment.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJComment : public cocos2d::CCObject {
public:
    static GJComment* create();
    bool init();
    virtual ~GJComment();

    std::string m_commentString;
    std::string m_uploadDate;
    int m_accountID;
    int m_commentID;
    int m_likeCount;
    bool m_isLiked;
    int m_levelID;
    std::string m_username;
    int m_userID;
    int m_userScore;
    bool m_isModerator;
    unsigned char m_rating;
    unsigned char m_colorR;
    unsigned char m_colorG;
    unsigned char m_colorB;
};