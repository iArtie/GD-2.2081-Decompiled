#include "MessagesProfilePage.h"
#include "FLAlertLayer.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "GJUserScore.h"
#include "GJCommentListLayer.h"
#include "UploadActionPopup.h"
#include "LoadingCircle.h"
#include "CustomListView.h"
#include "BoomListView.h"
#include "CCMenuItemToggler.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "CCScale9Sprite.h"

void MessagesProfilePage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (btn2 && layer->getTag() == 1)
        deleteSelected();
}

MessagesProfilePage* MessagesProfilePage::create(bool sent)
{
    MessagesProfilePage* ret = new MessagesProfilePage();
    if (ret->init(sent)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MessagesProfilePage::deleteSelected()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(m_messageKey.c_str());

    if (!storedLevels)
        return;

    cocos2d::CCArray* selectedArray = cocos2d::CCArray::create();
    int newlyDeletedCount = 0;

    cocos2d::ccArray* data = storedLevels->data;
    if (data->num) {
        cocos2d::CCObject** arr = data->arr;
        cocos2d::CCObject** end = &arr[data->num - 1];
        while (arr <= end) {
            cocos2d::CCObject* obj = *arr;
            if (!obj)
                break;
            if (obj->m_uAutoReleaseCount & 0xFF0000) {
                selectedArray->addObject(obj);
                if (!(obj->m_uAutoReleaseCount & 0xFF)) {
                    obj->m_uAutoReleaseCount |= 1;
                    ++newlyDeletedCount;
                }
            }
            ++arr;
        }
    }

    if (selectedArray->count() == 0)
        return;

    GameLevelManager::sharedState()->m_messageListDelegate = this;
    GameLevelManager::sharedState()->m_uploadActionDelegate = this;
    glm = GameLevelManager::sharedState();
    if (glm->deleteUserMessages(0, selectedArray, m_sentMessages)) {
        m_actionPopup = UploadActionPopup::create(this, "Deleting Messages...");
        m_actionPopup->show();
    }

    if (newlyDeletedCount == 0) {
        m_toggledAll = false;
        m_toggleAllToggler->toggle(false);
        return;
    }

    glm = GameLevelManager::sharedState();
    GJAccountManager* accountMgr = GJAccountManager::sharedState();
    GJUserScore* score = glm->userInfoForAccountID(accountMgr->m_accountID - accountMgr->m_unkInt1);
    if (score) {
        int newCount = score->m_newMsgCount - newlyDeletedCount;
        if (newCount < 0)
            newCount = 0;
        score->m_newMsgCount = newCount;
    }

    m_toggledAll = false;
    m_toggleAllToggler->toggle(false);
}

void MessagesProfilePage::forceReloadMessages(bool sent)
{
    if (m_sentMessages == sent)
        loadPage(0);
}

bool MessagesProfilePage::init(bool sent)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_sentMessages = sent;
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCRect rect = cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Messages", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 20.0f));
    titleLabel->limitLabelWidth(185.0f, 0.8f, 0.0f);

    m_levelsLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_levelsLabel, 20);
    m_levelsLabel->setAnchorPoint(ccp(1.0f, 0.5f));
    m_levelsLabel->setPosition(ccp(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 20.0f));

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(MessagesProfilePage::onClose));
    backBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    m_buttonMenu->setPosition(ccp(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    if (!m_sentMessages) {
        cocos2d::CCSprite* sentSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_messagessent_001.png");
        CCMenuItemSpriteExtra* sentBtn = CCMenuItemSpriteExtra::create(
            sentSprite, nullptr, this, menu_selector(MessagesProfilePage::onSentMessages));
        m_buttonMenu->addChild(sentBtn);
        sentBtn->setPosition(m_buttonMenu->convertToNodeSpace(
            ccp(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)));
    }

    cocos2d::CCSprite* delSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_delBtn_001.png");
    cocos2d::CCNode* delNode = ButtonSprite::create(
        delSprite, 0, true, "GJ_button_04.png", 36.0f);
    delNode->setScale(0.7f);
    CCMenuItemSpriteExtra* delBtn = CCMenuItemSpriteExtra::create(
        delNode, nullptr, this, menu_selector(MessagesProfilePage::onDeleteSelected));
    m_buttonMenu->addChild(delBtn);
    cocos2d::CCPoint delPoint = ccp(winSize.width * 0.5f - 220.0f + 20.0f, winSize.height * 0.5f - 145.0f + 21.0f);
    delBtn->setPosition(m_buttonMenu->convertToNodeSpace(delPoint));

    cocos2d::CCSprite* checkOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    cocos2d::CCSprite* checkOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    checkOn->setScale(0.7f);
    checkOff->setScale(0.7f);
    m_toggleAllToggler = CCMenuItemToggler::create(
        checkOff, checkOn, this, menu_selector(MessagesProfilePage::onToggleAllObjects));
    m_buttonMenu->addChild(m_toggleAllToggler);
    m_toggleAllToggler->setPosition(delBtn->getPosition() + ccp(46.0f, 0.0f));
    m_toggleAllToggler->setSizeMult(1.1f);

    cocos2d::CCLabelBMFont* selectAllLabel = cocos2d::CCLabelBMFont::create("Select All", "bigFont.fnt");
    m_mainLayer->addChild(selectAllLabel, 10);
    selectAllLabel->setPosition(delPoint + ccp(80.0f, 0.0f));
    selectAllLabel->setScale(0.6f);

    m_errorLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_errorLabel, 10);
    m_errorLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
    m_errorLabel->setScale(0.6f);
    m_errorLabel->setVisible(false);

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    prevSprite->setScale(0.8f);
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(MessagesProfilePage::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 195.0f, winSize.height * 0.5f)));
    m_prevButton->setSizeMult(2.0f);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    nextSprite->setScale(0.8f);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(MessagesProfilePage::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 195.0f, winSize.height * 0.5f)));
    m_nextButton->setSizeMult(2.0f);

    m_prevButton->setVisible(false);
    m_nextButton->setVisible(false);

    cocos2d::CCSprite* refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshButton = CCMenuItemSpriteExtra::create(
        refreshSprite, nullptr, this, menu_selector(MessagesProfilePage::onUpdate));
    m_refreshButton->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshButton);
    m_refreshButton->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f - 145.0f + 10.0f + 1.0f)));
    m_refreshButton->setVisible(false);

    loadPage(0);

    return true;
}

