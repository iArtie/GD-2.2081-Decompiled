#include "Managers/MusicDownloadManager.h"
#include "Delegates/MusicDownloadDelegate.h"
#include "Delegates/MusicDelegateHandler.h"
#include "Models/SongInfoObject.h"
#include "Models/MusicArtistObject.h"
#include "Models/SFXInfoObject.h"
#include "Models/SFXFolderObject.h"
#include "Objects/OptionsObject.h"
#include "Cocos/CCURLObject.h"
#include "Utils/DS_Dictionary.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Models/GJHttpResult.h"
#include "Layers/FLAlertLayer.h"
#include "Platform/ThirdParty/MD5.h"
#include "ZipUtils.h"
#include <cstring>
#include <cstdlib>
#include <ctime>

static int optionsStrCompare(const void* a, const void* b)
{
    cocos2d::CCString* strA = *(cocos2d::CCString**)a;
    cocos2d::CCString* strB = *(cocos2d::CCString**)b;
    return strcmp(strA->getCString(), strB->getCString());
}

static int songCompare(const void* a, const void* b)
{
    SongInfoObject* songA = *(SongInfoObject**)a;
    SongInfoObject* songB = *(SongInfoObject**)b;
    return songA->m_songID - songB->m_songID;
}

void MusicDownloadManager::addDLToActive(const char* tag)
{
    cocos2d::CCNode* node = cocos2d::CCNode::create();
    addDLToActive(tag, node);
}

void MusicDownloadManager::addDLToActive(const char* tag, cocos2d::CCObject* obj)
{
    std::string key = tag;
    m_activeDownloads->setObject(obj, key);
}

void MusicDownloadManager::addMusicDownloadDelegate(MusicDownloadDelegate* delegate)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        if (handler->m_delegate == delegate)
            return;
    }
    MusicDelegateHandler* handler = MusicDelegateHandler::create(delegate);
    m_musicDownloadDelegates->addObject(handler);
}

SongInfoObject* MusicDownloadManager::addSongObjectFromString(std::string str)
{
    cocos2d::CCDictionary* dict = GameLevelManager::responseToDict(str, true);
    SongInfoObject* songObj = SongInfoObject::create(dict);
    int songID = songObj->m_songID;
    if (!songID)
        return nullptr;

    cocos2d::CCString* idStr = cocos2d::CCString::createWithFormat("%i", songID);
    std::string idKey = idStr->getCString();
    cocos2d::CCObject* existing = m_songObjects->objectForKey(idKey);
    if (existing)
        songObj->copyValues();

    cocos2d::CCString* verifiedStr = (cocos2d::CCString*)dict->valueForKey("8");
    bool verified = verifiedStr->boolValue();

    songObj->m_verified = !verified;

    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", songID);
    std::string key = keyStr->getCString();
    m_songObjects->setObject(songObj, key);
    return songObj;
}

void MusicDownloadManager::clearSong(int songID)
{
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", songID);
    std::string key = keyStr->getCString();
    m_songObjects->removeObjectForKey(key);
}

void MusicDownloadManager::clearUnusedSongs()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCDictionary* usedSongIDs = glm->getAllUsedSongIDs();
    cocos2d::CCArray* allSongs = getAllSongs();
    unsigned int count = allSongs->count();
    for (unsigned int i = 0; i < count; i++)
    {
        cocos2d::CCObject* obj = allSongs->objectAtIndex(i);
        SongInfoObject* song = (SongInfoObject*)obj;
        if (isSongDownloaded(song->m_songID) || usedSongIDs->objectForKey(std::to_string(song->m_songID)))
            continue;
        clearSong(song->m_songID);
    }
}

cocos2d::CCDictionary* MusicDownloadManager::createArtistsInfo(std::string str)
{
    if (str.empty())
        return nullptr;

    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find("~|~");
    while (end != std::string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + 3;
        end = str.find("~|~", start);
    }
    tokens.push_back(str.substr(start));

    if (tokens.size() <= 1)
        return nullptr;

    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    for (size_t i = 0; i + 1 < tokens.size(); i += 2)
    {
        int artistID = atoi(tokens[i].c_str());
        if (artistID <= 0)
            continue;
        std::string name = tokens[i + 1];
        MusicArtistObject* artist = MusicArtistObject::create(artistID, name, "", "");
        dict->setObject(artist, std::to_string(artistID));
    }
    return dict;
}

void MusicDownloadManager::createSongsInfo(std::string songsStr, std::string artistsStr)
{
    if (songsStr.empty())
        return;

    cocos2d::CCDictionary* artistsInfo = createArtistsInfo(artistsStr);

    std::vector<std::string> songTokens;
    size_t start = 0;
    size_t end = songsStr.find("~:~");
    while (end != std::string::npos)
    {
        songTokens.push_back(songsStr.substr(start, end - start));
        start = end + 3;
        end = songsStr.find("~:~", start);
    }
    songTokens.push_back(songsStr.substr(start));

    for (size_t i = 0; i < songTokens.size(); i++)
    {
        SongInfoObject* songObj = addSongObjectFromString(songTokens[i]);
        if (songObj && artistsInfo && songObj->m_artistID > 0)
        {
            std::string artistsList;
            std::unordered_set<int>* artistIDs = (std::unordered_set<int>*)songObj->m_artistIDs;
            if (artistIDs)
            {
                for (auto& artistID : *artistIDs)
                {
                    cocos2d::CCObject* artistObj = artistsInfo->objectForKey(std::to_string(artistID));
                    if (artistObj)
                    {
                        MusicArtistObject* artist = (MusicArtistObject*)artistObj;
                        if (!artistsList.empty())
                            artistsList += ",";
                        artistsList += std::to_string(artistID) + "," + artist->m_artistName;
                    }
                }
            }
            songObj->m_extraArtists = artistsList;
        }
    }
}

