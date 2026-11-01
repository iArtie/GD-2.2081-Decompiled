#include "CharacterColorPage.h"
#include "CharacterColorDelegate.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "ColorChannelSprite.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "SimplePlayer.h"
#include "DialogLayer.h"
#include "DialogObject.h"
#include "InfoAlertButton.h"
#include "cocos2d.h"

void CharacterColorPage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    // Empty implementation
}

int CharacterColorPage::activeColorForMode(int mode) {
    GameManager* gm = GameManager::sharedState();
    
    if (mode == 1) {
        return gm->m_playerColor2.value;
    } else if (mode == 2) {
        return gm->m_playerGlowColor.value;
    } else {
        return gm->m_playerColor.value;
    }
}

bool CharacterColorPage::checkColor(int id, UnlockType type) {
    GameManager* gm = GameManager::sharedState();
    bool unlocked = gm->isColorUnlocked(id, type);
    
    if (!unlocked && m_delegate) {
        m_delegate->showUnlockPopup(id, type);
    }
    
    return unlocked;
}

int CharacterColorPage::colorForIndex(int index) {
    static int colorMap[] = {
        // Full color map from decompiled data
    };
    
    if (index >= 0 && index < 143) {
        return colorMap[index];
    }
    return -1;
}

CharacterColorPage* CharacterColorPage::create() {
    CharacterColorPage* page = new CharacterColorPage();
    if (page && page->init()) {
        page->autorelease();
        return page;
    }
    delete page;
    return nullptr;
}

void CharacterColorPage::createColorMenu() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint menuPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 59.0f);

    m_cursors = cocos2d::CCArray::create();
    m_cursors->retain();

    for (int i = 0; i < 3; i++) {
        cocos2d::CCSprite* cursor = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
        cursor->setScale(0.7f);
        m_mainLayer->addChild(cursor, 11);
        m_cursors->addObject(cursor);
    }

    m_colorButtons = cocos2d::CCDictionary::create();
    m_colorButtons->retain();

    for (int i = 0; i < 144; i++) {
        int colorId = colorForIndex(i);
        if (colorId < 0) {
            continue;
        }

        float row = floorf(static_cast<float>(i) / 18.0f);
        int col = i % 18;

        ColorChannelSprite* colorSprite = ColorChannelSprite::create();
        m_colorButtons->setObject(colorSprite, colorId);
        colorSprite->setScale(0.65f);

        // Lock icon for locked colors
        cocos2d::CCSprite* lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        lockSprite->setScale(0.8f);
        lockSprite->setTag(100);
        colorSprite->addChild(lockSprite);
        lockSprite->setPosition(cocos2d::CCPoint(colorSprite->getContentSize().width * 0.5f, colorSprite->getContentSize().height * 0.5f));

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            colorSprite, nullptr, this, menu_selector(CharacterColorPage::onPlayerColor)
        );
        item->setTag(colorId);
        m_buttonMenu->addChild(item);

        cocos2d::CCPoint offset = offsetForIndex(i);
        cocos2d::CCPoint pos = cocos2d::CCPoint(
            (col * 24.0f - 204.0f + offset.x) * 12.0f + 6.0f,
            (96.0f - row * 24.0f + offset.y) * 12.0f
        );

        pos += menuPos;
        pos = m_buttonMenu->convertToNodeSpace(pos);
        item->setPosition(pos);

        // Set color
        GameManager* gm = GameManager::sharedState();
        cocos2d::ccColor3B color = gm->colorForIdx(colorId);
        colorSprite->setColor(color);
    }
}

