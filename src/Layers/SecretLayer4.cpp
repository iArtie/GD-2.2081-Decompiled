#include "Layers/SecretLayer4.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Managers/AchievementManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/LevelSelectLayer.h"
#include "Layers/RewardUnlockLayer.h"
#include "Sprites/GJChestSprite.h"
#include "Models/GJRewardItem.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/GameToolbox.h"

SecretLayer4* SecretLayer4::create()
{
    SecretLayer4* returnVal = new SecretLayer4();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

cocos2d::CCScene* SecretLayer4::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer4* layer = SecretLayer4::create();
    scene->addChild(layer);
    return scene;
}

bool SecretLayer4::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("SecretSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("SecretSheet.plist");

    GameManager::sharedState()->fadeInMusic("secretLoop03.mp3");
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_messageThreads = cocos2d::CCDictionary::create();
    m_messageThreads->retain();

    struct ThreadEntry { const char* achievement; int tag; };
    static const ThreadEntry kThreads[] = {
        { "geometry.ach.v3.secret01", 1 },
        { "geometry.ach.v3.secret02", 2 },
        { "geometry.ach.v3.secret03", 3 },
        { "geometry.ach.v3.secret04", 4 },
        { "geometry.ach.v3.secret05", 5 },
        { "geometry.ach.v3.secret06", 6 },
        { "geometry.ach.v3.secret07", 7 },
    };

    for (const auto& entry : kThreads)
    {
        if (!AchievementManager::sharedState()->isAchievementEarned(entry.achievement))
        {
            cocos2d::CCNode* node = cocos2d::CCNode::create();
            node->setTag(entry.tag);
            m_messageThreads->setObject(node, std::to_string(entry.tag));
        }
    }

    if (!GameManager::sharedState()->getUGV("9"))
    {
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.8f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer4::showFirstDialog)),
            nullptr));

        m_basicMessageIndex = 30;
    }

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -3);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(100, 0, 40));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height - 24.0f));

    cocos2d::CCPoint inputPos(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    m_messageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(m_messageLabel);
    m_messageLabel->setPosition(inputPos + cocos2d::CCPoint(0.0f, 43.0f));

    updateMessageLabel(getMessage());

    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
    inputBg->setOpacity(100);
    inputBg->setContentSize(cocos2d::CCSize(200.0f, 40.0f));
    addChild(inputBg, -2);
    inputBg->setPosition(inputPos);
    inputBg->setColor(cocos2d::ccc3(100, 100, 100));

    m_searchInput = CCTextInputNode::create(180.0f, 50.0f, "...", "Thonburi");
    addChild(m_searchInput, 20);
    m_searchInput->m_delegate = this;
    m_searchInput->setPosition(inputPos);
    m_searchInput->setMaxLabelScale(0.7f);
    m_searchInput->setMaxLabelWidth(180.0f);
    m_searchInput->m_textField->setVisible(false);
    m_searchInput->m_maxLabelLength = 100;
    m_searchInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");

    updateSearchLabel("");

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer4::onBack));
    backBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(menu, 1);
    menu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    cocos2d::CCNode* submitNode = cocos2d::CCNode::create();
    submitNode->setContentSize(cocos2d::CCSize(80.0f, 80.0f));

    cocos2d::CCSprite* lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock3_001.png");
    submitNode->addChild(lockSprite);
    lockSprite->setPosition(cocos2d::CCPoint(40.0f, 40.0f));

    m_submitButton = CCMenuItemSpriteExtra::create(submitNode, nullptr, menu_selector(SecretLayer4::onSubmit));
    menu->addChild(m_submitButton);
    m_submitButton->setPosition(menu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f)));
    m_submitButton->setSizeMult(1.0f);

    if (!GameStatsManager::sharedState()->isSpecialChestUnlocked("0003"))
    {
        cocos2d::CCMenu* chestMenu = cocos2d::CCMenu::create();
        addChild(chestMenu, 10);

        GJRewardItem* reward = GameStatsManager::sharedState()->getRewardForSpecialChest("0003");
        GJChestSprite* chest = GJChestSprite::create(reward->m_rewardType);
        chest->switchToState(ChestState::Closed, 0);
        chest->setScale(0.6f);

        CCMenuItemSpriteExtra* chestBtn = CCMenuItemSpriteExtra::create(chest, nullptr, menu_selector(SecretLayer4::onChest01));
        chestBtn->setContentSize(cocos2d::CCSize(60.0f, 48.0f));
        chest->setPosition(chestBtn->getContentSize().width * 0.5f, chestBtn->getContentSize().height * 0.5f);

        chestMenu->addChild(chestBtn);
        chestBtn->setPosition(chestMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width - 60.0f, 60.0f)));
    }

    return true;
}

