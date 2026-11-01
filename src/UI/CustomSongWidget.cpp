#include "UI/CustomSongWidget.h"
#include "Audio/FMODAudioEngine.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/SongInfoLayer.h"
#include "Audio/AudioAssetsBrowser.h"
#include "Layers/FLAlertLayer.h"
#include "extension/CCHttpClient.h"

CustomSongWidget* CustomSongWidget::create(SongInfoObject* songInfo, CustomSongDelegate* delegate, bool showPlay, bool showDownload, bool showMore, int bgColor, int textColor, bool showSize, bool isLibrary)
{
    CustomSongWidget* pRet = new CustomSongWidget();
    if (pRet && pRet->init(songInfo, delegate, showPlay, showDownload, showMore, bgColor, textColor, showSize, isLibrary))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomSongWidget::init(SongInfoObject* songInfo, CustomSongDelegate* delegate, bool showPlay, bool showDownload, bool showMore, int bgColor, int textColor, bool showSize, bool isLibrary)
{
    if (!cocos2d::CCNode::init())
        return false;
    
    m_songInfoObject = songInfo;
    if (songInfo) songInfo->retain();
    m_songDelegate = delegate;
    m_playbackDelegate = nullptr;
    m_isMusicLibrary = isLibrary;
    m_isRobtopSong = false;
    m_showDownloadBtn = showDownload;
    m_showSelectSongBtn = true;
    m_showPlayMusicBtn = showPlay;
    m_isNotDownloading = false;
    m_hasSFX = false;
    m_unkBool2 = false;
    m_hasLibrarySongs = false;
    m_lengthMod = 1.0f;
    m_totalBytes = 0;
    m_isInCell = false;
    m_customSongID = songInfo ? songInfo->m_songID : 0;
    
    // Initialize UI elements (labels, buttons, sprites)
    // This would create all the visual components
    
    updateSongInfo();
    updatePlaybackBtn();
    
    return true;
}

void CustomSongWidget::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    int tag = layer->getTag(layer);
    
    if (tag == 1 && btn2)
    {
        GameManager::sharedState()->setGameVariable("0016", true);
        onDownload(nullptr);
    }
    else if (tag == 2 && btn2)
    {
        deleteSong();
    }
}

void CustomSongWidget::addExtraVisuals(bool newMusic, bool newLibraryMusic)
{
    if (!newMusic && !newLibraryMusic)
        return;
    
    cocos2d::CCSprite* icon = nullptr;
    if (newMusic && newLibraryMusic)
    {
        icon = cocos2d::CCSprite::createWithSpriteFrameName("newMusicIcon2_001.png");
        icon->setPosition(cocos2d::CCPoint(-157.0f, 20.0f));
        addChild(icon, 4);
        
        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "GJ_square07.png", cocos2d::CCRect(0, 0, 80, 80));
        bg->setColor(cocos2d::ccColor3B(0, 255, 0));
        bg->setContentSize(cocos2d::CCSize(320, 50));
        addChild(bg, -1);
    }
    else if (newMusic)
    {
        icon = cocos2d::CCSprite::createWithSpriteFrameName("newMusicIcon_001.png");
        icon->setPosition(cocos2d::CCPoint(-157.0f, 20.0f));
        addChild(icon, 4);
    }
}

void CustomSongWidget::deleteSong()
{
    if (!m_songInfoObject)
        return;
    
    std::string path = MusicDownloadManager::sharedState()->pathForSong(m_songInfoObject->m_songID);
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(path))
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    
    MusicDownloadManager::sharedState()->deleteSong(m_songInfoObject->m_songID);
    updateSongInfo();
    
    m_errorLabel->setColor(cocos2d::ccColor3B(255, 100, 0));
    m_errorLabel->setString("Song Deleted");
    if (m_isMusicLibrary)
        m_errorLabel->limitLabelWidth(140.0f, 0.35f, 0.0f);
    else
        m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
    showError(true);
}

void CustomSongWidget::downloadAssetFailed(int id, GJAssetType type, GJSongError errorType)
{
    for (auto it = m_undownloadedAssets.begin(); it != m_undownloadedAssets.end(); ++it)
    {
        if (it->m_id == id && it->m_assetType == type)
        {
            it->m_status++;
            it->m_isDownloading = false;
            m_undownloadedAssets.erase(it);
            processNextMultiAsset();
            break;
        }
    }
}

