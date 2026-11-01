#include "AchievementNotifier.h"
#include "FMODAudioEngine.h"
#include "GameToolbox.h"

static int g_achievementCounter = 0;

AchievementNotifier* AchievementNotifier::sharedState()
{
    static AchievementNotifier* instance = nullptr;
    if (!instance)
    {
        instance = new AchievementNotifier();
        instance->init();
    }
    return instance;
}

bool AchievementNotifier::init()
{
    m_achievementBarArray = cocos2d::CCArray::create();
    m_achievementBarArray->retain();
    m_activeAchievementBar = nullptr;
    m_nextScene = nullptr;
    return true;
}

AchievementNotifier::~AchievementNotifier()
{
    CC_SAFE_RELEASE(m_achievementBarArray);
    CC_SAFE_RELEASE(m_activeAchievementBar);
}

void AchievementNotifier::achievementDisplayFinished()
{
    if (m_activeAchievementBar)
    {
        m_activeAchievementBar->removeMeAndCleanup();
        CC_SAFE_RELEASE(m_activeAchievementBar);
        m_activeAchievementBar = nullptr;
    }
    showNextAchievement();
}

void AchievementNotifier::showNextAchievement()
{
    if (m_achievementBarArray->count() > 0)
    {
        FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
        engine->playEffect("achievement_01.ogg", (g_achievementCounter * 0.05f) + 1.0f, 0.0f, 0.3f);
        g_achievementCounter++;

        AchievementBar* bar = (AchievementBar*)m_achievementBarArray->objectAtIndex(0);
        m_activeAchievementBar = bar;
        bar->retain();
        m_achievementBarArray->removeObjectAtIndex(0, true);

        if (m_nextScene)
        {
            bar->m_nextScene = m_nextScene;
        }

        bar->show();
    }
    else
    {
        g_achievementCounter = 0;
    }
}

void AchievementNotifier::willSwitchToScene(cocos2d::CCScene* scene)
{
    m_nextScene = scene;

    if (m_activeAchievementBar)
    {
        if (scene != m_activeAchievementBar->getParent())
        {
            m_activeAchievementBar->retain();
            m_activeAchievementBar->removeFromParentAndCleanup(false);
            scene->addChild(m_activeAchievementBar, 105);
            m_activeAchievementBar->release();
            m_activeAchievementBar->resumeSchedulerAndActions();
        }
    }
}