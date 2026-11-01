#include "SecretLayer3.h"
#include "CCMenuItemSpriteExtra.h"
#include "DungeonBarsSprite.h"
#include "RewardUnlockLayer.h"
#include "GJChestSprite.h"
#include "GJRewardItem.h"
#include "AchievementManager.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "FMODAudioEngine.h"
#include "SecretLayer2.h"
#include "PlatformToolbox.h"
#include "GameToolbox.h"

SecretLayer3* SecretLayer3::create()
{
    SecretLayer3* returnVal = new SecretLayer3();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

cocos2d::CCScene* SecretLayer3::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer3* layer = SecretLayer3::create();
    scene->addChild(layer);
    return scene;
}

bool SecretLayer3::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    cocos2d::CCTexture2DPixelFormat prevFormat = cocos2d::CCTexture2D::defaultAlphaPixelFormat();
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(cocos2d::kCCTexture2DPixelFormat_RGBA8888);
    cocos2d::CCTextureCache::sharedTextureCache()->addImage("DungeonSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("DungeonSheet.plist");
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(prevFormat);

    GameManager::sharedState()->fadeInMusic("secretLoop02.mp3");
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -3);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(30, 0, 100));

    cocos2d::CCSprite* gate = cocos2d::CCSprite::createWithSpriteFrameName("dungeon_gate_001.png");
    addChild(gate, 2);
    gate->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 5.0f));

    cocos2d::CCSprite* foreground = cocos2d::CCSprite::createWithSpriteFrameName("dungeon_foreground_001.png");
    addChild(foreground, 3);
    foreground->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, 15.0f));

    cocos2d::CCSprite* fire1 = cocos2d::CCSprite::createWithSpriteFrameName("Fire_02_looped_001.png");
    addChild(fire1, 4);
    fire1->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 160.0f, winSize.height * 0.5f + 50.0f));

    cocos2d::CCSprite* fire2 = cocos2d::CCSprite::createWithSpriteFrameName("Fire_02_looped_001.png");
    addChild(fire2, 3);
    fire2->setScale(fire1->getScale() * 2.0f);
    fire2->setPosition(fire1->getPosition());
    fire1->setColor(cocos2d::ccc3(255, 0, 0));
    fire2->setColor(cocos2d::ccc3(255, 255, 255));

    cocos2d::CCArray* fireFrames = cocos2d::CCArray::create();
    cocos2d::CCArray* fireFrames2 = cocos2d::CCArray::create();

    for (int i = 1; i < 10; i++)
    {
        cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("Fire_02_looped_%03i.png", i);
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
        if (frame) fireFrames->addObject(frame);

        cocos2d::CCString* frameName2 = cocos2d::CCString::createWithFormat("Fire_02_2_looped_%03i.png", i);
        cocos2d::CCSpriteFrame* frame2 = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName2->getCString());
        if (frame2) fireFrames2->addObject(frame2);
    }

    fire1->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCAnimate::create(
        cocos2d::CCAnimation::createWithSpriteFrames(fireFrames, 0.08f))));
    fire2->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCAnimate::create(
        cocos2d::CCAnimation::createWithSpriteFrames(fireFrames2, 0.08f))));

    cocos2d::CCSprite* shine = cocos2d::CCSprite::createWithSpriteFrameName("dungeonShine_001.png");
    shine->setPosition(fire1->getPosition() + cocos2d::CCPoint(0.0f, -10.0f));
    addChild(shine, 5);
    shine->setScale(1.2f);
    shine->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    shine->setOpacity(200);
    shine->setColor(cocos2d::ccc3(255, 200, 0));

    shine->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCSequence::create(
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(1.6f, 160), 1.3f),
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(1.6f, 225), 1.3f),
        nullptr)));

    shine->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCSequence::create(
        cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(1.0f, 1.0f, 1.0f), 1.5f),
        cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(1.0f, 1.0f, 1.2f), 1.5f),
        nullptr)));

    cocos2d::CCSprite* torch = cocos2d::CCSprite::createWithSpriteFrameName("dungeonTorch_001.png");
    addChild(torch, 6);
    torch->setPosition(fire1->getPosition() + cocos2d::CCPoint(-1.0f, -10.0f));

    cocos2d::CCSprite* torchShine = cocos2d::CCSprite::createWithSpriteFrameName("dungeonShine_001.png");
    torchShine->setPosition(fire1->getPosition() + cocos2d::CCPoint(0.0f, -14.0f));
    addChild(torchShine, 7);
    torchShine->setScale(0.6f);
    torchShine->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    torchShine->setOpacity(175);
    torchShine->setColor(cocos2d::ccc3(255, 150, 0));

    cocos2d::CCSprite* barsBottom = cocos2d::CCSprite::createWithSpriteFrameName("dungeon_bars_bottom_001.png");
    addChild(barsBottom, 12);
    barsBottom->setPosition(gate->getPosition() + cocos2d::CCPoint(0.0f, -52.0f));

    m_dungeonBars = DungeonBarsSprite::create();
    addChild(m_dungeonBars, 9);
    m_dungeonBars->setPosition(gate->getPosition() + cocos2d::CCPoint(-2.0f, 10.0f));

    if (GameManager::sharedState()->getUGV("13"))
        m_dungeonBars->setVisible(false);

    m_demonEyes = cocos2d::CCSprite::createWithSpriteFrameName("secretEyes_01_001.png");
    m_demonEyes->setPosition(gate->getPosition() + cocos2d::CCPoint(13.5f, 6.0f));
    addChild(m_demonEyes, 10);

    if (!GameManager::sharedState()->getUGV("12") || GameManager::sharedState()->getUGV("13"))
        m_demonEyes->setVisible(false);
    else
        animateEyes();

    m_demonBody = cocos2d::CCSprite::createWithSpriteFrameName("secretBody_01_001.png");
    addChild(m_demonBody, 9);
    m_demonBody->setPosition(m_demonEyes->getPosition() + cocos2d::CCPoint(0.0f, -2.0f));
    m_demonBody->setVisible(false);

    cocos2d::CCMenu* lockMenu = cocos2d::CCMenu::create();
    addChild(lockMenu, 12);

    m_locksArray = cocos2d::CCArray::create();
    m_locksArray->retain();

    cocos2d::CCPoint locksBase = gate->getPosition() + cocos2d::CCPoint(160.0f, 20.0f);
    cocos2d::CCPoint locksNode = lockMenu->convertToNodeSpace(locksBase);

    if (!GameManager::sharedState()->getUGV("14"))
    {
        cocos2d::CCSprite* lock1 = cocos2d::CCSprite::createWithSpriteFrameName("secretLock01_001.png");
        CCMenuItemSpriteExtra* lockBtn1 = CCMenuItemSpriteExtra::create(lock1, nullptr, menu_selector(SecretLayer3::onClick));
        lockMenu->addChild(lockBtn1);
        lockBtn1->setPosition(locksNode + cocos2d::CCPoint(-30.0f, -25.0f));
        lockBtn1->setTag(1);
        m_locksArray->addObject(lockBtn1);
    }

    if (!GameManager::sharedState()->getUGV("15"))
    {
        cocos2d::CCSprite* lock2 = cocos2d::CCSprite::createWithSpriteFrameName("secretLock02_001.png");
        CCMenuItemSpriteExtra* lockBtn2 = CCMenuItemSpriteExtra::create(lock2, nullptr, menu_selector(SecretLayer3::onClick));
        lockMenu->addChild(lockBtn2);
        lockBtn2->setPosition(locksNode + cocos2d::CCPoint(0.0f, 25.0f));
        lockBtn2->setTag(2);
        m_locksArray->addObject(lockBtn2);
    }

    if (!GameManager::sharedState()->getUGV("16"))
    {
        cocos2d::CCSprite* lock3 = cocos2d::CCSprite::createWithSpriteFrameName("secretLock03_001.png");
        CCMenuItemSpriteExtra* lockBtn3 = CCMenuItemSpriteExtra::create(lock3, nullptr, menu_selector(SecretLayer3::onClick));
        lockMenu->addChild(lockBtn3);
        lockBtn3->setPosition(locksNode + cocos2d::CCPoint(30.0f, -25.0f));
        lockBtn3->setTag(3);
        m_locksArray->addObject(lockBtn3);
    }

    if (!GameManager::sharedState()->getUGV("12"))
    {
        for (unsigned int i = 0; i < m_locksArray->count(); i++)
        {
            cocos2d::CCNode* lock = static_cast<cocos2d::CCNode*>(m_locksArray->objectAtIndex(i));
            lock->setVisible(false);
        }
    }

    if (!GameStatsManager::sharedState()->isSpecialChestUnlocked("0001"))
    {
        GJRewardItem* reward = GameStatsManager::sharedState()->getRewardForSpecialChest("0001");
        GJChestSprite* chest = GJChestSprite::create(reward->m_rewardType);
        chest->switchToState(ChestState::Closed, 0);
        chest->setScale(0.6f);

        cocos2d::CCArray* children = chest->getChildren();
        if (children->count() > 0)
        {
            cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(children->objectAtIndex(0));
            child->setColor(cocos2d::ccc3(255, 100, 0));
        }

        CCMenuItemSpriteExtra* chestBtn = CCMenuItemSpriteExtra::create(chest, nullptr, menu_selector(SecretLayer3::onChest01));
        chestBtn->setContentSize(cocos2d::CCSize(60.0f, 48.0f));
        chest->setPosition(chestBtn->getContentSize().width * 0.5f, chestBtn->getContentSize().height * 0.5f);
        lockMenu->addChild(chestBtn);
        chestBtn->setPosition(lockMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width - 80.0f, 60.0f)));
    }

    if (!GameStatsManager::sharedState()->isSpecialChestUnlocked("0002"))
    {
        GJRewardItem* reward = GameStatsManager::sharedState()->getRewardForSpecialChest("0002");
        GJChestSprite* chest = GJChestSprite::create(reward->m_rewardType);
        chest->switchToState(ChestState::Closed, 0);
        chest->setScale(0.6f);

        CCMenuItemSpriteExtra* chestBtn = CCMenuItemSpriteExtra::create(chest, nullptr, menu_selector(SecretLayer3::onChest02));
        m_secretChest = chestBtn;
        chestBtn->setContentSize(cocos2d::CCSize(60.0f, 48.0f));
        chest->setPosition(chestBtn->getContentSize().width * 0.5f, chestBtn->getContentSize().height * 0.5f);

        cocos2d::CCMenu* chestMenu = cocos2d::CCMenu::create();
        addChild(chestMenu, 10);
        chestMenu->addChild(chestBtn);
        chestBtn->setPosition(chestMenu->convertToNodeSpace(
            gate->getPosition() + cocos2d::CCPoint(0.0f, -24.0f)));

        chestBtn->m_animationEnabled = false;
        chestBtn->m_colorEnabled = true;

        if (!GameManager::sharedState()->getUGV("13"))
        {
            m_secretChest->setEnabled(false);
            m_secretChest->setVisible(false);
        }
    }

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer3::onBack));
    backBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    return true;
}

