#include "AppDelegate.h"
#include "AdToolbox.h"
#include "FMODAudioEngine.h"
#include "GameManager.h"
#include "GJObjectDecoder.h"
#include "LoadingLayer.h"
#include "LocalLevelManager.h"
#include "ObjectDecoder.h"
#include "PlayLayer.h"
#include "PlatformToolbox.h"
#include "AchievementNotifier.h"
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

void AppDelegate::applicationDidEnterBackground()
{
    pauseSound();
    pauseGame();
    trySaveGame(false);
    
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->stopAnimation();
    director->pause();
    
    if (m_glViewSetup)
    {
        PlatformToolbox::onNativePause();
        GameManager::sharedState()->applicationDidEnterBackground();
    }
}

bool AppDelegate::applicationDidFinishLaunching()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    srand(tv.tv_sec * tv.tv_usec);
    
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    
    std::string writePath = "/data/data/com.robtopx.geometryjump/";
    fileUtils->setWriteablePath(writePath);
    
    setupGLView();
    director->setProjection(kCCDirectorProjection2D);
    CCContentManager::sharedManager();
    
    ObjectDecoder* decoder = ObjectDecoder::sharedDecoder();
    GJObjectDecoder* gjDecoder = GJObjectDecoder::sharedDecoder();
    decoder->m_delegate = gjDecoder;
    
    GameManager::sharedState();
    LocalLevelManager::sharedState();
    
    GameManager* gm = GameManager::sharedState();
    gm->loadVideoSettings();
    gm->startUpdate();
    
    director->setDepthTest(false);
    
    float refreshRate = PlatformToolbox::getDeviceRefreshRate();
    director->setAnimationInterval(1.0f / refreshRate);
    director->m_bDisplayStats = true;
    
    AdToolbox::setupAds();
    
    cocos2d::CCScene* scene = LoadingLayer::scene(false);
    director->runWithScene(scene);
    
    return true;
}

void AppDelegate::applicationWillBecomeActive()
{
    GameManager::sharedState()->applicationWillEnterForeground();
}

void AppDelegate::applicationWillEnterForeground()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->stopAnimation();
    director->resume();
    director->startAnimation();
    
    FMODAudioEngine::sharedEngine()->resumeAudio();
    
    PlatformToolbox::onNativeResume();
    PlatformToolbox::onToggleKeyboard();
    
    GameManager* gm = GameManager::sharedState();
    
    cocos2d::CCActionManager* actionManager = director->getActionManager();
    actionManager->removeActionByTag(1, gm);
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(gm, callfunc_selector(GameManager::applicationWillEnterForeground));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delay, callFunc, nullptr);
    sequence->setTag(1);
    
    actionManager->addAction(sequence, gm, false);
}

float AppDelegate::bgScale()
{
    return cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorMax();
}

void AppDelegate::checkSound()
{
    if (musicTest())
    {
        FMODAudioEngine::sharedEngine()->pauseAllMusic(true);
    }
}

void AppDelegate::hideLoadingCircle()
{
    // Empty implementation
}

void AppDelegate::loadingIsFinished()
{
    m_glViewSetup = true;
    PlatformToolbox::reportLoadingFinished();
}

bool AppDelegate::musicTest()
{
    if (GameManager::sharedState()->m_loadingLevel)
        return true;
    
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    if (!runningScene)
        return false;
    
    if (GameManager::sharedState()->m_playLayer)
    {
        if (!m_startupScriptFilename.empty())
            return true;
        
        if (!PlayLayer::isGameplayActive(GameManager::sharedState()->m_playLayer))
            return true;
    }
    
    return runningScene->m_eObjType == LevelEditorLayerObjectType;
}

void AppDelegate::pauseGame()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    
    if (runningScene &&
        runningScene->m_eObjType == PlayLayerObjectType &&
        GameManager::sharedState()->m_playLayer &&
        !GameManager::sharedState()->m_playLayer->m_hasCompletedLevel &&
        m_startupScriptFilename.empty())
    {
        GameManager::sharedState()->m_playLayer->pauseGame(true);
    }
}

void AppDelegate::pauseSound()
{
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    engine->pauseAllEffects();
    
    if (m_glViewSetup)
    {
        engine->pauseAllMusic(true);
    }
    
    engine->stop();
    engine->update(0.0f);
}

void AppDelegate::platformShutdown()
{
    // Empty implementation
}

void AppDelegate::resumeSound()
{
    if (musicTest())
        return;
    
    if (GameManager::sharedState()->m_playLayer ||
        !GameManager::getGameVariable(GameManager::sharedState(), "0122"))
    {
        FMODAudioEngine::sharedEngine()->resumeAllMusic();
    }
    
    FMODAudioEngine::sharedEngine()->resumeAllEffects();
}

void AppDelegate::setIdleTimerDisabled(bool disabled)
{
    // Empty implementation
}

void AppDelegate::setupGLView()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;
    
    cocos2d::CCSize designSize(480.0f, 320.0f);
    
    GameManager::sharedState();
    LocalLevelManager::sharedState();
    
    cocos2d::CCEGLView* glView = cocos2d::CCEGLView::sharedOpenGLView();
    cocos2d::CCDirector::sharedDirector()->setOpenGLView(glView);
    
    cocos2d::CCSize frameSize = glView->getFrameSize();
    
    m_isIphoneX = (frameSize.width == 2436.0f);
    m_isLowMemoryDevice = PlatformToolbox::isLowMemoryDevice();
    
    glView->setFrameSize(frameSize.width, frameSize.height);
    
    cocos2d::CCDirector::sharedDirector()->setupScreenScale(designSize, frameSize, cocos2d::TextureQuality::kCCTextureQualityHigh);
}

void AppDelegate::showLoadingCircle(bool unk1, bool unk2, bool unk3)
{
    // Empty implementation
}

void AppDelegate::trySaveGame(bool force)
{
    if (force)
    {
        m_lastSaveTime = 0.0f;
    }
    else if (m_lastSaveTime == 0.0f)
    {
        // Continue to save
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        float currentTime = (float)ts.tv_sec + (float)ts.tv_nsec / 1000000000.0f;
        
        if (currentTime - m_lastSaveTime < 10.0f)
            return;
    }
    
    if (!m_glViewSetup)
    {
        PlatformToolbox::gameDidSave();
        return;
    }
    
    GameManager::sharedState()->save();
    LocalLevelManager::sharedState()->save();
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    m_lastSaveTime = (float)ts.tv_sec + (float)ts.tv_nsec / 1000000000.0f;
    
    PlatformToolbox::gameDidSave();
}

void AppDelegate::willSwitchToScene(cocos2d::CCScene* scene)
{
    AchievementNotifier::sharedState()->willSwitchToScene(scene);
}

AppDelegate::~AppDelegate()
{
    // Destructor
}