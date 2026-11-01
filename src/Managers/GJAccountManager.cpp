#include "Managers/GJAccountManager.h"
#include "Utils/DS_Dictionary.h"
#include "Managers/GameManager.h"
#include "Managers/LocalLevelManager.h"
#include "GameLevelManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/AchievementManager.h"
#include "Managers/MusicDownloadManager.h"
#include "cocos2d/CCApplication.h"

GJAccountManager* GJAccountManager::sharedState()
{
    static GJAccountManager* instance = nullptr;
    if (!instance)
    {
        instance = new GJAccountManager();
        instance->init();
    }
    return instance;
}

bool GJAccountManager::init()
{
    if (!cocos2d::CCNode::init())
    {
        return false;
    }
    
    m_activeDownloads = cocos2d::CCDictionary::create();
    m_activeDownloads->retain();
    
    return true;
}

void GJAccountManager::addDLToActive(const char* tag)
{
    cocos2d::CCNode* node = cocos2d::CCNode::create();
    m_activeDownloads->setObject(node, tag);
}

bool GJAccountManager::isDLActive(const char* tag)
{
    return m_activeDownloads->objectForKey(tag) != nullptr;
}

cocos2d::CCObject* GJAccountManager::getDLObject(const char* tag)
{
    return m_activeDownloads->objectForKey(tag);
}

void GJAccountManager::removeDLFromActive(const char* tag)
{
    m_activeDownloads->removeObjectForKey(tag);
}

std::string GJAccountManager::getShaPassword(std::string password)
{
    std::string result = password + "I29fmAn120103Ts";
    // Implementación simplificada del SHA1
    return result;
}

void GJAccountManager::loginAccount(std::string username, std::string gjp2)
{
    if (isDLActive("login_account"))
    {
        return;
    }
    
    addDLToActive("login_account");
    
    GameManager* gameManager = GameManager::sharedState();
    std::string udid = gameManager->m_playerUDID;
    
    std::string request = "udid=" + udid + "&userName=" + username + "&gjp2=" + gjp2;
    // Procesar request HTTP
    // onLoginAccountCompleted se llamará cuando termine
}

void GJAccountManager::onLoginAccountCompleted(std::string response, std::string tag)
{
    removeDLFromActive("login_account");
    
    if (response == "-1")
    {
        if (m_accountLoginDelegate)
        {
            m_accountLoginDelegate->loginAccountFailed(AccountDisabled);
        }
        return;
    }
    
    // Procesar respuesta
    // Si éxito, llamar a loginAccountFinished
    if (m_accountLoginDelegate)
    {
        m_accountLoginDelegate->loginAccountFinished(1, 1);
    }
}

void GJAccountManager::unlinkFromAccount()
{
    m_GJP2 = "";
    m_password = "";
    m_username = "";
    m_accountID = 0;
    
    GameManager::sharedState()->m_shouldLoadUnlockValueKeeper = true;
    GameManager::sharedState()->firstLoad();
    GameManager::sharedState()->m_shouldLoadUnlockValueKeeper = false;
    
    LocalLevelManager::sharedState()->firstLoad();
    
    if (m_accountDelegate)
    {
        m_accountDelegate->accountStatusChanged();
    }
    
    GameManager::sharedState()->accountStatusChanged();
}

void GJAccountManager::dataLoaded(DS_Dictionary* dict)
{
    m_username = dict->getStringForKey("GJA_001");
    m_accountID = dict->getIntegerForKey("GJA_003");
    m_GJP2 = dict->getStringForKey("GJA_005");
}

void GJAccountManager::encodeDataTo(DS_Dictionary* dict)
{
    dict->setStringForKey("GJA_001", m_username);
    dict->setIntegerForKey("GJA_003", m_accountID);
    dict->setStringForKey("GJA_005", m_GJP2);
}

void GJAccountManager::firstSetup()
{
}

bool GJAccountManager::getAccountBackupURL()
{
    if (isDLActive("burl_account"))
    {
        return false;
    }
    
    addDLToActive("burl_account");
    // Realizar petición HTTP para obtener URL de backup
    return true;
}

