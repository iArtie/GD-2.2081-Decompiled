#include "FRequestProfilePage.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "GJUserScore.h"
#include "GJCommentListLayer.h"
#include "LoadingCircle.h"
#include "UploadActionPopup.h"
#include "CCMenuItemSpriteExtra.h"
#include "CustomListView.h"
#include "ProfilePage.h"
#include "FLAlertLayer.h"
#include "ButtonSprite.h"
#include "CCMenuItemToggler.h"

void FRequestProfilePage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2 && layer->getTag() == 3) {
        deleteSelected();
    }
}

FRequestProfilePage* FRequestProfilePage::create(bool sent) {
    FRequestProfilePage* ret = new FRequestProfilePage();
    if (ret && ret->init(sent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void FRequestProfilePage::deleteSelected() {
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* requests = glm->getStoredOnlineLevels(m_key.c_str());
    
    if (!requests) return;
    
    cocos2d::CCArray* selected = cocos2d::CCArray::create();
    int count = 0;
    
    for (unsigned int i = 0; i < requests->count(); i++) {
        cocos2d::CCObject* obj = requests->objectAtIndex(i);
        // Check if selected (using offset 22, byte 1)
        if (*(unsigned char*)((char*)obj + 0x58)) {
            selected->addObject(obj);
            *(unsigned char*)((char*)obj + 0x58) = 0;
            count++;
        }
    }
    
    if (selected->count() > 0) {
        GameLevelManager::sharedState()->m_friendRequestDelegate = this;
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;
        
        if (GameLevelManager::sharedState()->deleteFriendRequests(false, selected, false)) {
            std::string msg = "Deleting Requests...";
            m_uploadPopup = UploadActionPopup::create(this, msg);
            m_uploadPopup->show();
        }
        
        // Update friend request count
        GJAccountManager* acctMgr = GJAccountManager::sharedState();
        GJUserScore* userInfo = GameLevelManager::sharedState()->userInfoForAccountID(
            acctMgr->m_accountID - acctMgr->m_unkInt1);
        if (userInfo) {
            int newCount = userInfo->m_friendReqCount - count;
            userInfo->m_friendReqCount = (newCount < 0) ? 0 : newCount;
        }
    }
}

void FRequestProfilePage::forceReloadRequests(bool sent) {
    if (m_sent == sent) {
        loadPage(0);
    }
}

bool FRequestProfilePage::init(bool sent) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    m_sent = sent;
    m_noElasticity = true;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440, 290));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Title
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 17.0f));
    titleLabel->limitLabelWidth(185.0f, 0.8f, 0.0f);
    
    // Subtitle
    cocos2d::CCLabelBMFont* subtitleLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(subtitleLabel, 10);
    cocos2d::CCPoint titlePos = titleLabel->getPosition();
    subtitleLabel->setPosition(cocos2d::CCPoint(titlePos.x, titlePos.y - 20.0f));
    subtitleLabel->setScale(0.6f);
    
    // Page label
    m_pageLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_pageLabel, 20);
    m_pageLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_pageLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 20.0f));
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this, menu_selector(FRequestProfilePage::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Loading circle
    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);
    m_circle->m_sprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Error label
    m_errorLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_errorLabel, 10);
    m_errorLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    m_errorLabel->setScale(0.6f);
    m_errorLabel->setVisible(false);
    
    // Previous page button
    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    prevSprite->setScale(0.8f);
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, nullptr, this, menu_selector(FRequestProfilePage::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f - 195.0f, winSize.height * 0.5f)));
    m_prevButton->setSizeMult(2.0f);
    m_prevButton->setVisible(false);
    
    // Next page button
    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    nextSprite->setScale(0.8f);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, nullptr, this, menu_selector(FRequestProfilePage::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f + 195.0f, winSize.height * 0.5f)));
    m_nextButton->setSizeMult(2.0f);
    m_nextButton->setVisible(false);
    
    // Refresh button
    cocos2d::CCSprite* refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshButton = CCMenuItemSpriteExtra::create(refreshSprite, nullptr, this, menu_selector(FRequestProfilePage::onUpdate));
    m_refreshButton->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshButton);
    m_refreshButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f - 145.0f + 10.0f + 1.0f)));
    m_refreshButton->setVisible(false);
    
    if (!m_sent) {
        // Sent requests button
        cocos2d::CCSprite* sentSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_pending_001.png");
        CCMenuItemSpriteExtra* sentBtn = CCMenuItemSpriteExtra::create(sentSprite, nullptr, this, menu_selector(FRequestProfilePage::onSentRequests));
        m_buttonMenu->addChild(sentBtn);
        sentBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)));
        
        // Delete selected button
        cocos2d::CCNode* deleteBtnNode = ButtonSprite::create("Delete", 36.0f, true, "GJ_button_04.png", false);
        deleteBtnNode->setScale(0.7f);
        CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteBtnNode, nullptr, this, menu_selector(FRequestProfilePage::onDeleteSelected));
        m_buttonMenu->addChild(deleteBtn);
        deleteBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 20.0f, winSize.height * 0.5f - 145.0f + 21.0f)));
        
        // Toggle all button
        cocos2d::CCSprite* checkOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        cocos2d::CCSprite* checkOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn->setScale(0.7f);
        checkOff->setScale(0.7f);
        CCMenuItemToggler* toggleAll = CCMenuItemToggler::create(checkOff, checkOn, this, menu_selector(FRequestProfilePage::onToggleAllObjects));
        m_buttonMenu->addChild(toggleAll);
        cocos2d::CCPoint deletePos = deleteBtn->getPosition();
        toggleAll->setPosition(cocos2d::CCPoint(deletePos.x + 46.0f, deletePos.y));
        toggleAll->setSizeMult(1.1f);
        
        // Toggle label
        cocos2d::CCLabelBMFont* toggleLabel = cocos2d::CCLabelBMFont::create();
        m_mainLayer->addChild(toggleLabel, 10);
        toggleLabel->setPosition(cocos2d::CCPoint(deletePos.x + 80.0f, deletePos.y));
        toggleLabel->setScale(0.6f);
    }
    
    GameLevelManager::sharedState()->m_friendRequestDelegate = this;
    loadPage(0);
    
    return true;
}