void SecretLayer4::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
}

void SecretLayer4::dialogClosed(DialogLayer* layer)
{
}

void SecretLayer4::keyBackClicked()
{
    onBack(nullptr);
}

void SecretLayer4::onBack(cocos2d::CCObject* sender)
{
    m_searchInput->onClickTrackNode(false);
    setKeypadEnabled(false);
    GameManager::sharedState()->fadeInMenuMusic();

    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, LevelSelectLayer::scene(0)));

    m_exiting = true;
}

void SecretLayer4::onExit()
{
    cocos2d::CCLayer::onExit();

    if (m_exiting)
    {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("SecretSheet.plist");
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("SecretSheet.png");
    }
}

void SecretLayer4::onChest01(cocos2d::CCObject* sender)
{
    sender->setVisible(false);

    GJRewardItem* reward = GameStatsManager::sharedState()->unlockSpecialChest("0003");

    if (reward)
    {
        RewardUnlockLayer* layer = RewardUnlockLayer::create(reward->m_rewardType, nullptr);
        addChild(layer, 100);
        layer->showCollectReward(reward);
    }
}

std::string SecretLayer4::getBasicMessage()
{
    m_basicMessageCount++;

    int index;

    if (m_basicMessageIndex < 0)
        index = (int)llroundf((float)rand() * 4.6566e-10f * 20.0f);
    else
        index = m_basicMessageIndex + 1;

    m_basicMessageIndex = index;

    static const char* kMessages[] = {
        "Why have I been awoken?",
        "Leave this place",
        "You are not welcome here",
        "None shall pass",
        "You test my patience",
        "...",
        "How long will you keep this up?",
        "Don't you have other things to do?",
        "...",
        "Gah I can't stand this anymore!",
        "The smell, it's killing me",
        "Can you smell it?",
        "Chicken, burning chicken",
        "El pollo ardiente...",
        "Ok, playtime is over",
        "Tick tock",
        "Time is running out",
        "...",
        "Really? Still here?",
        "This is my first conversation in 1000 years",
        "Not what I was hoping for...",
    };

    if (index < 0 || index >= 21)
    {
        m_basicMessageIndex = 0;
        return kMessages[0];
    }

    return kMessages[index];
}

std::string SecretLayer4::getErrorMessage()
{
    int index;

    if (m_errorMessageIndex < 0)
        index = (int)llroundf((float)rand() * 4.6566e-10f * 30.0f);
    else
        index = m_errorMessageIndex + 1;

    m_errorMessageIndex = index;

    static const char* kMessages[] = {
        "No, just no",
        "Swing and a miss!",
        "I told you, it wont work...",
        "Door is now open. Oh wait, no it's not.",
        "Is that actually your guess?",
        nullptr,
        "Nope",
        "Give up, please",
        "Door is still locked",
        "That tickles!",
        "Try harder please",
        "Don't make me angry",
        "You won't like me when im angry...",
        "Oh no, you just unlocked... NOTHING",
        "Only 3 attempts left!",
        "Only 2 attempts left!",
        "LAST ATTEMPT!",
        "Just kidding, unlimited attempts",
        "Maybe you should do something else?",
        "This is getting ridiculus...",
        "Go collect some stars",
        "How about no?",
        nullptr,
        "Error, error. Bad answers.",
        "Failure",
        "May I suggest thinking?",
        "WRONG",
        "You're hopeless...",
        "Really, still here?",
        nullptr,
    };

    if (index < 0 || index >= 30)
    {
        m_errorMessageIndex = 0;
        return "No, just no";
    }

    if (index == 5)
        return "Go home " + GameManager::sharedState()->m_playerName + ".";
    if (index == 22)
        return GameManager::sharedState()->m_playerName + ", please give up...";
    if (index == 29)
        return "What kind of name is " + GameManager::sharedState()->m_playerName + " anyway?";

    return kMessages[index];
}

std::string SecretLayer4::getMessage()
{
    if (m_selectedThread)
        return getThreadMessage();

    if ((float)(1.0f - (float)m_basicMessageCount * 0.05f) < (float)rand() * 4.6566e-10f)
        selectAThread();

    if (m_selectedThread)
        return getThreadMessage();

    return getBasicMessage();
}