bool CharacterColorPage::init() {
    if (!FLAlertLayer::init(this, 150)) {
        return false;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_width = 450.0f;
    m_height = 300.0f;

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square05.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setTag(-1);
    bg->setContentSize(cocos2d::CCSize(m_width, m_height));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(CharacterColorPage::onClose)
    );
    closeBtn->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(
        10.0f - (m_width * 0.5f - winSize.width * 0.5f),
        (m_height * 0.5f + winSize.height * 0.5f) - 10.0f
    ));
    m_mainLayer->addChild(m_buttonMenu, 10);

    // Player objects display
    m_playerObjects = cocos2d::CCArray::create();
    m_playerObjects->retain();

    cocos2d::CCPoint playerPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + m_height * 0.5f - 62.0f);

    // Background for player icons
    cocos2d::extension::CCScale9Sprite* playerBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    playerBg->setTag(45);
    playerBg->setContentSize(cocos2d::CCSize(m_width - 30.0f, 60.0f));
    m_mainLayer->addChild(playerBg, 8);
    playerBg->setPosition(playerPos);

    cocos2d::CCPoint basePos = cocos2d::CCPoint(0, 0);

    // Create player icons for all icon types
    int frame = GameManager::sharedState()->m_playerFrame.value;

    for (int i = 1; i <= 8; i++) {
        SimplePlayer* player = SimplePlayer::create(frame);
        cocos2d::CCPoint pos = cocos2d::CCPoint((i - 1) * 52.0f - 182.0f, 0.0f) + basePos + playerPos;
        m_mainLayer->addChild(player, 10);
        player->setPosition(pos);
        m_playerObjects->addObject(player);
        player->setScale(1.15f);

        IconType iconType = IconType::Cube;
        int iconFrame = frame;

        switch (i) {
            case 2:
                iconType = IconType::Ship;
                iconFrame = GameManager::sharedState()->m_playerShip.value;
                break;
            case 3:
                iconType = IconType::Ball;
                iconFrame = GameManager::sharedState()->m_playerBall.value;
                break;
            case 4:
                iconType = IconType::Ufo;
                iconFrame = GameManager::sharedState()->m_playerBird.value;
                break;
            case 5:
                iconType = IconType::Wave;
                iconFrame = GameManager::sharedState()->m_playerDart.value;
                break;
            case 6:
                iconType = IconType::Robot;
                iconFrame = GameManager::sharedState()->m_playerRobot.value;
                break;
            case 7:
                iconType = IconType::Spider;
                iconFrame = GameManager::sharedState()->m_playerSpider.value;
                break;
            case 8:
                iconType = IconType::Swing;
                iconFrame = GameManager::sharedState()->m_playerSwing.value;
                break;
            default:
                break;
        }

        if (i == 2) {
            // Ship toggle button
            SimplePlayer* shipPlayer = SimplePlayer::create(frame);
            CCMenuItemSpriteExtra* shipItem = CCMenuItemSpriteExtra::create(
                shipPlayer, nullptr, this, menu_selector(CharacterColorPage::toggleShip)
            );
            shipItem->setTag(1);
            shipItem->setContentSize(cocos2d::CCSize(30.0f, 30.0f));
            shipPlayer->setPosition(cocos2d::CCPoint(15.0f, 15.0f));
            m_buttonMenu->addChild(shipItem);
            shipItem->setPosition(m_buttonMenu->convertToNodeSpace(pos));
            m_playerObjects->addObject(shipPlayer);
            shipPlayer->setScale(1.15f);
            shipPlayer->updatePlayerFrame(GameManager::sharedState()->m_playerShip.value, IconType::Ship);
        }

        player->updatePlayerFrame(iconFrame, iconType);
    }

    // Mode buttons
    m_modeButtons = cocos2d::CCArray::create();
    m_modeButtons->retain();

    const char* modeNames[] = {"Col1", "Col2", "Glow"};
    
    for (int i = 0; i < 3; i++) {
        cocos2d::CCNode* btnNode = ButtonSprite::create(
            modeNames[i],
            "bigFont.fnt",
            "GJ_button_01.png",
            20.0f
        );
        btnNode->setScale(0.85f);

        CCMenuItemSpriteExtra* btnItem = CCMenuItemSpriteExtra::create(
            btnNode, nullptr, this, menu_selector(CharacterColorPage::onMode)
        );
        btnItem->setSizeMult(1.1f);
        btnItem->setTag(i);
        m_buttonMenu->addChild(btnItem);

        cocos2d::CCPoint pos = cocos2d::CCPoint(
            (m_width * 0.5f + winSize.width * 0.5f - 137.0f) + i * 50.0f,
            (m_height * 0.5f + winSize.height * 0.5f) - 17.0f
        );
        btnItem->setPosition(m_buttonMenu->convertToNodeSpace(pos));
        m_modeButtons->addObject(btnNode);
    }

    updateIconColors();
    createColorMenu();

    // Glow toggle
    cocos2d::CCSprite* glowOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    cocos2d::CCSprite* glowOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    glowOn->setScale(0.6f);
    glowOff->setScale(0.6f);

    m_glowToggler = CCMenuItemToggler::create(
        glowOn, glowOff, this, menu_selector(CharacterColorPage::toggleGlow)
    );
    m_glowToggler->toggle(!GameManager::sharedState()->m_playerGlow);
    m_glowToggler->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_glowToggler);

    cocos2d::CCPoint glowPos = cocos2d::CCPoint(
        26.0f - (m_width * 0.5f - winSize.width * 0.5f),
        23.0f - (m_height * 0.5f - winSize.height * 0.5f)
    );
    m_glowToggler->setPosition(m_buttonMenu->convertToNodeSpace(glowPos));

    // Glow label
    m_glowLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_glowLabel, 2);
    m_glowLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_glowLabel->setScale(0.45f);
    m_glowLabel->setPosition(glowPos + cocos2d::CCPoint(18.0f, 0.0f));

    updateColorMode(0);

    return true;
}