void CustomSongWidget::downloadAssetFinished(int id, GJAssetType type)
{
    for (auto it = m_undownloadedAssets.begin(); it != m_undownloadedAssets.end(); ++it)
    {
        if (it->m_id == id && it->m_assetType == type)
        {
            if (type == Song)
                m_songs.insert(id);
            else if (type == SFX)
                m_sfx.insert(id);
            
            it->m_isDownloading = false;
            m_undownloadedAssets.erase(it);
            updateMultiAssetInfo(false);
            processNextMultiAsset();
            break;
        }
    }
}

void CustomSongWidget::downloadFailed()
{
    m_sliderGroove->setVisible(false);
    unschedule(schedule_selector(CustomSongWidget::updateDownloadProgress));
    m_isNotDownloading = false;
    updateSongInfo();
}

void CustomSongWidget::downloadSFXFailed(int id, GJSongError errorType)
{
    downloadAssetFailed(id, SFX, errorType);
}

void CustomSongWidget::downloadSFXFinished(int id)
{
    downloadAssetFinished(id, SFX);
}

void CustomSongWidget::downloadSongFailed(int id, GJSongError errorType)
{
    if (m_hasSFX)
    {
        downloadAssetFailed(id, Song, errorType);
    }
    else if (verifySongID(id))
    {
        updateError(errorType);
        downloadFailed();
    }
}

void CustomSongWidget::downloadSongFinished(int id)
{
    if (m_hasSFX)
    {
        downloadAssetFinished(id, Song);
    }
    else if (verifySongID(id))
    {
        m_errorLabel->setColor(cocos2d::ccColor3B(0, 215, 0));
        m_errorLabel->setString("Download complete.");
        if (m_isMusicLibrary)
            m_errorLabel->limitLabelWidth(140.0f, 0.35f, 0.0f);
        else
            m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
        showError(true);
    }
}

void CustomSongWidget::downloadSongStarted(int id)
{
    if (verifySongID(id))
        startMonitorDownload();
}

void CustomSongWidget::getSongInfoIfUnloaded()
{
    if (m_songInfoObject && m_songInfoObject->m_unloaded)
    {
        m_songInfoObject->m_songName = "Loading";
        updateSongInfo();
        onGetSongInfo(nullptr);
    }
}

void CustomSongWidget::loadSongInfoFailed(int id, GJSongError errorType)
{
    if (m_nextSongIDs.find(id) != m_nextSongIDs.end())
    {
        m_nextSongIDs.erase(id);
        downloadSongFailed(id, errorType);
    }
}

void CustomSongWidget::loadSongInfoFinished(SongInfoObject* object)
{
    if (m_nextSongIDs.find(object->m_songID) != m_nextSongIDs.end())
    {
        m_nextSongIDs.erase(object->m_songID);
        MusicDownloadManager::sharedState()->downloadSong(object->m_songID);
    }
    else if (verifySongID(object->m_songID))
    {
        updateSongObject(object);
        if (m_isNotDownloading)
            startDownload();
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(CustomSongWidget::updateSongInfo)),
            nullptr
        ));
    }
}

void CustomSongWidget::musicActionFinished(GJMusicAction action)
{
    if (action == DownloadOrUpdate && m_hasLibrarySongs)
    {
        m_hasLibrarySongs = false;
        onDownload(nullptr);
    }
}

void CustomSongWidget::onCancelDownload(cocos2d::CCObject* sender)
{
    MusicDownloadManager::sharedState()->stopDownload(m_customSongID);
    downloadSongFailed(m_customSongID, DownloadCancelled);
}

void CustomSongWidget::onDelete(cocos2d::CCObject* sender)
{
    if (!m_songInfoObject)
        return;
    
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Delete Song",
        "Do you want to <cr>delete</c> this song?",
        "No",
        "Yes"
    );
    alert->setTag(2);
    alert->show();
}

