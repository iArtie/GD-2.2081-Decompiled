#include "Layers/AccountLayer.h"
#include "Layers/AccountHelpLayer.h"
#include "Layers/AccountLoginLayer.h"
#include "Managers/GJAccountManager.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "UI/LoadingCircle.h"

AccountLayer* AccountLayer::create()
{
    AccountLayer* layer = new AccountLayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        layer->customSetup();
        return layer;
    }
    
    delete layer;
    return nullptr;
}

void AccountLayer::customSetup()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    GJAccountManager* manager = GJAccountManager::sharedState();
    manager->m_accountDelegate = this;
    manager->m_backupDelegate = this;
    manager->m_syncDelegate = this;
    
    m_isLoggedIn = (manager->m_accountID != 0);
    
    m_linkedAccountTitle = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_linkedAccountTitle->setScale(0.6f);
    m_linkedAccountTitle->setPosition(centerX, centerY + 100.0f);
    m_mainLayer->addChild(m_linkedAccountTitle);
    
    m_textArea = TextArea::create("", "bigFont.fnt", 1.0f, 500.0f, cocos2d::CCPoint(0.5f, 0.5f), 0.0f, 0);
    m_textArea->setScale(0.4f);
    m_textArea->setPosition(centerX, centerY + 50.0f);
    m_mainLayer->addChild(m_textArea);
    
    cocos2d::CCNode* loginBtn = ButtonSprite::create("Login");
    m_loginButton = CCMenuItemSpriteExtra::create(
        loginBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onLogin)
    );
    m_buttonMenu->addChild(m_loginButton);
    
    cocos2d::CCNode* registerBtn = ButtonSprite::create("Register");
    m_registerButton = CCMenuItemSpriteExtra::create(
        registerBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onRegister)
    );
    m_buttonMenu->addChild(m_registerButton);
    
    cocos2d::CCNode* backupBtn = ButtonSprite::create("Backup");
    m_backupButton = CCMenuItemSpriteExtra::create(
        backupBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onBackup)
    );
    m_buttonMenu->addChild(m_backupButton);
    
    cocos2d::CCNode* syncBtn = ButtonSprite::create("Sync");
    m_syncButton = CCMenuItemSpriteExtra::create(
        syncBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onSync)
    );
    m_buttonMenu->addChild(m_syncButton);
    
    cocos2d::CCNode* helpBtn = ButtonSprite::create("Help");
    m_helpButton = CCMenuItemSpriteExtra::create(
        helpBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onHelp)
    );
    m_buttonMenu->addChild(m_helpButton);
    
    cocos2d::CCNode* moreBtn = ButtonSprite::create("More");
    m_moreButton = CCMenuItemSpriteExtra::create(
        moreBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLayer::onMore)
    );
    m_buttonMenu->addChild(m_moreButton);
    
    cocos2d::CCPoint menuPos(centerX, centerY - 100.0f);
    cocos2d::CCArray* menuItems = cocos2d::CCArray::create();
    menuItems->addObject(m_loginButton);
    menuItems->addObject(m_registerButton);
    menuItems->addObject(m_backupButton);
    menuItems->addObject(m_syncButton);
    menuItems->addObject(m_helpButton);
    menuItems->addObject(m_moreButton);
    GameToolbox::alignItemsHorizontally(menuItems, 10.0f, menuPos);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = true;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setPosition(centerX, centerY);
    
    updatePage(false);
}

void AccountLayer::updatePage(bool changed)
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    m_isLoggedIn = (manager->m_accountID != 0);
    
    if (m_isLoggedIn)
    {
        m_linkedAccountTitle->setString(
            cocos2d::CCString::createWithFormat("Logged in as: %s", manager->m_username.c_str())->getCString()
        );
        m_textArea->setString("Your account is linked.\nUse Backup and Sync to manage your data.");
        m_loginButton->setVisible(false);
        m_registerButton->setVisible(false);
        m_backupButton->setVisible(true);
        m_syncButton->setVisible(true);
    }
    else
    {
        m_linkedAccountTitle->setString("Not Logged In");
        m_textArea->setString("Create an account or login to\nbackup and sync your data.");
        m_loginButton->setVisible(true);
        m_registerButton->setVisible(true);
        m_backupButton->setVisible(false);
        m_syncButton->setVisible(false);
    }
}

void AccountLayer::doBackup()
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    if (manager->m_accountID == 0)
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Error",
            "You must be logged in to backup."
        );
        alert->show();
        return;
    }
    
    showLoadingUI();
    manager->getAccountBackupURL();
}

void AccountLayer::doSync()
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    if (manager->m_accountID == 0)
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Error",
            "You must be logged in to sync."
        );
        alert->show();
        return;
    }
    
    showLoadingUI();
    manager->getAccountSyncURL();
}

void AccountLayer::showLoadingUI()
{
    toggleUI(false);
    m_loadingCircle->setVisible(true);
}

void AccountLayer::hideLoadingUI()
{
    toggleUI(true);
    m_loadingCircle->setVisible(false);
}

void AccountLayer::toggleUI(bool enable)
{
    m_buttonMenu->setEnabled(enable);
    setTouchEnabled(enable);
    setKeypadEnabled(enable);
}

void AccountLayer::exitLayer()
{
    removeFromParentAndCleanup(true);
}

void AccountLayer::layerHidden()
{
    setVisible(false);
}

void AccountLayer::onLogin(cocos2d::CCObject* sender)
{
    AccountLoginLayer* loginLayer = AccountLoginLayer::create("");
    addChild(loginLayer, 100);
    loginLayer->show();
}

void AccountLayer::onRegister(cocos2d::CCObject* sender)
{
    AccountLoginLayer* loginLayer = AccountLoginLayer::create("");
    addChild(loginLayer, 100);
    loginLayer->show();
}

void AccountLayer::onBackup(cocos2d::CCObject* sender)
{
    doBackup();
}

void AccountLayer::onSync(cocos2d::CCObject* sender)
{
    doSync();
}

void AccountLayer::onHelp(cocos2d::CCObject* sender)
{
    AccountHelpLayer* helpLayer = AccountHelpLayer::create();
    addChild(helpLayer, 100);
    helpLayer->showLayer(false);
}

void AccountLayer::onMore(cocos2d::CCObject* sender)
{
    AccountHelpLayer* helpLayer = AccountHelpLayer::create();
    addChild(helpLayer, 100);
    helpLayer->showLayer(false);
}

void AccountLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (btn2)
    {
        // Manejar respuesta de alerta
    }
}

void AccountLayer::accountStatusChanged()
{
    updatePage(true);
}

void AccountLayer::backupAccountFailed(BackupAccountError errorType, int response)
{
    hideLoadingUI();
    
    std::string message = "Backup failed.";
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Backup Error",
        message.c_str()
    );
    alert->show();
}

void AccountLayer::backupAccountFinished()
{
    hideLoadingUI();
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Backup Success",
        "Your account has been backed up successfully."
    );
    alert->show();
}

void AccountLayer::syncAccountFailed(BackupAccountError errorType, int response)
{
    hideLoadingUI();
    
    std::string message = "Sync failed.";
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Sync Error",
        message.c_str()
    );
    alert->show();
}

void AccountLayer::syncAccountFinished()
{
    hideLoadingUI();
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Sync Success",
        "Your account has been synced successfully."
    );
    alert->show();
}

AccountLayer::~AccountLayer()
{
}