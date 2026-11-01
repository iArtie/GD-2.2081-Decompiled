#include "UI/CustomSFXWidget.h"
#include "Audio/FMODAudioEngine.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"

CustomSFXWidget* CustomSFXWidget::create(SFXInfoObject* object, CustomSFXDelegate* delegate, bool showPlay, bool showDownload, bool showDelete, int bgColor, bool compactMode)
{
    CustomSFXWidget* pRet = new CustomSFXWidget();
    if (pRet && pRet->init(object, delegate, showPlay, showDownload, showDelete, bgColor, compactMode))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomSFXWidget::init(SFXInfoObject* object, CustomSFXDelegate* delegate, bool showPlay, bool showDownload, bool showDelete, int bgColor, bool compactMode)
{
    if (!cocos2d::CCNode::init())
        return false;
    
    m_sfxObject = object;
    if (object) object->retain();
    m_delegate = delegate;
    m_compactMode = compactMode;
    m_showPlay = showPlay;
    m_showDownload = showDownload;
    m_showDelete = showDelete;
    m_showCancel = false;
    m_lengthMod = 1.0f;
    m_sfxID = object ? object->m_sfxID : 0;
    
    // Initialize UI elements (labels, buttons, sprites)
    // This would create the title label, ID label, error label,
    // progress bar, clock sprite, and buttons (play, download, delete, select, cancel)
    
    updateSFXInfo();
    
    return true;
}

void CustomSFXWidget::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    int tag = layer->getTag(layer);
    
    if (tag == 1 && btn2)
    {
        GameManager::sharedState()->setGameVariable("0016", true);
        onDownload(nullptr);
    }
    else if (tag == 3 && btn2)
    {
        deleteSFX();
    }
}

void CustomSFXWidget::deleteSFX()
{
    if (!m_sfxObject)
        return;
    
    FMODAudioEngine::sharedEngine()->stopAllEffects();
    MusicDownloadManager::sharedState()->deleteSFX(m_sfxObject->m_sfxID);
    updateSFXInfo();
    
    m_errorLabel->setVisible(true);
    m_errorLabel->setColor(cocos2d::ccColor3B(255, 100, 0));
    m_errorLabel->setString("SFX Deleted");
    m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
}

void CustomSFXWidget::downloadFailed()
{
    hideLoadingArt();
    unschedule(schedule_selector(CustomSFXWidget::updateDownloadProgress));
    m_showCancel = false;
    updateSFXInfo();
}

void CustomSFXWidget::downloadSFXFailed(int id, GJSongError errorType)
{
    if (verifySFXID(id))
    {
        updateError(errorType);
        downloadFailed();
    }
}

void CustomSFXWidget::downloadSFXFinished(int id)
{
    if (verifySFXID(id) && !GameManager::sharedState()->getGameVariable("0143"))
    {
        onPlayback(nullptr);
    }
}

void CustomSFXWidget::hideLoadingArt()
{
    if (m_clockSprite && m_clockSprite->isVisible())
    {
        m_clockSprite->stopAllActions();
        m_clockSprite->runAction(cocos2d::CCSequence::create(
            cocos2d::CCFadeTo::create(0.2f, 0),
            cocos2d::CCHide::create(),
            nullptr
        ));
    }
    m_progressOutlineSprite->setVisible(false);
}

void CustomSFXWidget::onCancelDownload(cocos2d::CCObject* sender)
{
    MusicDownloadManager::sharedState()->stopDownload(m_sfxID);
    downloadSFXFailed(m_sfxID, DownloadCancelled);
}

void CustomSFXWidget::onDelete(cocos2d::CCObject* sender)
{
    if (!m_sfxObject)
        return;
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Delete SFX",
        "Do you want to <cr>Delete</c> this SFX?",
        "No",
        "Yes"
    );
    alert->setTag(3);
    alert->show();
}

void CustomSFXWidget::onDownload(cocos2d::CCObject* sender)
{
    if (!m_sfxObject)
        return;
    
    m_errorLabel->setVisible(false);
    
    if (GameManager::sharedState()->getGameVariable("0016"))
    {
        showLoadingArt();
        m_downloadButton->setVisible(false);
        m_cancelButton->setVisible(true);
        m_deleteButton->setVisible(false);
        startDownload();
    }
    else
    {
        MusicDownloadManager::sharedState()->showTOS(this);
    }
}