void CustomSongWidget::onDownload(cocos2d::CCObject* sender)
{
    m_errorLabel->setVisible(false);
    if (m_bgSpr)
        m_bgSpr->setVisible(false);
    
    if (!GameManager::sharedState()->getGameVariable("0016"))
    {
        MusicDownloadManager::sharedState()->showTOS(this);
        return;
    }
    
    if (m_hasSFX)
    {
        m_downloadBtn->setVisible(false);
        m_cancelDownloadBtn->setVisible(true);
        if (!MusicDownloadManager::sharedState()->m_customContentURL.empty())
        {
            startMultiAssetDownload();
            return;
        }
        m_hasLibrarySongs = true;
        MusicDownloadManager::sharedState()->getCustomContentURL();
        return;
    }
    
    if (!m_songInfoObject)
        return;
    
    m_sliderGroove->setVisible(true);
    m_downloadBtn->setVisible(false);
    m_cancelDownloadBtn->setVisible(true);
    m_deleteBtn->setVisible(false);
    
    if (m_customSongID > 9999999 && MusicDownloadManager::sharedState()->m_customContentURL.empty())
    {
        m_hasLibrarySongs = true;
        MusicDownloadManager::sharedState()->getCustomContentURL();
        return;
    }
    
    if (m_songInfoObject->m_verified)
    {
        startDownload();
    }
    else
    {
        m_isNotDownloading = true;
        MusicDownloadManager::sharedState()->getSongInfo(m_customSongID, true);
    }
}

void CustomSongWidget::onGetSongInfo(cocos2d::CCObject* sender)
{
    if (m_songInfoObject)
    {
        m_errorLabel->setVisible(false);
        if (m_bgSpr)
            m_bgSpr->setVisible(false);
        MusicDownloadManager::sharedState()->getSongInfo(m_customSongID, false);
        toggleUpdateButton(false);
    }
}

void CustomSongWidget::onInfo(cocos2d::CCObject* sender)
{
    if (!m_hasSFX)
        return;
    
    std::vector<int> songIds;
    std::vector<int> sfxIds;
    
    for (int id : m_songs)
        songIds.push_back(id);
    for (int id : m_sfx)
        sfxIds.push_back(id);
    
    AudioAssetsBrowser* browser = AudioAssetsBrowser::create(&songIds, &sfxIds);
    browser->show();
}

void CustomSongWidget::onMore(cocos2d::CCObject* sender)
{
    if (!m_songInfoObject)
        return;
    
    if (m_isRobtopSong)
    {
        SongInfoLayer* layer = SongInfoLayer::create(m_songInfoObject->m_songID);
        layer->show();
        return;
    }
    
    SongInfoObject* info = MusicDownloadManager::sharedState()->getSongInfoObject(m_songInfoObject->m_songID);
    if (!info)
        info = m_songInfoObject;
    
    std::string youtubeLink = "";
    std::string artistLink = "";
    std::string songLink = "";
    
    if (!info->m_youtubeChannel.empty())
        youtubeLink = "https://youtube.com/" + info->m_youtubeChannel;
    else
        youtubeLink = "";
    
    if (!info->m_downloadLinkOverride.empty())
        songLink = info->m_downloadLinkOverride;
    else if (info->m_nongType == 1)
        songLink = info->m_songUrl;
    else if (!info->m_youtubeVideo.empty())
        songLink = "https://youtube.com/watch?v=" + info->m_youtubeVideo;
    else
        songLink = "https://newgrounds.com/audio/listen/" + std::to_string(info->m_songID);
    
    if (info->m_songID <= 9999999)
    {
        artistLink = "https://newgrounds.com/audio/artist/" + info->m_artistName;
    }
    else
    {
        MusicArtistObject* artist = MusicDownloadManager::sharedState()->getMusicArtistForID(info->m_artistID);
        if (artist)
        {
            artistLink = artist->m_artistURL;
            if (youtubeLink.empty())
                youtubeLink = artist->m_artistYouTube;
        }
    }
    
    if (!youtubeLink.empty())
    {
        youtubeLink = "https://youtube.com/" + youtubeLink;
    }
    
    std::string tags = info->getTagsString();
    
    SongInfoLayer* layer = SongInfoLayer::create(
        info->m_songName,
        info->getArtistNames(),
        songLink,
        artistLink,
        youtubeLink,
        tags,
        info->m_songID,
        info->m_nongType
    );
    layer->show();
}

