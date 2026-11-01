// GJComment.cpp
#include "GJComment.h"

GJComment* GJComment::create() {
    GJComment* instance = new GJComment();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJComment::init() {
    m_commentString = "";
    m_uploadDate = "";
    m_accountID = 0;
    m_commentID = 0;
    m_likeCount = 0;
    m_isLiked = false;
    m_levelID = 0;
    m_username = "";
    m_userID = 0;
    m_userScore = 0;
    m_isModerator = false;
    m_rating = 0;
    m_colorR = 0;
    m_colorG = 0;
    m_colorB = 0;
    return true;
}

GJComment::~GJComment() {
    // Strings cleaned up automatically
}