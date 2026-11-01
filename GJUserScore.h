#pragma once

#include "cocos2d.h"
#include <string>

enum class IconType {
    Cube = 0,
    Ship = 1,
    Ball = 2,
    UFO = 3,
    Wave = 4,
    Robot = 5,
    Spider = 6,
    Swing = 7
};

enum class LeaderboardStat {
    None = 0,
    Stars = 1,
    Demons = 2,
    Moons = 3,
    UserCoins = 4,
    Diamonds = 5,
    CreatorPoints = 6
};

class GJUserScore : public cocos2d::CCObject {
public:
    static GJUserScore* create();
    bool init() override;
    bool isCurrentUser();
    void mergeWithScore(GJUserScore* other);
    virtual ~GJUserScore();

    std::string m_userName;
    std::string m_userUDID;
    int m_accountID;
    int m_iconID;
    IconType m_iconType;
    int m_color1;
    int m_color2;
    int m_color3;
    int m_stars;
    int m_demons;
    int m_secretCoins;
    int m_creatorPoints;
    int m_diamonds;
    int m_moons;
    int m_playerRank;
    int m_messageState;
    int m_friendReqStatus;
    int m_commentHistoryStatus;
    std::string m_youtubeURL;
    std::string m_twitterURL;
    std::string m_twitchURL;
    std::string m_instagramURL;
    std::string m_tiktokURL;
    std::string m_discordUsername;
    std::string m_customString;
    bool m_isGlobalMod;
    int m_globalModID;
    bool m_isFriend;
    int m_friendRequestStatus;
    int m_modBadgeLevel;
    int m_leaderboardRank;
    int m_creatorPointsRank;
    bool m_isArtist;
    bool m_unkBool;
    int m_userCoins;
    int m_specialRank;
    int m_globalRank;
    int m_leaderboardScore;
    int m_accountID2;
    int m_gauntletCompleted;
    std::string m_socialMedia1;
    std::string m_socialMedia2;
    std::string m_socialMedia3;
    bool m_isFollowing;
    bool m_isBlocked;
    int m_newMsgCount;
    int m_friendReqCount;
    int m_special;
    bool m_glowEnabled;
    bool m_toggled;
    bool m_newFriendRequest;
    int m_scoreType;
    int m_userID;
    int m_levelMode;
    bool m_leaderboardMode;
    int m_unkString;
    int m_unkInt1;
    int m_unkInt2;
    int m_unkInt3;
    int m_unkInt4;
    int m_unkInt5;
    int m_unkInt6;
    int m_unkInt7;
    int m_unkInt8;
    int m_unkInt9;
    int m_unkInt10;
    int m_unkInt11;
    int m_unkInt12;
    int m_unkInt13;
    int m_unkInt14;
    int m_unkInt15;
    int m_unkInt16;
    bool m_unkBool2;
    bool m_unkBool3;
    bool m_unkBool4;
    bool m_unkBool5;
    bool m_unkBool6;
    bool m_unkBool7;
    bool m_unkBool8;
    int m_unkInt17;
    int m_unkInt18;
    int m_unkInt19;
    int m_unkInt20;
    int m_unkInt21;
    int m_unkInt22;
    int m_unkInt23;
    int m_unkInt24;
    int m_unkInt25;
    int m_unkInt26;
    int m_unkInt27;
    int m_unkInt28;
    int m_unkInt29;
    int m_unkInt30;
    int m_unkInt31;
    int m_unkInt32;
    int m_unkInt33;
    int m_unkInt34;
};