void CustomSongWidget::onPlayback(cocos2d::CCObject* sender)
{
    if (m_playbackDelegate)
    {
        m_playbackDelegate->onPlayback(m_songInfoObject);
        return;
    }
    
    if (!m_songInfoObject)
        return;
    
    std::string path = MusicDownloadManager::sharedState()->pathForSong(m_songInfoObject->m_songID);
    
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(path))
    {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    }
    else if (m_songInfoObject)
    {
        FMODAudioEngine::sharedEngine()->playMusic(path, 0, 0.0f, 0);
    }
    
    MusicDownloadManager::sharedState()->songStateChanged();
}

void CustomSongWidget::onSelect(cocos2d::CCObject* sender)
{
    if (!m_songInfoObject)
        return;
    
    int songID = m_songInfoObject->m_songID;
    if (m_songDelegate)
        m_songDelegate->songIDChanged(songID);
    
    updateSongInfo();
    if (m_isInCell)
        MusicDownloadManager::sharedState()->songStateChanged();
}

void CustomSongWidget::positionInfoObjects()
{
    if (m_isMusicLibrary)
    {
        m_songLabel->setPosition(cocos2d::CCPoint(-150.0f, 9.0f));
        m_songIDLabel->setPosition(cocos2d::CCPoint(-149.0f, -9.0f));
        m_errorLabel->setPosition(cocos2d::CCPoint(0.0f, -22.0f));
        m_sliderGroove->setPosition(m_errorLabel->getPosition() + cocos2d::CCPoint(0.0f, -0.5f));
        m_bgSpr->setPosition(m_sliderGroove->getPosition());
    }
    else
    {
        bool hasNCS = (m_songInfoObject && m_songInfoObject->m_nongType > 0);
        
        m_songLabel->setPosition(cocos2d::CCPoint(-140.0f, 29.0f));
        m_artistLabel->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(1.0f, -24.0f));
        m_errorLabel->setPosition(m_artistLabel->getPosition() + cocos2d::CCPoint(1.0f, -20.0f));
        m_songIDLabel->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(1.0f, -31.0f));
        m_sliderGroove->setPosition(cocos2d::CCPoint(-34.0f, m_errorLabel->getPosition().y));
        
        if (hasNCS)
        {
            bool isChompo = (m_songInfoObject->m_nongType == 2);
            std::string prefix = isChompo ? "chompo" : "ncs";
            std::string suffix = m_isMusicLibrary ? "small" : "med";
            
            std::string frameName = prefix + "_" + suffix + "_001.png";
            m_ncsLogo->setDisplayFrame(
                cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName)
            );
            
            float offset = isChompo ? 45.0f : 37.0f;
            if (m_isMusicLibrary)
            {
                m_ncsLogo->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(0.0f, -1.0f));
                m_songLabel->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(offset, 0.0f));
                m_artistLabel->setPosition(m_artistLabel->getPosition() + cocos2d::CCPoint(offset, 0.0f));
            }
            else
            {
                offset = isChompo ? 61.0f : 53.0f;
                m_ncsLogo->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(0.0f, -2.0f));
                m_songLabel->setPosition(m_songLabel->getPosition() + cocos2d::CCPoint(offset, 0.0f));
            }
        }
    }
    
    m_ncsLogo->setVisible(m_songInfoObject && m_songInfoObject->m_nongType > 0);
}