void MusicDownloadManager::dataLoaded(DS_Dictionary* dict)
{
    if (!m_songObjects)
    {
        cocos2d::CCDictionary* loadedSongs = dict->getDictForKey("MDLM_001", false);
        if (loadedSongs != m_songObjects)
        {
            if (loadedSongs) loadedSongs->retain();
            if (m_songObjects) m_songObjects->release();
            m_songObjects = loadedSongs;
        }
    }
    else
    {
        cocos2d::CCDictionary* loadedSongs = dict->getDictForKey("MDLM_001", false);
        cocos2d::CCArray* keys = loadedSongs->allKeys();
        for (unsigned int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            std::string keyStr = key->getCString();
            if (!m_songObjects->objectForKey(keyStr))
            {
                cocos2d::CCObject* obj = loadedSongs->objectForKey(keyStr);
                m_songObjects->setObject(obj, keyStr);
            }
        }
    }

    if (!m_MDLM003)
    {
        cocos2d::CCDictionary* loaded003 = dict->getDictForKey("MDLM_003", false);
        if (loaded003 != m_MDLM003)
        {
            if (loaded003) loaded003->retain();
            if (m_MDLM003) m_MDLM003->release();
            m_MDLM003 = loaded003;
        }
    }
    else
    {
        cocos2d::CCDictionary* loaded003 = dict->getDictForKey("MDLM_003", false);
        cocos2d::CCArray* keys = loaded003->allKeys();
        for (unsigned int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            std::string keyStr = key->getCString();
            if (!m_MDLM003->objectForKey(keyStr))
            {
                cocos2d::CCObject* obj = loaded003->objectForKey(keyStr);
                m_MDLM003->setObject(obj, keyStr);
            }
        }
    }

    int priorityFromDict = dict->getIntegerForKey("MDLM_002");
    if (m_songPriority < priorityFromDict)
        m_songPriority = priorityFromDict;
}

void MusicDownloadManager::deleteSFX(int id)
{
    std::string path = pathForSFX(id);
    remove(path.c_str());
}

void MusicDownloadManager::deleteSong(int id)
{
    std::string path = pathForSong(id);
    remove(path.c_str());
}

void MusicDownloadManager::downloadCustomSong(int id)
{
    if (m_customContentURL.empty())
        return;
    const char* downloadKey = getSongDownloadKey(id);
    if (isDLActive(downloadKey))
        return;

    downloadSongStarted(id);
    std::string path = "/music/" + GameToolbox::intToString(id) + ".ogg";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onDownloadSongCompleted, 0, 0);
    addDLToActive(downloadKey, request);
}

void MusicDownloadManager::downloadMusicLibrary()
{
    if (m_customContentURL.empty() || isDLActive("dl_music_l"))
        return;
    m_downloadingMusicLibrary = true;
    std::string path = "/music/musiclibrary_02.dat";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onDownloadMusicLibraryCompleted, 0, 0);
    addDLToActive("dl_music_l", request);
}

void MusicDownloadManager::downloadSFX(int id)
{
    if (m_customContentURL.empty())
        return;
    const char* downloadKey = getSFXDownloadKey(id);
    if (isDLActive(downloadKey))
        return;

    std::string path = "/sfx/s" + GameToolbox::intToString(id) + ".ogg";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onDownloadSFXCompleted, 5, 10);
    addDLToActive(downloadKey, request);
}

void MusicDownloadManager::downloadSFXFailed(int id, GJSongError errorCode)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->downloadSFXFailed(id, errorCode);
    }
}

void MusicDownloadManager::downloadSFXFinished(int id)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->downloadSFXFinished(id);
    }
}

void MusicDownloadManager::downloadSFXLibrary()
{
    if (m_customContentURL.empty() || isDLActive("dl_sfx_l"))
        return;
    m_requestedSfxLibraryDownloaded = true;
    std::string path = "/sfx/sfxlibrary.dat";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onDownloadSFXLibraryCompleted, 0, 0);
    addDLToActive("dl_sfx_l", request);
}

void MusicDownloadManager::downloadSong(int id)
{
    if (id > 10000000)
    {
        downloadCustomSong(id);
        return;
    }
    const char* downloadKey = getSongDownloadKey(id);
    if (isDLActive(downloadKey))
        return;

    downloadSongStarted(id);
    std::string url;
    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    SongInfoObject* songObj = manager->getSongInfoObject(id);
    if (songObj)
        url = songObj->m_songUrl;

    if (url.empty())
        url = "https://" + std::to_string(id);

    cocos2d::CCString* fileNameStr = cocos2d::CCString::createWithFormat("%i.mp3", id);
    std::string fileName = fileNameStr->getCString();

    cocos2d::extension::CCHttpRequest* request = new cocos2d::extension::CCHttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, cocos2d::extension::callfuncND_selector(MusicDownloadManager::onDownloadSongCompleted));
    request->setTag(fileName.c_str());
    request->retain();
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    request->release();
    addDLToActive(downloadKey, request);
}

void MusicDownloadManager::downloadSongFailed(int id, GJSongError errorCode)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->downloadSongFailed(id, errorCode);
    }
}

void MusicDownloadManager::downloadSongFinished(int id)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->downloadSongFinished(id);
    }
}

void MusicDownloadManager::downloadSongStarted(int id)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        if (handler->m_delegate)
            handler->m_delegate->downloadSongStarted(id);
    }
}

void MusicDownloadManager::encodeDataTo(DS_Dictionary* dict)
{
    dict->setDictForKey("MDLM_001", m_songObjects);
    dict->setDictForKey("MDLM_003", m_MDLM003);
    dict->setIntegerForKey("MDLM_002", m_songPriority);
}

