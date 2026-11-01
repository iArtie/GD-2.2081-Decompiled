#include "AccountLoginLayer.h"
#include "GJAccountManager.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "LoadingCircle.h"
#include "GameToolbox.h"
#include "cocos2d/CCApplication.h"
#include "cocos-ext.h"

void AccountLoginLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (layer->getTag() == 2)
    {
        onClose(nullptr);
    }
}

AccountLoginLayer* AccountLoginLayer::create(std::string username)
{
    AccountLoginLayer* layer = new AccountLoginLayer();
    if (layer && layer->init(username))
    {
        layer->autorelease();
        return layer;
    }
    
    delete layer;
    return nullptr;
}

bool AccountLoginLayer::init(std::string username)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
    {
        return false;
    }
    
    incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);
    m_noElasticity = true;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setColor(cocos2d::ccc3(255, 255, 255));
    bg->setContentSize(cocos2d::CCSize(400.0f, 300.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(centerX, centerY);
    
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Login", "bigFont.fnt");
    titleLabel->setPosition(centerX, centerY + 150.0f - 16.0f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    m_joystickConnected = -1;
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    cocos2d::CCArray* menuItems = cocos2d::CCArray::create();
    
    cocos2d::CCNode* cancelBtn = ButtonSprite::create("Cancel");
    CCMenuItemSpriteExtra* cancelItem = CCMenuItemSpriteExtra::create(
        cancelBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLoginLayer::onClose)
    );
    m_buttonMenu->addChild(cancelItem);
    menuItems->addObject(cancelItem);
    
    cocos2d::CCNode* loginBtn = ButtonSprite::create("Login");
    CCMenuItemSpriteExtra* loginItem = CCMenuItemSpriteExtra::create(
        loginBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLoginLayer::onSubmit)
    );
    m_buttonMenu->addChild(loginItem);
    menuItems->addObject(loginItem);
    
    cocos2d::CCPoint menuPos(centerX, centerY - 150.0f + 22.0f);
    menuPos = m_mainLayer->getPosition() + menuPos;
    menuPos = m_buttonMenu->convertToNodeSpace(menuPos);
    GameToolbox::alignItemsHorizontally(menuItems, 15.0f, menuPos);
    
    cocos2d::CCSize inputSize(300.0f, 30.0f);
    cocos2d::CCPoint usernamePos(centerX, centerY + 80.0f);
    
    // Crear campo de username
    m_usernameLabel = cocos2d::CCLabelBMFont::create("Username:", "bigFont.fnt");
    m_usernameLabel->setPosition(usernamePos.x - 150.0f, usernamePos.y + 15.0f);
    m_usernameLabel->setScale(0.4f);
    m_mainLayer->addChild(m_usernameLabel);
    
    m_usernameInput = CCTextInputNode::create(inputSize, usernamePos, "");
    m_usernameInput->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    m_usernameInput->m_maxLabelLength = 16;
    m_mainLayer->addChild(m_usernameInput);
    
    if (!username.empty())
    {
        m_usernameInput->setString(username);
    }
    
    cocos2d::CCPoint passwordPos = usernamePos + cocos2d::CCPoint(0.0f, -50.0f);
    
    // Crear campo de password
    m_passwordLabel = cocos2d::CCLabelBMFont::create("Password:", "bigFont.fnt");
    m_passwordLabel->setPosition(passwordPos.x - 150.0f, passwordPos.y + 15.0f);
    m_passwordLabel->setScale(0.4f);
    m_mainLayer->addChild(m_passwordLabel);
    
    m_passwordInput = CCTextInputNode::create(inputSize, passwordPos, "");
    m_passwordInput->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~!*'();:@+$,/?#[]&");
    m_passwordInput->m_maxLabelLength = 20;
    m_passwordInput->m_usePasswordChar = true;
    m_passwordInput->setMaxLabelScale(1.0f);
    m_mainLayer->addChild(m_passwordInput);
    
    menuItems->removeAllObjects();
    
    cocos2d::CCNode* forgotUserBtn = ButtonSprite::create("Forgot Username", "goldFont.fnt", "GJ_button_05.png", 0.8f);
    CCMenuItemSpriteExtra* forgotUserItem = CCMenuItemSpriteExtra::create(
        forgotUserBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLoginLayer::onForgotUser)
    );
    m_buttonMenu->addChild(forgotUserItem);
    
    cocos2d::CCNode* forgotPassBtn = ButtonSprite::create("Forgot Password", "goldFont.fnt", "GJ_button_05.png", 0.8f);
    CCMenuItemSpriteExtra* forgotPassItem = CCMenuItemSpriteExtra::create(
        forgotPassBtn,
        nullptr,
        this,
        cocos2d::menu_selector(AccountLoginLayer::onForgotPassword)
    );
    m_buttonMenu->addChild(forgotPassItem);
    
    menuItems->addObject(forgotUserItem);
    menuItems->addObject(forgotPassItem);
    
    cocos2d::CCPoint forgotPos(centerX, centerY - 45.0f);
    forgotPos = m_mainLayer->getPosition() + forgotPos;
    forgotPos = m_buttonMenu->convertToNodeSpace(forgotPos);
    GameToolbox::alignItemsVertically(menuItems, 15.0f, forgotPos);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = true;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setPosition(centerX, centerY);
    
    return true;
}

