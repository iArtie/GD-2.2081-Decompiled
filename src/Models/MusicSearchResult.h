#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class OptionsObject;
class OptionsObjectDelegate;

class MusicSearchResult : public cocos2d::CCObject {
public:
    static MusicSearchResult* create(GJSongType songType);

    bool init(GJSongType songType);
    void updateObjects(AudioSortType type);
    cocos2d::CCArray* applyArtistFilters(cocos2d::CCArray* objects);
    cocos2d::CCArray* applyTagFilters(cocos2d::CCArray* objects);
    void createArtistFilterObjects();
    void createTagFilterObjects();
    void stateChanged(OptionsObject* object);
    void updateFutureCount(cocos2d::CCArray* objects, cocos2d::CCArray* allObjects);
    cocos2d::CCArray* getFilesMatchingSearch(cocos2d::CCArray* objects, std::string query);

    virtual ~MusicSearchResult();

protected:
    AudioSortType m_sortType = AudioSortType::NameAscending;
    GJSongType m_songType = GJSongType::Music;
    std::string m_searchQuery;
    cocos2d::CCArray* m_filterObjects = nullptr;
    cocos2d::CCArray* m_tagFilterObjects = nullptr;
    OptionsObjectDelegate* m_optionsDelegate = nullptr;
    cocos2d::CCArray* m_artistFilterObjects = nullptr;
    bool m_tagFilter = false;
    bool m_artistFilter = false;
    int m_futureCount = -1;
};