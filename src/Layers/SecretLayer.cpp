#include "Layers/SecretLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Managers/AchievementManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/MenuLayer.h"
#include "Utils/PlatformToolbox.h"

SecretLayer* SecretLayer::create()
{
    SecretLayer* returnVal = new SecretLayer();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

cocos2d::CCScene* SecretLayer::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer* layer = SecretLayer::create();
    scene->addChild(layer);
    return scene;
}

bool SecretLayer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    GameManager::sharedState()->fadeInMusic("secretLoop.mp3");
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_messageThreads = cocos2d::CCDictionary::create();
    m_messageThreads->retain();

    struct ThreadEntry { const char* achievement; int tag; };
    static const ThreadEntry kThreads[] = {
        { "geometry.ach.secret06", 6 },
        { "geometry.ach.secret07", 7 },
        { "geometry.ach.secret08", 8 },
        { "geometry.ach.secret09", 9 },
        { "geometry.ach.secret10", 10 },
        { "geometry.ach.secret13", 13 },
        { "geometry.ach.secret14", 14 },
        { "geometry.ach.secret15", 15 },
        { "geometry.ach.secret16", 16 },
        { "geometry.ach.secret17", 17 },
        { "geometry.ach.secret19", 19 },
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

    if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.returnVal.secret03"))
    {
        if (GameManager::sharedState()->getUGV("3"))
        {
            cocos2d::CCNode* node = cocos2d::CCNode::create();
            node->setTag(11);
            m_messageThreads->setObject(node, "11");
        }
    }

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -3);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(0, 75, 100));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height - 24.0f));

    cocos2d::CCPoint inputPos(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f);

    m_messageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(m_messageLabel);
    m_messageLabel->setPosition(inputPos + cocos2d::CCPoint(0.0f, 43.0f));

    std::string initialMessage = getMessage();
    updateMessageLabel(initialMessage);

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
    m_searchInput->m_maxLabelLength = 20;
    m_searchInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

    updateSearchLabel("");

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer::onBack));
    backBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    cocos2d::CCSprite* submitSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock_001.png");
    m_submitButton = CCMenuItemSpriteExtra::create(submitSprite, nullptr, menu_selector(SecretLayer::onSubmit));
    backMenu->addChild(m_submitButton);
    m_submitButton->setPosition(backMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f)));
    m_submitButton->setSizeMult(0.7f);

    return true;
}

void SecretLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
}

void SecretLayer::keyBackClicked()
{
    onBack(nullptr);
}

void SecretLayer::onBack(cocos2d::CCObject* sender)
{
    m_searchInput->onClickTrackNode(false);
    setKeypadEnabled(false);
    GameManager::sharedState()->fadeInMenuMusic();

    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, MenuLayer::scene(false)));
}

