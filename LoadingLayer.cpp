#include "LoadingLayer.h"
#include "GameManager.h"
#include "GameLevelManager.h"
#include "MenuLayer.h"
#include "TextArea.h"
#include "FMODAudioEngine.h"
#include "ObjectManager.h"
#include "AchievementManager.h"
#include "ObjectToolbox.h"
#include "CCAnimateFrameCache.h"
#include "CCContentManager.h"
#include "CCCircleWave.h"
#include "AppDelegate.h"
#include "CCTextInputNode.h"

LoadingLayer* LoadingLayer::create(bool refresh)
{
    LoadingLayer* ret = new LoadingLayer();
    if (ret->init(refresh)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

const char* LoadingLayer::getLoadingString()
{
    if (m_fromRefresh)
        return "Loading resources";
    int idx = rand() % 100 - 1;
    switch (idx) {
        case 0: return "Listen to the music to help time your jumps";
        case 1: return "Back for more are ya?";
        case 2: return "Use practice mode to learn the layout of a level";
        case 3: return "If at first you don't succeed, try, try again...";
        case 4: return "Customize your character's icon and color!";
        case 5: return "You can download all songs from the level select page!";
        case 6: return "Spikes are not your friends, don't forget to jump";
        case 7: return "Build your own levels using the level editor";
        case 8: return "Go online to play other players levels!";
        case 9: return "Can you beat them all?";
        case 10: return "Here be dragons...";
        case 11: return "Pro tip: Don't crash";
        case 12: return "Hold down to keep jumping";
        case 13: return "The spikes whisper to me...";
        case 14: return "Looking for pixels";
        case 15: return "Loading awesome soundtracks...";
        case 16: return "What if the spikes are the good guys?";
        case 17: return "Pro tip: Jump";
        case 18: return "Does anyone even read this?";
        case 19: return "Collecting scrap metal";
        case 20: return "Waiting for planets to align";
        case 21: return "Starting the flux capacitor";
        case 22: return "Wandering around aimlessly";
        case 23: return "Where did I put that coin...";
        case 24: return "Loading the progressbar";
        case 25: return "Calculating chance of success";
        case 26: return "Hiding secrets";
        case 27: return "Drawing pretty pictures";
        case 28: return "Programmer is sleeping, please wait";
        case 29: return "RobTop is Love, RobTop is Life";
        case 30: return "Play, Crash, Rage, Quit, Repeat";
        case 31: return "Only one button required to crash";
        case 32: return "Such wow, very amaze.";
        case 33: return "Fus Ro DASH!";
        case 34: return "Loading Rage Cannon";
        case 35: return "Counting to 1337";
        case 36: return "It's all in the timing";
        case 37: return "Fake spikes are fake";
        case 38: return "Spikes... OF DOOM!";
        case 39: return "Why don't you go outside?";
        case 40: return "Loading will be finished... soon";
        case 41: return "This seems like a good\nplace to hide a secret...";
        case 42: return "The Vault Keeper's name is 'Spooky'...";
        case 43: return "Hope the big guy doesn't wake up...";
        case 44: return "Shhhh! You're gonna wake the big one!";
        case 45: return "I have been expecting you.";
        case 46: return "A wild RubRub appeared!";
        case 47: return "So many secrets...";
        case 48: return "Hiding rocket launcher";
        case 49: return "It's Over 9000!";
        case 50: return "Programming amazing AI";
        case 51: return "Hiding secret vault";
        case 52: return "Spooky doesn't get out much";
        case 53: return "RubRub was here";
        case 54: return "Warp Speed";
        case 55: return "So, what's up?";
        case 56: return "Hold on, reading the manual";
        case 57: return "I don't know how this works...";
        case 58: return "Why u have to be mad?";
        case 59: return "It is only game...";
        case 60: return "What's a Ninja Muffin?";
        case 61: return "Update 2.3 coming soon";
        case 62: return "Why are you back? Just to suffer?";
        case 63: return "I am porfesional progarmer";
        case 64: return "Welcome to Robert Games";
        case 65: return "Winner of the Fastest Update award";
        case 66: return "You got something to eat?";
        case 67: return "Time to crash at 96%";
        case 68: return "Most relaxing game ever made";
        case 69: return "Now with more colors!";
        case 70: return "Troll Headquarters";
        case 71: return "Welcome to the friendliest place on earth";
        case 72: return "Loading Shape Jumper";
        case 73: return "Made by RobTroll";
        case 74: return "Why can't you just be nice";
        case 75: return "Loading the loading screen";
        case 76: return "Get ready for big fail";
        case 77: return "RTX OFF";
        case 78: return "It's Rob Toppin Time!";
        case 79: return "It's you again. I can smell it.";
        case 80: return "Eat. Sleep. Dash. Repeat.";
        case 81: return "Impossible geometry!";
        case 82: return "Breaking physics...";
        case 83: return "Trying to find more FPS";
        case 84: return "Stay calm, or panic. Whatever you prefer.";
        case 85: return "It's about time...";
        case 86: return "Loading done. Just waiting.";
        case 87: return "Gathering pixels...";
        case 88: return "At the end of the day, it is night.";
        case 89: return "You waited how many years!?";
        case 90: return "50% of the time, you crash, everytime.";
        case 91: return "Dont Dash. Open Inside.";
        case 92: return "Fixing bugs, hold on.";
        case 93: return "Adding bugs, hold on.";
        case 94: return "What are these physics...";
        case 95: return "Loading what, exactly?";
        case 96: return "Dashing to victory!";
        case 97: return "I'm something of a programmer myself.";
        case 98: return "You know what? I don't know...";
        default: return "Unlock new icons and colors by completing achievements!";
    }
}

bool LoadingLayer::init(bool refresh)
{
    if (!cocos2d::CCLayer::init())
        return false;

    m_fromRefresh = refresh;
    cocos2d::CCDirector::sharedDirector()->m_bDisplayStats = true;
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(cocos2d::kCCTexture2DPixelFormat_RGBA8888);

    if (m_fromRefresh) {
        if (GameManager::sharedState()->m_canGetLevelSaveData) {
            GameManager::sharedState()->m_canGetLevelSaveData = false;
            GameLevelManager* glm = GameLevelManager::sharedState();
            glm->getLevelSaveData();
        }
    } else {
        FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
        engine->setup();
        if (GameManager::sharedState()->m_canGetLevelSaveData) {
            GameManager::sharedState()->m_canGetLevelSaveData = false;
            GameLevelManager* glm = GameLevelManager::sharedState();
            glm->getLevelSaveData();
        }
    }

    cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
    texCache->addImage("GJ_LaunchSheet.png", false);
    cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    frameCache->addSpriteFramesWithFile("GJ_LaunchSheet.plist");

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    GameManager* gm = GameManager::sharedState();
    cocos2d::CCTexture2D* bgTexture = gm->getBGTexture(1);
    cocos2d::CCSprite* bgSprite = cocos2d::CCSprite::createWithTexture(bgTexture);
    this->addChild(bgSprite);
    bgSprite->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
    bgSprite->setScale(AppDelegate::get()->bgScale());
    bgSprite->setColor(ccc3(0, 102, 255));

    cocos2d::CCSprite* logo = cocos2d::CCSprite::createWithSpriteFrameName("GJ_logo_001.png");
    this->addChild(logo);
    logo->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCSprite* robtopLogo = cocos2d::CCSprite::createWithSpriteFrameName("RobTopLogoBig_001.png");
    this->addChild(robtopLogo);
    robtopLogo->setPosition(logo->getPosition() + ccp(0.0f, 80.0f));

    m_loadStep = 0;

    cocos2d::CCSprite* cocosLogo = cocos2d::CCSprite::createWithSpriteFrameName("cocos2DxLogo.png");
    this->addChild(cocosLogo);
    cocosLogo->setPosition(ccp(winSize.width - 34.0f, 13.0f));
    cocosLogo->setScale(0.6f);

    cocos2d::CCSprite* fmodLogo = cocos2d::CCSprite::createWithSpriteFrameName("fmodLogo.png");
    this->addChild(fmodLogo);
    fmodLogo->setPosition(cocosLogo->getPosition() + ccp(0.0f, 20.0f));
    fmodLogo->setScale(0.6f);
    fmodLogo->setColor(ccc3(255, 255, 255));

    m_caption = cocos2d::CCLabelBMFont::create(getLoadingString(), "bigFont.fnt");
    this->addChild(m_caption);
    m_caption->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 70.0f));
    m_caption->setScale(0.7f);
    m_caption->setVisible(false);

    m_textArea = TextArea::create(getLoadingString(), "goldFont.fnt", 1.0f, 440.0f, ccp(0.5f, 0.5f), 28.0f, false);
    this->addChild(m_textArea);
    m_textArea->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 100.0f));
    m_textArea->setScale(0.7f);

    float captionWidth = m_caption->getContentSize().width;
    if (captionWidth > 300.0f) {
        m_caption->setScale(300.0f / captionWidth);
    }

    if (m_caption->getScale() < 0.7f) {
        m_caption->setScale(m_caption->getScale());
    } else {
        m_caption->setScale(0.7f);
    }

    cocos2d::CCSprite* sliderGroove = cocos2d::CCSprite::create("slidergroove.png");
    this->addChild(sliderGroove, 3);

    m_sliderBar = cocos2d::CCSprite::create("sliderBar.png");
    float grooveHeight = sliderGroove->getContentSize().height;
    m_sliderGrooveXPos = grooveHeight - 4.0f;
    m_sliderGrooveHeight = 8.0f;

    cocos2d::ccTexParams texParams;
    texParams.minFilter = GL_LINEAR;
    texParams.magFilter = GL_LINEAR;
    texParams.wrapS = GL_REPEAT;
    texParams.wrapT = GL_REPEAT;
    m_sliderBar->getTexture()->setTexParameters(&texParams);

    sliderGroove->addChild(m_sliderBar, -1);
    m_sliderBar->setAnchorPoint(ccp(0.0f, 0.0f));
    m_sliderBar->setPosition(ccp(2.0f, 4.0f));

    sliderGroove->setPosition(ccp(
        m_caption->getPosition().x,
        m_textArea->getPosition().y + 40.0f));

    updateProgress(0);

    cocos2d::CCDirector::sharedDirector()->getActionManager()->addAction(
        cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadAssets)),
            nullptr),
        this, false);

    return true;
}

