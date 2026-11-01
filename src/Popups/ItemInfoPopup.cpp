#include "Popups/ItemInfoPopup.h"
#include "UI/GJItemIcon.h"
#include "Managers/AchievementManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/GameManager.h"
#include "Layers/GJGarageLayer.h"
#include "Pages/ProfilePage.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

ItemInfoPopup* ItemInfoPopup::create(int id, UnlockType type) {
    auto popup = new ItemInfoPopup();
    if (popup && popup->init(id, type)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool ItemInfoPopup::init(int id, UnlockType type) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    setKeypadEnabled(true);
    setTouchEnabled(true);
    
    m_itemID = id;
    m_unlockType = type;
    m_accountID = 0;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(300.0f, 230.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    cocos2d::CCPoint descPos(centerX, centerY - 22.0f);
    
    auto descBg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    descBg->setColor(cocos2d::ccc3(100, 100, 100));
    descBg->setOpacity(60);
    descBg->setContentSize(cocos2d::CCSize(260.0f, 90.0f));
    m_mainLayer->addChild(descBg, -1);
    descBg->setPosition(descPos);
    
    std::string typeName = nameForUnlockType(id, type);
    auto titleLabel = cocos2d::CCLabelBMFont::create(cocos2d::CCString::createWithFormat("%s %i", typeName.c_str(), id)->getCString(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 99.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.8f);
    
    // Achievement badge
    auto achievement = AchievementManager::sharedState()->achievementForUnlock(id, type);
    if (!achievement.empty()) {
        int limit = AchievementManager::sharedState()->limitForAchievement(achievement);
        if (limit >= 2 && limit <= 4) {
            const char* badgeFrame = "gj_subzeroLogo_001.png";
            if (limit == 2) badgeFrame = "gj_worldLogo_001.png";
            else if (limit == 4) badgeFrame = "GJ_md_001.png";
            
            auto badge = cocos2d::CCSprite::createWithSpriteFrameName(badgeFrame);
            m_mainLayer->addChild(badge);
            badge->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
            badge->setScale(0.4f);
            badge->setPosition(cocos2d::CCPoint(centerX - 140.0f, centerY + 105.0f));
        }
    }
    
    auto icon = GJItemIcon::createBrowserItem(type, id);
    icon->setScale(1.25f);
    m_mainLayer->addChild(icon, 12);
    icon->setPosition(cocos2d::CCPoint(centerX, centerY + 56.0f));
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    auto okBtn = ButtonSprite::create("OK");
    auto okItem = CCMenuItemSpriteExtra::create(okBtn, this, menu_selector(ItemInfoPopup::onClose));
    m_buttonMenu->addChild(okItem);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX, centerY - 90.0f));
    
    // Determine unlock state and build description
    auto stats = GameStatsManager::sharedState();
    int unlockState = stats->getItemUnlockState(id, type);
    bool isUnlocked = GameManager::sharedState()->isIconUnlocked(id, GameManager::unlockTypeToIconType(type));
    bool isDefault = (unlockState == 0 && isUnlockedByDefault(id, type));
    
    std::string unlockText;
    std::string unlockLabel;
    std::string extraText;
    
    switch (unlockState) {
        case 5:
            unlockLabel = "Secret Shop";
            unlockText = cocos2d::CCString::createWithFormat("There may or may not exist a <cy>shop</c> where you can <cl>buy</c> this <cg>%s</c>.", typeName.c_str())->getCString();
            break;
        case 6:
            unlockLabel = "Community Shop";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>buy</c> this <cg>%s</c> in the <cy>community shop</c>!", typeName.c_str())->getCString();
            break;
        case 7:
            unlockLabel = "Mechanic Shop";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>buy</c> this <cg>%s</c> at the <cp>Mechanic</c>!", typeName.c_str())->getCString();
            break;
        case 8:
            unlockLabel = "Diamond Shop";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>buy</c> this <cg>%s</c> in the <cy>diamond shop</c>!", typeName.c_str())->getCString();
            break;
        case 4:
            unlockLabel = "Shop";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>buy</c> this <cg>%s</c> in the <cy>shop</c>!", typeName.c_str())->getCString();
            break;
        case 2:
            unlockLabel = "Secret Chest";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>find</c> this <cg>%s</c> in a secret <cy>chest</c>!", typeName.c_str())->getCString();
            extraText = getRandomChestMessage();
            break;
        case 3:
            unlockLabel = "Special Chest";
            unlockText = cocos2d::CCString::createWithFormat("You can <cl>find</c> this <cg>%s</c> in a special <cy>chest</c>!", typeName.c_str())->getCString();
            extraText = getRandomChestMessage();
            break;
        case 1:
            unlockLabel = GJGarageLayer::titleForUnlock(id, type);
            unlockText = cocos2d::CCString::createWithFormat("%s to <cl>unlock</c> this <cg>%s</c>!", GJGarageLayer::descriptionForUnlock(id, type).c_str(), typeName.c_str())->getCString();
            break;
        default:
            if (isUnlocked || isDefault) {
                if (isDefault) {
                    unlockLabel = "Default";
                    unlockText = cocos2d::CCString::createWithFormat("This <cg>%s</c> is <cl>unlocked</c> by default.", typeName.c_str())->getCString();
                } else {
                    unlockLabel = stats->getSpecialUnlockDescription(id, type, isUnlocked);
                    if (!unlockLabel.empty()) {
                        unlockText = cocos2d::CCString::createWithFormat("%s to <cl>unlock</c> this <cg>%s</c>!", unlockLabel.c_str(), typeName.c_str())->getCString();
                        unlockLabel = "";
                    }
                }
            } else {
                unlockLabel = getRandomNotYetMessage();
                unlockText = cocos2d::CCString::createWithFormat("This <cg>%s</c> can be <cl>unlocked</c> in update <cy>2.21</c>.", typeName.c_str())->getCString();
            }
            break;
    }
    
    if (!unlockText.empty()) {
        auto textArea = TextArea::create(unlockText, "bigFont.fnt", 1.0f, 600.0f, cocos2d::CCPoint(42.0f, 0.0f), 0.5f, cocos2d::CCPoint(0.5f, 1.0f));
        m_mainLayer->addChild(textArea, 10);
        textArea->setPosition(descPos + cocos2d::CCPoint(0.0f, -2.0f));
        textArea->setScale(0.4f);
        
        if (!extraText.empty()) {
            auto extraLabel = cocos2d::CCLabelBMFont::create(extraText.c_str(), "bigFont.fnt");
            extraLabel->setPosition(descPos + cocos2d::CCPoint(0.0f, -35.0f));
            m_mainLayer->addChild(extraLabel);
            extraLabel->setScale(0.6f);
            extraLabel->setColor(cocos2d::ccc3(100, 100, 100));
            extraLabel->setOpacity(75);
            textArea->setPosition(textArea->getPosition() + cocos2d::CCPoint(0.0f, 5.0f));
        }
    }
    
    if (!unlockLabel.empty()) {
        auto label = cocos2d::CCLabelBMFont::create(unlockLabel.c_str(), "bigFont.fnt");
        label->setPosition(descPos + cocos2d::CCPoint(0.0f, 30.0f));
        m_mainLayer->addChild(label);
        label->limitLabelWidth(180.0f, 0.55f, 0.0f);
        if (unlockState == 4) {
            label->setScale(0.6f);
        }
    }
    
    // Credit info
    int accountId = stats->accountIDForIcon(id, type);
    m_accountID = accountId;
    if (accountId != 0) {
        std::string username = stats->usernameForAccountID(accountId);
        if (!username.empty()) {
            auto creditLabel = cocos2d::CCLabelBMFont::create(cocos2d::CCString::createWithFormat("By %s", username.c_str())->getCString(), "bigFont.fnt");
            creditLabel->limitLabelWidth(100.0f, 0.5f, 0.0f);
            
            auto creditItem = CCMenuItemSpriteExtra::create(creditLabel, this, menu_selector(ItemInfoPopup::onCredit));
            m_buttonMenu->addChild(creditItem);
            creditItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 90.0f, centerY - 94.0f)));
            
            if (accountId <= 0) {
                creditLabel->setColor(cocos2d::ccc3(100, 100, 255));
                creditItem->setEnabled(false);
            }
        }
    }
    
    return true;
}