void SecretLayer3::dialogClosed(DialogLayer* layer)
{
    int tag = layer->getTag();

    if (tag == 4)
    {
        for (int i = 0; i < 2; i++)
        {
            cocos2d::CCLayerColor* fade = cocos2d::CCLayerColor::create();
            addChild(fade, 100);
            fade->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
            fade->runAction(cocos2d::CCSequence::create(
                cocos2d::CCFadeTo::create(2.0f, 0),
                cocos2d::CCCallFunc::create(fade, callfunc_selector(cocos2d::CCNode::removeFromParent)),
                nullptr));
        }

        m_demonEyes->stopAllActions();
        m_demonEyes->setVisible(false);
        m_demonBody->setVisible(false);

        if (m_secretChest)
        {
            m_secretChest->setVisible(true);
            m_secretChest->setEnabled(true);
        }

        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(3.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::revealStep5)),
            nullptr));
    }
    else if (tag == 5)
    {
        GameManager::sharedState()->setUGV("13", 1);
        m_lockInput = false;
    }
    else if (tag == 3)
    {
        revealStep3();
    }
    else if (tag == 1)
    {
        firstInteractionStep3();
    }
    else if (tag == 2)
    {
        GameManager::sharedState()->setUGV("12", 1);
        m_lockInput = false;
    }
    else if (tag == 6)
    {
        m_lockInput = false;
    }
}

