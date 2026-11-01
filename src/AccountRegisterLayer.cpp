#include "AccountRegisterLayer.h"
#include "AccountManager.h"
#include "GJAccountManager.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "LoadingCircle.h"
#include "GameToolbox.h"

AccountRegisterLayer* AccountRegisterLayer::create()
{
    AccountRegisterLayer* layer = new AccountRegisterLayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool AccountRegisterLayer::init()
{
    if (!FLAlertLayer::init(this, 150))
        return false;

    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(400, 300));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(center);

    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Create Account", "goldFont.fnt");
    title->setPosition(ccp(center.x, center.y + 150 - 16));
    title->setScale(0.6f);
    m_mainLayer->addChild(title);

    // Menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCArray* menuItems = cocos2d::CCArray::create();

    // Cancel button
    ButtonSprite* cancelBtn = ButtonSprite::create("Cancel", 0);
    CCMenuItemSpriteExtra* cancelItem = CCMenuItemSpriteExtra::create(
        cancelBtn, nullptr, this, menu_selector(AccountRegisterLayer::onClose));
    m_buttonMenu->addChild(cancelItem);
    menuItems->addObject(cancelItem);

    // Submit button
    ButtonSprite* submitBtn = ButtonSprite::create("Submit", 0);
    CCMenuItemSpriteExtra* submitItem = CCMenuItemSpriteExtra::create(
        submitBtn, nullptr, this, menu_selector(AccountRegisterLayer::onSubmit));
    m_buttonMenu->addChild(submitItem);
    menuItems->addObject(submitItem);

    cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(
        ccp(center.x, center.y - 150 + 22));
    GameToolbox::alignItemsHorizontally(menuItems, 15.0f, menuPos, true);

    // Username
    cocos2d::CCSize inputSize(300, 30);
    cocos2d::CCPoint usernamePos(center.x, center.y + 90);

    createTextBackground(ccp(usernamePos.x, usernamePos.y), inputSize);
    
    m_usernameLabel = createTextLabel(
        ccp(usernamePos.x - 145, usernamePos.y),
        "Username: (shown to other players)",
        inputSize);

    m_usernameField = createTextInput(
        ccp(usernamePos.x, usernamePos.y),
        inputSize,
        "Username",
        224);
    m_usernameField->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    m_usernameField->m_maxLabelLength = 15;

    // Password
    cocos2d::CCPoint passwordPos = ccp(usernamePos.x, usernamePos.y - 45);

    createTextBackground(ccp(passwordPos.x, passwordPos.y), inputSize);
    
    m_passwordLabel = createTextLabel(
        ccp(passwordPos.x - 145, passwordPos.y),
        "Password:",
        inputSize);

    m_passwordField = createTextInput(
        ccp(passwordPos.x, passwordPos.y),
        inputSize,
        "Password",
        224);
    m_passwordField->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~!*'();:@+$,/?#[]&");
    m_passwordField->m_maxLabelLength = 20;
    m_passwordField->m_usePasswordChar = true;
    m_passwordField->setMaxLabelScale(1.0f);

    // Confirm Password
    cocos2d::CCPoint confirmPos = ccp(passwordPos.x, passwordPos.y - 45);

    createTextBackground(ccp(confirmPos.x, confirmPos.y), inputSize);
    
    m_confirmPasswordLabel = createTextLabel(
        ccp(confirmPos.x - 145, confirmPos.y),
        "Confirm Password:",
        inputSize);

    m_confirmPasswordField = createTextInput(
        ccp(confirmPos.x, confirmPos.y),
        inputSize,
        "Confirm Password",
        224);
    m_confirmPasswordField->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~!*'();:@+$,/?#[]&");
    m_confirmPasswordField->m_maxLabelLength = 20;
    m_confirmPasswordField->m_usePasswordChar = true;
    m_confirmPasswordField->setMaxLabelScale(1.0f);
    m_confirmPasswordField->forceOffset();

    // Email
    cocos2d::CCPoint emailPos = ccp(confirmPos.x, confirmPos.y - 45);

    createTextBackground(ccp(emailPos.x, emailPos.y), inputSize);
    
    m_emailLabel = createTextLabel(
        ccp(emailPos.x - 145, emailPos.y),
        "Email:",
        inputSize);

    m_emailField = createTextInput(
        ccp(emailPos.x, emailPos.y),
        inputSize,
        "Email",
        224);
    m_emailField->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_@.");
    m_emailField->m_maxLabelLength = 50;
    m_emailField->forceOffset();

    // Verify Email
    cocos2d::CCPoint verifyPos = ccp(emailPos.x, emailPos.y - 45);

    createTextBackground(ccp(verifyPos.x, verifyPos.y), inputSize);
    
    m_verifyEmailLabel = createTextLabel(
        ccp(verifyPos.x - 145, verifyPos.y),
        "Verify Email:",
        inputSize);

    m_verifyEmailField = createTextInput(
        ccp(verifyPos.x, verifyPos.y),
        inputSize,
        "Verify Email",
        224);
    m_verifyEmailField->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_@.");
    m_verifyEmailField->m_maxLabelLength = 50;
    m_verifyEmailField->forceOffset();

    // Loading circle
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = true;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setPosition(center);

    return true;
}