std::string ItemInfoPopup::nameForUnlockType(int id, UnlockType type) {
    switch (type) {
        case UnlockType::Cube: return "Cube";
        case UnlockType::MainColor: return "Main Color";
        case UnlockType::SecondaryColor: return "Secondary Color";
        case UnlockType::Ship: return "Ship";
        case UnlockType::Ball: return "Ball";
        case UnlockType::UFO: return "UFO";
        case UnlockType::Wave: return "Wave";
        case UnlockType::Robot: return "Robot";
        case UnlockType::Spider: return "Spider";
        case UnlockType::Trail: return "Trail";
        case UnlockType::DeathEffect: return "Death Effect";
        case UnlockType::Special: {
            switch (id) {
                case 4: return "Master Emblem";
                case 6: return "The Path of Fire";
                case 7: return "The Path of Ice";
                case 8: return "The Path of Poison";
                case 9: return "The Path of Shadow";
                case 10: return "The Path of Lava";
                case 11: return "The Path of Earth";
                case 12: return "The Path of Blood";
                case 13: return "The Path of Metal";
                case 14: return "The Path of Light";
                case 15: return "The Path of Souls";
                case 16: return "Music Customizer";
                case 17: return "Music Unlocker";
                default: return "Item";
            }
        }
        case UnlockType::Swing: return "Swing";
        case UnlockType::Jetpack: return "Jetpack";
        case UnlockType::ShipFire: return "Ship Fire";
        default: return "NA";
    }
}

bool ItemInfoPopup::isUnlockedByDefault(int id, UnlockType type) {
    if (type == UnlockType::Cube) return id <= 4;
    if (type == UnlockType::Ship || type == UnlockType::Ball) return id <= 3;
    return id <= 1;
}

std::string ItemInfoPopup::getRandomChestMessage() {
    const char* messages[] = {
        "Nothing happened.",
        "Without a key, it cannot be.",
        "Why you click?",
        "You are not yet ready.",
        "A key, you must have.",
        "The lock wont budge.",
        "A demon guards it. Find him.",
        "You have no power here.",
        "Forget about this one.",
        "A key... My kingdom for a key..."
    };
    int idx = rand() % 10;
    return messages[idx];
}

std::string ItemInfoPopup::getRandomNotYetMessage() {
    const char* messages[] = {
        "Not yet", "Huh?", "Stay tuned!",
        "Coming soon!", "In the oven", "It's cooking",
        "Wait for it", "Not now", "Unavailable",
        "Almost there", "Soon..."
    };
    int idx = rand() % 11;
    return messages[idx];
}

void ItemInfoPopup::keyBackClicked() {
    onClose(nullptr);
}

void ItemInfoPopup::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ItemInfoPopup::onCredit(cocos2d::CCObject* sender) {
    if (m_accountID > 0) {
        auto profile = ProfilePage::create(m_accountID, 0);
        profile->show();
    }
}

ItemInfoPopup::~ItemInfoPopup() {
}