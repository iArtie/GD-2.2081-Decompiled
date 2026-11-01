#include "SongInfoObject.h"
#include "GameManager.h"
#include "MusicDownloadManager.h"
#include "MusicArtistObject.h"

void SongInfoObject::addTags(std::string tags) {
    if (tags.empty())
        return;

    if (tags == ",")
        return;

    std::vector<std::string> parts = GameToolbox::splitString(tags, ",");
    for (auto& part : parts) {
        int tag = atoi(part.c_str());
        if (tag > 0)
            m_tags.insert(tag);
    }
}

bool SongInfoObject::canEncode() {
    return true;
}

bool SongInfoObject::containsTag(int tag) {
    return m_tags.find(tag) != m_tags.end();
}

void SongInfoObject::copyValues() {
    if (!m_downloadLinkOverride.empty())
        m_downloadLinkOverride = m_downloadLinkOverride;
    if (!m_youtubeVideo.empty())
        m_youtubeVideo = m_youtubeVideo;
    if (!m_youtubeChannel.empty())
        m_youtubeChannel = m_youtubeChannel;
    if (!m_songUrl.empty())
        m_songUrl = m_songUrl;
    if (!m_extraArtists.empty())
        m_extraArtists = m_extraArtists;
    if (!m_extraArtistNames.empty())
        m_extraArtistNames = m_extraArtistNames;
}

SongInfoObject* SongInfoObject::create(cocos2d::CCDictionary* dict) {
    int songID = dict->valueForKey("1")->intValue();
    std::string songName = dict->valueForKey("2")->getCString();
    std::string artistName = dict->valueForKey("4")->getCString();
    int artistID = dict->valueForKey("3")->intValue();
    float fileSize = dict->valueForKey("5")->floatValue();
    std::string youtubeVideo = dict->valueForKey("6")->getCString();
    std::string youtubeChannel = dict->valueForKey("7")->getCString();
    int nongType = dict->valueForKey("11")->intValue();
    std::string songUrl = dict->valueForKey("10")->getCString();
    std::string downloadLinkOverride = dict->valueForKey("16")->getCString();
    std::string extraArtists = dict->valueForKey("12")->getCString();
    bool isNew = dict->valueForKey("13")->boolValue();
    int libraryOrder = dict->valueForKey("14")->intValue();

    if (songUrl.size() > 1)
        songUrl = cocos2d::ZipUtils::urlDecode(songUrl);
    else
        songUrl = "";

    if (downloadLinkOverride.size() > 1)
        downloadLinkOverride = cocos2d::ZipUtils::urlDecode(downloadLinkOverride);
    else
        downloadLinkOverride = "";

    SongInfoObject* obj = new SongInfoObject();
    obj->m_songID = songID;
    obj->m_songName = songName;
    obj->m_artistName = artistName;
    obj->m_artistID = artistID;
    obj->m_fileSize = fileSize;
    obj->m_youtubeVideo = youtubeVideo;
    obj->m_youtubeChannel = youtubeChannel;
    obj->m_nongType = nongType;
    obj->m_songUrl = songUrl;
    obj->m_downloadLinkOverride = downloadLinkOverride;
    obj->m_extraArtists = extraArtists;
    obj->m_isNew = isNew;
    obj->m_libraryOrder = libraryOrder;

    std::string extraArtistNames = dict->valueForKey("15")->getCString();
    obj->m_extraArtistNames = extraArtistNames;

    obj->autorelease();
    return obj;
}

SongInfoObject* SongInfoObject::createWithCoder(DS_Dictionary* dict) {
    int songID = dict->getIntegerForKey("1");
    std::string songName = dict->getStringForKey("2");
    std::string artistName = dict->getStringForKey("4");
    int artistID = dict->getIntegerForKey("3");
    float fileSize = dict->getFloatForKey("5");
    std::string youtubeVideo = dict->getStringForKey("6");
    std::string youtubeChannel = dict->getStringForKey("7");
    int nongType = dict->getIntegerForKey("11");
    std::string songUrl = dict->getStringForKey("10");
    std::string downloadLinkOverride = dict->getStringForKey("16");
    std::string extraArtists = dict->getStringForKey("12");
    bool isNew = dict->getBoolForKey("13");
    int libraryOrder = dict->getIntegerForKey("14");
    int priority = dict->getIntegerForKey("9");

    SongInfoObject* obj = new SongInfoObject();
    obj->m_songID = songID;
    obj->m_songName = songName;
    obj->m_artistName = artistName;
    obj->m_artistID = artistID;
    obj->m_fileSize = fileSize;
    obj->m_youtubeVideo = youtubeVideo;
    obj->m_youtubeChannel = youtubeChannel;
    obj->m_nongType = nongType;
    obj->m_songUrl = songUrl;
    obj->m_downloadLinkOverride = downloadLinkOverride;
    obj->m_extraArtists = extraArtists;
    obj->m_isNew = isNew;
    obj->m_libraryOrder = libraryOrder;
    obj->m_priority = priority;

    std::string extraArtistNames = dict->getStringForKey("15");
    obj->m_extraArtistNames = extraArtistNames;

    obj->autorelease();
    return obj;
}

