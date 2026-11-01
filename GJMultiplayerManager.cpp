#include "GJMultiplayerManager.h"
#include "GJMPDelegate.h"
#include "GJComment.h"
#include "GJUserScore.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "LevelTools.h"
#include "GameToolbox.h"
#include "UploadActionDelegate.h"

GJMultiplayerManager* GJMultiplayerManager::sharedState() {
    static GJMultiplayerManager* instance = nullptr;
    if (!instance) {
        instance = new GJMultiplayerManager();
        instance->init();
    }
    return instance;
}

bool GJMultiplayerManager::init() {
    if (!CCNode::init()) {
        return false;
    }
    
    m_activeDownloads = cocos2d::CCDictionary::create();
    m_activeDownloads->retain();
    
    m_lobbyScores = cocos2d::CCDictionary::create();
    m_lobbyScores->retain();
    
    m_scoreArrays = cocos2d::CCDictionary::create();
    m_scoreArrays->retain();
    
    m_lobbyComments = cocos2d::CCDictionary::create();
    m_lobbyComments->retain();
    
    return true;
}

void GJMultiplayerManager::addComment(const std::string& str, int lobbyID) {
    auto array = static_cast<cocos2d::CCArray*>(m_lobbyComments->objectForKey(lobbyID));
    if (!array) {
        array = cocos2d::CCArray::create();
        m_lobbyComments->setObject(array, lobbyID);
    }
    
    auto dict = GameLevelManager::responseToDict(str);
    auto comment = GJComment::create();
    
    auto account = GJAccountManager::sharedState();
    if (comment->m_accountID == account->m_accountID - account->m_unkInt1) {
        if (comment->m_commentID != -1) {
            for (int i = 0; i < array->count(); ++i) {
                auto obj = array->objectAtIndex(i);
                if (obj->m_accountID == account->m_accountID - account->m_unkInt1 && obj->m_commentID == -1) {
                    obj->m_commentID = comment->m_commentID;
                    return;
                }
            }
        }
    }
    
    comment->m_unkMultiplayerBool = true;
    
    auto score = static_cast<GJUserScore*>(m_lobbyScores->objectForKey(comment->m_accountID));
    if (score) {
        comment->m_userName = score->m_userName;
    }
    
    array->addObject(comment);
}

void GJMultiplayerManager::addDLToActive(const char* tag) {
    auto node = cocos2d::CCNode::create();
    m_activeDownloads->setObject(node, tag);
}

void GJMultiplayerManager::createAndAddComment(const std::string& text, int lobbyID) {
    auto account = GJAccountManager::sharedState();
    auto str = cocos2d::CCString::createWithFormat(
        "2~%s~8~%i~9~7 minutes~6~-1",
        text.c_str(),
        account->m_accountID - account->m_unkInt1
    );
    
    addComment(str->getCString(), lobbyID);
    
    if (m_mpDelegate) {
        m_mpDelegate->updateComments();
    }
}

void GJMultiplayerManager::dataLoaded(DS_Dictionary* dict) {
}

void GJMultiplayerManager::encodeDataTo(DS_Dictionary* dict) {
}

bool GJMultiplayerManager::exitLobby(int id) {
    auto tag = cocos2d::CCString::createWithFormat("%i_exitLobby", id)->getCString();
    
    if (isDLActive(tag)) {
        return false;
    }
    
    addDLToActive(tag);
    
    std::string base = getBasePostString();
    std::string secret = "mfv3899";
    std::string params = "&gameID=" + std::to_string(id) + "&secret=" + secret;
    std::string url = "https://www.boomlings.com/database/exitLobby.php";
    
    ProcessHttpRequest(url, base + params, tag, HttpType::ExitLobby);
    
    return true;
}

void GJMultiplayerManager::firstSetup() {
}

std::string GJMultiplayerManager::getBasePostString() {
    auto manager = GameLevelManager::sharedState();
    std::string base = manager->getBasePostString();
    auto account = GJAccountManager::sharedState();
    base += "&username=" + account->m_username;
    return base;
}

cocos2d::CCObject* GJMultiplayerManager::getDLObject(const char* tag) {
    return m_activeDownloads->objectForKey(tag);
}

int GJMultiplayerManager::getLastCommentIDForGame(int id) {
    auto array = static_cast<cocos2d::CCArray*>(m_lobbyComments->objectForKey(id));
    if (!array || array->count() == 0) {
        return 0;
    }
    
    int maxId = 0;
    for (int i = 0; i < array->count(); ++i) {
        auto comment = array->objectAtIndex(i);
        int commentId = comment->m_commentID;
        if (maxId < commentId) {
            maxId = commentId;
        }
    }
    return maxId;
}

void GJMultiplayerManager::handleItND(cocos2d::CCNode* node, void* data) {
    auto result = static_cast<GJHttpResult*>(data);
    
    bool success = result->isSuccess();
    std::string response = result->getResponse();
    std::string tag = result->getRequestTag();
    GJHttpType type = result->getHttpType();
    
    handleIt(success, response, tag, type);
    
    result->release();
}

bool GJMultiplayerManager::isDLActive(const char* tag) {
    return getDLObject(tag) != nullptr;
}

