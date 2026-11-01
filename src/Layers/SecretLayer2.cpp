#include "Layers/SecretLayer2.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Managers/AchievementManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "Models/GJGameLevel.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/CreatorLayer.h"
#include "Layers/SecretLayer3.h"
#include "Layers/SecretNumberLayer.h"
#include "Layers/PlayLayer.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/GameToolbox.h"

SecretLayer2* SecretLayer2::create()
{
    SecretLayer2* returnVal = new SecretLayer2();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

cocos2d::CCScene* SecretLayer2::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer2* layer = SecretLayer2::create();
    scene->addChild(layer);
    return scene;
}

bool SecretLayer2::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("SecretSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("SecretSheet.plist");

    GameManager::sharedState()->fadeInMusic("secretLoop02.mp3");
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_messageThreads = cocos2d::CCDictionary::create();
    m_messageThreads->retain();

    struct ThreadEntry { const char* achievement; int tag; bool required; };
    static const ThreadEntry kThreads[] = {
        { "geometry.ach.returnVal.secret01", 1, false },
        { "geometry.ach.returnVal.secret03", 3, false },
        { "geometry.ach.returnVal.secret05", 5, false },
        { "geometry.ach.returnVal.secret06", 6, false },
        { "geometry.ach.returnVal.secret08", 8, false },
        { "geometry.ach.returnVal.secret09", 9, false },
        { "geometry.ach.returnVal.secret10", 10, false },
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

    if (!GameManager::sharedState()->getUGV("1"))
    {
        cocos2d::CCNode* node = cocos2d::CCNode::create();
        node->setTag(20);
        m_messageThreads->setObject(node, "20");
    }

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -3);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(75, 0, 100));

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
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer2::onBack));
    backBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(menu, 1);
    menu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    cocos2d::CCSprite* lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock2_001.png");
    cocos2d::CCSprite* lockInner = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock2_2_001.png");
    lockSprite->addChild(lockInner, -1);
    lockInner->setPosition(cocos2d::CCNode::convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f)));

    m_submitButton = CCMenuItemSpriteExtra::create(lockSprite, nullptr, menu_selector(SecretLayer2::onSubmit));
    menu->addChild(m_submitButton);
    m_submitButton->setPosition(menu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f)));
    m_submitButton->setSizeMult(0.7f);

    const char* doorSprite = GameManager::sharedState()->getUGV("3") ? "secretDoor_open_001.png" : "secretDoor_closed_001.png";
    cocos2d::CCSprite* doorNode = cocos2d::CCSprite::createWithSpriteFrameName(doorSprite);
    m_secretDoorButton = CCMenuItemSpriteExtra::create(doorNode, nullptr, menu_selector(SecretLayer2::onDoor));
    m_secretDoorButton->m_animationEnabled = false;
    m_secretDoorButton->m_colorEnabled = true;
    menu->addChild(m_secretDoorButton);
    m_secretDoorButton->setPosition(menu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width - 45.0f, 45.0f)));

    m_secretLevelLabels = cocos2d::CCArray::create();
    m_secretLevelLabels->retain();

    cocos2d::CCSprite* playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    playSprite->setScale(0.5f);
    m_secretLevelButton = CCMenuItemSpriteExtra::create(playSprite, nullptr, menu_selector(SecretLayer2::onSecretLevel));
    menu->addChild(m_secretLevelButton);

    cocos2d::CCPoint levelButtonPos(winSize.width - 45.0f, 130.0f);
    m_secretLevelButton->setPosition(menu->convertToNodeSpace(levelButtonPos));

    cocos2d::CCLabelBMFont* label1 = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    label1->setScale(0.4f);
    addChild(label1);
    label1->setPosition(levelButtonPos + cocos2d::CCPoint(0.0f, 46.0f));
    m_secretLevelLabels->addObject(label1);

    cocos2d::CCLabelBMFont* label2 = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    label2->setScale(0.4f);
    addChild(label2);
    label2->setPosition(label1->getPosition() + cocos2d::CCPoint(0.0f, -15.0f));
    m_secretLevelLabels->addObject(label2);

    if (GameStatsManager::sharedState()->getStat("13") <= 199)
    {
        cocos2d::CCPoint diamondPos = levelButtonPos + cocos2d::CCPoint(3.0f, -30.0f);

        cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
        addChild(diamondIcon);
        diamondIcon->setPosition(diamondPos + cocos2d::CCPoint(12.0f, -1.0f));
        diamondIcon->setScale(0.7f);
        m_secretLevelLabels->addObject(diamondIcon);

        cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create("200", "bigFont.fnt");
        addChild(diamondLabel);
        diamondLabel->setPosition(diamondPos + cocos2d::CCPoint(-8.5f, 0.0f));
        diamondLabel->setScale(0.4f);
        m_secretLevelLabels->addObject(diamondLabel);
    }

    if (!GameManager::sharedState()->getUGV("1"))
    {
        m_secretLevelButton->setEnabled(false);
        m_secretLevelButton->setVisible(false);

        for (unsigned int i = 0; i < m_secretLevelLabels->count(); i++)
        {
            cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(m_secretLevelLabels->objectAtIndex(i));
            node->setVisible(false);
        }
    }

    if (!GameManager::sharedState()->getUGV("3"))
    {
        GJGameLevel* mainLevel = GameLevelManager::sharedState()->getMainLevel(3001, true);
        if (mainLevel && mainLevel->getNormalPercent() > 99)
        {
            runAction(cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.8f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer2::showCompletedLevel)),
                nullptr));
        }
    }

    return true;
}