cocos2d::CCArray* MusicDownloadManager::filterMusicByTag(int id, cocos2d::CCArray* songs)
{
    cocos2d::CCArray* returnVal = cocos2d::CCArray::create();
    if (songs)
    {
        cocos2d::ccArray* data = songs->data;
        if (data->num)
        {
            SongInfoObject** arr = (SongInfoObject**)data->arr;
            for (SongInfoObject** i = &arr[data->num - 1]; i >= arr; ++arr)
            {
                SongInfoObject* songInfoObject = *arr;
                if (!*arr)
                    break;
                if (SongInfoObject::containsTag(*arr, id))
                    cocos2d::CCArray::addObject(returnVal, songInfoObject);
            }
        }
    }
    return returnVal;
}

cocos2d::CCArray* MusicDownloadManager::filterMusicByArtistID(int artistID, cocos2d::CCArray* songs)
{
    cocos2d::CCArray* returnVal = cocos2d::CCArray::create();
    if (songs)
    {
        for (unsigned int i = 0; i < songs->count(); i++)
        {
            SongInfoObject* song = (SongInfoObject*)songs->objectAtIndex(i);
            if (!song)
                break;
            if (song->m_artistID == artistID)
                returnVal->addObject(song);
        }
    }
    return returnVal;
}

void MusicDownloadManager::firstSetup()
{
    if (!m_songObjects)
    {
        cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
        dict->retain();
        m_songObjects = dict;
    }
    m_songPriority = 0;
}

std::string MusicDownloadManager::generateCustomContentURL(std::string path)
{
    int intVal = time(nullptr);
    std::string expires = GameToolbox::intToString(intVal / 1000000000 + 3600);

    std::string md5Input = "DQIHAV8MVAUdDgBVVh0NBwQHHQEEDw8dDAQHBAdaAVNWAQkH";
    md5Input += expires;
    md5Input += path;

    unsigned int md5State[4] = { 1732584193, 4023233417, 2562383102, 271733878 };
    std::vector<unsigned char> md5Block;
    size_t len = md5Input.length();
    md5Block.resize(64, 0);
    memcpy(md5Block.data(), md5Input.c_str(), len);
    md5Block[len] = 0x80;
    uint64_t bitLen = len * 8;
    memcpy(md5Block.data() + 56, &bitLen, 8);
    MD5::MD5Transform(md5State, md5Block.data());

    unsigned char digest[16];
    memcpy(digest, md5State, 16);

    char* b64Out = nullptr;
    base64Encode((const unsigned char*)digest, 16, &b64Out);
    std::string token = b64Out;

    size_t pos;
    while ((pos = token.find('=')) != std::string::npos)
        token.erase(pos, 1);
    while ((pos = token.find('+')) != std::string::npos)
        token.replace(pos, 1, "-");
    while ((pos = token.find('/')) != std::string::npos)
        token.replace(pos, 1, "_");

    std::string result = m_customContentURL + path + "?token=" + token + "&expires=" + expires;
    return result;
}

void MusicDownloadManager::generateResourceAssetList()
{
    m_resourceSfxUnorderedSet.clear();
    static const int sfxAssets[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
        101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
        141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
        161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
        181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
        201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
        221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
        241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260
    };
    for (int id : sfxAssets)
        m_resourceSfxUnorderedSet.insert(id);

    m_resourceSongUnorderedSet.clear();
    static const int songAssets[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int id : songAssets)
        m_resourceSongUnorderedSet.insert(id);
}

cocos2d::CCArray* MusicDownloadManager::getAllMusicArtists(OptionsObjectDelegate* delegate)
{
    cocos2d::CCArray* result = cocos2d::CCArray::createWithCapacity(m_musicArtists->count());
    if (m_musicArtists)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_musicArtists->m_pElements;
        if (element)
        {
            cocos2d::CCDictElement* next = (cocos2d::CCDictElement*)element->hh.next;
            while (element)
            {
                MusicArtistObject* artist = (MusicArtistObject*)element->m_pObject;
                OptionsObject* option = OptionsObject::create(artist->m_artistID, false, artist->m_artistName, delegate);
                result->addObject(option);
                cocos2d::CCDictElement* temp = next ? (cocos2d::CCDictElement*)next->hh.next : nullptr;
                element = next;
                next = temp;
            }
        }
    }
    if (result->count() > 1)
        qsort(result->data->arr, result->data->num, 4u, optionsStrCompare);
    return result;
}