void CharacterColorPage::keyBackClicked() {
    onClose(nullptr);
}

cocos2d::CCPoint CharacterColorPage::offsetForIndex(int index) {
    float yOffset = 0.0f;
    
    if (index >= 54 && index <= 135) {
        yOffset = -0.8f;
    } else if (index > 135) {
        yOffset = 0.4f;
    }

    // Check if index is in the "special" list that needs x offset
    bool needsXOffset = false;
    int specialIndices[] = {4,5,6,7,13,14,15,16,22,23,24,25,31,32,33,34,40,41,42,43,49,50,51,52,58,59,60,61,67,68,69,70,76,77,78,79,85,86,87,88,94,95,96,97,103,104,105,106,112,113,114,115,136,137,138,139,140,141,142};
    
    for (int i = 0; i < 57; i++) {
        if (index == specialIndices[i]) {
            needsXOffset = true;
            break;
        }
    }

    return cocos2d::CCPoint(needsXOffset ? 1.0f : 0.0f, yOffset);
}

void CharacterColorPage::onClose(cocos2d::CCObject* sender) {
    if (m_delegate) {
        m_delegate->playerColorChanged();
    }
    this->setKeypadEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void CharacterColorPage::onMode(cocos2d::CCObject* sender) {
    int mode = sender->getTag();

    if (mode == 2) {
        GameManager* gm = GameManager::sharedState();
        if (!gm->isIconUnlocked(2, UnlockType::Special)) {
            // Show unlock dialog
            std::string message = "Complete <cy>10</c> <cg>Map Packs</c> to unlock <co>Glow</c>.";
            std::string title = "The Shopkeeper";
            
            DialogObject* dialog = DialogObject::create(title, message, 5, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            DialogLayer* layer = DialogLayer::create(dialog, 2);
            m_mainLayer->addChild(layer, 100);
            layer->updateChatPlacement(DialogPlacement::Center);
            layer->animateInRandomSide();
            return;
        }
    }

    updateColorMode(mode);
}

void CharacterColorPage::onPlayerColor(cocos2d::CCObject* sender) {
    UnlockType unlockType = (m_colorMode == 1) ? UnlockType::Col2 : UnlockType::Col1;
    int colorId = sender->getTag();

    if (!checkColor(colorId, unlockType)) {
        return;
    }

    if (colorId == activeColorForMode(m_colorMode)) {
        if (m_delegate) {
            m_delegate->showUnlockPopup(colorId, unlockType);
        }
        return;
    }

    GameManager* gm = GameManager::sharedState();
    gm->m_scoreValid = false;

    switch (m_colorMode) {
        case 1:
            gm->m_playerColor2.value = colorId;
            gm->m_playerColor2.seed = rand() % 1000;
            gm->m_playerColor2.random = colorId + gm->m_playerColor2.seed;
            break;
        case 2:
            gm->m_playerGlowColor.value = colorId;
            gm->m_playerGlowColor.seed = rand() % 1000;
            gm->m_playerGlowColor.random = colorId + gm->m_playerGlowColor.seed;
            break;
        default:
            gm->m_playerColor.value = colorId;
            gm->m_playerColor.seed = rand() % 1000;
            gm->m_playerColor.random = colorId + gm->m_playerColor.seed;
            break;
    }

    cocos2d::CCObject* cursor = m_cursors->objectAtIndex(m_colorMode);
    cocos2d::CCPoint worldPos = sender->convertToWorldSpace(sender->getPosition());
    cocos2d::CCPoint localPos = m_mainLayer->convertToNodeSpace(worldPos);
    cursor->setPosition(localPos);

    updateIconColors();
}

void CharacterColorPage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void CharacterColorPage::show() {
    if (!m_noElasticity) {
        unsigned char opacity = this->getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();

        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));

        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, pos);
        cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(moveTo, 0.6f);
        m_mainLayer->runAction(ease);

        if (m_scene && m_ZOrder > 0) {
            m_scene->addChild(this, m_ZOrder);
        } else {
            m_scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
            int highestZ = m_scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
            m_scene->addChild(this, m_ZOrder);
        }

        this->setOpacity(0);
        cocos2d::CCFadeTo* fadeIn = cocos2d::CCFadeTo::create(0.14f, opacity);
        this->runAction(fadeIn);
        this->setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void CharacterColorPage::toggleGlow(cocos2d::CCObject* sender) {
    GameManager* gm = GameManager::sharedState();
    gm->m_playerGlow = !gm->m_playerGlow;
    updateIconColors();
}

void CharacterColorPage::toggleGlowItems(bool visible) {
    m_glowToggler->setEnabled(visible);
    m_glowToggler->setVisible(visible);
    m_glowLabel->setVisible(visible);
}

void CharacterColorPage::toggleShip(cocos2d::CCObject* sender) {
    SimplePlayer* player = static_cast<SimplePlayer*>(sender);
    
    if (sender->getTag() == 1) {
        GameManager* gm = GameManager::sharedState();
        player->updatePlayerFrame(gm->m_playerJetpack.value, IconType::Jetpack);
        player->setScale(1.15f);
        sender->setTag(8);
    } else {
        GameManager* gm = GameManager::sharedState();
        player->updatePlayerFrame(gm->m_playerShip.value, IconType::Ship);
        player->setScale(1.15f);
        sender->setTag(1);
    }
}

void CharacterColorPage::updateColorMode(int mode) {
    m_colorMode = mode;

    int activeCol1 = activeColorForMode(0);
    int activeCol2 = activeColorForMode(1);
    int activeGlow = activeColorForMode(2);

    // Update color buttons
    if (m_colorButtons) {
        CCDictElement* element = m_colorButtons->m_pElements;
        while (element) {
            ColorChannelSprite* sprite = static_cast<ColorChannelSprite*>(element->m_pObject);
            int colorId = sprite->getTag();

            cocos2d::CCPoint worldPos = sprite->convertToWorldSpace(sprite->getPosition());
            cocos2d::CCPoint localPos = m_mainLayer->convertToNodeSpace(worldPos);

            if (activeCol1 == colorId) {
                m_cursors->objectAtIndex(0)->setPosition(localPos);
            }
            if (activeCol2 == colorId) {
                m_cursors->objectAtIndex(1)->setPosition(localPos);
            }
            if (activeGlow == colorId) {
                m_cursors->objectAtIndex(2)->setPosition(localPos);
            }

            UnlockType unlockType = (m_colorMode == 1) ? UnlockType::Col2 : UnlockType::Col1;
            bool isUnlocked = GameManager::sharedState()->isColorUnlocked(colorId, unlockType);
            
            // Show/hide lock icon
            cocos2d::CCSprite* lockSprite = static_cast<cocos2d::CCSprite*>(sprite->getChildByTag(100));
            if (lockSprite) {
                lockSprite->setVisible(!isUnlocked);
            }

            if (isUnlocked || GameStatsManager::sharedState()->getItemUnlockState(colorId, unlockType) > 0) {
                sprite->setColor(cocos2d::ccc3(255, 255, 255));
            } else {
                sprite->setColor(cocos2d::ccc3(0, 0, 0));
            }

            element = static_cast<CCDictElement*>(element->hh.next);
        }
    }

    // Update mode buttons
    for (int i = 0; i < 3; i++) {
        cocos2d::CCObject* cursor = m_cursors->objectAtIndex(i);
        ButtonSprite* btn = static_cast<ButtonSprite*>(m_modeButtons->objectAtIndex(i));

        if (m_colorMode == i) {
            cursor->setColor(cocos2d::ccc3(255, 255, 255));
            cursor->getParent()->reorderChild(cursor, 11);
            btn->updateBGImage("GJ_button_01.png");
        } else {
            cursor->setColor(cocos2d::ccc3(50, 50, 50));
            cursor->getParent()->reorderChild(cursor, 1);
            btn->updateBGImage("GJ_button_04.png");
        }
    }

    toggleGlowItems(m_colorMode == 2);
}

void CharacterColorPage::updateIconColors() {
    bool glowEnabled = GameManager::sharedState()->m_playerGlow;

    GameManager* gm = GameManager::sharedState();
    cocos2d::ccColor3B color1 = gm->colorForIdx(gm->m_playerColor.value);
    cocos2d::ccColor3B color2 = gm->colorForIdx(gm->m_playerColor2.value);
    cocos2d::ccColor3B glowColor = gm->colorForIdx(gm->m_playerGlowColor.value);

    for (int i = 0; i < m_playerObjects->count(); i++) {
        SimplePlayer* player = static_cast<SimplePlayer*>(m_playerObjects->objectAtIndex(i));
        if (player) {
            player->setColor(color1);
            player->setSecondColor(color2);
            player->m_hasGlowOutline = glowEnabled;

            if (gm->m_playerGlowColor.value < 0) {
                player->disableCustomGlowColor();
            } else {
                player->enableCustomGlowColor(glowColor);
            }

            player->updateColors();
        }
    }
}

CharacterColorPage::~CharacterColorPage() {
    CC_SAFE_RELEASE_NULL(m_cursors);
    CC_SAFE_RELEASE_NULL(m_modeButtons);
    CC_SAFE_RELEASE_NULL(m_playerObjects);
    CC_SAFE_RELEASE_NULL(m_colorButtons);
}