bool GJAccountManager::getAccountSyncURL()
{
    if (isDLActive("surl_account"))
    {
        return false;
    }
    
    addDLToActive("surl_account");
    // Realizar petición HTTP para obtener URL de sync
    return true;
}

bool GJAccountManager::backupAccount(std::string url)
{
    if (isDLActive("bak_account"))
    {
        return false;
    }
    
    addDLToActive("bak_account");
    
    GameManager::sharedState()->m_quickSave = true;
    MusicDownloadManager::sharedState()->clearUnusedSongs();
    
    std::string gameSave = GameManager::sharedState()->getCompressedSaveString();
    m_gameManagerSize = gameSave.length();
    
    GameManager::sharedState()->m_quickSave = false;
    
    LocalLevelManager::sharedState()->updateLevelOrder();
    std::string levelSave = LocalLevelManager::sharedState()->getCompressedSaveString();
    m_localLevelsSize = levelSave.length();
    
    // Realizar petición HTTP
    return true;
}

void GJAccountManager::onBackupAccountCompleted(std::string response, std::string tag)
{
    removeDLFromActive("bak_account");
    
    if (response == "-1")
    {
        if (m_backupDelegate)
        {
            m_backupDelegate->backupAccountFailed(GenericError, 0);
        }
        return;
    }
    
    // Procesar respuesta
    if (m_backupDelegate)
    {
        m_backupDelegate->backupAccountFinished();
    }
}

bool GJAccountManager::syncAccount(std::string url)
{
    if (isDLActive("sync_account"))
    {
        return false;
    }
    
    addDLToActive("sync_account");
    
    // Realizar petición HTTP
    return true;
}

void GJAccountManager::onSyncAccountCompleted(std::string response, std::string tag)
{
    removeDLFromActive("sync_account");
    
    if (response == "-1")
    {
        if (m_syncDelegate)
        {
            m_syncDelegate->syncAccountFailed(GenericError, 0);
        }
        return;
    }
    
    // Procesar respuesta
    if (m_syncDelegate)
    {
        m_syncDelegate->syncAccountFinished();
    }
}

void GJAccountManager::registerAccount(std::string response, std::string tag, std::string data)
{
    if (isDLActive("reg_account"))
    {
        return;
    }
    
    addDLToActive("reg_account");
    // Realizar petición HTTP
}

void GJAccountManager::onRegisterAccountCompleted(std::string response, std::string tag)
{
    removeDLFromActive("reg_account");
    
    if (response == "1")
    {
        if (m_accountRegisterDelegate)
        {
            m_accountRegisterDelegate->registerAccountFinished();
        }
    }
    else
    {
        if (m_accountRegisterDelegate)
        {
            m_accountRegisterDelegate->registerAccountFailed((AccountError)atoi(response.c_str()));
        }
    }
}

void GJAccountManager::onGetAccountBackupURLCompleted(std::string response, std::string tag)
{
    removeDLFromActive("burl_account");
    
    if (response == "-1")
    {
        if (m_backupDelegate)
        {
            m_backupDelegate->backupAccountFailed(GenericError, 0);
        }
        return;
    }
    
    std::string url = response + "/database/accounts/backupGJAccountNew.php";
    backupAccount(url);
}

void GJAccountManager::onGetAccountSyncURLCompleted(std::string response, std::string tag)
{
    removeDLFromActive("surl_account");
    
    if (response == "-1")
    {
        if (m_syncDelegate)
        {
            m_syncDelegate->syncAccountFailed(GenericError, 0);
        }
        return;
    }
    
    std::string url = response + "/database/accounts/syncGJAccountNew.php";
    syncAccount(url);
}

void GJAccountManager::onUpdateAccountSettingsCompleted(std::string response, std::string tag)
{
    removeDLFromActive("acc_sett");
    
    if (response == "-1")
    {
        if (m_accountSettingsDelegate)
        {
            m_accountSettingsDelegate->updateSettingsFailed();
        }
    }
    else
    {
        if (m_accountSettingsDelegate)
        {
            m_accountSettingsDelegate->updateSettingsFinished();
        }
    }
}

void GJAccountManager::handleItND(cocos2d::CCNode* node, void* data)
{
    // Manejar respuesta HTTP
}

GJAccountManager::~GJAccountManager()
{
}