cocos2d::CCArray* MusicDownloadManager::getAllMusicObjects(GJSongType type)
{
    cocos2d::CCArray* result = cocos2d::CCArray::createWithCapacity(m_musicObjects->count());
    if (m_musicObjects)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_musicObjects->m_pElements;
        while (element)
        {
            SongInfoObject* song = (SongInfoObject*)element->m_pObject;
            if (type == GJSongType::Music || song->m_nongType == (int)type)
                result->addObject(song);
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
    return result;
}

cocos2d::CCArray* MusicDownloadManager::getAllMusicTags(OptionsObjectDelegate* delegate)
{
    cocos2d::CCArray* result = cocos2d::CCArray::createWithCapacity(m_musicTags.size());
    for (auto it = m_musicTags.begin(); it != m_musicTags.end(); ++it)
    {
        OptionsObject* option = OptionsObject::create(it->first, false, it->second, delegate);
        result->addObject(option);
    }
    if (result->count() > 1)
        qsort(result->data->arr, result->data->num, 4u, optionsStrCompare);
    return result;
}

cocos2d::CCArray* MusicDownloadManager::getAllSFXObjects(bool onlySFX)
{
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    if (m_sfxObjects)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_sfxObjects->m_pElements;
        while (element)
        {
            SFXInfoObject* sfx = (SFXInfoObject*)element->m_pObject;
            bool isFolder = (bool)(uintptr_t)sfx->m_folder;
            if (onlySFX && isFolder)
            {
                element = (cocos2d::CCDictElement*)element->hh.next;
                continue;
            }
            result->addObject(sfx);
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
    return result;
}

cocos2d::CCArray* MusicDownloadManager::getAllSongs()
{
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    if (m_songObjects)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_songObjects->m_pElements;
        while (element)
        {
            result->addObject(element->m_pObject);
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }
    return result;
}

void MusicDownloadManager::getCustomContentURL()
{
    if (isDLActive("get_c_url"))
        return;
    addDLToActive("get_c_url");
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string postString = glm->getBasePostString();
    ProcessHttpRequest("https://www.boomlings.com/database/getCustomContentURL.php", postString, "get_c_url", GJHttpType::GetCustomContentURL);
}

cocos2d::CCObject* MusicDownloadManager::getDLObject(const char* id)
{
    std::string key = id;
    return m_activeDownloads->objectForKey(key);
}

int MusicDownloadManager::getDownloadProgress(int id)
{
    const char* downloadKey = getSongDownloadKey(id);
    cocos2d::CCObject* obj = getDLObject(downloadKey);
    if (obj)
        return obj->getTag();
    return -1;
}

cocos2d::CCArray* MusicDownloadManager::getDownloadedSongs()
{
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    if (m_songObjects)
    {
        cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)m_songObjects->m_pElements;
        if (element)
        {
            cocos2d::CCDictElement* next = (cocos2d::CCDictElement*)element->hh.next;
            while (element)
            {
                SongInfoObject* song = (SongInfoObject*)element->m_pObject;
                if (isSongDownloaded(song->m_songID))
                    result->addObject(song);
                cocos2d::CCDictElement* temp = next ? (cocos2d::CCDictElement*)next->hh.next : nullptr;
                element = next;
                next = temp;
            }
        }
    }
    if (result->count() > 1)
        qsort(result->data->arr, result->data->num, 4u, songCompare);
    return result;
}

MusicArtistObject* MusicDownloadManager::getMusicArtistForID(int id)
{
    if (m_musicArtists)
        return (MusicArtistObject*)m_musicArtists->objectForKey(std::to_string(id));
    return nullptr;
}

SongInfoObject* MusicDownloadManager::getMusicObject(int id)
{
    if (m_musicObjects)
        return (SongInfoObject*)m_musicObjects->objectForKey(std::to_string(id));
    return nullptr;
}

const char* MusicDownloadManager::getSFXDownloadKey(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("sfx_%i", id);
    return str->getCString();
}

int MusicDownloadManager::getSFXDownloadProgress(int id)
{
    const char* downloadKey = getSFXDownloadKey(id);
    cocos2d::CCObject* obj = getDLObject(downloadKey);
    if (obj)
        return obj->getTag();
    return -1;
}

SFXFolderObject* MusicDownloadManager::getSFXFolderObjectForID(int id)
{
    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    SFXInfoObject* sfx = manager->getSFXObject(id);
    if (!sfx)
        return nullptr;
    bool isFolder = (bool)(uintptr_t)sfx->m_folder;
    if (isFolder)
        return (SFXFolderObject*)sfx;
    MusicDownloadManager* manager2 = MusicDownloadManager::sharedState();
    SFXInfoObject* folderSfx = manager2->getSFXObject(sfx->m_folderID);
    if (!folderSfx)
        return nullptr;
    bool folderIsFolder = (bool)(uintptr_t)folderSfx->m_folder;
    if (!folderIsFolder)
        return nullptr;
    return (SFXFolderObject*)folderSfx;
}

std::string MusicDownloadManager::getSFXFolderPathForID(int id, bool nextFolder)
{
    std::string result = "";
    SFXFolderObject* folder = getSFXFolderObjectForID(id);
    if (nextFolder)
        folder = getSFXFolderObjectForID(folder->m_folderID);
    int intVal = 0;
    while (intVal <= 9 && folder)
    {
        intVal++;
        result += " \\ ";
        result += folder->m_name;
        folder = getSFXFolderObjectForID(folder->m_folderID);
    }
    return result;
}

SFXInfoObject* MusicDownloadManager::getSFXObject(int id)
{
    if (m_sfxObjects)
        return (SFXInfoObject*)m_sfxObjects->objectForKey(std::to_string(id));
    return nullptr;
}

const char* MusicDownloadManager::getSongDownloadKey(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("d_%i", id);
    return str->getCString();
}

void MusicDownloadManager::getSongInfo(int id, bool download)
{
    unsigned int intVal = download;
    SongInfoObject* songObj = getSongInfoObject(id);
    if (songObj && songObj->m_verified >= intVal)
    {
        std::string songUrl = songObj->m_songUrl;
        if (!songUrl.empty())
        {
            loadSongInfoFinished(songObj);
            return;
        }
    }

    const char* songInfoKey = getSongInfoKey(id);
    if (isDLActive(songInfoKey))
        return;

    addDLToActive(songInfoKey);
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string basePost = glm->getBasePostString();

    cocos2d::CCString* secretStr = cocos2d::CCString::createWithFormat("%c%s%s%c%c%s",
        (char*)off_50 + 7, "mfd", "2893", 103, 98, "7");
    std::string secret = secretStr->getCString();

    cocos2d::CCString* paramStr = cocos2d::CCString::createWithFormat("&songID=%i&secret=%s", id, secret.c_str());
    std::string params = paramStr->getCString();

    std::string url = "https://www.boomlings.com/database/getGJSongInfo.php";
    std::string tag = std::to_string(id);

    ProcessHttpRequest(url, params, tag, GJHttpType::GetSongInfo);
}

const char* MusicDownloadManager::getSongInfoKey(int id)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("i_%i", id);
    return str->getCString();
}

SongInfoObject* MusicDownloadManager::getSongInfoObject(int id)
{
    cocos2d::CCString* keyStr = cocos2d::CCString::createWithFormat("%i", id);
    std::string key = keyStr->getCString();
    SongInfoObject* result = (SongInfoObject*)m_songObjects->objectForKey(key);
    if (result || id <= 10000000)
        return result;
    return getMusicObject(id);
}