std::string SecretLayer::getBasicMessage()
{
    m_basicMessageCount++;

    int index;

    if (m_basicMessageIndex < 0)
        index = (int)llroundf((float)rand() * 4.6566e-10f * 10.0f);
    else
        index = m_basicMessageIndex + 1;

    m_basicMessageIndex = index;

    static const char* kMessages[] = {
        "How did you get in here?!",
        "What are you poking around for?",
        "Don't you have better things to do?",
        "There is no spoon",
        "Go away!",
        "I should have hid this room better...",
        "You're not supposed to be in here...",
        "RubRub won't like this...",
        "zzzZZZZ...",
        "Don't touch that!",
        "Why U touch my stuff?",
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

    if (index < 0 || index >= 30)
    {
        m_basicMessageIndex = 0;
        return kMessages[0];
    }

    return kMessages[index];
}

std::string SecretLayer::getMessage()
{
    if (m_selectedThread)
        return getThreadMessage();

    if (m_doomedIndex <= 0)
    {
        if ((float)(1.0f - (float)m_basicMessageCount * 0.08f) < (float)rand() * 4.6566e-10f)
            selectAThread();
    }

    if (m_selectedThread)
        return getThreadMessage();

    return getBasicMessage();
}

std::string SecretLayer::getThreadMessage()
{
    m_basicMessageCount = 0;

    std::string result = "zzzzzzZZZZZZZZZ";

    switch (m_selectedThread)
    {
        case 6:
            switch (m_threadMessageIndex)
            {
                case 0: result = "You're here for my gold, aren't you?"; break;
                case 1: result = "I had a secret coin once"; break;
                case 2: result = "It was so beautiful"; break;
                case 3: result = "I called it 'Sparky'"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 7:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Don't tell RubRub, but I stole an icon"; break;
                case 1: result = "He will never notice..."; break;
                case 2: result = "I hid it with my name as the password"; break;
                case 3: result = "Muahahaha!"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 8:
            switch (m_threadMessageIndex)
            {
                case 0: result = "You want a hint... don't you?"; break;
                case 1: result = "I've hidden a password in the game"; break;
                case 2: result = "I won't tell you where it is..."; break;
                case 3: result = "It's definitely not on the support page."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 9:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I like the new Robot mode"; break;
                case 1: result = "I even have my own Robot"; break;
                case 2: result = "RubRub gave it to me as a present"; break;
                case 3: result = "No, I won't give you a hint..."; break;
                case 4: result = "I said no..."; break;
                case 5: result = "You will have to ask RubRub..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 10:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Ok, I will give you a hint."; break;
                case 1: result = "Without it, I'm dead."; break;
                case 2: result = "If I'm not, then I'm behind."; break;
                case 3: result = "What am I?"; break;
                case 4: result = "That didn't go very well..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 11:
            switch (m_threadMessageIndex)
            {
                case 0: result = "So you met the GateKeeper..."; break;
                case 1: result = "I heard he made fun of me"; break;
                case 2: result = "Made fun of 'Sparky'..."; break;
                case 3: break;
                case 4: result = "Like he has a better name"; break;
                case 5: result = "What does 'Glubfub' mean anyway?"; break;
                case 6: result = "I mean... oh no..."; break;
                case 7:
                    result = "I DIDNT TELL YOU ANYTHING!";
                    m_selectedThread = 0;
                    GameManager::sharedState()->setUGV("3", 1);
                    break;
                default: break;
            }
            break;
        case 13:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Why are you still here?"; break;
                case 1: result = "You're so stubborn"; break;
                case 2: result = "Stubborn as a Fox..."; break;
                case 3: result = "wait, that's not right"; break;
                case 4: result = "Gah, I forgot the word"; break;
                case 5: result = "Those animals that are stubborn"; break;
                case 6: result = "Nevermind..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 14:
            switch (m_threadMessageIndex)
            {
                case 0:
                case 1: result = "It does get lonely sometimes"; break;
                case 2: result = "It's so quiet..."; break;
                case 3: result = "Except for the annoying music"; break;
                case 4: result = "Just keeps looping"; break;
                case 5: result = "Over and over and over"; break;
                case 6: result = "It never ends"; break;
                case 7: result = "It doesn't have an end"; break;
                case 8: result = "...no ending"; break;
                case 9: result = "That's like 5 hints"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 15:
            switch (m_threadMessageIndex)
            {
                case 0: result = "There's a trail hidden somewhere"; break;
                case 1: result = "But RubRub didn't tell me where"; break;
                case 2: result = "...or what the password is"; break;
                case 3: result = "You will have to ask him"; break;
                case 4: result = "Fly, you fool!"; break;
                case 5: result = "...or use the force"; break;
                case 6: result = "Whichever comes first"; break;
                case 7: result = "Sneaky hobbitses..."; break;
                case 8: result = "Gryffindor!"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 16:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Secrets secrets secrets..."; break;
                case 1: result = "It's hard to keep track of"; break;
                case 2: result = "makes me feel Lost..."; break;
                case 3: result = "Maybe we are in a twilight zone?"; break;
                case 4: result = "Reminds me of a book I read"; break;
                case 5: result = "The pages were weird"; break;
                case 6: result = "Page one was page two"; break;
                case 7: result = "Page three was page six"; break;
                case 8: result = "Made it really hard to read"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 17:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I don't feel good about this..."; break;
                case 1: result = "RubRub will be back soon"; break;
                case 2: result = "...unless he's already here"; break;
                case 3: result = "Watching, waiting, lurking..."; break;
                case 4: result = "He knows every move I make"; break;
                case 5: result = "Every move YOU make"; break;
                case 6: result = "But maybe you can stop him?"; break;
                case 7: result = "Maybe you are the key?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 19:
            switch (m_threadMessageIndex)
            {
                case 0: result = "What is the hardest Demon?"; break;
                case 1: result = "..."; break;
                case 2: result = "Even harder than Extreme Demons"; break;
                case 3: result = "There must be more"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 101:
            switch (m_threadMessageIndex)
            {
                case 0: result = "So... whats up?"; break;
                case 1: result = "The back button is in the corner."; break;
                case 2: result = "Ready to leave yet?"; break;
                case 3: result = "Whenever you're ready..."; break;
                case 4: result = "Fine, I can wait."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 102:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I have an idea for a gamemode"; break;
                case 1: result = "You play as a cube"; break;
                case 2: result = "that can jump"; break;
                case 3: result = "...and there are spikes"; break;
                case 4: result = "nevermind..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 103:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Look behind you!"; break;
                case 1: result = "I thought I saw something"; break;
                case 2: result = "Maybe it's just my imagination"; break;
                case 3: result = "Or was it?"; break;
                case 4: result = "DUN DUN DUN!"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 104:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Don't you get tired of clicking?"; break;
                case 1: result = "I mean, isn't it boring?"; break;
                case 2: result = "Click click click"; break;
                case 3: result = "Is that all you do?"; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 105:
            switch (m_threadMessageIndex)
            {
                case 0: result = "I heard a joke the other day"; break;
                case 1: result = "Two cubes walk into a bar"; break;
                case 2: result = "wait... did they walk or slide?"; break;
                case 3: result = "I don't think cubes can walk"; break;
                case 4: result = "Nevermind..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 120:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Are you always like this?"; break;
                case 1: result = "I mean, what's the point?"; break;
                case 2: result = "Let me guess, you're gonna push the button?"; break;
                case 3: result = "What a surprise..."; m_selectedThread = 0; break;
                default: break;
            }
            break;
        case 121:
            switch (m_threadMessageIndex)
            {
                case 0: result = "Roses are red"; break;
                case 1: result = "Violets are blue"; break;
                case 2: result = "Click the back button"; break;
                case 3: result = "So I get rid of you"; m_selectedThread = 0; break;
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

void SecretLayer::selectAThread()
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

void SecretLayer::onSubmit(cocos2d::CCObject* sender)
{
    std::string input = m_searchInput->getString();

    for (size_t i = 0; i < input.length(); i++)
        input[i] = tolower(input[i]);

    std::string playerName = GameManager::sharedState()->m_playerName;

    for (size_t i = 0; i < playerName.length(); i++)
        playerName[i] = tolower(playerName[i]);

    size_t pos = 0;
    while ((pos = input.find(" ", pos)) != std::string::npos)
    {
        input.erase(pos, 1);
    }

    m_searchInput->setString("");

    m_doomedIndex = 0;

    struct PasswordEntry { const char* password; const char* achievement; const char* message; };

    if (input == "lenny")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret05"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret05", 100, false);
            updateMessageLabel("Noooooo!!!!");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "sparky")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret06"))
        {
            FMODAudioEngine::sharedEngine()->playEffect("highscoreGet02.ogg");
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret06", 100, false);
            GameStatsManager::sharedState()->storeSecretCoin("secret06");
            GameStatsManager::sharedState()->incrementStat("8");
            updateMessageLabel("My coin.. NOOOOOO!!!");
            playCoinEffect();
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "spooky")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret07"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret07", 100, false);
            updateMessageLabel("WHAT!? How did you know my name!?!");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "blockbite")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret08"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret08", 100, false);
            updateMessageLabel("How do you know my secrets!?");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "robotop")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret09"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret09", 100, false);
            updateMessageLabel("So he told you?...");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "ahead" || input == "a head")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret10"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret10", 100, false);
            updateMessageLabel("You learn quickly!");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "mule")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret13"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret13", 100, false);
            updateMessageLabel("Ah, right you are.");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "neverending")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret14"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret14", 100, false);
            updateMessageLabel("You're pretty good at this");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "gandalfpotter")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret15"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret15", 100, false);
            updateMessageLabel("That was weird...");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == playerName && !playerName.empty())
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret17"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret17", 100, false);
            updateMessageLabel("He could be watching right now...");
            m_selectedThread = 0;
            return;
        }
    }
    else if (input == "finalboss")
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret16"))
        {
            static const char* kDoomed[] = { "8", "16", "30", "32", "46", "84" };
            static const char* kDoomedMessages[] = {
                "How did you get in here?!",
                "...",
                "What do you think you're doing?",
                "Those numbers, aren't they...",
                "NO NO NO NO!!!",
                "GAH!! You have doomed us all!",
            };

            if (m_doomedIndex < 6 && input == kDoomed[m_doomedIndex])
            {
                updateMessageLabel(kDoomedMessages[m_doomedIndex]);
                m_messageLabel->setColor(cocos2d::ccc3(255, 100, 100));
                m_doomedIndex++;

                if (m_doomedIndex > 5)
                {
                    m_doomedIndex = 0;
                    GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret16", 100, false);
                }

                m_selectedThread = 0;
                return;
            }
        }
    }
    else
    {
        if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret19"))
        {
            GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret19", 100, false);
            updateMessageLabel("Maybe Rub should add some of these");
            m_selectedThread = 0;
            return;
        }
    }

    std::string newMessage = getMessage();
    updateMessageLabel(newMessage);

    if (m_selectedThread > 0)
    {
        if (m_selectedThread <= 100)
            m_messageLabel->setColor(cocos2d::ccc3(200, 200, 200));
        else
            m_messageLabel->setColor(cocos2d::ccc3(255, 0, 200));
    }
    else
    {
        m_messageLabel->setColor(cocos2d::ccc3(255, 100, 100));
    }
}

void SecretLayer::playCoinEffect()
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

void SecretLayer::textChanged(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer::textInputClosed(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer::textInputOpened(CCTextInputNode* node)
{
    std::string text = node->getString();

    if (text.empty())
        m_searchInput->updateLabel(" ");
}

void SecretLayer::updateMessageLabel(std::string text)
{
    m_messageLabel->setString(text.c_str());
    m_messageLabel->limitLabelWidth(320.0f, 0.6f, 0.0f);
}

void SecretLayer::updateSearchLabel(const char* text)
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

SecretLayer::~SecretLayer()
{
}