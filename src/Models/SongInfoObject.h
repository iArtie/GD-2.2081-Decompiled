#pragma once

#include "Platform/cocos2d.h"
#include "Utils/DS_Dictionary.h"
#include <string>
#include <set>
#include <unordered_set>

class SongInfoObject : public cocos2d::CCObject {
public:
    static SongInfoObject* create(cocos2d::CCDictionary* dict);
    static SongInfoObject* createWithCoder(DS_Dictionary* dict);
    void addTags(std::string tags);
    bool canEncode();
    bool containsTag(int tag);
    void copyValues();
    void encodeWithCoder(DS_Dictionary* dict);
    std::string getArtistNames(int unused);
    int getExtraArtistCount();
    std::string getTagsString(bool shortTags);
    void updateArtists(std::string artists);
    virtual ~SongInfoObject();

protected:
    int m_songID = 0;
    std::string m_songName;
    std::string m_artistName;
    int m_artistID = 0;
    float m_fileSize = 0.0f;
    int m_nongType = 0;
    bool m_isNew = false;
    int m_libraryOrder = 0;
    std::string m_youtubeVideo;
    std::string m_youtubeChannel;
    std::string m_downloadLinkOverride;
    std::string m_songUrl;
    std::string m_extraArtists;
    std::string m_extraArtistNames;
    int m_priority = 0;
    std::unordered_set<int> m_artistIDs;
    std::set<int> m_tags;
    std::string m_shortTagsString;
    std::string m_longTagsString;
};