AccountRegisterLayer::~AccountRegisterLayer()
{
    CC_SAFE_RELEASE(m_loadingCircle);
    CC_SAFE_RELEASE(m_mainLayer);
}

void AccountRegisterLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (layer->getTag() == 1)
        onClose(nullptr);
}

bool AccountRegisterLayer::allowTextInput(CCTextInputNode* node)
{
    return !m_lockInput;
}

void AccountRegisterLayer::disableNodes()
{
    m_usernameField->onClickTrackNode(false);
    m_passwordField->onClickTrackNode(false);
    m_confirmPasswordField->onClickTrackNode(false);
    m_emailField->onClickTrackNode(false);
    m_verifyEmailField->onClickTrackNode(false);
}

void AccountRegisterLayer::hideLoadingUI()
{
    toggleUI(true);
    m_loadingCircle->setVisible(false);
}

void AccountRegisterLayer::keyBackClicked()
{
    onClose(nullptr);
}

void AccountRegisterLayer::onClose(cocos2d::CCObject* sender)
{
    disableNodes();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void AccountRegisterLayer::onSubmit(cocos2d::CCObject* sender)
{
    resetLabels();
    disableNodes();

    std::string username = m_usernameField->getString();
    std::string password = m_passwordField->getString();
    std::string confirmPassword = m_confirmPasswordField->getString();
    std::string email = m_emailField->getString();
    std::string verifyEmail = m_verifyEmailField->getString();

    if (!validUser(username))
    {
        updateLabel(AccountError::TooShortLessThan3);
        return;
    }

    if (!validPassword(password))
    {
        updateLabel(AccountError::TooShortLessThan6);
        return;
    }

    if (password != confirmPassword)
    {
        updateLabel(AccountError::PasswordsDoNotMatch);
        return;
    }

    if (!validEmail(email))
    {
        updateLabel(AccountError::InvalidEmail);
        return;
    }

    if (email != verifyEmail)
    {
        updateLabel(AccountError::EmailsDoNotMatch);
        return;
    }

    GJAccountManager::sharedState()->m_accountRegisterDelegate = this;
    GJAccountManager::sharedState()->registerAccount(username, password, email);
    showLoadingUI();
}

void AccountRegisterLayer::registerAccountFailed(AccountError errorType)
{
    updateLabel(errorType);
    hideLoadingUI();
}

void AccountRegisterLayer::registerAccountFinished()
{
    std::string email = m_emailField->getString();
    std::string message = "Thank you for registering.\nA confirmation email has been sent to your inbox <cy>" + email + "</c>.\nPlease <cg>activate</c> your account.";
    
    FLAlertLayer* alert = FLAlertLayer::create(this, "Created", message.c_str());
    alert->setTag(1);
    alert->show();
    m_loadingCircle->setVisible(false);
}

void AccountRegisterLayer::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void AccountRegisterLayer::resetLabel(int tag)
{
    switch (tag)
    {
        case 1:
            m_usernameLabel->setString("Username: (shown to other players)");
            m_usernameLabel->setColor(ccc3(255, 255, 255));
            break;
        case 2:
            m_passwordLabel->setString("Password:");
            m_passwordLabel->setColor(ccc3(255, 255, 255));
            break;
        case 3:
            m_confirmPasswordLabel->setString("Confirm Password:");
            m_confirmPasswordLabel->setColor(ccc3(255, 255, 255));
            break;
        case 4:
            m_emailLabel->setString("Email:");
            m_emailLabel->setColor(ccc3(255, 255, 255));
            break;
        case 5:
            m_verifyEmailLabel->setString("Verify Email:");
            m_verifyEmailLabel->setColor(ccc3(255, 255, 255));
            break;
        default:
            break;
    }
}

void AccountRegisterLayer::resetLabels()
{
    resetLabel(1);
    resetLabel(2);
    resetLabel(3);
    resetLabel(4);
    resetLabel(5);
}

void AccountRegisterLayer::showLoadingUI()
{
    disableNodes();
    toggleUI(false);
    m_loadingCircle->setVisible(true);
}

void AccountRegisterLayer::textChanged(CCTextInputNode* node)
{
    // Empty
}

void AccountRegisterLayer::textInputClosed(CCTextInputNode* node)
{
    int tag = node->getTag();
    resetLabel(tag);

    switch (tag)
    {
        case 1: // Username
            if (!validUser(node->getString()) && !node->getString().empty())
                updateLabel(AccountError::TooShortLessThan3);
            m_lockInput = nullptr;
            break;
        case 2: // Password
        {
            resetLabel(3);
            std::string confirm = m_confirmPasswordField->getString();
            bool isMatch = (node->getString() == confirm);
            
            if (!validPassword(node->getString()) && !node->getString().empty())
            {
                updateLabel(AccountError::TooShortLessThan6);
                m_lockInput = nullptr;
                break;
            }
            
            if (!node->getString().empty() && !confirm.empty() && !isMatch)
                updateLabel(AccountError::PasswordsDoNotMatch);
            m_lockInput = nullptr;
            break;
        }
        case 3: // Confirm Password
        {
            std::string password = m_passwordField->getString();
            bool isMatch = (password == node->getString());
            
            if (!password.empty() && !node->getString().empty() && !isMatch)
                updateLabel(AccountError::PasswordsDoNotMatch);
            m_lockInput = nullptr;
            break;
        }
        case 4: // Email
            if (!validEmail(node->getString()) && !node->getString().empty())
                updateLabel(AccountError::InvalidEmail);
            m_lockInput = nullptr;
            break;
        default:
            m_lockInput = nullptr;
            break;
    }
}

void AccountRegisterLayer::textInputOpened(CCTextInputNode* node)
{
    // Empty
}

void AccountRegisterLayer::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, ccp(0, 0)));
}

