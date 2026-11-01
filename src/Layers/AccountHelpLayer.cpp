#include "Layers/AccountHelpLayer.h"
#include "Layers/AccountLayer.h"
#include "Layers/AccountLoginLayer.h"
#include "Managers/GJAccountManager.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "cocos2d/CCApplication.h"

AccountHelpLayer* AccountHelpLayer::create()
{
    AccountHelpLayer* layer = new AccountHelpLayer();
    if (layer && layer->init("Account Help"))
    {
        layer->autorelease();
        return layer;
    }
    
    delete layer;
    return nullptr;
}

void AccountHelpLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (layer->getTag() == 4)
    {
        if (btn2)
        {
            doUnlink();
        }
    }
    else if (layer->getTag() == 5)
    {
        if (btn2)
        {
            cocos2d::CCApplication::sharedApplication()->openURL("https://example.com/account/manage");
        }
    }
    else if (layer->getTag() == 3 && btn2)
    {
        verifyUnlink();
    }
}

void AccountHelpLayer::customSetup()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_loginStatusLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_loginStatusLabel->setScale(0.8f);
    m_loginStatusLabel->setPosition(centerX, centerY + 90.0f);
    m_mainLayer->addChild(m_loginStatusLabel);
    
    cocos2d::CCPoint textSize(35.0f, 0.0f);
    m_textArea = TextArea::create("", "bigFont.fnt", 1.0f, 1000.0f, textSize, 0.0f, 0);
    m_textArea->setScale(0.4f);
    m_textArea->setPosition(m_loginStatusLabel->getPosition() + cocos2d::CCPoint(0.0f, -18.0f));
    m_mainLayer->addChild(m_textArea);
    
    cocos2d::CCNode* refreshButton = ButtonSprite::create("Refresh Login");
    m_refreshLoginButton = CCMenuItemSpriteExtra::create(
        refreshButton,
        nullptr,
        this,
        cocos2d::menu_selector(AccountHelpLayer::onReLogin)
    );
    m_buttonMenu->addChild(m_refreshLoginButton);
    
    cocos2d::CCNode* manageButton = ButtonSprite::create("Manage Account");
    CCMenuItemSpriteExtra* manageAccountBtn = CCMenuItemSpriteExtra::create(
        manageButton,
        nullptr,
        this,
        cocos2d::menu_selector(AccountHelpLayer::onAccountManagement)
    );
    m_buttonMenu->addChild(manageAccountBtn);
    
    cocos2d::CCNode* unlinkButton = ButtonSprite::create("Unlink Account");
    m_unlinkAccountButton = CCMenuItemSpriteExtra::create(
        unlinkButton,
        nullptr,
        this,
        cocos2d::menu_selector(AccountHelpLayer::onUnlink)
    );
    m_buttonMenu->addChild(m_unlinkAccountButton);
    
    cocos2d::CCPoint menuPos = m_mainLayer->convertToWorldSpace(cocos2d::CCPoint(centerX, centerY + 30.0f));
    menuPos = m_buttonMenu->convertToNodeSpace(menuPos);
    
    m_refreshLoginButton->setPosition(menuPos);
    manageAccountBtn->setPosition(menuPos + cocos2d::CCPoint(0.0f, -45.0f));
    m_unlinkAccountButton->setPosition(menuPos + cocos2d::CCPoint(0.0f, -90.0f));
    
    updatePage();
    
    GJAccountManager::sharedState()->m_accountDelegate = this;
    m_shouldShowAccountLayer = true;
}

void AccountHelpLayer::doUnlink()
{
    GJAccountManager::sharedState()->unlinkFromAccount();
    exitLayer();
}

void AccountHelpLayer::exitLayer()
{
    GJDropDownLayer::exitLayer(nullptr);
}

void AccountHelpLayer::layerHidden()
{
    if (m_shouldShowAccountLayer)
    {
        AccountLayer* accountLayer = AccountLayer::create();
        getParent()->addChild(accountLayer, 100);
        accountLayer->showLayer(false);
    }
    
    GJDropDownLayer::layerHidden();
}

void AccountHelpLayer::onAccountManagement(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Manage",
        "Open the account management page?"
    );
    alert->setTag(5);
    alert->show();
}

void AccountHelpLayer::onReLogin(cocos2d::CCObject* sender)
{
    AccountLoginLayer* loginLayer = AccountLoginLayer::create("");
    loginLayer->show();
}

void AccountHelpLayer::onUnlink(cocos2d::CCObject* sender)
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    std::string username = manager->m_username;
    
    std::string message = cocos2d::CCString::createWithFormat(
        "Are you sure you want to <cg>unlink</c> from the account <cl>%s</c>?\n"
        "<cy>Unlinking will delete all data from this device.</c>",
        username.c_str()
    )->getCString();
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Unlink Account",
        message.c_str()
    );
    
    alert->m_button2->updateBGImage("GJ_button_05.png");
    alert->setTag(3);
    alert->show();
}

void AccountHelpLayer::updatePage()
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    bool isLoggedIn = (manager->m_accountID != manager->m_unkInt1);
    
    if (isLoggedIn)
    {
        std::string username = manager->m_username;
        m_loginStatusLabel->setString(
            cocos2d::CCString::createWithFormat("Linked to account: %s", username.c_str())->getCString()
        );
        m_textArea->setString("Login required");
        m_textArea->colorAllCharactersTo(cocos2d::ccc3(255, 255, 255));
    }
    else
    {
        m_loginStatusLabel->setString("Not logged in");
        m_textArea->setString("Create an account to backup\nand sync your data");
        m_textArea->colorAllCharactersTo(cocos2d::ccc3(255, 255, 255));
    }
    
    m_unlinkAccountButton->setVisible(manager->m_accountID != manager->m_unkInt1);
}

void AccountHelpLayer::verifyUnlink()
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Warning",
        "This will <cr>delete</c> ALL <cl>save data</c>.\n"
        "Do you want to continue?\n"
        "<cy>(You cannot undo this action)</c>"
    );
    
    alert->setTag(4);
    alert->m_button2->updateBGImage("GJ_button_06.png");
    alert->show();
}

AccountHelpLayer::~AccountHelpLayer()
{
}