void SecretLayer3::animateEyes()
{
    int randomValue = rand();
    float delay = (float)randomValue * 4.6566e-10f * 4.0f + 2.0f;

    m_demonEyes->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCScaleTo::create(0.03f, 1.0f),
        cocos2d::CCHide::create(),
        cocos2d::CCDelayTime::create(0.1f),
        cocos2d::CCShow::create(),
        cocos2d::CCScaleTo::create(0.02f, 1.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::animateEyes)),
        nullptr));
}

void SecretLayer3::keyBackClicked()
{
    onBack(nullptr);
}

void SecretLayer3::onBack(cocos2d::CCObject* sender)
{
    if (m_lockInput)
        return;

    if (GameManager::sharedState()->getUGV("12"))
    {
        setKeypadEnabled(false);
        cocos2d::CCDirector::sharedDirector()->replaceScene(
            cocos2d::CCTransitionFade::create(0.5f, SecretLayer2::scene()));
    }
    else
    {
        firstInteractionStep1();
    }
}

void SecretLayer3::onExit()
{
    cocos2d::CCLayer::onExit();
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("DungeonSheet.plist");
    cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("DungeonSheet.png");
}

void SecretLayer3::firstInteractionStep1()
{
    m_lockInput = true;
    m_demonEyes->stopAllActions();
    m_demonEyes->setScale(1.0f);
    m_demonEyes->setOpacity(0);
    m_demonEyes->setVisible(true);
    m_demonEyes->runAction(cocos2d::CCFadeIn::create(1.0f));

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(2.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::firstInteractionStep2)),
        nullptr));
}