bool FRequestProfilePage::isCorrect(const char* key) {
    return m_key == key;
}

void FRequestProfilePage::keyBackClicked() {
    onClose(nullptr);
}

void FRequestProfilePage::loadFRequestsFailed(const char* key, GJErrorCode errorType) {
    if (!isCorrect(key)) return;
    
    m_circle->setVisible(false);
    m_refreshButton->setVisible(true);
    m_errorLabel->setString((errorType == NotFound) ? "Nothing found" : "Something went wrong");
    m_errorLabel->setVisible(true);
}

void FRequestProfilePage::loadFRequestsFinished(cocos2d::CCArray* scores, const char* key) {
    if (!isCorrect(key)) return;
    
    setupCommentsBrowser(scores);
    m_circle->setVisible(false);
    m_refreshButton->setVisible(true);
    m_errorLabel->setVisible(false);
}

void FRequestProfilePage::loadPage(int page) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string key = glm->getFriendRequestKey(m_sent, page);
    
    if (m_key != key) {
        untoggleAll();
    }
    m_key = key;
    
    m_circle->setVisible(true);
    m_refreshButton->setVisible(false);
    m_errorLabel->setVisible(false);
    m_prevButton->setVisible(page > 0);
    
    bool hasStored = glm->getStoredOnlineLevels(m_key.c_str()) != nullptr;
    m_nextButton->setVisible(hasStored);
    
    cocos2d::CCArray* requests = glm->getStoredOnlineLevels(m_key.c_str());
    if (!requests) {
        setupCommentsBrowser(nullptr);
        glm->getFriendRequests(m_sent, page, m_itemCount);
        m_page = page;
        return;
    }
    
    std::string pageInfo = glm->getPageInfo(m_key.c_str());
    setupPageInfo(pageInfo, m_key.c_str());
    loadFRequestsFinished(requests, m_key.c_str());
    m_page = page;
}

void FRequestProfilePage::onClose(cocos2d::CCObject* sender) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    untoggleAll();
    
    if (m_sent) {
        FRequestProfilePage* page = FRequestProfilePage::create(false);
        page->show();
    } else {
        GJAccountManager* acctMgr = GJAccountManager::sharedState();
        ProfilePage* profile = ProfilePage::create(acctMgr->m_accountID - acctMgr->m_unkInt1, true);
        profile->m_noElasticity = true;
        profile->show();
    }
    
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FRequestProfilePage::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    if (m_uploadPopup == popup) {
        m_uploadPopup = nullptr;
    }
}

void FRequestProfilePage::onDeleteSelected(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* requests = glm->getStoredOnlineLevels(m_key.c_str());
    
    if (!requests) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Nothing here...", "No requests selected.", "OK");
        alert->show();
        return;
    }
    
    bool hasSelected = false;
    for (unsigned int i = 0; i < requests->count(); i++) {
        cocos2d::CCObject* obj = requests->objectAtIndex(i);
        if (*(unsigned char*)((char*)obj + 0x58)) {
            hasSelected = true;
            break;
        }
    }
    
    if (hasSelected) {
        std::string msg = "Are you sure you want to <cr>delete</c> the selected <cg>requests</c>?";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Delete", msg, "Back", "Delete", false, 380.0f, true, 0.0f, 1.0f);
        alert->m_button2->updateBGImage("GJ_button_06.png");
        alert->setTag(3);
        alert->show();
    } else {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Nothing here...", "No requests selected.", "OK");
        alert->show();
    }
}

void FRequestProfilePage::onNextPage(cocos2d::CCObject* sender) {
    loadPage(m_page + 1);
}