void CustomSongWidget::processNextMultiAsset()
{
    if (m_undownloadedAssets.empty())
    {
        m_errorLabel->setColor(cocos2d::ccColor3B(0, 215, 0));
        m_errorLabel->setString("Download complete.");
        if (m_isMusicLibrary)
            m_errorLabel->limitLabelWidth(140.0f, 0.35f, 0.0f);
        else
            m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
        showError(true);
        m_unkBool2 = false;
        updateMultiAssetInfo(true);
        return;
    }
    
    size_t availableThreads = cocos2d::extension::CCHttpClient::getInstance()->availableThreadCount();
    
    for (auto& asset : m_undownloadedAssets)
    {
        if (!asset.m_isDownloading && asset.m_status <= 1)
        {
            asset.m_isDownloading = true;
            
            if (asset.m_assetType == Song)
            {
                if (asset.m_id > 9999999)
                {
                    MusicDownloadManager::sharedState()->downloadSong(asset.m_id);
                }
                else
                {
                    m_nextSongIDs.insert(asset.m_id);
                    MusicDownloadManager::sharedState()->getSongInfo(asset.m_id, false);
                }
            }
            else if (asset.m_assetType == SFX)
            {
                MusicDownloadManager::sharedState()->downloadSFX(asset.m_id);
            }
            
            if (availableThreads > 0)
            {
                availableThreads--;
                if (availableThreads == 0)
                    break;
            }
        }
        else if (asset.m_status > 1)
        {
            updateMultiAssetInfo(true);
            m_errorLabel->setColor(cocos2d::ccColor3B(255, 50, 25));
            m_errorLabel->setString("Some assets failed to download.");
            m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
            showError(false);
        }
    }
}

void CustomSongWidget::showError(bool fast)
{
    m_errorLabel->stopAllActions();
    m_errorLabel->setVisible(true);
    m_errorLabel->setOpacity(255);
    
    if (m_isMusicLibrary && m_bgSpr)
    {
        m_bgSpr->stopAllActions();
        m_bgSpr->setVisible(true);
        m_bgSpr->setOpacity(255);
        
        cocos2d::ccColor3B color = m_errorLabel->getColor();
        m_bgSpr->setColor(cocos2d::ccColor3B(color.r * 0.5f, color.g * 0.5f, color.b * 0.5f));
        
        float width = m_errorLabel->getContentSize().width * m_errorLabel->getScale();
        m_bgSpr->setContentSize(cocos2d::CCSize((width + 10.0f) / 0.2f, 60.0f));
        m_bgSpr->setScale(0.2f);
    }
    
    float delay = fast ? 0.8f : 1.6f;
    m_errorLabel->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCFadeTo::create(0.5f, 0),
        nullptr
    ));
    
    if (m_bgSpr)
    {
        m_bgSpr->runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(delay),
            cocos2d::CCFadeTo::create(0.5f, 0),
            nullptr
        ));
    }
}

void CustomSongWidget::songStateChanged()
{
    updateSongInfo();
    updatePlaybackBtn();
}

void CustomSongWidget::startDownload()
{
    m_isNotDownloading = false;
    MusicDownloadManager::sharedState()->downloadSong(m_customSongID);
    startMonitorDownload();
}

void CustomSongWidget::startMonitorDownload()
{
    updateProgressBar(0);
    m_sliderGroove->setVisible(true);
    unschedule(schedule_selector(CustomSongWidget::updateDownloadProgress));
    schedule(schedule_selector(CustomSongWidget::updateDownloadProgress), 0.0f);
}

void CustomSongWidget::startMultiAssetDownload()
{
    m_undownloadedAssets.clear();
    
    for (int id : m_songs)
    {
        GJAssetDownloadAction action;
        action.m_id = id;
        action.m_assetType = Song;
        action.m_status = 0;
        action.m_isDownloading = false;
        m_undownloadedAssets.push_back(action);
    }
    
    for (int id : m_sfx)
    {
        GJAssetDownloadAction action;
        action.m_id = id;
        action.m_assetType = SFX;
        action.m_status = 0;
        action.m_isDownloading = false;
        m_undownloadedAssets.push_back(action);
    }
    
    processNextMultiAsset();
}

void CustomSongWidget::toggleUpdateButton(bool enabled)
{
    m_getSongInfoBtn->setColor(enabled ? cocos2d::ccColor3B(255, 255, 255) : cocos2d::ccColor3B(128, 128, 128));
    m_getSongInfoBtn->setEnabled(enabled);
}

void CustomSongWidget::updateDownloadProgress(float dt)
{
    int progress = MusicDownloadManager::sharedState()->getDownloadProgress(m_songInfoObject->m_songID);
    
    if (progress == -1 || !m_songInfoObject)
    {
        unschedule(schedule_selector(CustomSongWidget::updateDownloadProgress));
        updateSongInfo();
    }
    else
    {
        updateProgressBar(progress);
    }
}