void CustomSFXWidget::onPlayback(cocos2d::CCObject* sender)
{
    if (!m_sfxObject)
        return;
    
    FMODAudioEngine::sharedEngine()->stopAllEffects();
    
    if (!m_delegate || !m_delegate->overridePlaySFX(m_sfxObject))
    {
        std::string path = MusicDownloadManager::sharedState()->pathForSFX(m_sfxObject->m_sfxID);
        FMODAudioEngine::sharedEngine()->playEffectAsync(path);
    }
}

void CustomSFXWidget::onSelect(cocos2d::CCObject* sender)
{
    if (m_sfxObject && m_delegate)
    {
        m_delegate->sfxObjectSelected(m_sfxObject);
    }
}

void CustomSFXWidget::showLoadingArt()
{
    if (m_clockSprite)
    {
        cocos2d::CCPoint pos;
        float offset = m_compactMode ? 12.0f : 14.0f;
        float labelWidth;
        float scale;
        
        if (m_compactMode)
        {
            labelWidth = m_idLabel->getContentSize().width * m_idLabel->getScale();
            pos = m_idLabel->getPosition() + cocos2d::CCPoint(labelWidth + offset, 0.0f);
        }
        else
        {
            labelWidth = m_titleLabel->getContentSize().width * m_titleLabel->getScale();
            pos = m_titleLabel->getPosition() + cocos2d::CCPoint(labelWidth + offset, 0.0f);
        }
        
        m_clockSprite->stopAllActions();
        m_clockSprite->setOpacity(25);
        m_clockSprite->setPosition(pos);
        m_clockSprite->runAction(cocos2d::CCFadeTo::create(0.2f, 175));
    }
    
    m_progressOutlineSprite->setVisible(true);
}

void CustomSFXWidget::songStateChanged()
{
    updateSFXInfo();
    updatePlaybackBtn();
}

void CustomSFXWidget::startDownload()
{
    m_showCancel = false;
    MusicDownloadManager::sharedState()->downloadSFX(m_sfxID);
    startMonitorDownload();
}

void CustomSFXWidget::startMonitorDownload()
{
    updateProgressBar(0);
    showLoadingArt();
    unschedule(schedule_selector(CustomSFXWidget::updateDownloadProgress));
    schedule(schedule_selector(CustomSFXWidget::updateDownloadProgress), 0.0f);
}

void CustomSFXWidget::updateDownloadProgress(float dt)
{
    int progress = MusicDownloadManager::sharedState()->getSFXDownloadProgress(m_sfxObject->m_sfxID);
    
    if (progress == -1 || !m_sfxObject)
    {
        unschedule(schedule_selector(CustomSFXWidget::updateDownloadProgress));
        updateSFXInfo();
    }
    else
    {
        updateProgressBar(progress);
    }
}

void CustomSFXWidget::updateError(GJSongError errorType)
{
    m_errorLabel->setVisible(true);
    m_errorLabel->setColor(cocos2d::ccColor3B(255, 50, 25));
    
    switch (errorType)
    {
        case NotAllowed:
            m_errorLabel->setString("SFX is not allowed for use.");
            break;
        case DownloadCancelled:
            m_errorLabel->setString("Download canceled.");
            break;
        case FailedToFetch:
            m_errorLabel->setString("Failed to fetch SFX info.");
            break;
        default:
            m_errorLabel->setString("Download failed. Please try again later.");
            break;
    }
    m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
}

void CustomSFXWidget::updateLengthMod(float mod)
{
    m_lengthMod = mod;
    updateSFXInfo();
}

void CustomSFXWidget::updatePlaybackBtn()
{
    // Update playback button state
}

void CustomSFXWidget::updateProgressBar(int progress)
{
    if (progress < 0) progress = 0;
    
    float maxWidth = m_progressOutlineSprite->getContentSize().width - 4.0f;
    float width = (progress / 100.0f) * maxWidth;
    if (width > maxWidth) width = maxWidth;
    
    m_progressBarSprite->setTextureRect(cocos2d::CCRect(0, 0, width, 8.0f));
}