int MusicDownloadManager::getSongPriority()
{
    int returnVal = m_songPriority + 1;
    m_songPriority = returnVal;
    return returnVal;
}

void MusicDownloadManager::handleIt(char success, std::string response, std::string tag, GJHttpType httpType)
{
    if (!success)
        response = "-1";
    if (httpType == GJHttpType::GetSongInfo)
    {
        onGetSongInfoCompleted(response, tag);
    }
    else if (httpType == GJHttpType::GetCustomContentURL)
    {
        onGetCustomContentURLCompleted(response, tag);
    }
}

void MusicDownloadManager::handleItND(cocos2d::CCNode* node, void* data)
{
    GJHttpResult* result = (GJHttpResult*)data;
    handleIt(result->isSuccess() ? 1 : 0, result->getResponse(), result->getRequestTag(), result->getHttpType());
    cocos2d::CCObject::release((cocos2d::CCObject*)data);
}

void MusicDownloadManager::incrementPriorityForSong(int id)
{
    SongInfoObject* songObj = getSongInfoObject(id);
    if (songObj)
        songObj->m_priority = getSongPriority();
}

bool MusicDownloadManager::init()
{
    bool returnVal = cocos2d::CCNode::init();
    if (returnVal)
    {
        cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
        m_activeDownloads = dict;
        cocos2d::CCObject::retain(dict);

        cocos2d::CCArray* array = cocos2d::CCArray::create();
        m_musicDownloadDelegates = array;
        cocos2d::CCObject::retain(array);

        generateResourceAssetList();
    }
    return returnVal;
}

bool MusicDownloadManager::isDLActive(const char* tag)
{
    return getDLObject(tag) != nullptr;
}

bool MusicDownloadManager::isMusicLibraryLoaded()
{
    return m_musicObjects && m_musicObjects->count() != 0;
}

bool MusicDownloadManager::isResourceSFX(int id)
{
    return m_resourceSfxUnorderedSet.find(id) != m_resourceSfxUnorderedSet.end();
}

bool MusicDownloadManager::isResourceSong(int id)
{
    return m_resourceSongUnorderedSet.find(id) != m_resourceSongUnorderedSet.end();
}

bool MusicDownloadManager::isRunningActionForSongID(int id)
{
    const char* songInfoKey = getSongInfoKey(id);
    bool returnVal = isDLActive(songInfoKey);
    if (!returnVal)
    {
        const char* songDownloadKey = getSongDownloadKey(id);
        return isDLActive(songDownloadKey);
    }
    return returnVal;
}

bool MusicDownloadManager::isSFXDownloaded(int id)
{
    if (isResourceSFX(id))
        return true;
    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    std::string path = pathForSFX(id);
    return fileUtils->isFileExist(path);
}

bool MusicDownloadManager::isSFXLibraryLoaded()
{
    return m_sfxObjects && m_sfxObjects->count() != 0;
}

bool MusicDownloadManager::isSongDownloaded(int id)
{
    if (isResourceSong(id))
        return true;
    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    std::string path = pathForSong(id);
    return fileUtils->isFileExist(path);
}

void MusicDownloadManager::limitDownloadedSongs()
{
    int menuSongID = GameManager::sharedState()->m_customMenuSongID;
    int practiceSongID = GameManager::sharedState()->m_customPracticeSongID;
    cocos2d::CCArray* downloaded = getDownloadedSongs();
    while (downloaded->count() > 0x32)
    {
        cocos2d::CCObject* obj = downloaded->lastObject();
        SongInfoObject* song = (SongInfoObject*)obj;
        if (menuSongID != song->m_songID && practiceSongID != song->m_songID)
            deleteSong(song->m_songID);
        downloaded->removeLastObject();
    }
}

void MusicDownloadManager::loadSongInfoFailed(int id, GJSongError errorCode)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->loadSongInfoFailed(id, errorCode);
    }
}

void MusicDownloadManager::loadSongInfoFinished(SongInfoObject* object)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->loadSongInfoFinished(object);
    }
}

void MusicDownloadManager::musicActionFailed(GJMusicAction action)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->musicActionFailed(action);
    }
}

void MusicDownloadManager::musicActionFinished(GJMusicAction action)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->musicActionFinished(action);
    }
}

std::string MusicDownloadManager::nameForTagID(int id)
{
    auto it = m_musicTags.find(id);
    if (it != m_musicTags.end())
        return it->second;
    return "";
}

void MusicDownloadManager::onDownloadMusicLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
}

void MusicDownloadManager::onDownloadSFXCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
}

void MusicDownloadManager::onDownloadSFXLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
}

void MusicDownloadManager::onDownloadSongCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
    if (!response || !response->isSucceed())
    {
        downloadSongFailed(0, GJSongError::FailedToFetch);
        return;
    }
    std::string tag = response->getHttpRequest()->getTag();
    int songID = atoi(tag.c_str());
    removeDLFromActive(getSongDownloadKey(songID));
    if (response->getResponseCode() == 200)
        downloadSongFinished(songID);
    else
        downloadSongFailed(songID, GJSongError::FailedToFetch);
}

void MusicDownloadManager::onGetCustomContentURLCompleted(std::string response, std::string tag)
{
    removeDLFromActive("get_c_url");
    m_customContentURL = response;
    getSongInfo(0, false);
}

void MusicDownloadManager::onGetSongInfoCompleted(std::string response, std::string tag)
{
    int id = atoi(tag.c_str());
    const char* songInfoKey = getSongInfoKey(id);
    removeDLFromActive(songInfoKey);

    if (response == "-1")
    {
        loadSongInfoFailed(id, GJSongError::FailedToFetch);
        return;
    }
    if (response == "-2")
    {
        loadSongInfoFailed(id, GJSongError::NotAllowed);
        return;
    }

    SongInfoObject* songObj = addSongObjectFromString(response);
    if (songObj)
        loadSongInfoFinished(songObj);
    else
        loadSongInfoFailed(id, GJSongError::FailedToFetch);
}

