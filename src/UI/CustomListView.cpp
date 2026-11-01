#include "UI/CustomListView.h"
#include "Cells/GJUserCell.h"
#include "Cells/StatsCell.h"
#include "Cells/AchievementCell.h"
#include "Cells/LevelCell.h"
#include "Cells/CommentCell.h"
#include "Cells/SongCell.h"
#include "Cells/GJScoreCell.h"
#include "Cells/MapPackCell.h"
#include "Cells/CustomSongCell.h"
#include "Cells/GJRequestCell.h"
#include "Cells/GJMessageCell.h"
#include "Cells/GJLevelScoreCell.h"
#include "Cells/ArtistCell.h"
#include "Cells/SmartTemplateCell.h"
#include "Cells/CustomSFXCell.h"
#include "Cells/CustomMusicCell.h"
#include "Cells/OptionsCell.h"
#include "Cells/LevelListCell.h"
#include "Cells/GJLocalLevelScoreCell.h"
#include "Cells/URLCell.h"
#include "Managers/GameManager.h"

CustomListView* CustomListView::create(cocos2d::CCArray* entries, BoomListType type, float height, float width)
{
    CustomListView* pRet = new CustomListView();
    if (pRet && pRet->init(entries, type, height, width))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

float CustomListView::getCellHeight(BoomListType type)
{
    // Static array of cell heights for different types
    static float cellHeights[] = {
        90.0f, 90.0f, 60.0f, 60.0f, 90.0f, 90.0f, 60.0f, 60.0f, 60.0f, 60.0f,
        50.0f, 70.0f, 90.0f, 70.0f, 60.0f, 60.0f, 70.0f, 60.0f, 60.0f, 60.0f,
        60.0f, 70.0f, 70.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 70.0f,
        60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f
    };
    
    int index = type - 2;
    if (index >= 0 && index < 38)
        return cellHeights[index];
    return 90.0f;
}

TableViewCell* CustomListView::getListCell(const char* identifier)
{
    float cellHeight = CustomListView::getCellHeight(m_type);
    TableViewCell* cell = nullptr;
    
    switch (m_type)
    {
        case User:
        case User2:
            cell = GJUserCell::create(identifier, m_width, cellHeight);
            break;
            
        case Stats:
            cell = StatsCell::create(identifier, m_width, cellHeight);
            break;
            
        case Achievement:
            cell = AchievementCell::create(identifier, m_width, cellHeight);
            break;
            
        case Level:
        case Level2:
            cell = LevelCell::create(identifier, m_width, cellHeight);
            break;
            
        case Comment:
        case Comment4:
            cell = CommentCell::create(identifier, m_width, cellHeight);
            break;
            
        case Comment2:
        case Comment3:
            cell = CommentCell::create(identifier, m_width, cellHeight);
            ((CommentCell*)cell)->m_compactMode = true;
            break;
            
        case Song:
            cell = SongCell::create(identifier, m_width, cellHeight);
            break;
            
        case Score:
            cell = GJScoreCell::create(identifier, m_width, cellHeight);
            break;
            
        case MapPack:
            cell = MapPackCell::create(identifier, m_width, cellHeight);
            break;
            
        case CustomSong:
            cell = CustomSongCell::create(identifier, m_width, cellHeight);
            break;
            
        case Request:
            cell = GJRequestCell::create(identifier, m_width, cellHeight);
            break;
            
        case Message:
            cell = GJMessageCell::create(identifier, m_width, cellHeight);
            break;
            
        case LevelScore:
            cell = GJLevelScoreCell::create(identifier, m_width, cellHeight);
            break;
            
        case Artist:
            cell = ArtistCell::create(identifier, m_width, cellHeight);
            break;
            
        case SmartTemplate:
            cell = SmartTemplateCell::create(identifier, m_width, cellHeight);
            break;
            
        case SFX:
            cell = CustomSFXCell::create(identifier, m_width, cellHeight);
            break;
            
        case SFX2:
            cell = CustomSFXCell::create(identifier, m_width, cellHeight);
            ((CustomSFXCell*)cell)->m_compactMode = true;
            break;
            
        case CustomMusic:
            cell = CustomMusicCell::create(identifier, m_width, cellHeight);
            break;
            
        case Options:
            cell = OptionsCell::create(identifier, m_width, cellHeight);
            break;
            
        case LevelList:
        case LevelList2:
            cell = LevelListCell::create(identifier, m_width, cellHeight);
            break;
            
        case Level3:
        case Level4:
            cell = LevelCell::create(identifier, m_width, cellHeight);
            ((LevelCell*)cell)->m_compactView = true;
            break;
            
        case LevelList3:
            cell = LevelListCell::create(identifier, m_width, cellHeight);
            ((LevelListCell*)cell)->m_addingLevel = true;
            break;
            
        case LocalLevelScore:
            cell = GJLocalLevelScoreCell::create(identifier, m_width, cellHeight);
            break;
            
        case URL:
            cell = URLCell::create(identifier, m_width, cellHeight);
            break;
            
        default:
            cell = BoomListView::getListCell(identifier);
            break;
    }
    
    return cell;
}

void CustomListView::loadCell(TableViewCell* cell, int index)
{
    switch (m_type)
    {
        case User:
        case User2:
        {
            GJUserScore* score = (GJUserScore*)m_entries->objectAtIndex(index);
            ((GJUserCell*)cell)->updateBGColor(index);
            ((GJUserCell*)cell)->loadFromScore(score);
            break;
        }
        
        case Stats:
        {
            StatsObject* obj = (StatsObject*)m_entries->objectAtIndex(index);
            ((StatsCell*)cell)->loadFromObject(obj);
            ((StatsCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Achievement:
        {
            cocos2d::CCDictionary* dict = (cocos2d::CCDictionary*)m_entries->objectAtIndex(index);
            ((AchievementCell*)cell)->loadFromDict(dict);
            ((AchievementCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Level:
        case Level2:
        case Level3:
        case Level4:
        {
            GJGameLevel* level = (GJGameLevel*)m_entries->objectAtIndex(index);
            ((LevelCell*)cell)->loadFromLevel(level);
            ((LevelCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Comment:
        case Comment2:
        case Comment3:
        case Comment4:
        {
            GJComment* comment = (GJComment*)m_entries->objectAtIndex(index);
            ((CommentCell*)cell)->loadFromComment(comment);
            ((CommentCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Song:
        {
            SongObject* song = (SongObject*)m_entries->objectAtIndex(index);
            ((SongCell*)cell)->loadFromObject(song);
            ((SongCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Score:
        {
            GJUserScore* score = (GJUserScore*)m_entries->objectAtIndex(index);
            ((GJScoreCell*)cell)->updateBGColor(index);
            ((GJScoreCell*)cell)->loadFromScore(score);
            break;
        }
        
        case MapPack:
        {
            GJMapPack* pack = (GJMapPack*)m_entries->objectAtIndex(index);
            ((MapPackCell*)cell)->loadFromMapPack(pack);
            ((MapPackCell*)cell)->updateBGColor(index);
            break;
        }
        
        case CustomSong:
        {
            SongInfoObject* song = (SongInfoObject*)m_entries->objectAtIndex(index);
            ((CustomSongCell*)cell)->loadFromObject(song);
            ((CustomSongCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Request:
        {
            GJUserScore* score = (GJUserScore*)m_entries->objectAtIndex(index);
            ((GJRequestCell*)cell)->updateBGColor(index);
            ((GJRequestCell*)cell)->loadFromScore(score);
            break;
        }
        
        case Message:
        {
            GJUserMessage* msg = (GJUserMessage*)m_entries->objectAtIndex(index);
            ((GJMessageCell*)cell)->loadFromMessage(msg);
            ((GJMessageCell*)cell)->updateBGColor(index);
            break;
        }
        
        case LevelScore:
        {
            GJUserScore* score = (GJUserScore*)m_entries->objectAtIndex(index);
            ((GJLevelScoreCell*)cell)->updateBGColor(index);
            ((GJLevelScoreCell*)cell)->loadFromScore(score);
            break;
        }
        
        case Artist:
        {
            SongInfoObject* song = (SongInfoObject*)m_entries->objectAtIndex(index);
            ((ArtistCell*)cell)->loadFromObject(song);
            ((ArtistCell*)cell)->updateBGColor(index);
            break;
        }
        
        case SmartTemplate:
        {
            GJSmartTemplate* tmpl = (GJSmartTemplate*)m_entries->objectAtIndex(index);
            ((SmartTemplateCell*)cell)->loadFromObject(tmpl);
            ((SmartTemplateCell*)cell)->updateBGColor(index);
            break;
        }
        
        case SFX:
        case SFX2:
        {
            SFXInfoObject* sfx = (SFXInfoObject*)m_entries->objectAtIndex(index);
            ((CustomSFXCell*)cell)->loadFromObject(sfx);
            ((CustomSFXCell*)cell)->updateBGColor(index);
            break;
        }
        
        case CustomMusic:
        {
            SongInfoObject* song = (SongInfoObject*)m_entries->objectAtIndex(index);
            ((CustomMusicCell*)cell)->loadFromObject(song);
            ((CustomMusicCell*)cell)->updateBGColor(index);
            break;
        }
        
        case Options:
        {
            OptionsObject* obj = (OptionsObject*)m_entries->objectAtIndex(index);
            ((OptionsCell*)cell)->loadFromObject(obj);
            ((OptionsCell*)cell)->updateBGColor(index);
            break;
        }
        
        case LevelList:
        case LevelList2:
        case LevelList3:
        {
            GJLevelList* list = (GJLevelList*)m_entries->objectAtIndex(index);
            ((LevelListCell*)cell)->loadFromList(list);
            ((LevelListCell*)cell)->updateBGColor(index);
            break;
        }
        
        case LocalLevelScore:
        {
            GJLocalScore* score = (GJLocalScore*)m_entries->objectAtIndex(index);
            ((GJLocalLevelScoreCell*)cell)->loadFromScore(score);
            ((GJLocalLevelScoreCell*)cell)->updateBGColor(index);
            break;
        }
        
        case URL:
        {
            CCURLObject* url = (CCURLObject*)m_entries->objectAtIndex(index);
            ((URLCell*)cell)->loadFromObject(url);
            ((URLCell*)cell)->updateBGColor(index);
            break;
        }
        
        default:
            BoomListView::loadCell(cell, index);
            break;
    }
}

void CustomListView::reloadAll()
{
    if (!m_tableView || !m_tableView->m_cellArray)
        return;
    
    cocos2d::CCArray* cells = m_tableView->m_cellArray;
    
    switch (m_type)
    {
        case Level:
        case Level2:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                LevelCell* cell = (LevelCell*)cells->objectAtIndex(i);
                if (cell)
                    LevelCell::updateToggle(cell);
            }
            break;
            
        case MapPack:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                MapPackCell* cell = (MapPackCell*)cells->objectAtIndex(i);
                if (cell)
                    MapPackCell::reloadCell(cell);
            }
            break;
            
        case CustomSong:
        case CustomMusic:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                CustomSongCell* cell = (CustomSongCell*)cells->objectAtIndex(i);
                if (cell && CustomSongCell::shouldReload(cell))
                {
                    cell->loadFromObject(cell->m_songInfoObject);
                    cell->updateBGColor(i);
                }
            }
            break;
            
        case Request:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                GJRequestCell* cell = (GJRequestCell*)cells->objectAtIndex(i);
                if (cell)
                    GJRequestCell::updateToggle(cell);
            }
            break;
            
        case Message:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                GJMessageCell* cell = (GJMessageCell*)cells->objectAtIndex(i);
                if (cell)
                    GJMessageCell::updateToggle(cell);
            }
            break;
            
        case SmartTemplate:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                SmartTemplateCell* cell = (SmartTemplateCell*)cells->objectAtIndex(i);
                if (cell)
                {
                    cell->loadFromObject(cell->m_smartTemplate);
                    cell->updateBGColor(i);
                }
            }
            break;
            
        case SFX:
        case SFX2:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                CustomSFXCell* cell = (CustomSFXCell*)cells->objectAtIndex(i);
                if (cell && CustomSFXCell::shouldReload(cell))
                {
                    cell->loadFromObject(cell->m_sfxObject);
                    cell->updateBGColor(i);
                }
            }
            break;
            
        case Options:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                OptionsCell* cell = (OptionsCell*)cells->objectAtIndex(i);
                if (cell)
                {
                    cell->loadFromObject(cell->m_optionsObject);
                    cell->updateBGColor(i);
                }
            }
            break;
            
        case Level3:
        case Level4:
            for (unsigned int i = 0; i < cells->count(); i++)
            {
                LevelCell* cell = (LevelCell*)cells->objectAtIndex(i);
                if (cell)
                    LevelCell::updateCellMode(cell, m_cellMode);
            }
            break;
            
        default:
            break;
    }
}

void CustomListView::setupList(float yOffset)
{
    m_itemSeparation = CustomListView::getCellHeight(m_type);
    
    // Lock list if content fits
    if ((float)(m_entries->count() * m_itemSeparation) < m_height)
        BoomListView::lockList(this, true);
    
    float scrollTop = 2.0f;
    float scrollBottom = 2.0f;
    
    switch (m_type)
    {
        case User:
        case Level:
        case Level2:
        case Score:
        case User2:
        case Request:
        case Message:
        case LevelScore:
        case SFX:
        case SFX2:
        case CustomMusic:
        case Options:
        case LevelList:
        case Level3:
        case LevelList2:
        case LevelList3:
        case Level4:
        case LocalLevelScore:
            scrollBottom = 2.0f;
            scrollTop = m_entries ? m_entries->count() * 0.5f : 0.0f;
            break;
            
        case Achievement:
        case MapPack:
        case CustomSong:
            scrollBottom = 2.0f;
            scrollTop = 2.0f;
            break;
            
        case Comment:
        case Comment2:
        case Comment3:
        case Comment4:
        case Artist:
        case URL:
            scrollBottom = 2.0f;
            scrollTop = m_entries ? (m_entries->count() * 0.5f) - 1.0f : 1.0f;
            break;
            
        case List:
            scrollBottom = 10.0f;
            scrollTop = -6.0f;
            break;
            
        default:
            scrollTop = 0.0f;
            scrollBottom = 2.0f;
            break;
    }
    
    m_tableView->reloadData();
    m_tableView->m_scrollLimitTop = scrollTop;
    m_tableView->m_scrollLimitBottom = scrollBottom;
    
    // Handle specific positioning based on type
    if (m_type >= 3 && m_type <= 4 || m_type == 26 || m_type == 29)
    {
        if (yOffset != 0.0f && !m_locked)
        {
            if (yOffset > m_tableView->getMaxY())
                yOffset = m_tableView->getMaxY();
            m_tableView->setContentOffset(cocos2d::CCPoint(m_tableView->m_contentLayer->getPosition()->x, yOffset));
        }
        else if (GameManager::sharedState()->m_currentLevelID)
        {
            int targetIndex = -1;
            int levelID = GameManager::sharedState()->m_currentLevelID;
            
            for (unsigned int i = 0; i < m_entries->count(); i++)
            {
                cocos2d::CCObject* obj = m_entries->objectAtIndex(i);
                if (levelID == obj[5].m_uOrderOfArrival - obj[5].m_unknown5 ||
                    levelID == obj[15].m_uIndexInArray)
                {
                    targetIndex = i;
                    break;
                }
            }
            
            if (targetIndex >= 0)
            {
                float halfHeight = m_height * 0.5f;
                if ((float)targetIndex * m_itemSeparation > halfHeight)
                {
                    float maxY = -(halfHeight - (m_itemSeparation * (targetIndex + 1))) - (m_itemSeparation * 0.5f);
                    maxY += m_tableView->m_contentLayer->getPosition()->y;
                    if (maxY > m_tableView->getMaxY())
                        maxY = m_tableView->getMaxY();
                    m_tableView->setContentOffset(cocos2d::CCPoint(
                        m_tableView->m_contentLayer->getPosition()->x, maxY));
                }
            }
        }
    }
    else
    {
        if (m_entries->count() == 1)
            m_tableView->moveToTopWithOffset(m_itemSeparation);
        else
            m_tableView->moveToTop();
    }
}

CustomListView::~CustomListView()
{
    // Base class handles cleanup
}