void AccountRegisterLayer::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, ccp(0, yOffset)));
}

void AccountRegisterLayer::toggleUI(bool enable)
{
    m_buttonMenu->setEnabled(enable);
    m_usernameField->setTouchEnabled(enable);
    m_passwordField->setTouchEnabled(enable);
    m_confirmPasswordField->setTouchEnabled(enable);
    m_emailField->setTouchEnabled(enable);
    m_verifyEmailField->setTouchEnabled(enable);
    setTouchEnabled(enable);
    setKeypadEnabled(enable);
}

void AccountRegisterLayer::updateLabel(AccountError type)
{
    const char* message = nullptr;
    cocos2d::CCLabelBMFont* target = nullptr;

    switch (type)
    {
        case AccountError::TooShortLessThan3:
            message = "Too short. Minimum 3 characters";
            target = m_usernameLabel;
            break;
        case AccountError::TooShortLessThan6:
            message = "Too short. Minimum 6 characters";
            target = m_passwordLabel;
            break;
        case AccountError::PasswordsDoNotMatch:
            message = "Passwords do not match";
            target = m_confirmPasswordLabel;
            break;
        case AccountError::InvalidEmail:
            message = "Email is invalid";
            target = m_emailLabel;
            break;
        case AccountError::EmailsDoNotMatch:
            message = "Emails do not match";
            target = m_verifyEmailLabel;
            break;
        case AccountError::InvalidUsername:
            message = "Username is invalid";
            target = m_usernameLabel;
            break;
        case AccountError::PasswordInvalid:
            message = "Password is invalid";
            target = m_passwordLabel;
            break;
        case AccountError::AlreadyUsedEmail:
            message = "Email is already in use";
            target = m_emailLabel;
            break;
        case AccountError::AlreadyUsedUsername:
            message = "Username is already in use";
            target = m_usernameLabel;
            break;
        default:
            message = "Something went wrong.";
            target = m_usernameLabel;
            break;
    }

    if (target)
    {
        target->setString(message);
        target->setColor(ccc3(255, 0, 0));
    }
}

bool AccountRegisterLayer::validEmail(std::string email)
{
    // Simple email validation
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return atPos != std::string::npos && dotPos != std::string::npos && atPos > 0 && dotPos > atPos + 1;
}

bool AccountRegisterLayer::validPassword(std::string password)
{
    return password.length() >= 6;
}

bool AccountRegisterLayer::validUser(std::string username)
{
    return username.length() >= 3;
}