void MusicDownloadManager::onProcessHttpRequestCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
    cocos2d::extension::CCHttpRequest* request = response->getHttpRequest();
    std::string tag = request->getTag();
    GJHttpType httpType = request->getRequestTypeGJ();
    std::string responseStr = GameToolbox::getResponse(response);
    handleIt(true, responseStr, tag, httpType);
}

void MusicDownloadManager::onTryUpdateMusicLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
}

void MusicDownloadManager::onTryUpdateSFXLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response)
{
}

void MusicDownloadManager::parseMusicLibrary()
{
    std::string folderPath = pathForSongFolder(0);
    std::string filePath = folderPath + "musiclibrary.dat";

    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    unsigned long fileSize = 0;
    unsigned char* fileData = fileUtils->getFileData(filePath.c_str(), "r", &fileSize);

    if (!fileData)
        return;

    unsigned char* decompressed = cocos2d::ZipUtils::decompressString2(fileData, false, fileSize, 11);
    delete[] fileData;

    if (!decompressed)
        return;

    std::string data((char*)decompressed);
    delete[] decompressed;

    if (data.empty())
        return;

    std::vector<std::string> sections;
    size_t start = 0;
    size_t end = data.find("|");
    while (end != std::string::npos)
    {
        sections.push_back(data.substr(start, end - start));
        start = end + 1;
        end = data.find("|", start);
    }
    sections.push_back(data.substr(start));

    if (sections.size() < 4)
        return;

    if (m_musicObjects)
        m_musicObjects->release();
    if (m_musicArtists)
        m_musicArtists->release();

    m_musicObjects = cocos2d::CCDictionary::create();
    cocos2d::CCObject::retain(m_musicObjects);
    m_musicArtists = cocos2d::CCDictionary::create();
    cocos2d::CCObject::retain(m_musicArtists);

    m_musicTags.clear();
    m_musicObjectCount = 0;
    m_ncsSongCount = 0;

    m_musicLibraryVersion = atoi(sections[0].c_str());

    std::vector<std::string> artistTokens;
    start = 0;
    end = sections[1].find(";");
    while (end != std::string::npos)
    {
        artistTokens.push_back(sections[1].substr(start, end - start));
        start = end + 1;
        end = sections[1].find(";", start);
    }
    artistTokens.push_back(sections[1].substr(start));

    for (size_t i = 0; i < artistTokens.size(); i++)
    {
        std::vector<std::string> fields;
        start = 0;
        end = artistTokens[i].find(",");
        while (end != std::string::npos)
        {
            fields.push_back(artistTokens[i].substr(start, end - start));
            start = end + 1;
            end = artistTokens[i].find(",", start);
        }
        fields.push_back(artistTokens[i].substr(start));

        if (fields.size() >= 4)
        {
            int artistID = atoi(fields[0].c_str());
            std::string artistName = fields[1];
            std::string artistURL = fields[2];
            std::string artistYouTube = fields[3];

            MusicArtistObject* artist = MusicArtistObject::create(artistID, artistName, artistURL, artistYouTube);
            m_musicArtists->setObject(artist, std::to_string(artistID));
        }
    }

    std::vector<std::string> songTokens;
    start = 0;
    end = sections[2].find(";");
    while (end != std::string::npos)
    {
        songTokens.push_back(sections[2].substr(start, end - start));
        start = end + 1;
        end = sections[2].find(";", start);
    }
    songTokens.push_back(sections[2].substr(start));

    for (size_t i = 0; i < songTokens.size(); i++)
    {
        std::vector<std::string> fields;
        start = 0;
        end = songTokens[i].find(",");
        while (end != std::string::npos)
        {
            fields.push_back(songTokens[i].substr(start, end - start));
            start = end + 1;
            end = songTokens[i].find(",", start);
        }
        fields.push_back(songTokens[i].substr(start));

        if (fields.size() >= 6)
        {
            int songID = atoi(fields[0].c_str());
            std::string songName = fields[1];
            int artistID = atoi(fields[2].c_str());
            float fileSize = (float)atoi(fields[3].c_str()) * 0.00000095367f;
            int duration = atoi(fields[4].c_str());
            std::string songUrl = fields[5];

            int nongType = 0;
            bool isNew = false;
            int libraryOrder = 0;
            int priority = 0;
            std::string youtubeVideo;
            std::string youtubeChannel;

            if (fields.size() > 6)
                nongType = atoi(fields[6].c_str());
            if (fields.size() > 7)
                isNew = atoi(fields[7].c_str()) != 0;
            if (fields.size() > 8)
                libraryOrder = atoi(fields[8].c_str());
            if (fields.size() > 9)
                priority = atoi(fields[9].c_str());
            if (fields.size() > 10)
                youtubeVideo = fields[10];
            if (fields.size() > 11)
                youtubeChannel = fields[11];

            cocos2d::CCObject* artistObj = m_musicArtists->objectForKey(std::to_string(artistID));
            std::string artistName = "NA";
            if (artistObj)
            {
                MusicArtistObject* artist = (MusicArtistObject*)artistObj;
                artistName = artist->m_artistName;
            }

            SongInfoObject* songObj = SongInfoObject::create(
                songID, songName, artistName, artistID, songUrl,
                fileSize, youtubeVideo, youtubeChannel, libraryOrder,
                nongType, duration, isNew, priority, -1);

            songObj->m_duration = duration;
            songObj->m_verified = true;
            songObj->m_songOrder = priority;

            std::string tagsStr = fields.size() > 5 ? fields[5] : "";
            songObj->addTags(tagsStr);

            storeMusicObject(songObj);
            m_musicObjectCount++;
            if (nongType == 1)
                m_ncsSongCount++;
        }
    }

    std::vector<std::string> tagTokens;
    start = 0;
    end = sections[3].find(";");
    while (end != std::string::npos)
    {
        tagTokens.push_back(sections[3].substr(start, end - start));
        start = end + 1;
        end = sections[3].find(";", start);
    }
    tagTokens.push_back(sections[3].substr(start));

    for (size_t i = 0; i < tagTokens.size(); i++)
    {
        std::vector<std::string> fields;
        start = 0;
        end = tagTokens[i].find(",");
        while (end != std::string::npos)
        {
            fields.push_back(tagTokens[i].substr(start, end - start));
            start = end + 1;
            end = tagTokens[i].find(",", start);
        }
        fields.push_back(tagTokens[i].substr(start));

        if (fields.size() >= 2)
        {
            int tagID = atoi(fields[0].c_str());
            std::string tagName = fields[1];
            m_musicTags[tagID] = tagName;
        }
    }
}