void AccountLoginLayer::keyBackClicked()
{
    onClose(nullptr);
}

void AccountLoginLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void AccountLoginLayer::disableNodes()
{
    m_usernameInput->onClickTrackNode(false);
    m_passwordInput->onClickTrackNode(false);
}

void AccountLoginLayer::hideLoadingUI()
{
    toggleUI(true);
    m_loadingCircle->setVisible(false);
}

void AccountLoginLayer::showLoadingUI()
{
    disableNodes();
    toggleUI(false);
    m_loadingCircle->setVisible(true);
}

void AccountLoginLayer::toggleUI(bool enable)
{
    m_buttonMenu->setEnabled(enable);
    m_usernameInput->setTouchEnabled(enable);
    m_passwordInput->setTouchEnabled(enable);
    setTouchEnabled(enable);
    setKeypadEnabled(enable);
}

void AccountLoginLayer::resetLabel(int tag)
{
    if (tag == 1)
    {
        m_usernameLabel->setString("Username:");
        m_usernameLabel->setColor(cocos2d::ccc3(255, 255, 255));
    }
    else if (tag == 2)
    {
        m_passwordLabel->setString("Password:");
        m_passwordLabel->setColor(cocos2d::ccc3(255, 255, 255));
    }
}

void AccountLoginLayer::resetLabels()
{
    resetLabel(1);
    resetLabel(2);
}

void AccountLoginLayer::updateLabel(AccountError type)
{
    switch (type)
    {
        case 2:
            m_usernameLabel->setString("Already linked to different Steam account");
            m_usernameLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
        case 3:
            m_usernameLabel->setString("Account has been disabled");
            m_usernameLabel->setColor(cocos2d::ccc3(255, 0, 0));
            m_passwordLabel->setString("Contact RobTop Support for more info");
            m_passwordLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
        case 4:
            m_usernameLabel->setString("Already linked to different account");
            m_usernameLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
        case 5:
            m_usernameLabel->setString("Too short. Minimum 3 characters");
            m_usernameLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
        case 6:
            m_passwordLabel->setString("Too short. Minimum 6 characters");
            m_passwordLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
        default:
            m_usernameLabel->setString("Login failed");
            m_usernameLabel->setColor(cocos2d::ccc3(255, 0, 0));
            break;
    }
}

void AccountLoginLayer::onClose(cocos2d::CCObject* sender)
{
    disableNodes();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void AccountLoginLayer::onSubmit(cocos2d::CCObject* sender)
{
    resetLabels();
    disableNodes();
    
    m_username = m_usernameInput->getString();
    m_password = m_passwordInput->getString();
    
    if (m_username.length() <= 2)
    {
        updateLabel((AccountError)5);
        return;
    }
    
    if (m_password.length() <= 5)
    {
        updateLabel((AccountError)6);
        return;
    }
    
    GJAccountManager::sharedState()->m_accountLoginDelegate = this;
    
    std::string shaPass = GJAccountManager::sharedState()->getShaPassword(m_password);
    GJAccountManager::sharedState()->loginAccount(m_username, shaPass);
    
    showLoadingUI();
}

void AccountLoginLayer::loginAccountFailed(AccountError errorType)
{
    updateLabel(errorType);
    hideLoadingUI();
}

void AccountLoginLayer::loginAccountFinished(int accountID, int userID)
{
    GJAccountManager* manager = GJAccountManager::sharedState();
    manager->linkToAccount(m_username, m_password, accountID, userID);
    
    hideLoadingUI();
    
    std::string message = cocos2d::CCString::createWithFormat(
        "Thank you for logging in.\nYou have now linked your account:\n<cg>%s</c>",
        m_username.c_str()
    )->getCString();
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Login Success",
        message.c_str()
    );
    alert->setTag(2);
    alert->show();
}

void AccountLoginLayer::onForgotUser(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://example.com/forgot-username");
}

void AccountLoginLayer::onForgotPassword(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://example.com/forgot-password");
}

void AccountLoginLayer::textChanged(CCTextInputNode* node)
{
}

void AccountLoginLayer::textInputClosed(CCTextInputNode* node)
{
    int tag = node->getTag();
    resetLabel(tag);
    
    std::string text = node->getString();
    if (tag == 1 && text.length() <= 2)
    {
        updateLabel((AccountError)5);
    }
    else if (tag == 2 && text.length() <= 5)
    {
        updateLabel((AccountError)6);
    }
}

void AccountLoginLayer::textInputOpened(CCTextInputNode* node)
{
}

AccountLoginLayer::~AccountLoginLayer()
{
}