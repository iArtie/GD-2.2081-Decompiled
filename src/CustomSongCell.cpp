#include "CustomSongCell.h"
#include "CustomSongWidget.h"
#include "TableView.h"
#include "TableViewCellDelegate.h"
#include "FMODAudioEngine.h"
#include "MusicDownloadManager.h"

CustomSongCell* CustomSongCell::create(const char* identifier, float width, float height)
{
    CustomSongCell* pRet = new CustomSongCell();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomSongCell::init()
{
    if (!TableViewCell::init())
        return false;
    
    m_songInfoObject = nullptr;
    m_selected = false;
    
    return true;
}

void CustomSongCell::draw()
{
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    
    // Draw top and bottom borders
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

void CustomSongCell::loadFromObject(SongInfoObject* object)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_songInfoObject = object;
    
    // Create song widget
    CustomSongWidget* widget = CustomSongWidget::create(
        object,
        this,
        true,   // showPlayButton
        true,   // showDownloadButton
        false,  // showMoreButton
        0,      // bgColor
        0,      // textColor
        false,  // showSize
        false   // isLibrary
    );
    
    m_mainLayer->addChild(widget, 2);
    widget->setPosition(cocos2d::CCPoint(m_width * 0.5f, m_height * 0.5f));
    widget->m_isInCell = true;
    
    // Add delete button
    cocos2d::CCSprite* trashSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    trashSprite->setScale(0.6f);
    
    CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(
        trashSprite, nullptr, this, menu_selector(CustomSongCell::onDelete)
    );
    deleteBtn->setSizeMult(1.5f);
    
    cocos2d::CCMenu* deleteMenu = cocos2d::CCMenu::create(deleteBtn, nullptr);
    m_mainLayer->addChild(deleteMenu, 3);
    deleteMenu->setPosition(cocos2d::CCPoint(m_width * 0.5f + 90.0f, m_height * 0.5f - 24.0f));
    
    m_selected = (m_songInfoObject->m_songID == getActiveSongID());
}

void CustomSongCell::onDelete(cocos2d::CCObject* sender)
{
    if (!m_songInfoObject)
        return;
    
    // Check if song is currently playing
    std::string path = MusicDownloadManager::sharedState()->pathForSong(m_songInfoObject->m_songID);
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(path))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    }
    
    // Delete the song
    MusicDownloadManager::sharedState()->deleteSong(m_songInfoObject->m_songID);
    
    // Remove cell from table view
    m_tableView->deleteTableViewCell(this);
}

bool CustomSongCell::shouldReload()
{
    return (m_songInfoObject->m_songID == getActiveSongID()) != m_selected;
}

int CustomSongCell::getActiveSongID()
{
    TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
    if (delegate)
        return delegate->getSelectedCellIdx();
    return 0;
}

void CustomSongCell::songIDChanged(int id)
{
    TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
    if (delegate)
        delegate->cellPerformedAction(this, 15, Click, 0);
}

LevelSettingsObject* CustomSongCell::getLevelSettings()
{
    return nullptr;
}

std::string CustomSongCell::getSongFileName()
{
    return "";
}

void CustomSongCell::updateBGColor(int index)
{
    unsigned char color = (index & 1) ? 50 : 75;
    m_backgroundLayer->setColor(cocos2d::ccColor3B(color, color, color));
    m_backgroundLayer->setOpacity(255);
}

CustomSongCell::~CustomSongCell()
{
    // Base class handles cleanup
}