bool MessagesProfilePage::isCorrect(const char* key)
{
    return m_messageKey == key;
}

void MessagesProfilePage::keyBackClicked()
{
    onClose(nullptr);
}

void MessagesProfilePage::loadMessagesFailed(const char* key, GJErrorCode errorType)
{
    if (!isCorrect(key))
        return;

    m_loadingCircle->setVisible(false);
    m_refreshButton->setVisible(true);

    if (errorType == GJErrorCode::NotFound)
        m_errorLabel->setString("Nothing found");
    else
        m_errorLabel->setString("Something went wrong");
    m_errorLabel->setVisible(true);
}

void MessagesProfilePage::loadMessagesFinished(cocos2d::CCArray* messages, const char* key)
{
    if (!isCorrect(key))
        return;

    setupCommentsBrowser(messages);
    m_loadingCircle->setVisible(false);
    m_errorLabel->setVisible(false);
    m_refreshButton->setVisible(true);

    if (!m_sentMessages && messages && messages->count() > 0) {
        int unreadCount = 0;
        cocos2d::ccArray* data = messages->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr;
                if (!obj)
                    break;
                ++arr;
                if (!(obj->m_uAutoReleaseCount & 0xFF))
                    ++unreadCount;
            }
        }

        GameLevelManager* glm = GameLevelManager::sharedState();
        GJAccountManager* accountMgr = GJAccountManager::sharedState();
        GJUserScore* score = glm->userInfoForAccountID(accountMgr->m_accountID - accountMgr->m_unkInt1);
        if (score && score->m_newMsgCount < unreadCount)
            score->m_newMsgCount = unreadCount;
    }
}

