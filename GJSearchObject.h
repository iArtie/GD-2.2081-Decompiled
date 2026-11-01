#pragma once

#include <string>

enum class SearchType {
    Search = 0,
    Downloaded = 1,
    MostLiked = 2,
    Trending = 3,
    Recent = 4,
    UsersLevels = 5,
    Featured = 6,
    Magic = 7,
    Sends = 8,
    MapPack = 9,
    MapPackOnClick = 10,
    Awarded = 11,
    Followed = 12,
    Friends = 13,
    Users = 14,
    LikedGDW = 15,
    HallOfFame = 16,
    FeaturedGDW = 17,
    Similar = 18,
    Type19 = 19,
    TopListsUnused = 20,
    DailySafe = 21,
    WeeklySafe = 22,
    EventSafe = 23,
    Reported = 24,
    LevelListsOnClick = 25,
    Type26 = 26,
    Sent = 27,
    FeaturedLite = 28,
    Bonus = 29,
    StarAward = 30,
    MyLevels = 98,
    SavedLevels = 99,
    FavouriteLevels = 100,
    SmartTemplates = 101,
    MyLists = 102,
    FavouriteLists = 103
};

class GJSearchObject {
public:
    static GJSearchObject* create(SearchType searchType);
    static GJSearchObject* createFromKey(const char* key);
    const char* getKey();
    const char* getNextPageKey();
    GJSearchObject* getNextPageObject();
    GJSearchObject* getPageObject(int page);
    GJSearchObject* getPrevPageObject();
    bool isLevelSearchObject();
    virtual ~GJSearchObject();

    SearchType m_searchType;
    std::string m_searchQuery;
    std::string m_difficulty;
    std::string m_length;
    int m_starFilter;
    bool m_uncompletedFilter;
    bool m_featuredFilter;
    int m_songID;
    bool m_originalFilter;
    bool m_twoPlayerFilter;
    bool m_customSongFilter;
    bool m_songFilter;
    bool m_noStarFilter;
    bool m_coinsFilter;
    bool m_epicFilter;
    bool m_legendaryFilter;
    bool m_mythicFilter;
    bool m_completedFilter;
    bool m_demonFilter;
    int m_folder;
    int m_searchMode;
    int m_total;
    int m_page;
};