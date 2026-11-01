#pragma once

#include "cocos2d.h"
#include <string>

class DS_Dictionary;
class GJAccountRegisterDelegate;
class GJAccountLoginDelegate;
class GJAccountBackupDelegate;
class GJAccountSyncDelegate;
class GJAccountSettingsDelegate;

class GJAccountManager : public cocos2d::CCNode
{
public:
    static GJAccountManager* sharedState();
    
    bool init();
    
    void addDLToActive(const char* tag);
    bool backupAccount(std::string url);
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void firstSetup();
    bool getAccountBackupURL();
    bool getAccountSyncURL();
    cocos2d::CCObject* getDLObject(const char* tag);
    std::string getShaPassword(std::string password);
    void handleItND(cocos2d::CCNode* node, void* data);
    bool isDLActive(const char* tag);
    void loginAccount(std::string username, std::string gjp2);
    void onBackupAccountCompleted(std::string response, std::string tag);
    void onGetAccountBackupURLCompleted(std::string response, std::string tag);
    void onGetAccountSyncURLCompleted(std::string response, std::string tag);
    void onLoginAccountCompleted(std::string response, std::string tag);
    void onRegisterAccountCompleted(std::string response, std::string tag);
    void onSyncAccountCompleted(std::string response, std::string tag);
    void onUpdateAccountSettingsCompleted(std::string response, std::string tag);
    void registerAccount(std::string response, std::string tag, std::string data);
    void removeDLFromActive(const char* tag);
    bool syncAccount(std::string url);
    void unlinkFromAccount();
    
    ~GJAccountManager();

public:
    GJAccountRegisterDelegate* m_accountRegisterDelegate;
    int m_accountID;
    int m_unkInt1;
    std::string m_username;
    std::string m_GJP2;
    std::string m_password;
    int m_unkInt2;
    GJAccountLoginDelegate* m_accountLoginDelegate;
    GJAccountDelegate* m_accountDelegate;
    GJAccountBackupDelegate* m_backupDelegate;
    GJAccountSyncDelegate* m_syncDelegate;
    GJAccountSettingsDelegate* m_accountSettingsDelegate;
    int m_gameManagerSize;
    int m_localLevelsSize;
    cocos2d::CCDictionary* m_activeDownloads;
};