void MusicDownloadManager::parseSFXLibrary()
{
    std::string folderPath = pathForSFXFolder(0);
    std::string filePath = folderPath + "sfxlibrary.dat";

    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    unsigned long fileSize = 0;
    unsigned char* fileData = fileUtils->getFileData(filePath.c_str(), "r", &fileSize);

    if (!fileData)
        return;

    unsigned char* decompressed = cocos2d::ZipUtils::decompressString2(fileData, false, fileSize, 11);
    delete[] fileData;

    if (!decompressed)
        return;

    std::string data((char*)decompressed);
    delete[] decompressed;

    if (data.empty())
        return;

    std::vector<std::string> sections;
    size_t start = 0;
    size_t end = data.find("|");
    while (end != std::string::npos)
    {
        sections.push_back(data.substr(start, end - start));
        start = end + 1;
        end = data.find("|", start);
    }
    sections.push_back(data.substr(start));

    if (sections.size() < 2)
        return;

    if (m_sfxObjects)
        m_sfxObjects->release();
    if (m_sfxArtists)
        m_sfxArtists->release();

    m_sfxObjects = cocos2d::CCDictionary::create();
    cocos2d::CCObject::retain(m_sfxObjects);
    m_sfxArtists = cocos2d::CCArray::create();
    cocos2d::CCObject::retain(m_sfxArtists);

    m_sfxObjectCount = 0;

    std::vector<std::string> sfxTokens;
    start = 0;
    end = sections[0].find(";");
    while (end != std::string::npos)
    {
        sfxTokens.push_back(sections[0].substr(start, end - start));
        start = end + 1;
        end = sections[0].find(";", start);
    }
    sfxTokens.push_back(sections[0].substr(start));

    for (size_t i = 0; i < sfxTokens.size(); i++)
    {
        std::vector<std::string> fields;
        start = 0;
        end = sfxTokens[i].find(",");
        while (end != std::string::npos)
        {
            fields.push_back(sfxTokens[i].substr(start, end - start));
            start = end + 1;
            end = sfxTokens[i].find(",", start);
        }
        fields.push_back(sfxTokens[i].substr(start));

        if (fields.size() >= 6)
        {
            int sfxID = atoi(fields[0].c_str());
            std::string sfxName = fields[1];
            int folderID = atoi(fields[2].c_str());
            int size = atoi(fields[3].c_str());
            int duration = atoi(fields[4].c_str());
            int nongType = atoi(fields[5].c_str());

            if (sfxID == 1)
            {
                m_sfxLibraryVersion = atoi(sfxName.c_str());
                sfxName = "Root";
            }

            if (nongType)
            {
                SFXFolderObject* folderObj = SFXFolderObject::create(sfxID, sfxName, folderID);
                storeSFXInfoObject(folderObj);
            }
            else
            {
                SFXInfoObject* sfxObj = SFXInfoObject::create(sfxID, sfxName, folderID, size, duration);
                storeSFXInfoObject(sfxObj);
                m_sfxObjectCount++;
            }
        }
    }

    std::vector<std::string> artistTokens;
    start = 0;
    end = sections[1].find(";");
    while (end != std::string::npos)
    {
        artistTokens.push_back(sections[1].substr(start, end - start));
        start = end + 1;
        end = sections[1].find(";", start);
    }
    artistTokens.push_back(sections[1].substr(start));

    for (size_t i = 0; i < artistTokens.size(); i++)
    {
        std::vector<std::string> fields;
        start = 0;
        end = artistTokens[i].find(",");
        while (end != std::string::npos)
        {
            fields.push_back(artistTokens[i].substr(start, end - start));
            start = end + 1;
            end = artistTokens[i].find(",", start);
        }
        fields.push_back(artistTokens[i].substr(start));

        if (fields.size() >= 2)
        {
            std::string artistName = fields[0];
            std::string artistUrl = fields[1];
            CCURLObject* urlObj = CCURLObject::create(artistName, artistUrl);
            m_sfxArtists->addObject(urlObj);
        }
    }
}

std::string MusicDownloadManager::pathForSFX(int id)
{
    std::string result = pathForSFXFolder(id);
    result += "s";
    result += GameToolbox::intToString(id);
    result += ".ogg";
    return result;
}

std::string MusicDownloadManager::pathForSFXFolder(int id)
{
    std::string result = "";
    if (isResourceSFX(id))
    {
        result = "sfx/";
    }
    else
    {
        GameManager* gm = GameManager::sharedState();
        if (gm->getGameVariable("0033"))
        {
            cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
            std::string absolutePath = fileUtils->getWritablePath();
            result = absolutePath;
            result += "Resources\\";
        }
        else
        {
            cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
            std::string writablePath = fileUtils->getWritablePath();
            result = writablePath;
        }
    }
    return result;
}

std::string MusicDownloadManager::pathForSong(int id)
{
    std::string result = pathForSongFolder(id);
    result += GameToolbox::intToString(id);
    if (id <= 10000000)
        result += ".mp3";
    else
        result += ".ogg";
    return result;
}

