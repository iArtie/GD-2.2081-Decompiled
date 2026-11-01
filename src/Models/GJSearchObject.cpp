#include "Models/GJSearchObject.h"

GJSearchObject* GJSearchObject::create(SearchType searchType) {
    auto obj = new GJSearchObject();
    obj->m_searchType = searchType;
    obj->m_page = 0;
    obj->m_total = 0;
    obj->m_starFilter = 0;
    obj->m_songID = 0;
    obj->m_folder = 0;
    obj->m_searchMode = 0;
    obj->m_uncompletedFilter = false;
    obj->m_featuredFilter = false;
    obj->m_originalFilter = false;
    obj->m_twoPlayerFilter = false;
    obj->m_customSongFilter = false;
    obj->m_songFilter = false;
    obj->m_noStarFilter = false;
    obj->m_coinsFilter = false;
    obj->m_epicFilter = false;
    obj->m_legendaryFilter = false;
    obj->m_mythicFilter = false;
    obj->m_completedFilter = false;
    obj->m_demonFilter = false;
    return obj;
}

GJSearchObject* GJSearchObject::createFromKey(const char* key) {
    auto parts = splitString(key, "_");
    if (parts.size() <= 17) {
        return nullptr;
    }
    
    auto obj = new GJSearchObject();
    obj->m_searchType = static_cast<SearchType>(std::stoi(parts[0]));
    obj->m_searchQuery = parts[1];
    obj->m_difficulty = parts[2];
    obj->m_length = parts[3];
    obj->m_starFilter = std::stoi(parts[4]);
    obj->m_uncompletedFilter = parts[5] == "1";
    obj->m_featuredFilter = parts[6] == "1";
    obj->m_originalFilter = parts[7] == "1";
    obj->m_songID = parts[8].empty() ? 0 : std::stoi(parts[8]);
    obj->m_twoPlayerFilter = parts[9] == "1";
    obj->m_customSongFilter = parts[10] == "1";
    obj->m_songFilter = parts[11] == "1";
    obj->m_noStarFilter = parts[12] == "1";
    obj->m_coinsFilter = parts[13] == "1";
    obj->m_epicFilter = parts[14] == "1";
    obj->m_legendaryFilter = parts[15] == "1";
    obj->m_mythicFilter = parts[16] == "1";
    obj->m_completedFilter = parts[17] == "1";
    obj->m_demonFilter = parts[18] == "1";
    obj->m_folder = parts[19].empty() ? 0 : std::stoi(parts[20]);
    obj->m_searchMode = parts[21].empty() ? 0 : std::stoi(parts[21]);
    obj->m_page = std::stoi(parts[22]);
    
    return obj;
}

const char* GJSearchObject::getKey() {
    return getSearchKey(
        m_searchType,
        m_searchQuery,
        m_difficulty,
        m_length,
        m_page,
        m_starFilter,
        m_uncompletedFilter,
        m_featuredFilter,
        m_songID,
        m_originalFilter,
        m_twoPlayerFilter,
        m_customSongFilter,
        m_songFilter,
        m_noStarFilter,
        m_coinsFilter,
        m_epicFilter,
        m_legendaryFilter,
        m_mythicFilter,
        m_completedFilter,
        m_demonFilter,
        m_folder,
        m_searchMode
    );
}

const char* GJSearchObject::getNextPageKey() {
    return getSearchKey(
        m_searchType,
        m_searchQuery,
        m_difficulty,
        m_length,
        m_page + 1,
        m_starFilter,
        m_uncompletedFilter,
        m_featuredFilter,
        m_songID,
        m_originalFilter,
        m_twoPlayerFilter,
        m_customSongFilter,
        m_songFilter,
        m_noStarFilter,
        m_coinsFilter,
        m_epicFilter,
        m_legendaryFilter,
        m_mythicFilter,
        m_completedFilter,
        m_demonFilter,
        m_folder,
        m_searchMode
    );
}

GJSearchObject* GJSearchObject::getNextPageObject() {
    return getPageObject(m_page + 1);
}

GJSearchObject* GJSearchObject::getPageObject(int page) {
    auto obj = new GJSearchObject();
    obj->m_searchType = m_searchType;
    obj->m_searchQuery = m_searchQuery;
    obj->m_difficulty = m_difficulty;
    obj->m_length = m_length;
    obj->m_page = page;
    obj->m_starFilter = m_starFilter;
    obj->m_uncompletedFilter = m_uncompletedFilter;
    obj->m_featuredFilter = m_featuredFilter;
    obj->m_songID = m_songID;
    obj->m_originalFilter = m_originalFilter;
    obj->m_twoPlayerFilter = m_twoPlayerFilter;
    obj->m_customSongFilter = m_customSongFilter;
    obj->m_songFilter = m_songFilter;
    obj->m_noStarFilter = m_noStarFilter;
    obj->m_coinsFilter = m_coinsFilter;
    obj->m_epicFilter = m_epicFilter;
    obj->m_legendaryFilter = m_legendaryFilter;
    obj->m_mythicFilter = m_mythicFilter;
    obj->m_completedFilter = m_completedFilter;
    obj->m_demonFilter = m_demonFilter;
    obj->m_folder = m_folder;
    obj->m_searchMode = m_searchMode;
    obj->m_total = m_total;
    return obj;
}

GJSearchObject* GJSearchObject::getPrevPageObject() {
    int page = (m_page > 0) ? m_page - 1 : 0;
    return getPageObject(page);
}

bool GJSearchObject::isLevelSearchObject() {
    if (static_cast<int>(m_searchType) >= static_cast<int>(SearchType::Users)) {
        if (m_searchType != SearchType::Users && 
            (static_cast<int>(m_searchType) < static_cast<int>(SearchType::Users) || 
             static_cast<int>(m_searchType) - static_cast<int>(SearchType::Users) > 1)) {
            return m_searchMode == 0;
        }
    } else if (static_cast<int>(m_searchType) < static_cast<int>(SearchType::Sends) && 
               m_searchType != SearchType::UsersLevels) {
        return m_searchMode == 0;
    }
    return false;
}

GJSearchObject::~GJSearchObject() {
}