#include "GJUserScore.h"
#include "GJAccountManager.h"
#include "GameManager.h"

GJUserScore* GJUserScore::create() {
    auto score = new GJUserScore();
    if (score && score->init()) {
        score->autorelease();
        return score;
    }
    delete score;
    return nullptr;
}

bool GJUserScore::init() {
    m_userName = "";
    m_userUDID = "";
    m_accountID = 0;
    m_iconID = 1;
    m_iconType = IconType::Cube;
    m_color1 = 0;
    m_color2 = 3;
    m_color3 = 0;
    m_stars = 0;
    m_demons = 0;
    m_secretCoins = 0;
    m_creatorPoints = 0;
    m_diamonds = 0;
    m_moons = 0;
    m_playerRank = 0;
    m_messageState = 0;
    m_friendReqStatus = 0;
    m_commentHistoryStatus = 0;
    m_isGlobalMod = false;
    m_globalModID = 0;
    m_isFriend = false;
    m_friendRequestStatus = 0;
    m_modBadgeLevel = 0;
    m_leaderboardRank = 0;
    m_creatorPointsRank = 0;
    m_isArtist = false;
    m_unkBool = false;
    m_userCoins = 0;
    m_specialRank = 0;
    m_globalRank = 0;
    m_leaderboardScore = 0;
    m_accountID2 = 0;
    m_gauntletCompleted = 0;
    m_isFollowing = false;
    m_isBlocked = false;
    m_newMsgCount = 0;
    m_friendReqCount = 0;
    m_special = 0;
    m_glowEnabled = false;
    m_toggled = false;
    m_newFriendRequest = false;
    m_scoreType = 0;
    m_userID = 0;
    m_levelMode = 0;
    m_leaderboardMode = false;
    m_unkString = 0;
    return true;
}

bool GJUserScore::isCurrentUser() {
    auto accountManager = GJAccountManager::sharedState();
    int accountID = accountManager->m_accountID - accountManager->m_unkInt1;
    
    if (accountID && m_accountID == accountID) {
        return true;
    }
    
    auto gameManager = GameManager::sharedState();
    return m_userUDID == gameManager->m_playerUDID;
}

void GJUserScore::mergeWithScore(GJUserScore* other) {
    if (m_stars < other->m_stars) m_stars = other->m_stars;
    if (m_demons < other->m_demons) m_demons = other->m_demons;
    if (m_secretCoins < other->m_secretCoins) m_secretCoins = other->m_secretCoins;
    if (m_creatorPoints < other->m_creatorPoints) m_creatorPoints = other->m_creatorPoints;
    if (m_diamonds < other->m_diamonds) m_diamonds = other->m_diamonds;
    if (m_moons < other->m_moons) m_moons = other->m_moons;
}

GJUserScore::~GJUserScore() {
}