void MessagesProfilePage::loadPage(int page)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string messagesKey = glm->getMessagesKey(m_sentMessages, page);

    if (!m_messageKey.empty() && m_messageKey != messagesKey)
        untoggleAll();

    m_messageKey = messagesKey;

    m_loadingCircle->setVisible(true);
    m_refreshButton->setVisible(false);
    m_errorLabel->setVisible(false);

    if (page <= 0)
        m_prevButton->setVisible(false);
    else
        m_prevButton->setVisible(true);

    glm = GameLevelManager::sharedState();
    bool hasStored = glm->getStoredOnlineLevels(m_messageKey.c_str()) != nullptr;
    m_nextButton->setVisible(hasStored);

    GameLevelManager::sharedState()->m_messageListDelegate = this;

    glm = GameLevelManager::sharedState();
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(m_messageKey.c_str());
    if (!storedLevels) {
        setupCommentsBrowser(nullptr);
        glm = GameLevelManager::sharedState();
        glm->getUserMessages(m_sentMessages, page, m_itemCount);
        m_page = page;
        return;
    }

    glm = GameLevelManager::sharedState();
    std::string pageInfo = glm->getPageInfo(m_messageKey.c_str());
    setupPageInfo(pageInfo, m_messageKey.c_str());
    loadMessagesFinished(storedLevels, m_messageKey.c_str());
    m_page = page;
}

void MessagesProfilePage::onClose(cocos2d::CCObject* sender)
{
    if (m_listLayer) {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);

    untoggleAll();

    if (m_sentMessages) {
        MessagesProfilePage* newPage = MessagesProfilePage::create(false);
        newPage->show();
    } else {
        GJAccountManager* accountMgr = GJAccountManager::sharedState();
        ProfilePage* profilePage = ProfilePage::create(
            accountMgr->m_accountID - accountMgr->m_unkInt1, true);
        profilePage->m_noElasticity = true;
        profilePage->show();
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MessagesProfilePage::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();
    if (m_actionPopup == popup)
        m_actionPopup = nullptr;
}

void MessagesProfilePage::onDeleteSelected(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(m_messageKey.c_str());

    if (storedLevels) {
        cocos2d::ccArray* data = storedLevels->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr;
                if (!obj)
                    break;

                if (obj->m_uAutoReleaseCount & 0xFF0000) {
                    FLAlertLayer* alert = FLAlertLayer::create(
                        this, "Delete",
                        "Are you sure you want to <cr>delete</c> the selected <cg>messages</c>?");
                    alert->setTag(1);
                    alert->show();
                    return;
                }
                ++arr;
            }
        }
    }

    FLAlertLayer* alert = FLAlertLayer::create(
        nullptr, "Nothing here...", "No messages selected.");
    alert->show();
}

void MessagesProfilePage::onNextPage(cocos2d::CCObject* sender)
{
    loadPage(m_page + 1);
}

void MessagesProfilePage::onPrevPage(cocos2d::CCObject* sender)
{
    loadPage(m_page - 1);
}

void MessagesProfilePage::onSentMessages(cocos2d::CCObject* sender)
{
    if (m_listLayer) {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);

    MessagesProfilePage* newPage = MessagesProfilePage::create(true);
    newPage->show();

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MessagesProfilePage::onToggleAllObjects(cocos2d::CCObject* sender)
{
    m_toggledAll = !m_toggledAll;

    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(m_messageKey.c_str());

    if (storedLevels && storedLevels->count()) {
        cocos2d::ccArray* data = storedLevels->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr;
                if (!obj)
                    break;
                ++arr;
                if (m_toggledAll)
                    obj->m_uAutoReleaseCount |= 0xFF0000;
                else
                    obj->m_uAutoReleaseCount &= ~0xFF0000;
            }
        }
        CustomListView::reloadAll((CustomListView*)m_listLayer->m_list);
    }
}

void MessagesProfilePage::onUpdate(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    if ((glm->getTimeLeft("upd_messages", 5.0f) - 1) > 4) {
        glm = GameLevelManager::sharedState();
        glm->makeTimeStamp("upd_messages");
        glm = GameLevelManager::sharedState();
        glm->invalidateMessages(m_sentMessages, true);
    }
}