void SongInfoObject::encodeWithCoder(DS_Dictionary* dict) {
    bool quickSave = GameManager::sharedState()->m_quickSave;
    dict->setIntegerForKey("kCEK", 6);
    dict->setIntegerForKey("1", m_songID);
    dict->setStringForKey("2", m_songName);
    dict->setStringForKey("4", m_artistName);
    dict->setIntegerForKey("3", m_artistID);
    dict->setFloatForKey("5", m_fileSize);
    dict->setIntegerForKey("11", m_nongType);
    dict->setBoolForKey("13", m_isNew);
    dict->setIntegerForKey("14", m_libraryOrder);
    if (!quickSave) {
        if (!m_youtubeVideo.empty())
            dict->setStringForKey("6", m_youtubeVideo);
        if (!m_youtubeChannel.empty())
            dict->setStringForKey("7", m_youtubeChannel);
        if (!m_downloadLinkOverride.empty())
            dict->setStringForKey("16", m_downloadLinkOverride);
        dict->setStringForKey("10", m_songUrl);
        dict->setStringForKey("12", m_extraArtists);
        dict->setStringForKey("15", m_extraArtistNames);
    }
    dict->setIntegerForKey("9", m_priority);
}

std::string SongInfoObject::getArtistNames(int unused) {
    std::string result = m_artistName;
    if (!m_artistIDs.empty()) {
        std::unordered_map<int, std::string> artistNamesMap;
        if (!m_extraArtistNames.empty()) {
            std::vector<std::string> parts = GameToolbox::splitString(m_extraArtistNames, ",");
            for (size_t i = 0; i + 1 < parts.size(); i += 2) {
                int id = atoi(parts[i].c_str());
                if (id != 10004636 && id > 0)
                    artistNamesMap[id] = parts[i + 1];
            }
        }
        for (int id : m_artistIDs) {
            if (id == 10004636)
                continue;
            std::string name;
            MusicArtistObject* artist = MusicDownloadManager::sharedState()->getMusicArtistForID(id);
            if (artist)
                name = artist->m_artistName;
            if (name.empty()) {
                auto it = artistNamesMap.find(id);
                if (it != artistNamesMap.end())
                    name = it->second;
            }
            if (!name.empty()) {
                if (!result.empty())
                    result += ", ";
                result += name;
            }
        }
    }
    return result;
}

int SongInfoObject::getExtraArtistCount() {
    int count = m_artistIDs.size();
    if (m_artistIDs.find(10004636) != m_artistIDs.end())
        count--;
    return count;
}

std::string SongInfoObject::getTagsString(bool shortTags) {
    if (shortTags && !m_shortTagsString.empty())
        return m_shortTagsString;
    if (!shortTags && !m_longTagsString.empty())
        return m_longTagsString;

    if (m_tags.empty())
        return "";

    std::string result;
    int count = 0;
    for (int tag : m_tags) {
        std::string name = MusicDownloadManager::sharedState()->nameForTagID(tag);
        if (name.empty())
            continue;
        if (!result.empty())
            result += ", ";
        result += name;
        count++;
        if (shortTags && count > 3) {
            int remaining = m_tags.size() - count;
            if (remaining > 0)
                result += cocos2d::CCString::createWithFormat(" +%i", remaining)->getCString();
            break;
        }
    }

    if (shortTags)
        m_shortTagsString = result;
    else
        m_longTagsString = result;

    return result;
}

void SongInfoObject::updateArtists(std::string artists) {
    m_extraArtists = artists;
    m_artistIDs.clear();
    if (artists.empty())
        return;

    std::vector<std::string> parts = GameToolbox::splitString(artists, ",");
    for (auto& part : parts) {
        int id = atoi(part.c_str());
        if (id > 0)
            m_artistIDs.insert(id);
    }
}

SongInfoObject::~SongInfoObject() {
}