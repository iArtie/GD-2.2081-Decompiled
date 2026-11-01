#include "Cells/CustomSFXCell.h"
#include "UI/CustomSFXWidget.h"
#include "UI/TableView.h"
#include "Delegates/TableViewCellDelegate.h"

CustomSFXCell* CustomSFXCell::create(const char* identifier, float width, float height)
{
    CustomSFXCell* pRet = new CustomSFXCell();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomSFXCell::init()
{
    if (!TableViewCell::init())
        return false;
    
    m_sfxObject = nullptr;
    m_compactMode = false;
    m_selected = false;
    m_listType = 0;
    
    return true;
}

void CustomSFXCell::draw()
{
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    
    // Draw top and bottom borders
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

void CustomSFXCell::loadFromObject(SFXInfoObject* object)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_sfxObject = object;
    
    CustomSFXWidget* widget = CustomSFXWidget::create(
        object,
        this,
        true,   // showPlayButton
        true,   // showDownloadButton
        true,   // showMoreButton
        0,      // bgColor
        m_compactMode
    );
    
    m_mainLayer->addChild(widget, 2);
    widget->setPosition(cocos2d::CCPoint(m_width * 0.5f, m_height * 0.5f));
    m_selected = (m_sfxObject->m_sfxID == getActiveSFXID());
}

void CustomSFXCell::sfxObjectSelected(SFXInfoObject* object)
{
    TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
    if (delegate)
        delegate->cellPerformedAction(this, m_listType, Click, 0);
}

bool CustomSFXCell::shouldReload()
{
    return (m_sfxObject->m_sfxID == getActiveSFXID()) != m_selected;
}

int CustomSFXCell::getActiveSFXID()
{
    TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
    if (delegate)
        return delegate->getSelectedCellIdx();
    return 0;
}

void CustomSFXCell::updateBGColor(int index)
{
    unsigned char r, g, b;
    
    if (!m_compactMode)
    {
        r = g = b = (index & 1) ? 50 : 75;
    }
    else if (m_sfxObject)
    {
        if (m_sfxObject->m_sfxID == getActiveSFXID())
        {
            r = 200;
            g = 140;
            b = 50;
            m_backgroundLayer->setColor(cocos2d::ccColor3B(r, g, b));
            m_backgroundLayer->setOpacity(255);
            return;
        }
        r = g = b = (index & 1) ? 100 : 75;
    }
    else
    {
        r = g = b = (index & 1) ? 50 : 75;
    }
    
    m_backgroundLayer->setColor(cocos2d::ccColor3B(r, g, b));
    m_backgroundLayer->setOpacity(255);
}

CustomSFXCell::~CustomSFXCell()
{
    // Base class handles cleanup
}