void SecretLayer2::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
}

void SecretLayer2::dialogClosed(DialogLayer* layer)
{
    GameManager::sharedState()->setUGV("3", 1);

    cocos2d::CCSprite* doorSprite = static_cast<cocos2d::CCSprite*>(m_secretDoorButton->getNormalImage());
    doorSprite->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("secretDoor_open_001.png"));
}

void SecretLayer2::keyBackClicked()
{
    onBack(nullptr);
}

void SecretLayer2::onBack(cocos2d::CCObject* sender)
{
    m_searchInput->onClickTrackNode(false);
    setKeypadEnabled(false);
    GameManager::sharedState()->fadeInMenuMusic();

    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, CreatorLayer::scene()));

    m_exiting = true;
}

void SecretLayer2::onExit()
{
    cocos2d::CCLayer::onExit();

    if (m_exiting)
    {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("SecretSheet.plist");
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("SecretSheet.png");
    }
}

void SecretLayer2::onDoor(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->getUGV("3"))
    {
        cocos2d::CCDirector::sharedDirector()->replaceScene(
            cocos2d::CCTransitionFade::create(0.5f, SecretLayer3::scene()));
        return;
    }

    std::string message = "Not allowed";

    if (m_doorDialogIndex < 0)
        m_doorDialogIndex = (int)llroundf((float)rand() * 4.6566e-10f * 10.0f);
    else
        m_doorDialogIndex++;

    static const char* kDoorMessages[] = {
        "Sorry,<d020> I can't let you in.",
        "Its too dangerous, just stay here.",
        "Leave that alone...",
        "That area is off limits.",
        "I keep that door locked.\n<d030>RubRub's orders.",
        "I've only been down there once. <d030>Its pretty creepy.",
        "Just forget about it.",
        "Some things are best left alone.",
        "Even I dont want to go down there.",
        "Off. <d030> Limits.",
        "Does the word locked mean nothing to you?",
    };

    if (m_doorDialogIndex < 0 || m_doorDialogIndex >= 11)
    {
        m_doorDialogIndex = 0;
        message = kDoorMessages[0];
    }
    else
    {
        message = kDoorMessages[m_doorDialogIndex];
    }

    DialogObject* dialog = DialogObject::create("The Keymaster", message, 2, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    DialogLayer* dialogLayer = DialogLayer::create(dialog, 2);

    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateInRandomSide();
}