void CustomSongWidget::updateError(GJSongError errorType)
{
    m_errorLabel->setColor(cocos2d::ccColor3B(255, 50, 25));
    
    switch (errorType)
    {
        case NotAllowed:
            m_errorLabel->setString("Song is not allowed for use.");
            break;
        case DownloadCancelled:
            m_errorLabel->setString("Download canceled.");
            break;
        case FailedToFetch:
            m_errorLabel->setString("Failed to fetch song info.");
            break;
        default:
            m_errorLabel->setString("Download failed. Please try later.");
            break;
    }
    
    if (m_isMusicLibrary)
        m_errorLabel->limitLabelWidth(140.0f, 0.35f, 0.0f);
    else
        m_errorLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
    
    showError(false);
}

void CustomSongWidget::updateLengthMod(float lengthMod)
{
    m_lengthMod = lengthMod;
    updateSongInfo();
}

void CustomSongWidget::updateMultiAssetInfo(bool update)
{
    std::string songsStr = "";
    std::string sfxStr = "";
    std::string downloadedSongsStr = "";
    std::string downloadedSfxStr = "";
    
    int totalAssets = m_songs.size() + m_sfx.size();
    int downloadedAssets = 0;
    
    for (int id : m_songs)
    {
        if (!songsStr.empty()) songsStr += ", ";
        songsStr += std::to_string(id);
        if (MusicDownloadManager::sharedState()->isSongDownloaded(id))
            downloadedAssets++;
    }
    
    for (int id : m_sfx)
    {
        if (!sfxStr.empty()) sfxStr += ", ";
        sfxStr += std::to_string(id);
        if (MusicDownloadManager::sharedState()->isSFXDownloaded(id))
            downloadedAssets++;
    }
    
    m_hasSFX = (totalAssets > 0);
    m_unkBool2 = (downloadedAssets < totalAssets);
    
    if (m_infoBtn)
        m_infoBtn->setVisible(m_hasSFX);
    
    if (m_hasSFX)
    {
        std::string sizeStr = GameToolbox::formatBytes(m_totalBytes);
        std::string infoStr = "Songs: " + std::to_string(m_songs.size()) + "  SFX: " + std::to_string(m_sfx.size()) + "  Size: " + sizeStr;
        
        if (downloadedAssets < totalAssets && downloadedAssets > 0)
        {
            infoStr = infoStr + " (" + std::to_string(downloadedAssets) + "/" + std::to_string(totalAssets) + ")";
        }
        
        m_songIDLabel->setString(infoStr.c_str());
        m_songIDLabel->limitLabelWidth(230.0f, 0.4f, 0.0f);
    }
    
    if (update)
        updateSongInfo();
}

void CustomSongWidget::updatePlaybackBtn()
{
    if (!m_songInfoObject)
        return;
    
    cocos2d::CCNode* node = m_playbackBtn->getNormalImage();
    std::string path = MusicDownloadManager::sharedState()->pathForSong(m_songInfoObject->m_songID);
    
    if (FMODAudioEngine::sharedEngine()->isMusicPlaying(path))
    {
        node->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_stopMusicBtn_001.png")
        );
    }
    else
    {
        node->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_playMusicBtn_001.png")
        );
    }
}

void CustomSongWidget::updateProgressBar(int progress)
{
    if (progress < 0) progress = 0;
    
    float maxWidth = m_sliderGroove->getContentSize().width - 4.0f;
    float width = (progress / 100.0f) * maxWidth;
    if (width > maxWidth) width = maxWidth;
    
    m_sliderBar->setTextureRect(cocos2d::CCRect(0, 0, width, 8.0f));
}

