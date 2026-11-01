#include "Cells/CustomMusicCell.h"
#include "UI/CustomSongWidget.h"
#include "UI/TableView.h"
#include "Delegates/TableViewCellDelegate.h"

CustomMusicCell* CustomMusicCell::create(const char* identifier, float width, float height)
{
    CustomMusicCell* pRet = new CustomMusicCell();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomMusicCell::init()
{
    if (!CustomSongCell::init())
        return false;
    
    m_selected = false;
    m_songInfoObject = nullptr;
    
    return true;
}

void CustomMusicCell::loadFromObject(SongInfoObject* object)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_songInfoObject = object;
    
    bool isLibrary = false;
    TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
    if (delegate)
        isLibrary = (delegate->getCellDelegateType() == 1);
    else
        isLibrary = true;
    
    CustomSongWidget* widget = CustomSongWidget::create(
        m_songInfoObject,
        this,
        true,  // showPlayButton
        true,  // showDownloadButton
        true,  // showMoreButton
        0,     // bgColor
        0,     // textColor
        true,  // showSize
        isLibrary
    );
    
    m_mainLayer->addChild(widget, 2);
    widget->setPosition(cocos2d::CCPoint(m_width * 0.5f, m_height * 0.5f));
    widget->m_isInCell = true;
    widget->addExtraVisuals(m_songInfoObject->m_isNew, m_songInfoObject->m_libraryOrder > 0);
    
    m_selected = (m_songInfoObject->m_songID == getActiveSongID());
}

void CustomMusicCell::updateBGColor(int index)
{
    unsigned char color = 50;
    if ((index & 1) == 0)
        color = 75;
    
    m_backgroundLayer->setColor(cocos2d::ccColor3B(color, color, color));
    m_backgroundLayer->setOpacity(255);
}

CustomMusicCell::~CustomMusicCell()
{
    // Base class handles cleanup
}