void SecretLayer2::onSecretLevel(cocos2d::CCObject* sender)
{
    if (GameStatsManager::sharedState()->getStat("13") > 199)
    {
        GJGameLevel* mainLevel = GameLevelManager::sharedState()->getMainLevel(3001, false);
        GameManager::sharedState()->m_sceneEnum = 12;

        cocos2d::CCDirector::sharedDirector()->replaceScene(
            cocos2d::CCTransitionFade::create(0.5f, PlayLayer::scene(mainLevel, false, false)));
        return;
    }

    DialogObject* dialog = DialogObject::create("The Keymaster",
        "You are not prepared.\n<d030>Collect more diamonds...", 2, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    DialogLayer* dialogLayer = DialogLayer::create(dialog, 2);

    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateInRandomSide();
}

std::string SecretLayer2::getBasicMessage()
{
    m_basicMessageCount++;

    int index;

    if (m_basicMessageIndex < 0)
    {
        if ((float)rand() * 4.6566e-10f <= 0.8f)
        {
            m_basicMessageIndex = 0;
            index = 0;
        }
        else
        {
            m_basicMessageIndex = 9;
            index = 9;
        }
    }
    else
    {
        index = m_basicMessageIndex + 1;

        if (index > 29)
        {
            m_basicMessageIndex = 0;
            return "I have heard about you";
        }

        m_basicMessageIndex = index;
    }

    static const char* kMessages[] = {
        "I have heard about you",
        nullptr,
        "You think you can fool me",
        "You are deeply mistaken",
        "Go away!",
        "I should have hid this room better...",
        "You're not supposed to be in here...",
        "RubRub won't like this...",
        "zzzZZZZ...",
        "Don't touch that!",
        "Why you touch my stuff?",
        "RubRub better not find you in here...",
        "Can't you just leave?",
        "This is not the room you are looking for...",
        "Sneaky sneaky...",
        "It's my precious...",
        "You shall not pass!",
        "Don't push the button!",
        "You're gonna get me in trouble...",
        "This is getting ridiculus...",
        "Go collect some stars",
        "Maybe there are new levels?",
        "Just, stop bothering me",
        "I'm gonna stop talking",
        "...",
        "......",
        "GAH!",
        "You're hopeless...",
        "Really, still here?",
        "Fine, press the button",
    };

    if (index == 1)
        return GameManager::sharedState()->m_playerName + " the sneaky one...";

    return kMessages[index];
}

std::string SecretLayer2::getErrorMessage()
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

std::string SecretLayer2::getMessage()
{
    if (m_selectedThread)
        return getThreadMessage();

    if ((float)(1.0f - (float)m_basicMessageCount * 0.05f) < (float)rand() * 4.6566e-10f)
        selectAThread();

    if (m_selectedThread)
        return getThreadMessage();

    return getBasicMessage();
}

std::string SecretLayer2::getThreadMessage()
{
    m_basicMessageCount = 0;

    std::string result = "zzzzzzZZZZZZZZZ";

    switch (m_selectedThread)
    {
        case 1:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Oh my head..."; break;
                case 1: result = "I feel so drained"; break;
                case 2: result = "Would be nice to get some energy"; break;
                case 3: result = "You know, for my head..."; break;
                case 4: result = "Nothing? Ok..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 3:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I heard what you did"; break;
                case 1: result = "You little thief"; break;
                case 2: result = "You took 'Spookys' coin"; break;
                case 3: result = "That is pure evil..."; break;
                case 4: result = "I like it"; break;
                case 5: result = "I know what you're thinking"; break;
                case 6: result = "But you will never get my coin..."; break;
                case 7: result = "I keep it safe"; break;
                case 8:
                    result = "Too clever for you...";
                    m_selectedThread = 0;
                    GameManager::sharedState()->setUGV("11", 1);
                    break;
                default: break;
            }
            break;
        case 4:
            if (m_threadMessageIndex == 0)
                result = "This is thread 4";
            else if (m_threadMessageIndex == 1)
            {
                result = "Thread 4 end";
                m_selectedThread = 0;
            }
            break;
        case 5:
            switch (m_threadMessageIndex)
            {
                case 0: result = "So... there is a tree"; break;
                case 1: result = "A very short tree"; break;
                case 2: result = "In this tree there lives a Witch"; break;
                case 3: result = "A very short Witch"; break;
                case 4: result = "The Witch stinks of fish"; break;
                case 5: result = "My question to you is this..."; break;
                case 6: result = "On a scale from tree to fish"; break;
                case 7: result = "How many shinies do you have?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 6:
            switch (m_threadMessageIndex)
            {
                case 0: result = "..."; break;
                case 1: result = "Might as well give you a riddle"; break;
                case 2: result = "Since you insist on staying"; break;
                case 3: result = "'I have always been odd'"; break;
                case 4: result = "'Remove my start and I am even'"; break;
                case 5: result = "'When you complete this riddle'"; break;
                case 6: result = "'I hope you are leaving'"; break;
                case 7: result = "Can you go now?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 8:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Which came first?"; break;
                case 1: result = "The chicken or the egg?"; break;
                case 2: result = "..."; break;
                case 3: result = "The egg, of course."; break;
                case 4: result = "Then the egg laid the chicken"; break;
                case 5: result = "... and the chicken said"; break;
                case 6: result = "The password is on fire"; break;
                case 7: result = "I need to get some rest..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 9:
            if (m_threadMessageIndex == 0)
                result = "Remember the 2.1 Awards?";
            else if (m_threadMessageIndex == 1)
            {
                result = "me neither";
                m_selectedThread = 0;
            }
            break;
        case 10:
            switch (m_threadMessageIndex)
            {
                case 0: result = "It's been so long"; break;
                case 1: result = "I feel like I've forgotten something"; break;
                case 2: result = "..."; break;
                case 3: result = "Where's that smoke coming from?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 20:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Want to hear a secret?"; break;
                case 1: result = "Just don't tell RubRub"; break;
                case 2: result = "I am building my own level"; break;
                case 3: result = "I call it \"The Challenge\""; break;
                case 4: result = "Hope it gets featured"; break;
                case 5: result = "But probably not..."; break;
                case 6: result = "RubRub will never notice me"; m_selectedThread = 0; break;
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

void SecretLayer2::selectAThread()
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

void SecretLayer2::onSubmit(cocos2d::CCObject* sender)
{
    std::string input = m_searchInput->getString();

    for (size_t i = 0; i < input.length(); i++)
        input[i] = tolower(input[i]);

    size_t pos = 0;
    while ((pos = input.find(" ", pos)) != std::string::npos)
        input.erase(pos, 1);

    m_searchInput->setString("");

    if (input == "brainpower")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret01"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret01", 100, false);
            updateMessageLabel("O-oooooooooo AAAAE-A-A-I-A-U- JO-oooooooooooo!");
            m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
            cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 400.0f, 0.8f, 0.0f);
            m_selectedThread = 0;
            return;
        }
    }
    else
    {
        int secretNumber = GameManager::sharedState()->m_secretNumber.seed - GameManager::sharedState()->m_secretNumber.random;

        if (secretNumber > 0 && atoi(input.c_str()) == secretNumber)
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret02"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret02", 100, false);
                updateMessageLabel("Impressive...");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                m_selectedThread = 0;
                return;
            }
        }

        if (GameManager::sharedState()->getUGV("3") && input == "glubfub")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret03"))
            {
                FMODAudioEngine::sharedEngine()->playEffect("highscoreGet02.ogg");
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret03", 100, false);
                GameStatsManager::sharedState()->storeSecretCoin("secretB03");
                GameStatsManager::sharedState()->incrementStat("8");
                updateMessageLabel("NOOOO!! THIEF! THIEF!");
                playCoinEffect();
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "octocube")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret04"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret04", 100, false);
                updateMessageLabel("Ugh... Slippery");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 400.0f, 0.8f, 0.0f);
                m_selectedThread = 0;
                return;
            }
        }

        std::string statStr = std::to_string(GameStatsManager::sharedState()->getStat("6"));
        if (input == statStr)
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret05"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret05", 100, false);
                updateMessageLabel("I wish I had that many shinies...");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "seven")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret06"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret06", 100, false);
                updateMessageLabel("I should have been a doctor...");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 300.0f, 0.8f, 0.0f);
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "gimmiethecolor")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret07"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret07", 100, false);
                updateMessageLabel("How many colors do you need?");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 300.0f, 0.8f, 0.0f);
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "thechickenisonfire")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret08"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret08", 100, false);
                updateMessageLabel("Indeed it is...");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 300.0f, 0.8f, 0.0f);
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "d4shg30me7ry")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret09"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret09", 100, false);
                updateMessageLabel("Good times");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "thechickenisready")
        {
            if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.v2.secret10"))
            {
                GameManager::sharedState()->reportAchievementWithID("geometry.ach.v2.secret10", 100, false);
                updateMessageLabel("You overcooked it again!");
                m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
                m_selectedThread = 0;
                return;
            }
        }

        if (input == "thechallenge" && !GameManager::sharedState()->getUGV("1"))
        {
            updateMessageLabel("My level? You want to try it!?");
            m_messageLabel->setColor(cocos2d::ccc3(255, 0, 255));
            m_selectedThread = 0;
            showSecretLevel();
            return;
        }

        if (input == "cod3breaker")
        {
            updateMessageLabel(" ");
            m_selectedThread = 0;

            if (GameManager::sharedState()->m_secretNumber.seed - GameManager::sharedState()->m_secretNumber.random <= 0)
                GameManager::sharedState()->generateSecretNumber();

            SecretNumberLayer* layer = SecretNumberLayer::create();
            addChild(layer, 100);
            layer->playNumberEffect(GameManager::sharedState()->m_secretNumber.seed - GameManager::sharedState()->m_secretNumber.random);
            return;
        }

        if (!input.empty())
        {
            if (input == "unicorn")
                updateMessageLabel("Why do they have a horn?");
            else if (input == "rubrubpowah123")
                updateMessageLabel("Ultimate Haxxor mode enabled. Not...");
            else if (input == "battop")
                updateMessageLabel("He could be watching us right now...");
            else if (input == "kappa")
                updateMessageLabel("There is no Kappa icon...");
            else if (input == "robtop")
                updateMessageLabel("RUBRUBRUBRUBRUBRUB");
            else
                updateMessageLabel(getErrorMessage());

            m_messageLabel->setColor(cocos2d::ccc3(255, 150, 150));
            m_selectedThread = 0;
            return;
        }
    }

    updateMessageLabel(getMessage());

    if (m_selectedThread > 0)
        m_messageLabel->setColor(cocos2d::ccc3(200, 200, 200));
    else
        m_messageLabel->setColor(cocos2d::ccc3(255, 100, 100));
}