void CustomSongWidget::updateSongInfo()
{
    positionInfoObjects();
    
    if (!m_songInfoObject)
    {
        m_songLabel->setString("No song loaded");
        m_artistLabel->setString("");
        if (!m_hasSFX)
            m_songIDLabel->setString("");
        return;
    }
    
    if (!m_isMusicLibrary)
    {
        m_songLabel->setString(m_songInfoObject->m_songName.c_str());
        
        std::string artistText = "";
        if (!m_songInfoObject->m_artistName.empty())
            artistText = "By: " + m_songInfoObject->m_artistName;
        
        int extraArtists = m_songInfoObject->getExtraArtistCount();
        if (extraArtists > 0)
            artistText += " +" + std::to_string(extraArtists);
        
        m_artistLabel->setString(artistText.c_str());
        
        if (!m_hasSFX && !m_isRobtopSong)
        {
            if (m_songInfoObject->m_unloaded)
            {
                m_songIDLabel->setString(("SongID: " + std::to_string(m_songInfoObject->m_songID)).c_str());
            }
            else
            {
                std::string info = "SongID: " + std::to_string(m_songInfoObject->m_songID) + "  Size: " + 
                    cocos2d::CCString::createWithFormat("%.02fMB", m_songInfoObject->m_fileSize)->getCString();
                m_songIDLabel->setString(info.c_str());
            }
            
            if (m_songInfoObject->m_BPM > 0)
            {
                m_errorLabel->setColor(cocos2d::ccColor3B(0, 215, 0));
                m_errorLabel->setScale(0.4f);
                
                std::string bpmText;
                if (m_lengthMod <= 0.0f || m_lengthMod == 1.0f)
                    bpmText = "BPM: " + std::to_string(m_songInfoObject->m_BPM);
                else
                    bpmText = "BPM: " + std::to_string(m_songInfoObject->m_BPM) + 
                              " (" + std::to_string((int)(m_lengthMod * m_songInfoObject->m_BPM)) + ")";
                
                m_errorLabel->setString(bpmText.c_str());
                showError(false);
            }
        }
    }
    else
    {
        m_songLabel->setString(m_songInfoObject->m_songName.c_str());
        
        std::string artistText = m_songInfoObject->m_artistName;
        int extraArtists = m_songInfoObject->getExtraArtistCount();
        if (extraArtists > 0)
            artistText += " +" + std::to_string(extraArtists);
        
        m_artistLabel->setString(artistText.c_str());
        
        if (!m_hasSFX)
        {
            std::string infoStr = "ID: " + std::to_string(m_songInfoObject->m_songID) + 
                                  "  Size: " + cocos2d::CCString::createWithFormat("%.01fMB", m_songInfoObject->m_fileSize)->getCString();
            
            if (m_songInfoObject->m_duration > 0)
                infoStr += " (" + std::to_string(m_songInfoObject->m_duration) + "s)";
            
            std::string tags = m_songInfoObject->getTagsString();
            if (!tags.empty())
                infoStr += "    " + tags;
            
            m_songIDLabel->setString(infoStr.c_str());
        }
    }
    
    if (!m_hasSFX && m_songInfoObject)
    {
        int songID = m_songInfoObject->m_songID;
        
        if (MusicDownloadManager::sharedState()->isSongDownloaded(songID))
        {
            m_downloadBtn->setVisible(false);
            m_cancelDownloadBtn->setVisible(false);
            m_showDownloadBtn = false;
            
            if (m_songInfoObject->m_unloaded)
                m_getSongInfoBtn->setVisible(m_showDownloadBtn);
            
            m_deleteBtn->setVisible(m_showDownloadBtn && !m_getSongInfoBtn->isVisible());
            
            if (m_showSelectSongBtn && m_songDelegate)
            {
                m_selectSongBtn->setVisible(true);
                m_deleteBtn->setVisible(false);
                
                cocos2d::CCNode* node = m_selectSongBtn->getNormalImage();
                int activeID = m_songDelegate->getActiveSongID();
                
                if (songID == activeID)
                {
                    m_selectSongBtn->setEnabled(false);
                    node->setDisplayFrame(
                        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_selectSongOnBtn_001.png")
                    );
                }
                else
                {
                    m_selectSongBtn->setEnabled(true);
                    node->setDisplayFrame(
                        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_selectSongBtn_001.png")
                    );
                }
            }
            else
            {
                m_selectSongBtn->setVisible(false);
            }
            
            m_playbackBtn->setVisible(m_showPlayMusicBtn);
            m_sliderGroove->setVisible(false);
        }
        else
        {
            m_selectSongBtn->setVisible(false);
            m_playbackBtn->setVisible(false);
            m_deleteBtn->setVisible(false);
            
            if (m_isRobtopSong)
            {
                m_downloadBtn->setVisible(false);
                m_cancelDownloadBtn->setVisible(false);
            }
            else if (MusicDownloadManager::sharedState()->isRunningActionForSongID(m_customSongID) || m_isNotDownloading)
            {
                m_downloadBtn->setVisible(false);
                m_cancelDownloadBtn->setVisible(true);
            }
            else
            {
                bool showDownload = m_showDownloadBtn && !m_songInfoObject->m_unloaded;
                m_downloadBtn->setVisible(showDownload);
                m_cancelDownloadBtn->setVisible(false);
            }
            
            if (m_songInfoObject->m_unloaded)
                m_isRobtopSong = m_showDownloadBtn;
            
            m_getSongInfoBtn->setVisible(m_isRobtopSong);
            toggleUpdateButton(!m_songInfoObject->m_unloaded);
        }
    }
    
    // Update label widths
    if (!m_isMusicLibrary)
    {
        if (m_ncsLogo->isVisible())
        {
            m_songLabel->limitLabelWidth(190.0f, 0.75f, 0.0f);
            m_artistLabel->limitLabelWidth(165.0f, 0.75f, 0.0f);
        }
        else
        {
            m_songLabel->limitLabelWidth(240.0f, 0.8f, 0.0f);
            m_artistLabel->limitLabelWidth(175.0f, 0.8f, 0.0f);
        }
        
        if (!m_hasSFX)
            m_songIDLabel->limitLabelWidth(300.0f, 0.4f, 0.0f);
        
        // Position more button
        if (m_hasSFX)
        {
            cocos2d::CCPoint pos = m_artistLabel->getPosition();
            float width = m_artistLabel->getContentSize().width * m_artistLabel->getScaleX();
            m_moreBtn->setPosition(m_moreBtn->getParent()->convertToNodeSpace(
                cocos2d::CCPoint(pos.x + width + 35.0f, pos.y - 2.0f)
            ));
            m_moreBtn->setVisible(m_songInfoObject && !m_songInfoObject->m_unloaded);
        }
    }
    else
    {
        float songWidth = m_ncsLogo->isVisible() ? 130.0f : 140.0f;
        float artistExtra = m_ncsLogo->isVisible() ? 55.0f : 80.0f;
        
        m_songLabel->limitLabelWidth(songWidth, 0.5f, 0.0f);
        
        float remaining = songWidth - (m_songLabel->getContentSize().width * m_songLabel->getScale());
        if (remaining < 0.0f) remaining = 0.0f;
        
        m_artistLabel->limitLabelWidth(artistExtra + remaining, 0.5f, 0.0f);
        
        if (!m_hasSFX)
            m_songIDLabel->limitLabelWidth(220.0f, 0.6f, 0.0f);
        
        // Position more button
        float songLabelWidth = m_songLabel->getContentSize().width * m_songLabel->getScale();
        float artistLabelWidth = m_artistLabel->getContentSize().width * m_artistLabel->getScale();
        
        cocos2d::CCPoint songPos = m_songLabel->getPosition();
        float offset = (artistLabelWidth * 0.5f) + songLabelWidth + 4.0f;
        
        m_moreBtn->setPosition(m_moreBtn->getParent()->convertToNodeSpace(
            cocos2d::CCPoint(songPos.x + offset, songPos.y)
        ));
        
        m_artistLabel->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
        m_artistLabel->setPosition(m_moreBtn->getPosition());
    }
}

void CustomSongWidget::updateSongObject(SongInfoObject* songInfo)
{
    m_errorLabel->setVisible(false);
    if (m_bgSpr)
        m_bgSpr->setVisible(false);
    
    if (songInfo != m_songInfoObject)
    {
        if (songInfo) songInfo->retain();
        if (m_songInfoObject) m_songInfoObject->release();
        m_songInfoObject = songInfo;
    }
    
    m_customSongID = m_songInfoObject ? m_songInfoObject->m_songID : 0;
    updateSongInfo();
}

bool CustomSongWidget::verifySongID(int id)
{
    return m_customSongID == id && m_customSongID != 0;
}

CustomSongWidget::~CustomSongWidget()
{
    CC_SAFE_RELEASE(m_songInfoObject);
}