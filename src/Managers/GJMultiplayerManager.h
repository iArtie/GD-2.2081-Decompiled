#pragma once

#include "CCNode.h"
#include "Platform/cocos2d.h"
#include <string>

class GJMPDelegate;
class UploadActionDelegate;
class DS_Dictionary;

class GJMultiplayerManager : public cocos2d::CCNode {
public:
    void addComment(const std::string& str, int lobbyID);
    void addDLToActive(const char* tag);
    void createAndAddComment(const std::string& text, int lobbyID);
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    bool exitLobby(int id);
    void firstSetup();
    std::string getBasePostString();
    cocos2d::CCObject* getDLObject(const char* tag);
    int getLastCommentIDForGame(int id);
    void handleItND(cocos2d::CCNode* node, void* data);
    bool init() override;
    bool isDLActive(const char* tag);
    bool joinLobby(int id);
    void onExitLobbyCompleted(const std::string& response, const std::string& tag);
    void onJoinLobbyCompleted(const std::string& response, const std::string& tag);
    void onUploadCommentCompleted(const std::string& response, const std::string& tag);
    void removeDLFromActive(const char* tag);
    static GJMultiplayerManager* sharedState();
    bool uploadComment(const std::string& text, int lobbyID);
    virtual ~GJMultiplayerManager();

private:
    cocos2d::CCDictionary* m_activeDownloads;
    UploadActionDelegate* m_uploadDelegate;
    cocos2d::CCDictionary* m_lobbyComments;
    GJMPDelegate* m_mpDelegate;
    cocos2d::CCDictionary* m_lobbyScores;
    cocos2d::CCDictionary* m_scoreArrays;
};