std::string SecretLayer4::getThreadMessage()
{
    m_basicMessageCount = 0;

    std::string result = "zzzzzzZZZZZZZZZ";

    switch (m_selectedThread)
    {
        case 1:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I hide behind rocks and rills"; break;
                case 1: result = "under the mountains and hills"; break;
                case 2: result = "I cannot be seen or felt"; break;
                case 3: result = "nor heard or smelt"; break;
                case 4: result = "the more I am the less you see"; break;
                case 5: result = "what am I?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 2:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Shh, quiet!"; break;
                case 1: result = "Can you feel it closing in?"; break;
                case 2: result = "Shadows creep where it begins"; break;
                case 3: result = "Speak its name and it is gone"; break;
                case 4: result = "from every moment dusk till dawn"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 3:
            switch (m_threadMessageIndex)
            {
                case 0: result = "What always runs, but never walks"; break;
                case 1: result = "Often murmurs, never talks"; break;
                case 2: result = "Has a bed, but never sleeps"; break;
                case 3: result = "Has a mouth, but never eats?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 4:
            switch (m_threadMessageIndex)
            {
                case 0: result = "The less you consume"; break;
                case 1: result = "the stronger I become"; break;
                case 2: result = "wait too long"; break;
                case 3: result = "and death will come"; break;
                case 4: result = "What am I?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 5:
            switch (m_threadMessageIndex)
            {
                case 0: result = "My thunder comes before my lightning"; break;
                case 1: result = "My lightning comes before my rain"; break;
                case 2: result = "My rain dries up all the land it touches"; break;
                case 3: result = "What am I?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 6:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Legend tells of a level so hard no one can beat it"; break;
                case 1: result = "Anything harder than it is impossible"; break;
                case 2: result = "Don't even get me started"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 7:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Good levels are hard to find sometimes"; break;
                case 1: result = "Wish I had someone who could find them for me"; break;
                case 2: result = "..."; break;
                case 3: result = "Forget it..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        default:
            result = "zzzZZZZ";
            break;
    }

    m_threadMessageIndex++;
    return result;
}

void SecretLayer4::selectAThread()
{
    cocos2d::CCArray* keys = m_messageThreads->allKeys();

    int index;

    if (m_threadTag < 0)
        index = (int)llroundf((float)(keys->count() - 1) * (float)rand() * 4.6566e-10f);
    else
        index = m_threadTag + 1;

    m_threadTag = index;

    if (index >= (int)keys->count())
        m_threadTag = 0;

    if (m_messageThreads->count())
    {
        cocos2d::CCObject* key = keys->objectAtIndex(m_threadTag);
        cocos2d::CCNode* value = static_cast<cocos2d::CCNode*>(
            m_messageThreads->objectForKey(std::to_string(key->getTag())));

        if (value)
            m_selectedThread = value->getTag();
    }

    m_threadMessageIndex = 0;
}

void SecretLayer4::showDialog(int index)
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[5][2] = {
        { "This is random message 1", "Random message 1 end" },
        { "This is random message 2", "Random message 2 end" },
        { "This is random message 3", "Random message 3 end" },
        { "This is random message 4", "Random message 4 end" },
        { "This is random message 5", "Random message 5 end" },
    };

    if (index < 1 || index > 5)
        return;

    DialogObject* d1 = DialogObject::create("The Gatekeeper", kTexts[index - 1][0], 3, 1.0f, true, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d1);

    DialogObject* d2 = DialogObject::create("The Gatekeeper", kTexts[index - 1][1], 3, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d2);

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    dialogLayer->m_delegate = this;
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateIn(DialogAnimationType::FromLeft);
}

void SecretLayer4::showFirstDialog()
{
    GameManager::sharedState()->setUGV("9", 1);

    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    static const char* kTexts[] = {
        "Wait, you are not <cl>RubRub</c>...",
        "You seem to be skilled in the art of <co>deception</c>...",
        "But it does not matter, your <cl>trickery</c> ends here.",
        "<cr>Leave now, <d010>before you cause more problems<d010>.<d010>.<d010>.</c>",
    };

    for (const char* text : kTexts)
    {
        DialogObject* dialog = DialogObject::create("The Gatekeeper", text, 3, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateIn(DialogAnimationType::FromLeft);
}

void SecretLayer4::onSubmit(cocos2d::CCObject* sender)
{
    std::string input = m_searchInput->getString();

    for (size_t i = 0; i < input.length(); i++)
        input[i] = tolower(input[i]);

    size_t pos = 0;
    while ((pos = input.find(" ", pos)) != std::string::npos)
        input.erase(pos, 1);

    m_searchInput->setString("");

    struct PasswordEntry { const char* password; const char* achievement; const char* message; };
    static const PasswordEntry kPasswords[] = {
        { "darkness",      "geometry.ach.dwordPtr.secret01", "Impressive" },
        { "silence",       "geometry.ach.dwordPtr.secret02", "Something you always ruin..." },
        { "river",         "geometry.ach.dwordPtr.secret03", "Be like water my friend" },
        { "ariver",        "geometry.ach.dwordPtr.secret03", "Be like water my friend" },
        { "hunger",        "geometry.ach.dwordPtr.secret04", "A pizza would be nice" },
        { "volcano",       "geometry.ach.dwordPtr.secret05", "Hot answer, well done" },
        { "avolcano",      "geometry.ach.dwordPtr.secret05", "Hot answer, well done" },
        { "backontrack",   "geometry.ach.dwordPtr.secret06", "That's some skill" },
        { "givemehelper",  "geometry.ach.dwordPtr.secret07", "Is that even a real thing..." },
    };

    for (const auto& entry : kPasswords)
    {
        if (input == entry.password)
        {
            if (!AchievementManager::sharedState()->isAchievementEarned(entry.achievement))
            {
                GameManager::sharedState()->reportAchievementWithID(entry.achievement, 100, false);
                updateMessageLabel(entry.message);
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 400.0f, 0.8f, 0.0f);
                m_selectedThread = 0;
                return;
            }
        }
    }

    if (!input.empty())
    {
        updateMessageLabel(getErrorMessage());
        m_messageLabel->setColor(cocos2d::ccc3(255, 150, 150));
        m_selectedThread = 0;
        return;
    }

    updateMessageLabel(getMessage());

    if (m_selectedThread > 0)
        m_messageLabel->setColor(cocos2d::ccc3(200, 200, 200));
    else
        m_messageLabel->setColor(cocos2d::ccc3(255, 100, 100));
}

void SecretLayer4::playCoinEffect()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create("coinPickupEffect.plist");
    particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
    addChild(particles, 1);
    particles->setPosition(center);

    CCCircleWave* wave1 = CCCircleWave::create(0.0f, 25.0f, 0.3f, 0);
    wave1->m_color = cocos2d::ccc3(255, 200, 0);
    wave1->setPosition(center);
    addChild(wave1, 0);

    CCCircleWave* wave2 = CCCircleWave::create(5.0f, 60.0f, 0.3f, 0);
    wave2->m_color = cocos2d::ccc3(255, 255, 0);
    wave2->setPosition(center);
    addChild(wave2, 0);

    cocos2d::CCSprite* coin = cocos2d::CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
    addChild(coin, 2);
    coin->setPosition(center);

    cocos2d::CCArray* frames = cocos2d::CCArray::create();
    for (int i = 1; i <= 4; i++)
    {
        cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("secretCoin_01_%03d.png", i);
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
        if (frame)
            frames->addObject(frame);
    }

    cocos2d::CCAnimation* animation = cocos2d::CCAnimation::createWithSpriteFrames(frames, 0.05f);
    coin->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCAnimate::create(animation)));

    float randomOffset = ((float)rand() * 4.6566e-10f * 2.0f - 1.0f) * 50.0f;
    cocos2d::CCPoint target(center.x + randomOffset, center.y - 60.0f);

    ccBezierConfig config;
    config.controlPoint_1 = cocos2d::CCPoint(center.x, center.y + 90.0f);
    config.controlPoint_2 = cocos2d::CCPoint(target.x, center.y + 180.0f);
    config.endPosition = cocos2d::CCPoint(target.x, target.y - 20.0f);

    coin->runAction(cocos2d::CCSequence::create(
        cocos2d::CCBezierTo::create(0.75f, config),
        cocos2d::CCCallFunc::create(coin, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr));

    coin->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCFadeOut::create(0.3f),
        nullptr));
}

void SecretLayer4::textChanged(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer4::textInputClosed(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer4::textInputOpened(CCTextInputNode* node)
{
    std::string text = node->getString();

    if (text.empty())
        m_searchInput->updateLabel(" ");
}

void SecretLayer4::updateMessageLabel(std::string text)
{
    m_messageLabel->setString(text.c_str());
    cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 320.0f, 0.6f, 0.0f);
}

void SecretLayer4::updateSearchLabel(const char* text)
{
    cocos2d::CCLabelBMFont* textLabel = m_searchInput->m_textLabel;

    std::string str = text ? text : "";

    if (str.empty() || str == "...")
    {
        m_searchInput->updateLabel("...");
        textLabel->setColor(cocos2d::ccc3(140, 140, 140));
    }
    else
    {
        textLabel->setColor(cocos2d::ccc3(255, 255, 255));
    }
}

SecretLayer4::~SecretLayer4()
{
}