void MessagesProfilePage::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void MessagesProfilePage::setupCommentsBrowser(cocos2d::CCArray* messages)
{
    if (m_listLayer) {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    CustomListView* listView = nullptr;
    if (messages)
        listView = CustomListView::create(messages, BoomListType::Default, 196.0f, 340.0f);

    cocos2d::ccColor4B color = ccc4(0, 0, 0, 0);
    m_listLayer = GJCommentListLayer::create(listView, "Messages", color, 340.0f, 196.0f, false);
    m_mainLayer->addChild(m_listLayer);
    m_listLayer->setPosition(ccp(
        (winSize.width - 340.0f) * 0.5f,
        (winSize.height - 196.0f) * 0.5f - 10.0f + 5.0f));

    if (m_listLayer->m_list) {
        int priority = m_listLayer->m_list->getTouchPriority();
        m_buttonMenu->setTouchPriority(priority - 2);
    }
}

void MessagesProfilePage::setupPageInfo(std::string info, const char* key)
{
    if (!isCorrect(key))
        return;

    cocos2d::CCArray* tokens = cocos2d::CCArray::create();

    size_t start = 0;
    size_t end = info.find(':');
    while (end != std::string::npos) {
        std::string token = info.substr(start, end - start);
        if (!token.empty()) {
            cocos2d::CCString* str = cocos2d::CCString::create(token);
            tokens->addObject(str);
        }
        start = end + 1;
        end = info.find(':', start);
    }
    if (start < info.length()) {
        std::string token = info.substr(start);
        if (!token.empty()) {
            cocos2d::CCString* str = cocos2d::CCString::create(token);
            tokens->addObject(str);
        }
    }

    if (tokens->count() > 2) {
        cocos2d::CCString* str0 = (cocos2d::CCString*)tokens->objectAtIndex(0);
        m_itemCount = str0->intValue();
        cocos2d::CCString* str1 = (cocos2d::CCString*)tokens->objectAtIndex(1);
        m_pageStartIdx = str1->intValue();
        cocos2d::CCString* str2 = (cocos2d::CCString*)tokens->objectAtIndex(2);
        m_pageEndIdx = str2->intValue();
        updatePageArrows();
    }
}

void MessagesProfilePage::untoggleAll()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(m_messageKey.c_str());

    if (storedLevels) {
        cocos2d::ccArray* data = storedLevels->data;
        if (data->num) {
            cocos2d::CCObject** arr = data->arr;
            cocos2d::CCObject** end = &arr[data->num - 1];
            while (arr <= end) {
                cocos2d::CCObject* obj = *arr;
                if (!obj)
                    break;
                ++arr;
                obj->m_uAutoReleaseCount &= ~0xFF0000;
            }
        }
    }
}

void MessagesProfilePage::updateLevelsLabel()
{
    int total = m_pageStartIdx + m_pageEndIdx;
    if (total > (int)m_itemCount)
        total = m_itemCount;

    cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
        "%i to %i of %i",
        m_pageStartIdx + 1, total, m_itemCount);
    m_levelsLabel->setString(str->getCString());
}

void MessagesProfilePage::updatePageArrows()
{
    if (m_pageStartIdx) {
        m_prevButton->setVisible(true);
        if (m_pageStartIdx + m_pageEndIdx < (int)m_itemCount) {
            m_nextButton->setVisible(true);
            return;
        }
    } else {
        m_prevButton->setVisible(false);
        if (m_pageStartIdx + m_pageEndIdx < (int)m_itemCount) {
            m_nextButton->setVisible(true);
            return;
        }
    }
    m_nextButton->setVisible(false);
}

void MessagesProfilePage::uploadActionFailed(int id, int response)
{
    if (m_actionPopup)
        m_actionPopup->showFailMessage("Failed. Please try again later.");
}

void MessagesProfilePage::uploadActionFinished(int id, int response)
{
    if (m_actionPopup) {
        std::string message = "Unknown action finished";
        if (response == 39)
            message = "Messages removed";
        m_actionPopup->showSuccessMessage(message);
    }
}

MessagesProfilePage::~MessagesProfilePage()
{
    if (m_loadingCircle)
        m_loadingCircle->release();
}