std::string MusicDownloadManager::pathForSongFolder(int id)
{
    std::string result = "";
    if (isResourceSong(id))
    {
        result = "songs/";
    }
    else
    {
        GameManager* gm = GameManager::sharedState();
        if (gm->getGameVariable("0033"))
        {
            cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
            std::string absolutePath = fileUtils->getWritablePath();
            result = absolutePath;
            result += "Resources\\";
        }
        else
        {
            cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
            std::string writablePath = fileUtils->getWritablePath();
            result = writablePath;
        }
    }
    return result;
}

void MusicDownloadManager::removeDLFromActive(const char* tag)
{
    std::string key = tag;
    m_activeDownloads->removeObjectForKey(key);
}

void MusicDownloadManager::removeMusicDownloadDelegate(MusicDownloadDelegate* delegate)
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        if (handler->m_delegate == delegate)
        {
            m_musicDownloadDelegates->removeObjectAtIndex(i);
            return;
        }
    }
}

MusicDownloadManager* MusicDownloadManager::sharedState()
{
    static MusicDownloadManager* instance = nullptr;
    if (!instance)
    {
        instance = new MusicDownloadManager();
        instance->init();
    }
    return instance;
}

void MusicDownloadManager::showTOS(FLAlertLayerProtocol* delegate)
{
    std::string tosText = "Important notification - Terms of use. RobTop Games provides a library of music and sound effects for creative use within Geometry Dash. You are free to use these assets for creating and sharing levels within the game. However, redistributing, selling, manipulating, or using these assets outside the game is prohibited. If you share game-related content on social media or upload videos featuring custom music, please give credit to the respective artist. Regarding music from Newgrounds.com: Some custom songs are available via a third party website, Newgrounds.com. These songs, created by artists using Newgrounds.com, are under Creative Commons licensing terms accessible on Newgrounds.com. By downloading a song from Newgrounds, you accept these terms and any others designated by Newgrounds. RobTop Games is not the provider of songs from Newgrounds.com, nor a party to the licensing agreements, and does not store any audio files downloaded from Newgrounds.com. You are responsible for the content you create using all music and sound effects provided. Ensure your creations adhere to the game's community guidelines and do not infringe on any rights. The availability of music and sound effects in Geometry Dash is subject to change. RobTop Games may add or remove content from both the custom library and the Newgrounds.com selection without prior notice. We do not guarantee the continuous availability or suitability of any music or sound effects in the game. We may update these terms at any time, with your continued use of Geometry Dash signifying acceptance of any changes. By clicking the 'I Accept' button, you acknowledge and approve of the above-stated terms of use.";

    FLAlertLayer* alert = FLAlertLayer::create(delegate, "Terms of Use", tosText, "Cancel", "I Accept", true, 400.0f, false, 280.0f, 1.0f);
    alert->setTag(1);
    alert->show();
}

void MusicDownloadManager::songStateChanged()
{
    for (unsigned int i = 0; i < m_musicDownloadDelegates->count(); i++)
    {
        MusicDelegateHandler* handler = (MusicDelegateHandler*)m_musicDownloadDelegates->objectAtIndex(i);
        handler->m_delegate->songStateChanged();
    }
}

void MusicDownloadManager::stopDownload(int id)
{
    const char* downloadKey = getSongDownloadKey(id);
    cocos2d::CCObject* obj = getDLObject(downloadKey);
    if (obj)
    {
        cocos2d::extension::CCHttpRequest* request = (cocos2d::extension::CCHttpRequest*)obj;
        request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    }
    removeDLFromActive(downloadKey);
}

void MusicDownloadManager::storeMusicObject(SongInfoObject* object)
{
    m_musicObjects->setObject(object, std::to_string(object->m_songID));
}

void MusicDownloadManager::storeSFXInfoObject(SFXInfoObject* object)
{
    m_sfxObjects->setObject(object, std::to_string((int)(uintptr_t)object->m_sfxID));
    int folderID = (int)(uintptr_t)object->m_folderID;
    if (folderID > 0)
    {
        SFXFolderObject* folder = (SFXFolderObject*)m_sfxObjects->objectForKey(std::to_string(folderID));
        if (folder)
        {
            bool isFolder = (bool)(uintptr_t)folder->m_folder;
            if (isFolder)
            {
                cocos2d::CCArray* sfxList = (cocos2d::CCArray*)folder->m_sfxObjects;
                sfxList->addObject(object);
            }
        }
    }
}

void MusicDownloadManager::tryLoadLibraries()
{
    if (!m_triedToLoadLibraries)
    {
        m_triedToLoadLibraries = true;
        if (!isSFXLibraryLoaded())
            parseSFXLibrary();
        if (!isMusicLibraryLoaded())
            parseMusicLibrary();
    }
}

void MusicDownloadManager::tryUpdateMusicLibrary()
{
    if (m_customContentURL.empty() || isDLActive("v_music_l"))
        return;
    m_downloadingMusicLibrary = true;
    std::string path = "/music/musiclibrary_version_02.txt";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onTryUpdateMusicLibraryCompleted, 0, 0);
    addDLToActive("v_music_l", request);
}

void MusicDownloadManager::tryUpdateSFXLibrary()
{
    if (m_customContentURL.empty() || isDLActive("v_sfx_l"))
        return;
    m_requestedSfxLibraryDownloaded = true;
    std::string path = "/sfx/sfxlibrary_version.txt";
    std::string url = generateCustomContentURL(path);
    cocos2d::extension::CCHttpRequest* request = ProcessHttpGetRequest(url, "", MusicDownloadManager::onTryUpdateSFXLibraryCompleted, 0, 0);
    addDLToActive("v_sfx_l", request);
}

MusicDownloadManager::~MusicDownloadManager()
{
}