void FRequestProfilePage::onPrevPage(cocos2d::CCObject* sender) {
    loadPage(m_page - 1);
}

void FRequestProfilePage::onSentRequests(cocos2d::CCObject* sender) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    untoggleAll();
    
    FRequestProfilePage* page = FRequestProfilePage::create(true);
    page->show();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FRequestProfilePage::onToggleAllObjects(cocos2d::CCObject* sender) {
    m_toggledAll = !m_toggledAll;
    
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* requests = glm->getStoredOnlineLevels(m_key.c_str());
    
    if (requests && requests->count() > 0) {
        for (unsigned int i = 0; i < requests->count(); i++) {
            cocos2d::CCObject* obj = requests->objectAtIndex(i);
            *(unsigned char*)((char*)obj + 0x58) = m_toggledAll;
        }
        // Reload the list view
        if (m_listLayer && m_listLayer->m_list) {
            m_listLayer->m_list->reloadAll();
        }
    }
}

void FRequestProfilePage::onUpdate(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->getTimeLeft("upd_requests", 5.0f) > 5) {
        glm->makeTimeStamp("upd_requests");
        glm->invalidateRequests(m_sent, true);
    }
}

void FRequestProfilePage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void FRequestProfilePage::setupCommentsBrowser(cocos2d::CCArray* scores) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    CustomListView* listView = scores ? CustomListView::create(scores, BoomListType::User, 196.0f, 340.0f) : nullptr;
    
    m_listLayer = GJCommentListLayer::create(listView, "Friend Requests", 
        cocos2d::ccColor4B{0, 0, 0, 0}, 340.0f, 196.0f, false);
    m_mainLayer->addChild(m_listLayer);
    m_listLayer->setPosition(cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, (winSize.height - 196.0f) * 0.5f - 10.0f + 5.0f));
    
    if (m_listLayer && m_listLayer->m_list) {
        int priority = m_listLayer->m_list->getTouchPriority();
        m_buttonMenu->setTouchPriority(priority - 2);
    }
}

void FRequestProfilePage::setupPageInfo(std::string info, const char* key) {
    if (!isCorrect(key)) return;
    
    // Parse info string (format: "itemCount:pageStart:pageEnd")
    cocos2d::CCArray* parts = cocos2d::CCArray::create();
    size_t pos = 0;
    size_t found = info.find(':');
    
    while (found != std::string::npos) {
        std::string part = info.substr(pos, found - pos);
        if (!part.empty()) {
            parts->addObject(cocos2d::CCString::create(part.c_str()));
        }
        pos = found + 1;
        found = info.find(':', pos);
    }
    if (pos < info.length()) {
        std::string part = info.substr(pos);
        if (!part.empty()) {
            parts->addObject(cocos2d::CCString::create(part.c_str()));
        }
    }
    
    if (parts->count() > 2) {
        cocos2d::CCString* itemStr = (cocos2d::CCString*)parts->objectAtIndex(0);
        cocos2d::CCString* startStr = (cocos2d::CCString*)parts->objectAtIndex(1);
        cocos2d::CCString* endStr = (cocos2d::CCString*)parts->objectAtIndex(2);
        m_itemCount = itemStr->intValue();
        m_pageStartIdx = startStr->intValue();
        m_pageEndIdx = endStr->intValue();
        updateLevelsLabel();
        updatePageArrows();
    }
}

void FRequestProfilePage::untoggleAll() {
    GameLevelManager* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* requests = glm->getStoredOnlineLevels(m_key.c_str());
    
    if (requests) {
        for (unsigned int i = 0; i < requests->count(); i++) {
            cocos2d::CCObject* obj = requests->objectAtIndex(i);
            *(unsigned char*)((char*)obj + 0x58) = false;
        }
    }
}

void FRequestProfilePage::updateLevelsLabel() {
    // Update page label with item count
}

void FRequestProfilePage::updatePageArrows() {
    if (m_pageStartIdx > 0) {
        m_prevButton->setVisible(true);
    } else {
        m_prevButton->setVisible(false);
    }
    
    if (m_pageStartIdx + m_pageEndIdx < m_itemCount) {
        m_nextButton->setVisible(true);
    } else {
        m_nextButton->setVisible(false);
    }
}

void FRequestProfilePage::uploadActionFailed(int id, int response) {
    if (m_uploadPopup) {
        std::string msg = "Failed. Please try again later.";
        m_uploadPopup->showFailMessage(msg);
    }
}

void FRequestProfilePage::uploadActionFinished(int id, int response) {
    if (m_uploadPopup) {
        std::string msg = (response == 30) ? "Requests removed" : "Unknown action finished";
        m_uploadPopup->showSuccessMessage(msg);
    }
}

FRequestProfilePage::~FRequestProfilePage() {
    if (m_circle) {
        m_circle->release();
        m_circle = nullptr;
    }
}