void SecretLayer2::playCoinEffect()
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

void SecretLayer2::showCompletedLevel()
{
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    struct DialogEntry { const char* text; };

    static const DialogEntry kDialogs[] = {
        { "Huh!?" },
        { "My level<d010>.<d010>.<d010>.<d040> You beat it?" },
        { "I guess I underestimated you.<d030>\nIt will not happen again." },
        { "<d010>.<d010>.<d010>.<d030> Why are you still here?" },
        { "Ah of course,<d030> you want a <cg>prize</c>." },
        { "RubRub always gives prizes." },
        { "Uhm<d010>.<d010>.<d010>.<d030> Not sure what to<d010>.<d010>.<d010>." },
        { "Oh I know!" },
        { "I left an old chest down in the <co>basement</c>." },
        { "You can have that as a <cg>reward</c>!" },
        { "Take the <cy>door</c> on the right, I will open it for you." },
        { "Grab the chest and come back<d010>.<d010>.<d010>.\n<d050><cr><i000>DONT</i> <d050><i000>TOUCH</i> <d050><i000>ANYTHING</i></c>!" },
    };

    for (const auto& entry : kDialogs)
    {
        DialogObject* dialog = DialogObject::create("The Keymaster", entry.text, 2, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(dialog);
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    dialogLayer->m_delegate = this;
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateIn(DialogAnimationType::FromLeft);
}

void SecretLayer2::showSecretLevel()
{
    GameManager::sharedState()->setUGV("1", 1);

    m_secretLevelButton->setVisible(true);
    m_secretLevelButton->setEnabled(true);

    for (unsigned int i = 0; i < m_secretLevelLabels->count(); i++)
    {
        cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(m_secretLevelLabels->objectAtIndex(i));
        node->setVisible(true);
    }

    cocos2d::CCNode* sprite = m_secretLevelButton->getNormalImage();
    sprite->setScale(1.0f);
    sprite->setOpacity(0);
    sprite->runAction(cocos2d::CCEaseBounceOut::create(cocos2d::CCScaleTo::create(0.5f, 0.5f)));
    sprite->runAction(cocos2d::CCFadeIn::create(0.15f));
}

void SecretLayer2::textChanged(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer2::textInputClosed(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer2::textInputOpened(CCTextInputNode* node)
{
    std::string text = node->getString();

    if (text.empty())
        m_searchInput->updateLabel(" ");
}

void SecretLayer2::updateMessageLabel(std::string text)
{
    m_messageLabel->setString(text.c_str());
    cocos2d::CCLabelBMFont::limitLabelWidth(m_messageLabel, 320.0f, 0.6f, 0.0f);
}

void SecretLayer2::updateSearchLabel(const char* text)
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

SecretLayer2::~SecretLayer2()
{
}