void CustomSFXWidget::updateSFXInfo()
{
    if (!m_sfxObject)
    {
        m_titleLabel->setString("No SFX loaded");
        m_idLabel->setString("");
        return;
    }
    
    m_titleLabel->setString(m_sfxObject->m_name.c_str());
    
    std::string idText;
    if (m_sfxObject->m_folder)
    {
        int fileCount = m_sfxObject->m_files->count();
        idText = cocos2d::CCString::createWithFormat("Files: %i", fileCount)->getCString();
    }
    else if (!m_compactMode)
    {
        std::string sizeStr = GameToolbox::formatBytes(m_sfxObject->m_size);
        idText = cocos2d::CCString::createWithFormat("ID: %i  Size: %s", m_sfxObject->m_sfxID, sizeStr.c_str())->getCString();
    }
    else
    {
        idText = cocos2d::CCString::createWithFormat("%i", m_sfxObject->m_sfxID)->getCString();
    }
    
    if (m_sfxObject->m_duration > 0)
    {
        float duration = m_sfxObject->m_duration / 100.0f;
        std::string durationText;
        if (m_lengthMod <= 0.0f || m_lengthMod == 1.0f)
        {
            durationText = cocos2d::CCString::createWithFormat("%s  (%.2f s)", idText.c_str(), duration)->getCString();
        }
        else
        {
            float modifiedDuration = duration / m_lengthMod;
            durationText = cocos2d::CCString::createWithFormat("%s  (%.2f s) (%.2f s)", idText.c_str(), duration, modifiedDuration)->getCString();
        }
        m_idLabel->setString(durationText.c_str());
    }
    else
    {
        m_idLabel->setString(idText.c_str());
    }
    
    // Update button visibility based on SFX state
    if (!m_sfxObject || m_sfxObject->m_folder)
    {
        m_downloadButton->setVisible(false);
        m_cancelButton->setVisible(false);
        m_selectButton->setVisible(false);
        m_playButton->setVisible(false);
        m_showPlay = false;
        m_deleteButton->setVisible(m_showPlay);
        hideLoadingArt();
        return;
    }
    
    int sfxID = m_sfxObject->m_sfxID;
    
    if (MusicDownloadManager::sharedState()->isSFXDownloaded(sfxID))
    {
        m_downloadButton->setVisible(false);
        m_cancelButton->setVisible(false);
        m_deleteButton->setVisible(m_showDelete);
        
        if (m_showDelete && m_delegate)
        {
            m_selectButton->setVisible(true);
            cocos2d::CCNode* node = m_selectButton->getNormalImage();
            int activeID = m_delegate->getActiveSFXID();
            
            if (sfxID == activeID)
            {
                m_selectButton->setEnabled(false);
                node->setDisplayFrame(
                    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_selectSongOnBtn_001.png")
                );
            }
            else
            {
                m_selectButton->setEnabled(true);
                node->setDisplayFrame(
                    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_selectSongBtn_001.png")
                );
            }
        }
        else
        {
            m_selectButton->setVisible(false);
        }
    }
    else
    {
        m_selectButton->setVisible(false);
        m_playButton->setVisible(false);
        m_deleteButton->setVisible(false);
        
        if (MusicDownloadManager::sharedState()->isRunningActionForSongID(m_sfxID) || m_showCancel)
        {
            m_downloadButton->setVisible(false);
            m_cancelButton->setVisible(true);
        }
        else
        {
            m_downloadButton->setVisible(m_showDownload);
            m_cancelButton->setVisible(false);
        }
    }
    
    // Adjust label positions for compact mode
    if (m_compactMode)
    {
        m_titleLabel->limitLabelWidth(200.0f, 0.4f, 0.0f);
        m_idLabel->limitLabelWidth(200.0f, 0.6f, 0.0f);
        
        float labelWidth = m_titleLabel->getContentSize().width * m_titleLabel->getScale();
        m_idLabel->setPosition(m_titleLabel->getPosition() + cocos2d::CCPoint(labelWidth + 5.0f, 0.0f));
        m_errorLabel->setPosition(m_idLabel->getPosition());
    }
    else
    {
        m_titleLabel->limitLabelWidth(210.0f, 0.5f, 0.0f);
        m_idLabel->limitLabelWidth(300.0f, 0.6f, 0.0f);
    }
    
    m_deleteButton->setVisible(false);
}

void CustomSFXWidget::updateSFXObject(SFXInfoObject* object)
{
    m_errorLabel->setVisible(false);
    
    if (object != m_sfxObject)
    {
        if (object) object->retain();
        if (m_sfxObject) m_sfxObject->release();
        m_sfxObject = object;
    }
    
    m_sfxID = m_sfxObject ? m_sfxObject->m_sfxID : 0;
    updateSFXInfo();
}

bool CustomSFXWidget::verifySFXID(int id)
{
    return m_sfxID == id && m_sfxID != 0;
}

CustomSFXWidget::~CustomSFXWidget()
{
    CC_SAFE_RELEASE(m_sfxObject);
}