#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include <string>
#include <unordered_set>
#include <map>

class DS_Dictionary;
class MusicDownloadDelegate;
class MusicArtistObject;
class SongInfoObject;
class SFXInfoObject;
class SFXFolderObject;
class OptionsObjectDelegate;
class FLAlertLayerProtocol;
class MusicDelegateHandler;

class MusicDownloadManager : public cocos2d::CCNode {
public:
    static MusicDownloadManager* sharedState();

    bool init() override;

    void addDLToActive(const char* tag);
    void addDLToActive(const char* tag, cocos2d::CCObject* obj);
    void addMusicDownloadDelegate(MusicDownloadDelegate* delegate);
    SongInfoObject* addSongObjectFromString(std::string str);
    void clearSong(int songID);
    void clearUnusedSongs();
    cocos2d::CCDictionary* createArtistsInfo(std::string str);
    void createSongsInfo(std::string songsStr, std::string artistsStr);
    void dataLoaded(DS_Dictionary* dict);
    void deleteSFX(int id);
    void deleteSong(int id);
    void downloadCustomSong(int id);
    void downloadMusicLibrary();
    void downloadSFX(int id);
    void downloadSFXFailed(int id, GJSongError errorCode);
    void downloadSFXFinished(int id);
    void downloadSFXLibrary();
    void downloadSong(int id);
    void downloadSongFailed(int id, GJSongError errorCode);
    void downloadSongFinished(int id);
    void downloadSongStarted(int id);
    void encodeDataTo(DS_Dictionary* dict);
    cocos2d::CCArray* filterMusicByTag(int id, cocos2d::CCArray* songs);
    cocos2d::CCArray* filterMusicByArtistID(int artistID, cocos2d::CCArray* songs);
    void firstSetup();
    std::string generateCustomContentURL(std::string path);
    void generateResourceAssetList();
    cocos2d::CCArray* getAllMusicArtists(OptionsObjectDelegate* delegate);
    cocos2d::CCArray* getAllMusicObjects(GJSongType type);
    cocos2d::CCArray* getAllMusicTags(OptionsObjectDelegate* delegate);
    cocos2d::CCArray* getAllSFXObjects(bool onlySFX);
    cocos2d::CCArray* getAllSongs();
    void getCustomContentURL();
    cocos2d::CCObject* getDLObject(const char* id);
    int getDownloadProgress(int id);
    cocos2d::CCArray* getDownloadedSongs();
    MusicArtistObject* getMusicArtistForID(int id);
    SongInfoObject* getMusicObject(int id);
    const char* getSFXDownloadKey(int id);
    int getSFXDownloadProgress(int id);
    SFXFolderObject* getSFXFolderObjectForID(int id);
    std::string getSFXFolderPathForID(int id, bool nextFolder);
    SFXInfoObject* getSFXObject(int id);
    const char* getSongDownloadKey(int id);
    void getSongInfo(int id, bool download);
    const char* getSongInfoKey(int id);
    SongInfoObject* getSongInfoObject(int id);
    int getSongPriority();
    void handleIt(char success, std::string response, std::string tag, GJHttpType httpType);
    void handleItND(cocos2d::CCNode* node, void* data);
    void incrementPriorityForSong(int id);
    bool isDLActive(const char* tag);
    bool isMusicLibraryLoaded();
    bool isResourceSFX(int id);
    bool isResourceSong(int id);
    bool isRunningActionForSongID(int id);
    bool isSFXDownloaded(int id);
    bool isSFXLibraryLoaded();
    bool isSongDownloaded(int id);
    void limitDownloadedSongs();
    void loadSongInfoFailed(int id, GJSongError errorCode);
    void loadSongInfoFinished(SongInfoObject* object);
    void musicActionFailed(GJMusicAction action);
    void musicActionFinished(GJMusicAction action);
    std::string nameForTagID(int id);
    void onDownloadMusicLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onDownloadSFXCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onDownloadSFXLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onDownloadSongCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onGetCustomContentURLCompleted(std::string response, std::string tag);
    void onGetSongInfoCompleted(std::string response, std::string tag);
    void onProcessHttpRequestCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onTryUpdateMusicLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void onTryUpdateSFXLibraryCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void parseMusicLibrary();
    void parseSFXLibrary();
    std::string pathForSFX(int id);
    std::string pathForSFXFolder(int id);
    std::string pathForSong(int id);
    std::string pathForSongFolder(int id);
    void removeDLFromActive(const char* tag);
    void removeMusicDownloadDelegate(MusicDownloadDelegate* delegate);
    void showTOS(FLAlertLayerProtocol* delegate);
    void songStateChanged();
    void stopDownload(int id);
    void storeMusicObject(SongInfoObject* object);
    void storeSFXInfoObject(SFXInfoObject* object);
    void tryLoadLibraries();
    void tryUpdateMusicLibrary();
    void tryUpdateSFXLibrary();

    ~MusicDownloadManager();

protected:
    cocos2d::CCArray* m_musicDownloadDelegates = nullptr;
    cocos2d::CCDictionary* m_songObjects = nullptr;
    cocos2d::CCDictionary* m_activeDownloads = nullptr;
    int m_songPriority = 0;
    cocos2d::CCDictionary* m_MDLM003 = nullptr;
    cocos2d::CCDictionary* m_musicObjects = nullptr;
    cocos2d::CCDictionary* m_musicArtists = nullptr;
    std::map<int, std::string> m_musicTags;
    cocos2d::CCDictionary* m_sfxObjects = nullptr;
    std::string m_customContentURL;
    bool m_downloadingMusicLibrary = false;
    bool m_requestedSfxLibraryDownloaded = false;
    std::unordered_set<int> m_resourceSfxUnorderedSet;
    std::unordered_set<int> m_resourceSongUnorderedSet;
    cocos2d::CCArray* m_sfxArtists = nullptr;
    int m_sfxObjectCount = 0;
    int m_sfxLibraryVersion = 0;
    int m_musicObjectCount = 0;
    int m_ncsSongCount = 0;
    int m_musicLibraryVersion = 0;
    bool m_triedToLoadLibraries = false;
};