#include "MPLobbyLayer.h"
#include "FLAlertLayer.h"
#include "GJMultiplayerManager.h"
#include "MultiplayerLayer.h"
#include "ShareCommentLayer.h"
#include "UploadActionPopup.h"
#include "BoomListView.h"
#include "CCTextInputNode.h"
#include "ButtonSprite.h"
#include "AppDelegate.h"

void MPLobbyLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (btn2 && layer->getTag() == 1)
        tryExitLobby();
}

MPLobbyLayer* MPLobbyLayer::create(int type)
{
    MPLobbyLayer* ret = new MPLobbyLayer();
    if (ret->init(type)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MPLobbyLayer::didUploadMPComment(int id)
{
    m_lobbyDelay = 2.0f;
}

void MPLobbyLayer::joinLobbyFailed(int id, GJMPErrorCode errorType)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
        "MPLobbyLayer::joinLobbyFailed: %i - %i", id, (int)errorType);
    m_lobbyLabel->setString(str->getCString());
    updateLobby(m_lobbyDelay);
}

void MPLobbyLayer::joinLobbyFinished(int id)
{
    m_lobbyID = id;

    GJMultiplayerManager* mpManager = GJMultiplayerManager::sharedState();
    cocos2d::CCArray* array = (cocos2d::CCArray*)mpManager->m_scoreArrays->objectForKey(
        cocos2d::CCString::createWithFormat("%i", m_lobbyID)->getCString());

    if (array) {
        std::string result = cocos2d::CCString::createWithFormat("Game: %i", m_lobbyID)->getCString();

        for (unsigned int i = 0; i < array->count(); ++i) {
            cocos2d::CCObject* obj = array->objectAtIndex(i);
            unsigned int uid = obj[6].m_uID;
            std::string name = (const char*)&obj[5].m_unknown2;

            struct timespec ts;
            clock_gettime(0, &ts);
            double now = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;

            cocos2d::CCString* line = cocos2d::CCString::createWithFormat(
                "\n%i: %s - Last Action: %f",
                uid, name.c_str(),
                (now - *(double*)&obj[10].__vftable) / 1000.0);
            result += line->getCString();
        }
        m_lobbyLabel->setString(result.c_str());
    } else {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat(
            "MPLobbyLayer::joinLobbyFinished: %i", id);
        m_lobbyLabel->setString(str->getCString());
    }

    m_awaitingLobby = false;
    updateComments();

    float delay;
    if (m_awaitingLobby) {
        delay = 2.0f;
        m_lobbyDelay = 2.0f;
    } else {
        delay = m_lobbyDelay + 0.5f;
        if (delay > 10.0f)
            delay = 10.0f;
        m_lobbyDelay = delay;
    }
    updateLobby(delay);
}

void MPLobbyLayer::keyBackClicked()
{
    onBack(nullptr);
}

void MPLobbyLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == KEY_Enter)
        postComment();
}

void MPLobbyLayer::keyUp(cocos2d::enumKeyCodes key, double timestamp)
{
}

void MPLobbyLayer::onBack(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = MultiplayerLayer::scene();
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MPLobbyLayer::onBtn1(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this, "Exit Game",
        "Do you want to leave the lobby and exit the multiplayer game?",
        "Cancel", "Exit", 380.0f);
    alert->setTag(1);
    alert->show();
}

void MPLobbyLayer::onBtn2(cocos2d::CCObject* sender)
{
}

void MPLobbyLayer::onBtn3(cocos2d::CCObject* sender)
{
}

void MPLobbyLayer::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();
    if (m_uploadPopup == popup) {
        if (m_uploadPopup->m_succeeded && m_uploadPopup->getTag() == 1)
            onBack(nullptr);
        m_uploadPopup->m_delegate = nullptr;
        m_uploadPopup = nullptr;
    }
}

void MPLobbyLayer::onComment(cocos2d::CCObject* sender)
{
    ShareCommentLayer* layer = ShareCommentLayer::create(
        "Add Comment", 100, CommentType::Multiplayer, m_lobbyID, "");
    layer->show();
}

void MPLobbyLayer::onUpdateLobby()
{
    if (!m_lobbyJoined) {
        GJMultiplayerManager* mpManager = GJMultiplayerManager::sharedState();
        mpManager->joinLobby(m_lobbyID);
    }
}

void MPLobbyLayer::postComment()
{
    std::string comment = m_commentInput->getString();
    if (comment.empty())
        return;

    GJMultiplayerManager* mpManager = GJMultiplayerManager::sharedState();
    mpManager->uploadComment(m_commentInput->getString(), m_lobbyID);
    m_commentInput->setString("");
}

cocos2d::CCScene* MPLobbyLayer::scene(int type)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    MPLobbyLayer* layer = MPLobbyLayer::create(type);
    scene->addChild(layer);
    return scene;
}

void MPLobbyLayer::textChanged(CCTextInputNode* node)
{
}

void MPLobbyLayer::textInputClosed(CCTextInputNode* node)
{
}

void MPLobbyLayer::textInputOpened(CCTextInputNode* node)
{
}

void MPLobbyLayer::tryExitLobby()
{
    GJMultiplayerManager::sharedState()->m_uploadDelegate = this;

    GJMultiplayerManager* mpManager = GJMultiplayerManager::sharedState();
    if (mpManager->exitLobby(m_lobbyID)) {
        if (m_uploadPopup) {
            m_uploadPopup->m_delegate = nullptr;
            m_uploadPopup = nullptr;
        }
        m_uploadPopup = UploadActionPopup::create(this, "Leaving game...");
        m_uploadPopup->setTag(1);
        m_uploadPopup->show();
    }
}

void MPLobbyLayer::updateComments()
{
    GJMultiplayerManager* mpManager = GJMultiplayerManager::sharedState();
    cocos2d::CCArray* comments = (cocos2d::CCArray*)mpManager->m_lobbyComments->objectForKey(
        cocos2d::CCString::createWithFormat("%i", m_lobbyID)->getCString());

    if (comments && comments->count()) {
        bool added = false;
        BoomListView* list = m_listLayer->m_list;

        for (unsigned int i = 0; i < comments->count(); ++i) {
            cocos2d::CCObject* obj = comments->objectAtIndex(i);
            if (!list->m_entries->containsObject(obj)) {
                added = true;
                list->addObjectToList((cocos2d::CCNode*)obj);
            }
        }

        if (added) {
            list->m_tableView->reloadData();
            list->m_tableView->doConstraintContent(true);
            m_awaitingLobby = true;
        }
    }
}

void MPLobbyLayer::updateLobby(float delay)
{
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCCallFunc::create(this, callfunc_selector(MPLobbyLayer::onUpdateLobby)),
        nullptr);
    this->runAction(seq);
}

void MPLobbyLayer::uploadActionFailed(int id, int response)
{
    if (m_lobbyID == id && m_uploadPopup) {
        std::string message = "Unknown action failed";
        if (response == 58)
            message = "Could not exit game";
        m_uploadPopup->showFailMessage(message);
    }
}

void MPLobbyLayer::uploadActionFinished(int id, int response)
{
    if (m_lobbyID == id && m_uploadPopup) {
        std::string message = "Unknown action finished";
        if (response == 58) {
            message = "Did exit game";
            m_lobbyJoined = true;
        }
        m_uploadPopup->showSuccessMessage(message);

        if (m_lobbyJoined) {
            m_uploadPopup->m_delegate = nullptr;
            m_uploadPopup = nullptr;
            onBack(nullptr);
        }
    }
}

MPLobbyLayer::~MPLobbyLayer()
{
}