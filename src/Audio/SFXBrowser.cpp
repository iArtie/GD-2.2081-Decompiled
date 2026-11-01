#include "Audio/SFXBrowser.h"
#include "Delegates/SFXBrowserDelegate.h"
#include "Models/SFXSearchResult.h"
#include "Models/SFXFolderObject.h"
#include "Models/SFXInfoObject.h"
#include "Managers/MusicDownloadManager.h"
#include "Managers/GameManager.h"
#include "Layers/GJCommentListLayer.h"
#include "UI/CustomListView.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"
#include "Sprites/LoadingCircleSprite.h"
#include "Popups/TextAlertPopup.h"
#include "Layers/URLViewLayer.h"
#include "Popups/SearchSFXPopup.h"
#include "Layers/SelectSFXSortLayer.h"

SFXBrowser* SFXBrowser::create(int id)
{
    SFXBrowser* instance = new SFXBrowser();
    if (instance->init(id))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SFXBrowser::init(int id)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_sfxID = id;
    m_sortType = AudioSortType::Default;
    m_compactMode = GameManager::sharedState()->getGameVariable("0148");
    m_cellAmount = m_compactMode ? 40 : 20;
    m_libraryVersion = 0;
    m_snapToSelected = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(420.0f, 280.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_circleSprite = LoadingCircleSprite::create();
    addChild(m_circleSprite, 100);

    trySetupSFXBrowser();

    return true;
}

int SFXBrowser::getSelectedCellIdx()
{
    return 0;
}

void SFXBrowser::keyBackClicked()
{
    onClose(nullptr);
}

void SFXBrowser::musicActionFailed(GJMusicAction action)
{
    if (m_circleSprite)
        LoadingCircleSprite::hideCircle(m_circleSprite);

    if (action == GJMusicAction::UpdateSFXLibrary && !m_autoUpdating)
    {
        TextAlertPopup* popup = TextAlertPopup::create("SFX Library is the latest version.", 1.0f, 0.5f, 150, "");
        addChild(popup, 100);
    }
}

void SFXBrowser::musicActionFinished(GJMusicAction action)
{
    if (action == GJMusicAction::DownloadOrUpdate && m_gettingURL)
        onUpdateLibrary(nullptr);
    else
        trySetupSFXBrowser();
}

void SFXBrowser::onClearSearch(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    m_searchResult->m_searchQuery = "";
    m_searchResult->m_startIndex = 0;
    m_searchResult->updateObjects(m_sortType);
    setupList(m_searchResult);
}

void SFXBrowser::onClose(cocos2d::CCObject* sender)
{
    if (m_delegate)
        m_delegate->sfxBrowserClosed(this);

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SFXBrowser::onCredits(cocos2d::CCObject* sender)
{
    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    if (manager->m_sfxArtists && manager->m_sfxArtists->count() > 0)
    {
        URLViewLayer* layer = URLViewLayer::create("SFX Credits", manager->m_sfxArtists);
        addChild(layer, 100);
        layer->showLayer(false);
    }
}

void SFXBrowser::onExitFolder(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    SFXFolderObject* activeFolder = m_searchResult->getActiveFolder();
    if (!activeFolder)
        return;

    m_sfxID = activeFolder->m_sfxID;

    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    SFXFolderObject* parentFolder = manager->getSFXFolderObjectForID(activeFolder->m_folderID);
    m_searchResult->setActiveFolder(parentFolder);
    m_searchResult->updateObjects(m_sortType);
    m_searchResult->m_startIndex = m_searchResult->getSelectedPage(m_sfxID, m_cellAmount);
    setupList(m_searchResult);
}

void SFXBrowser::onPage(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    unsigned int totalObjects = m_searchResult->m_filterObjects->count();
    int startIndex = m_searchResult->m_startIndex;
    int maxPage = (int)ceilf((float)(totalObjects / m_cellAmount));
    int newPage = startIndex + ((sender->getTag() == 1) ? 1 : -1);

    if (newPage > maxPage || newPage < 0)
    {
        if (newPage == -1)
            onExitFolder(sender);
    }
    else
    {
        m_searchResult->m_startIndex = newPage;
        setupList(m_searchResult);
    }
}

void SFXBrowser::onSearch(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    SearchSFXPopup* popup = SearchSFXPopup::create(m_searchResult->m_searchQuery);
    popup->m_delegate = this;
    popup->m_ZOrder = getZOrder() + 1;
    popup->show();
}

void SFXBrowser::onSorting(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    SelectSFXSortLayer* layer = SelectSFXSortLayer::create(m_sortType);
    layer->m_delegate = this;
    layer->show();
}

void SFXBrowser::onToggleCompactMode(cocos2d::CCObject* sender)
{
    if (!m_searchResult)
        return;

    m_compactMode = GameManager::sharedState()->toggleGameVariable("0148");
    m_cellAmount = m_compactMode ? 40 : 20;
    m_searchResult->m_startIndex = 0;
    setupList(m_searchResult);
}

void SFXBrowser::onUpdateLibrary(cocos2d::CCObject* sender)
{
    m_autoUpdating = (sender == nullptr);

    if (!MusicDownloadManager::sharedState()->m_customContentURL.empty())
    {
        m_gettingURL = false;
        MusicDownloadManager::sharedState()->tryUpdateSFXLibrary();
    }
    else
    {
        m_gettingURL = true;
        MusicDownloadManager::sharedState()->getCustomContentURL();
    }
}

void SFXBrowser::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void SFXBrowser::setTextPopupClosed(SetTextPopup* popup, std::string text)
{
    if (!m_searchResult)
        return;

    m_searchResult->m_searchQuery = text;
    m_searchResult->m_startIndex = 0;
    m_searchResult->updateObjects(m_sortType);
    setupList(m_searchResult);
}

void SFXBrowser::setupList(SFXSearchResult* searchResult)
{
    if (!searchResult)
        return;

    if (m_libraryVersion > 0 && m_libraryVersion != MusicDownloadManager::sharedState()->m_sfxLibraryVersion)
    {
        std::string text = cocos2d::CCString::createWithFormat(
            "SFX Library Updated to version %i",
            MusicDownloadManager::sharedState()->m_sfxLibraryVersion)->getCString();

        TextAlertPopup* popup = TextAlertPopup::create(text, 1.0f, 0.5f, 150, "");
        addChild(popup, 100);
    }

    m_libraryVersion = MusicDownloadManager::sharedState()->m_sfxLibraryVersion;

    std::string infoText = cocos2d::CCString::createWithFormat(
        "SFX Library Version %i. SFX Count: %i",
        MusicDownloadManager::sharedState()->m_sfxLibraryVersion,
        MusicDownloadManager::sharedState()->m_sfxObjectCount)->getCString();

    m_infoLabel->setString(infoText.c_str());

    if (m_circleSprite)
        LoadingCircleSprite::hideCircle(m_circleSprite);

    if (m_sfxObjects)
    {
        m_sfxObjects->release();
        m_sfxObjects = nullptr;
    }

    if (m_listLayer)
    {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }

    if (m_searchResult != searchResult)
    {
        if (m_searchResult)
            m_searchResult->release();

        m_searchResult = searchResult;
        m_searchResult->retain();
    }

    m_clearSearchButton->setVisible(!m_searchResult->m_searchQuery.empty());
    m_backButton->setVisible(m_searchResult->getActiveFolder() != nullptr &&
                             m_searchResult->getActiveFolder()->m_folderID > 0 &&
                             m_searchResult->m_searchQuery.empty());

    std::string searchLabel = " ";
    if (!m_searchResult->m_searchQuery.empty())
        searchLabel = "\"" + m_searchResult->m_searchQuery + "\"";

    m_searchLabel->setString(searchLabel.c_str());
    m_searchLabel->limitLabelWidth(100.0f, 0.8f, 0.0f);

    std::string nameLabel;
    std::string pathLabel;

    if (m_searchResult->getActiveFolder())
    {
        SFXFolderObject* active = m_searchResult->getActiveFolder();
        nameLabel = active->m_name;
        pathLabel = MusicDownloadManager::sharedState()->getSFXFolderPathForID(active->m_sfxID, true);
    }

    m_nameLabel->setString(nameLabel.c_str());
    m_nameLabel->limitLabelWidth(150.0f, 0.5f, 0.0f);

    m_pathLabel->setString(pathLabel.c_str());
    m_pathLabel->limitLabelWidth(150.0f, 0.6f, 0.0f);

    cocos2d::CCPoint pathPos = m_nameLabel->getPosition();
    pathPos.x -= m_nameLabel->getContentSize().width * m_nameLabel->getScale() + 2.0f;
    m_pathLabel->setPosition(pathPos);

    cocos2d::CCArray* visibleObjects = cocos2d::CCArray::create();
    unsigned int startIdx = m_searchResult->m_startIndex * m_cellAmount;
    unsigned int endIdx = startIdx + m_cellAmount;

    for (unsigned int i = startIdx; i < endIdx && i < m_searchResult->m_filterObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_searchResult->m_filterObjects->objectAtIndex(i);
        visibleObjects->addObject(obj);
    }

    m_sfxObjects = visibleObjects;
    m_sfxObjects->retain();

    CustomListView* listView = CustomListView::create(m_sfxObjects, BoomListType::Default, 220.0f, 340.0f);
    m_listLayer = GJCommentListLayer::create(listView, "", cocos2d::ccc4(255, 255, 255, 255), 340.0f, 220.0f, true);
    m_mainLayer->addChild(m_listLayer, 10);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_listLayer->setPosition(cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, (winSize.height - 220.0f) * 0.5f - 10.0f));

    m_refreshButton->setVisible(true);
    updatePageLabel();
}

void SFXBrowser::setupSFXBrowser()
{
    if (MusicDownloadManager::sharedState()->m_customContentURL.empty())
        MusicDownloadManager::sharedState()->getCustomContentURL();

    SFXSearchResult* searchResult = SFXSearchResult::create(m_sfxID);

    SFXFolderObject* folder = MusicDownloadManager::sharedState()->getSFXFolderObjectForID(m_sfxID);
    if (!folder)
        folder = MusicDownloadManager::sharedState()->getSFXFolderObjectForID(1);

    searchResult->setActiveFolder(folder);
    searchResult->updateObjects(m_sortType);
    searchResult->m_startIndex = searchResult->getSelectedPage(m_sfxID, m_cellAmount);

    m_snapToSelected = true;
    setupList(searchResult);
    m_snapToSelected = false;

    if (!MusicDownloadManager::sharedState()->m_requestedSfxLibraryDownloaded)
        onUpdateLibrary(nullptr);
}

bool SFXBrowser::shouldSnapToSelected()
{
    return m_snapToSelected;
}

void SFXBrowser::sortSelectClosed(SelectSFXSortLayer* layer)
{
    if (!m_searchResult)
        return;

    m_sortType = layer->m_sortType;
    m_searchResult->updateObjects(m_sortType);
    m_searchResult->m_startIndex = 0;
    setupList(m_searchResult);
}

void SFXBrowser::trySetupSFXBrowser()
{
    if (MusicDownloadManager::sharedState()->m_sfxObjects)
    {
        setupSFXBrowser();
    }
    else if (!MusicDownloadManager::sharedState()->m_customContentURL.empty())
    {
        MusicDownloadManager::sharedState()->downloadSFXLibrary();
    }
    else
    {
        MusicDownloadManager::sharedState()->getCustomContentURL();
    }
}

void SFXBrowser::updatePageLabel()
{
    int totalObjects = m_searchResult->m_filterObjects->count();

    if (totalObjects <= 0)
    {
        m_pageLabel->setString("No matches");
        m_prevButton->setVisible(false);
        m_nextButton->setVisible(false);
        return;
    }

    int maxPage = (totalObjects - 1) / m_cellAmount;
    int startIdx = m_cellAmount * m_searchResult->m_startIndex;
    int endIdx = std::min(m_cellAmount + startIdx, totalObjects);

    std::string text = cocos2d::CCString::createWithFormat(
        "%i to %i of %i", startIdx + 1, endIdx, totalObjects)->getCString();

    m_pageLabel->setString(text.c_str());

    m_prevButton->setVisible(m_searchResult->m_startIndex > 0);
    m_nextButton->setVisible(m_searchResult->m_startIndex < maxPage);
}

SFXBrowser::~SFXBrowser()
{
}