bool GJMultiplayerManager::joinLobby(int id) {
    auto tag = cocos2d::CCString::createWithFormat("%i_joinLobby", id)->getCString();
    
    if (isDLActive(tag)) {
        return false;
    }
    
    addDLToActive(tag);
    
    std::string base = getBasePostString();
    int lastCommentId = getLastCommentIDForGame(id);
    std::string secret = "mfv3899";
    std::string params = "&gameID=" + std::to_string(id) + 
                        "&lastCommentID=" + std::to_string(lastCommentId) + 
                        "&secret=" + secret;
    std::string url = "https://www.boomlings.com/database/joinLobby.php";
    
    ProcessHttpRequest(url, base + params, tag, HttpType::JoinLobby);
    
    return true;
}

void GJMultiplayerManager::onExitLobbyCompleted(const std::string& response, const std::string& tag) {
    removeDLFromActive(tag.c_str());
    int lobbyId = std::stoi(tag);
    
    if (response != "-1") {
        if (m_uploadDelegate) {
            m_uploadDelegate->uploadActionFinished(lobbyId, 58);
        }
    } else {
        if (m_uploadDelegate) {
            m_uploadDelegate->uploadActionFailed(lobbyId, 58);
        }
    }
}

void GJMultiplayerManager::onJoinLobbyCompleted(const std::string& response, const std::string& tag) {
    removeDLFromActive(tag.c_str());
    int lobbyId = std::stoi(tag);
    
    if (response == "-1") {
        if (m_mpDelegate) {
            m_mpDelegate->joinLobbyFailed(lobbyId, GJMPErrorCode::Failed);
        }
        return;
    }
    
    auto parts = splitString(response, "#");
    if (parts.size() <= 2) {
        if (m_mpDelegate) {
            m_mpDelegate->joinLobbyFailed(lobbyId, GJMPErrorCode::Failed);
        }
        return;
    }
    
    int gameId = std::stoi(parts[1]);
    auto scoreArray = cocos2d::CCArray::create();
    
    auto scoresData = splitString(parts[2], ";");
    for (const auto& scoreStr : scoresData) {
        auto scoreParts = splitString(scoreStr, ",");
        if (scoreParts.size() > 2) {
            int accountId = std::stoi(scoreParts[0]);
            std::string username = scoreParts[1];
            int scoreValue = std::stoi(scoreParts[2]);
            
            auto userScore = GJUserScore::create();
            userScore->m_accountID = accountId;
            userScore->m_userName = username;
            
            auto account = GJAccountManager::sharedState();
            if (accountId == account->m_accountID - account->m_unkInt1) {
                // Current user
            }
            
            m_lobbyScores->setObject(userScore, accountId);
            scoreArray->addObject(userScore);
        }
    }
    
    m_scoreArrays->setObject(scoreArray, gameId);
    
    if (parts.size() > 3) {
        auto comments = splitString(parts[3], "|");
        for (const auto& commentStr : comments) {
            addComment(commentStr, gameId);
        }
    }
    
    if (m_mpDelegate) {
        m_mpDelegate->joinLobbyFinished(gameId);
    }
}

void GJMultiplayerManager::onUploadCommentCompleted(const std::string& response, const std::string& tag) {
    removeDLFromActive(tag.c_str());
    int lobbyId = std::stoi(tag);
    
    if (response != "-1") {
        if (m_mpDelegate) {
            m_mpDelegate->didUploadMPComment(lobbyId);
        }
    }
}

void GJMultiplayerManager::removeDLFromActive(const char* tag) {
    m_activeDownloads->removeObjectForKey(tag);
}

bool GJMultiplayerManager::uploadComment(const std::string& text, int lobbyID) {
    auto tag = cocos2d::CCString::createWithFormat("%i_comment", lobbyID)->getCString();
    
    std::string encoded = LevelTools::base64EncodeString(text);
    auto manager = GameLevelManager::sharedState();
    std::string cleaned = manager->removeDelimiterChars(encoded);
    
    std::string secret = "mfv3899";
    std::string params = "&comment=" + cleaned + 
                        "&gameID=" + std::to_string(lobbyID) + 
                        "&extra=" + "" +
                        "&secret=" + secret;
    
    // Generate checksum
    auto account = GJAccountManager::sharedState();
    std::string checkData = std::to_string(account->m_accountID - account->m_unkInt1) + 
                           cleaned + 
                           std::to_string(lobbyID) + 
                           "";
    auto checksum = calculateSHA1(checkData);
    auto encodedChecksum = cocos2d::ZipUtils::base64Encode(checksum, "52832");
    
    params += "&chk=" + encodedChecksum;
    
    std::string url = "https://www.boomlings.com/database/uploadComment.php";
    std::string base = getBasePostString();
    
    ProcessHttpRequest(url, base + params, tag, HttpType::UploadComment);
    
    createAndAddComment(text, lobbyID);
    
    return true;
}

GJMultiplayerManager::~GJMultiplayerManager() {
    if (m_activeDownloads) {
        m_activeDownloads->release();
    }
    if (m_lobbyScores) {
        m_lobbyScores->release();
    }
    if (m_scoreArrays) {
        m_scoreArrays->release();
    }
    if (m_lobbyComments) {
        m_lobbyComments->release();
    }
}