void SecretLayer3::firstInteractionStep2()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[] = {
        "<cr>Well,<d010> well,<d010> well.</c><d050>\nWhat do we have here?",
        "You look like someone who is interested in a <cl>deal</c><d010>.<d010>.<d010>.",
        "<cy>Help</c> me get out of here, and I will make it worth your while.",
        "See those locks over there?<d040>\n<cl>You need to unlock them.</c>",
    };

    for (const char* text : kTexts)
    {
        DialogObject* dialog = DialogObject::create("...", text, 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
    addChild(dialogLayer, 100);
    dialogLayer->m_delegate = this;
    dialogLayer->setTag(1);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Top);
    dialogLayer->animateIn(DialogAnimationType::FromCenter);

    animateEyes();
}

void SecretLayer3::firstInteractionStep3()
{
    for (unsigned int i = 0; i < m_locksArray->count(); i++)
    {
        CCMenuItemSpriteExtra* lock = static_cast<CCMenuItemSpriteExtra*>(m_locksArray->objectAtIndex(i));
        lock->setVisible(true);

        cocos2d::CCNode* sprite = lock->getNormalImage();
        sprite->setScale(2.0f);
        sprite->setOpacity(0);

        cocos2d::CCAction* spawn = cocos2d::CCSpawn::createWithTwoActions(
            cocos2d::CCEaseBounceOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f)),
            cocos2d::CCFadeIn::create(0.15f));

        sprite->runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(i * 0.1f),
            spawn,
            nullptr));
    }

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(2.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::firstInteractionStep4)),
        nullptr));
}

void SecretLayer3::firstInteractionStep4()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[] = {
        "<cl>RubRub</c> locked me in here, for no reason.",
        "He said there would be chicken.",
        "<cr>There was no chicken...</c>",
        "I cannot stay here any longer.<d050>\n<cr>You must find a way to release me.</c>",
    };

    for (const char* text : kTexts)
    {
        DialogObject* dialog = DialogObject::create("...", text, 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
    addChild(dialogLayer, 100);
    dialogLayer->m_delegate = this;
    dialogLayer->setTag(2);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Top);
    dialogLayer->animateIn(DialogAnimationType::FromCenter);
}

void SecretLayer3::revealStep1()
{
    m_lockInput = true;
    m_dungeonBars->animateOutBars();

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(4.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::revealStep2)),
        nullptr));
}

void SecretLayer3::revealStep2()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[] = {
        "I<d030>.<d030>.<d050>.<d030> I am <cg>free</c>...<d02>?",
        "So many years lost.<d050> <cl>Locked</c> away in that filthy cage...",
        "Now that I have my <cg>freedom</c>,<d020>\nI must continue where I left off.",
    };

    for (const char* text : kTexts)
    {
        DialogObject* dialog = DialogObject::create("...", text, 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
    addChild(dialogLayer, 100);
    dialogLayer->m_delegate = this;
    dialogLayer->setTag(3);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Top);
    dialogLayer->animateIn(DialogAnimationType::FromCenter);
}

void SecretLayer3::revealStep3()
{
    m_demonBody->setOpacity(0);
    m_demonBody->setVisible(true);
    m_demonBody->runAction(cocos2d::CCEaseInOut::create(cocos2d::CCFadeIn::create(3.0f), 1.5f));

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(4.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::revealStep4)),
        nullptr));
}