void LoadingLayer::loadAssets()
{
    if (m_loadStep <= 0) {
        cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
        texCache->addImage("GJ_GameSheet.png", false);
        cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
        frameCache->addSpriteFramesWithFile("GJ_GameSheet.plist");
    } else {
        switch (m_loadStep) {
            case 1: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("GJ_GameSheet02.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("GJ_GameSheet02.plist");
                break;
            }
            case 2: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("GJ_GameSheet03.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("GJ_GameSheet03.plist");
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->addImage("GJ_GameSheetEditor.png", false);
                cocos2d::CCSpriteFrameCache* frameCache2 = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache2->addSpriteFramesWithFile("GJ_GameSheetEditor.plist");
                break;
            }
            case 3: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("GJ_GameSheet04.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("GJ_GameSheet04.plist");
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->prepareAsyncLoading();
                break;
            }
            case 4: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("GJ_GameSheetGlow.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("GJ_GameSheetGlow.plist");
                break;
            }
            case 5: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("FireSheet_01.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("FireSheet_01.plist");
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->addImage("GJ_ShopSheet.png", false);
                cocos2d::CCSpriteFrameCache* frameCache2 = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache2->addSpriteFramesWithFile("GJ_ShopSheet.plist");
                cocos2d::CCTextureCache* texCache3 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache3->addImage("smallDot.png", false);
                cocos2d::CCTextureCache* texCache4 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache4->addImage("square02_001.png", false);
                cocos2d::CCTextureCache* texCache5 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache5->addImage("GJ_ParticleSheet.png", false);
                cocos2d::CCSpriteFrameCache* frameCache3 = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache3->addSpriteFramesWithFile("GJ_ParticleSheet.plist");
                cocos2d::CCTextureCache* texCache6 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache6->addImage("PixelSheet_01.png", false);
                cocos2d::CCSpriteFrameCache* frameCache4 = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache4->addSpriteFramesWithFile("PixelSheet_01.plist");
                break;
            }
            case 6: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("CCControlColourPickerSpriteSheet.png", false);
                cocos2d::CCSpriteFrameCache* frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
                frameCache->addSpriteFramesWithFile("CCControlColourPickerSpriteSheet.plist");
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->addImage("GJ_gradientBG.png", false);
                cocos2d::CCTextureCache* texCache3 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache3->addImage("edit_barBG_001.png", false);
                cocos2d::CCTextureCache* texCache4 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache4->addImage("GJ_button_01.png", false);
                cocos2d::CCTextureCache* texCache5 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache5->addImage("slidergroove2.png", false);
                cocos2d::CCTextureCache* texCache6 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache6->addImage("sliderBar2.png", false);
                break;
            }
            case 7: {
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("GJ_square01.png", false);
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->addImage("GJ_square02.png", false);
                cocos2d::CCTextureCache* texCache3 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache3->addImage("GJ_square03.png", false);
                cocos2d::CCTextureCache* texCache4 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache4->addImage("GJ_square04.png", false);
                cocos2d::CCTextureCache* texCache5 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache5->addImage("GJ_square05.png", false);
                cocos2d::CCTextureCache* texCache6 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache6->addImage("gravityLine_001.png", false);
                break;
            }
            case 8: {
                TextArea::create("temp", "chatFont.fnt", 1.0f, 200.0f, ccp(0.0f, 1.0f), 20.0f, false);
                cocos2d::CCTextureCache* texCache = cocos2d::CCTextureCache::sharedTextureCache();
                texCache->addImage("goldFont.png", false);
                cocos2d::CCTextureCache* texCache2 = cocos2d::CCTextureCache::sharedTextureCache();
                texCache2->addImage("bigFont.png", false);
                cocos2d::CCLabelBMFont::create("", "goldFont.fnt");
                cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
                break;
            }
            case 9: {
                ObjectManager* om = ObjectManager::instance();
                om->setup();
                CCAnimateFrameCache* animateCache = CCAnimateFrameCache::sharedSpriteFrameCache();
                animateCache->addSpriteFramesWithFile("Robot_AnimDesc.plist");
                CCContentManager* cm = CCContentManager::sharedManager();
                cm->addDict("glassDestroy01.plist", false);
                CCContentManager* cm2 = CCContentManager::sharedManager();
                cm2->addDict("coinPickupEffect.plist", false);
                CCContentManager* cm3 = CCContentManager::sharedManager();
                cm3->addDict("explodeEffect.plist", false);
                AchievementManager::sharedState();
                ObjectToolbox::sharedState();
                break;
            }
            case 10:
                break;
            case 11:
                if (GameManager::sharedState()->m_recordGameplay && !m_fromRefresh) {
                    GameManager::sharedState()->m_everyPlaySetup = true;
                }
                break;
            case 12:
                CCTextInputNode::create(200.0f, 50.0f, "Temp", "Thonburi");
                break;
            case 13:
                if (!m_fromRefresh) {
                    CCCircleWave* circleWave = CCCircleWave::create(10.0f, 5.0f, 0.3f, true);
                    this->addChild(circleWave);
                    circleWave->setPosition(ccp(-100.0f, -100.0f));
                }
                break;
            case 14:
                break;
            default:
                break;
        }
    }

    if (m_loadStep != 14) {
        int nextStep = m_loadStep + 1;
        m_loadStep = nextStep;
        updateProgress(7 * nextStep);
        cocos2d::CCDirector::sharedDirector()->getActionManager()->addAction(
            cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.01f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadAssets)),
                nullptr),
            this, false);
        return;
    }

    cocos2d::CCDirector::sharedDirector()->setSceneDelegate(AppDelegate::get());
    GameManager* gm = GameManager::sharedState();
    if (gm->getGameVariable("0115")) {
        cocos2d::CCDirector::sharedDirector()->toggleShowFPS(true, "chatFont.fnt", ccp(0.0f, 0.0f));
    }
    cocos2d::CCDirector::sharedDirector()->m_bFastMenu = GameManager::sharedState()->getGameVariable("0168");
    GameManager::sharedState()->updateCustomFPS();
    AppDelegate::get()->loadingIsFinished();
    if (!m_fromRefresh) {
        GameManager::sharedState()->updateMusic();
        GameManager::sharedState()->fadeInMenuMusic();
        GameManager::sharedState()->syncPlatformAchievements();
    }
    loadingFinished();
}

void LoadingLayer::loadingFinished()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCScene* scene;
    if (m_fromRefresh) {
        scene = MenuLayer::scene(true);
    } else {
        scene = MenuLayer::scene(false);
    }
    director->replaceScene(scene);
}

cocos2d::CCScene* LoadingLayer::scene(bool refresh)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    LoadingLayer* layer = LoadingLayer::create(refresh);
    scene->addChild(layer);
    return scene;
}

void LoadingLayer::updateProgress(int progress)
{
    float width = m_sliderGrooveXPos * ((float)progress / 100.0f);
    if (m_sliderGrooveXPos <= width)
        width = m_sliderGrooveXPos;
    cocos2d::CCRect rect(0.0f, 0.0f, width, m_sliderGrooveHeight);
    m_sliderBar->setTextureRect(rect);
}

LoadingLayer::~LoadingLayer()
{
}