void SecretLayer3::revealStep4()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    std::string playerName = GameManager::sharedState()->m_playerName;

    std::string destroyText = cocos2d::CCString::createWithFormat(
        "<cr>DESTROY</c> the world of <cg>Geometry Dash</c>!<d060> And I couldn't have done it without you <cl>%s</c>.",
        playerName.c_str())->getCString();

    std::vector<std::string> texts = {
        "<cy>RubRub</c> told me he was cooking some chicken.<d080> Then he locked me in.",
        "Now that you have freed me, I can continue my master plan.",
        destroyText,
        "But I almost forgot, I left you a small <cg>gift</c>. <d050>As a token of my appreciation.",
        "You should <cy>prepare</c> yourself for when I return.",
        "<cr>You will need it</c><d030>.<d035>.<d040>.",
    };

    for (const std::string& text : texts)
    {
        DialogObject* dialog = DialogObject::create("...", text.c_str(), 16, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
    addChild(dialogLayer, 100);
    dialogLayer->m_delegate = this;
    dialogLayer->setTag(4);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Top);
    dialogLayer->animateIn(DialogAnimationType::FromCenter);
}

void SecretLayer3::revealStep5()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[] = {
        "Hey I heard some noi...",
        "Wait.<d010>.<d010>.<d020> Is the gate<d010>.<d010>.<d010>.",
        "<d010>.<d010>.<d010>. You opened the gate<d010>.<d010>.<d010>.",
        "YOU OPENED THE GATE!\n<d030>GAH THIS IS BAD.",
        "<cr>BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD</c><d010>.<d010>.<d010>.",
    };

    static const int kActorIDs[] = { 2, 4, 17, 18, 18 };

    for (int i = 0; i < 5; i++)
    {
        DialogObject* dialog = DialogObject::create("The Keymaster", kTexts[i], kActorIDs[i], 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    addChild(dialogLayer, 100);
    dialogLayer->m_delegate = this;
    dialogLayer->setTag(5);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateIn(DialogAnimationType::FromLeft);
}

void SecretLayer3::showUnlockDialog()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    bool key1 = GameManager::sharedState()->getUGV("14");
    bool key2 = GameManager::sharedState()->getUGV("15");
    bool key3 = GameManager::sharedState()->getUGV("16");

    int total = key1 + key2 + key3;

    if (total != 2)
    {
        DialogObject* d1 = DialogObject::create("...", "The <cg>key</c>, you found it?", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d1);
        DialogObject* d2 = DialogObject::create("...", "Excellent, only <cl>two</c> locks left.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d2);
    }
    else
    {
        DialogObject* d1 = DialogObject::create("...", "You found a second <cg>key</c>, impressive<d010>.<d010>.<d010>.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d1);
        DialogObject* d2 = DialogObject::create("...", "Only <cy>one</c> lock left.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d2);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
    addChild(dialogLayer, 100);
    dialogLayer->setTag(6);
    dialogLayer->m_delegate = this;
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateInRandomSide();
}

void SecretLayer3::onChest01(cocos2d::CCObject* sender)
{
    if (m_lockInput)
        return;

    sender->setVisible(false);

    GJRewardItem* reward = GameStatsManager::sharedState()->unlockSpecialChest("0001");

    if (reward)
    {
        RewardUnlockLayer* layer = RewardUnlockLayer::create(reward->m_rewardType, nullptr);
        addChild(layer, 100);
        layer->showCollectReward(reward);
    }
}

void SecretLayer3::onChest02(cocos2d::CCObject* sender)
{
    if (m_lockInput)
        return;

    sender->setVisible(false);

    GJRewardItem* reward = GameStatsManager::sharedState()->unlockSpecialChest("0002");

    if (reward)
    {
        RewardUnlockLayer* layer = RewardUnlockLayer::create(reward->m_rewardType, nullptr);
        addChild(layer, 100);
        layer->showCollectReward(reward);
    }
}

void SecretLayer3::onClick(cocos2d::CCObject* sender)
{
    if (m_lockInput)
        return;

    int tag = sender->getTag();
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    if (tag == 1)
    {
        GameManager::sharedState()->setUGV("19", 1);

        if (!GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 1))
        {
            DialogObject* d1 = DialogObject::create("...", "The first <cl>key</c> is hidden in a <co>secret gauntlet</c>...", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d1);
            DialogObject* d2 = DialogObject::create("...", "Only a chosen <cl>few</c> can find it, but I will grant you this <cg>gift</c>...", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d2);
            DialogObject* d3 = DialogObject::create("...", "Complete the <cr>Chaos Gauntlet</c>, and the <cl>key</c> will be yours<d010>.<d010>.<d010>.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d3);

            DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
            addChild(dialogLayer, 100);
            dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
            dialogLayer->animateIn(DialogAnimationType::FromCenter);
            return;
        }

        GameManager::sharedState()->setUGV("14", 1);
    }
    else if (tag == 2)
    {
        if (!GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 2))
        {
            DialogObject* d1 = DialogObject::create("...", "They tried to <cl>hide</c> it from me,\nbut I have my <cr>spies</c>.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d1);
            DialogObject* d2 = DialogObject::create("...", "The <cg>second key</c> is locked away in a <cl>secret chest</c>.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d2);
            DialogObject* d3 = DialogObject::create("...", "Unlock <cy>50</c> secret chests to claim the <cg>key</c>.", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d3);

            DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
            addChild(dialogLayer, 100);
            dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
            dialogLayer->animateIn(DialogAnimationType::FromCenter);
            return;
        }

        GameManager::sharedState()->setUGV("15", 1);
    }
    else if (tag == 3)
    {
        if (!GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 3))
        {
            DialogObject* d1 = DialogObject::create("...", "The <co>third key</c> is hidden well.\n<d030>Even I have not been able to find it.", 15, 0.9f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d1);
            DialogObject* d2 = DialogObject::create("...", "I suspect it is guarded by <cr>The Gatekeeper</c>.\n<d030><cl>RubRub</c>'s trusted pet.<d010>.<d010>.<d010>.", 15, 0.8f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d2);
            DialogObject* d3 = DialogObject::create("...", "To claim the <co>key</c> you must fool him.\n<d040>I do not know how.", 15, 0.9f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d3);
            DialogObject* d4 = DialogObject::create("...", "<cg>You will find The Gatekeeper at the end of the world.</c>", 15, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d4);
            DialogObject* d5 = DialogObject::create("...", "<cg>Touch the ground beneath the promise, and the door will be revealed.</c>", 15, 0.8f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d5);

            GameManager::sharedState()->setUGV("6", 1);

            DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 4);
            addChild(dialogLayer, 100);
            dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
            dialogLayer->animateIn(DialogAnimationType::FromCenter);
            return;
        }

        GameManager::sharedState()->setUGV("16", 1);
    }

    int chestColorR, chestColorG, chestColorB;

    if (tag == 1) { chestColorR = 255; chestColorG = 100; chestColorB = 0; }
    else if (tag == 2) { chestColorR = 50; chestColorG = 255; chestColorB = 50; }
    else { chestColorR = 0; chestColorG = 200; chestColorB = 255; }

    sender->setVisible(false);

    FMODAudioEngine::sharedEngine()->playEffect("chestClick.ogg");
    FMODAudioEngine::sharedEngine()->playEffect("chestOpen01.ogg");

    cocos2d::CCNode* senderSprite = sender->getNormalImage();
    cocos2d::CCPoint worldPos = senderSprite->convertToWorldSpace(senderSprite->getPosition());

    cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create("coinPickupEffect.plist");
    particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
    particles->setStartColor({chestColorR / 255.0f, chestColorG / 255.0f, chestColorB / 255.0f, 1.0f});
    addChild(particles, 1);
    particles->setPosition(worldPos);

    CCCircleWave* wave1 = CCCircleWave::create(0.0f, 25.0f, 0.3f, 0);
    wave1->m_color = cocos2d::ccc3(chestColorR, chestColorG, chestColorB);
    wave1->setPosition(worldPos);
    addChild(wave1, 0);

    CCCircleWave* wave2 = CCCircleWave::create(5.0f, 60.0f, 0.3f, 0);
    wave2->m_color = cocos2d::ccc3(chestColorR, chestColorG, chestColorB);
    wave2->setPosition(worldPos);
    addChild(wave2, 0);

    int unlockedKeys = 0;
    if (GameManager::sharedState()->getUGV("14")) unlockedKeys++;
    if (GameManager::sharedState()->getUGV("15")) unlockedKeys++;
    if (GameManager::sharedState()->getUGV("16")) unlockedKeys++;

    if (unlockedKeys == 3)
    {
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::revealStep1)),
            nullptr));
    }
    else
    {
        m_lockInput = true;
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.5f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer3::showUnlockDialog)),
            nullptr));
    }
}

SecretLayer3::~SecretLayer3()
{
}