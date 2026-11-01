#include "Layers/UILayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Nodes/GJUINode.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Utils/PlatformToolbox.h"
#include "Layers/PlayLayer.h"
#include "Objects/PlayerObject.h"
#include "UIButtonConfig.h"

bool UILayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint location = cocos2d::CCNode::convertToNodeSpace(this, &cocos2d::CCTouch::getLocation(touch));
    bool leftSide = location.x <= winSize.width * 0.5f;

    if (!m_inPlatformer) {
        if (m_gameLayer->m_gameState.m_isDualMode || GameManager::sharedState()->getGameVariable("0011")) {
            if (!leftSide) {
                if (m_p2TouchId == -1) {
                    m_p2TouchId = touch->m_nId;
                    m_gameLayer->queueButton(1, true, true, touch->m_timestamp);
                }
                return true;
            }
        } else if (GameManager::sharedState()->getGameVariable("0010")) {
            if (m_p2TouchId == -1) {
                m_p2TouchId = touch->m_nId;
                m_gameLayer->queueButton(1, true, true, touch->m_timestamp);
            }
            return true;
        }

        if (m_p1TouchId != -1)
            return true;
        m_p1TouchId = touch->m_nId;
        m_gameLayer->queueButton(1, true, false, touch->m_timestamp);
        return true;
    }

    if (!m_dualMode) {
        if (m_initialized) {
            if (processUINodesTouch(UITouchType::Pressed, touch))
                return true;
            if (m_p1TouchId != -1 || m_dpadType)
                return true;
        }
        m_p1TouchId = touch->m_nId;
        m_gameLayer->queueButton(1, true, false, touch->m_timestamp);
        return true;
    }

    if (m_initialized)
        processUINodesTouch(UITouchType::Pressed, touch);
    return true;
}

void UILayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void UILayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    int touchId = touch->m_nId;
    if (m_inPlatformer) {
        if (m_dualMode || touchId != m_p1TouchId) {
            processUINodesTouch(UITouchType::Ended, touch);
            return;
        }
        m_p1TouchId = -1;
        m_gameLayer->queueButton(1, false, false, touch->m_timestamp);
        return;
    }
    if (touchId == m_p1TouchId) {
        m_p1TouchId = -1;
        m_gameLayer->queueButton(1, false, false, touch->m_timestamp);
    } else if (touchId == m_p2TouchId) {
        m_p2TouchId = -1;
        m_gameLayer->queueButton(1, false, true, touch->m_timestamp);
    }
}

void UILayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_inPlatformer)
        processUINodesTouch(UITouchType::Moved, touch);
}

UILayer* UILayer::create(GJBaseGameLayer* layer) {
    UILayer* returnVal = new UILayer();
    if (returnVal->init(layer)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool UILayer::init(GJBaseGameLayer* layer) {
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    m_gameLayer = layer;
    m_initialized = true;

    m_uiNodes = cocos2d::CCArray::create();
    m_uiNodes->retain();

    GameManager::sharedState()->prepareDPadSettings();

    for (int i = 0; i < 4; i++) {
        UIButtonConfig* config = (i <= 1) ? &GameManager::sharedState()->m_dpad1 : &GameManager::sharedState()->m_dpad4;
        GJUINode* node = GJUINode::create(config);
        addChild(node, 100);
        node->setVisible(false);
        m_uiNodes->addObject(node);
        if ((i & 0xFFFFFFFD) == 1)
            node->m_player2 = true;
    }

    setTouchEnabled(true);
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSprite* pauseSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseBtn_clean_001.png");
    pauseSprite->setOpacity(75);
    m_pauseBtn = CCMenuItemSpriteExtra::create(pauseSprite, nullptr, menu_selector(UILayer::onPause));
    m_pauseBtn->m_volume = 0.0f;
    m_pauseBtn->setSizeMult(4.0f);

    cocos2d::CCMenu* pauseMenu = cocos2d::CCMenu::create(m_pauseBtn, nullptr);
    addChild(pauseMenu, 16);

    float pauseX = GameManager::sharedState()->getGameVariable("0015")
        ? cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 15.0f
        : cocos2d::CCDirector::sharedDirector()->getScreenRight() - 15.0f;
    pauseMenu->setPosition(pauseX, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 15.0f);

    m_checkpointMenu = cocos2d::CCMenu::create();
    addChild(m_checkpointMenu, 16);

    m_controllerButtons = cocos2d::CCArray::create();
    m_controllerButtons->retain();

    cocos2d::CCSprite* checkSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkpointBtn_001.png");
    m_controllerButtons->addObject(checkSprite);
    CCMenuItemSpriteExtra* checkBtn = CCMenuItemSpriteExtra::create(checkSprite, nullptr, menu_selector(UILayer::onCheck));
    m_checkpointMenu->addChild(checkBtn);
    checkBtn->m_scaleMultiplier = 1.1f;

    cocos2d::CCSprite* deleteSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_removeCheckBtn_001.png");
    m_controllerButtons->addObject(deleteSprite);
    CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteSprite, nullptr, menu_selector(UILayer::onDeleteCheck));
    m_checkpointMenu->addChild(deleteBtn);
    deleteBtn->m_scaleMultiplier = 1.1f;

    cocos2d::CCArray* buttons = cocos2d::CCArray::create(checkBtn, deleteBtn, nullptr);
    cocos2d::CCPoint alignPos;
    GameToolbox::alignItemsHorisontally(buttons, 10.0f, alignPos, false);

    checkBtn->setSizeMult(1.5f);
    deleteBtn->setSizeMult(1.5f);
    m_checkpointMenu->setEnabled(true);
    toggleCheckpointsMenu(false);

    if (PlatformToolbox::isControllerConnected()) {
        cocos2d::CCSprite* bBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png");
        m_controllerButtons->addObject(bBtn);
        deleteSprite->addChild(bBtn);
        bBtn->setPosition(deleteSprite->getContentSize().width * 0.5f, deleteSprite->getContentSize().height * 0.5f - 25.0f);
        bBtn->setScale(0.8f);

        cocos2d::CCSprite* xBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_X_001.png");
        m_controllerButtons->addObject(xBtn);
        checkSprite->addChild(xBtn);
        xBtn->setPosition(checkSprite->getContentSize().width * 0.5f, checkSprite->getContentSize().height * 0.5f - 25.0f);
        xBtn->setScale(0.8f);
    }

    updateCheckState();
    togglePlatformerMode(false);
    return true;
}

bool UILayer::isJumpButtonPressed() {
    if (isJumpButtonPressed(1))
        return true;
    return isJumpButtonPressed(0);
}

void UILayer::keyBackClicked() {
    if (m_pauseBtn->isEnabled())
        onPause(nullptr);
}

void UILayer::keyDown(cocos2d::enumKeyCodes key, double timestamp) {
    handleKeypress(key, true, timestamp);
}

void UILayer::keyUp(cocos2d::enumKeyCodes key, double timestamp) {
    handleKeypress(key, false, timestamp);
}

void UILayer::draw() {
}

void UILayer::disableMenu() {
    m_pauseBtn->setEnabled(false);
}

void UILayer::doPause() {
    m_p1TouchId = -1;
    m_p2TouchId = -1;
    resetUINodeState();
    if (GameManager::sharedState()->m_playLayer) {
        GameManager::sharedState()->m_playLayer->pauseGame(false);
        ++GameManager::sharedState()->m_playLayer->m_gameState.m_pauseCounter;
    }
}

void UILayer::editorPlaytest(bool visible) {
    resetAllButtons();
    toggleMenuVisibility(visible);
    setVisible(visible);
}

void UILayer::enableEditorMode() {
    m_editorMode = true;
    m_pauseBtn->setVisible(false);
    disableMenu();
    editorPlaytest(false);
}

void UILayer::enableMenu() {
    m_pauseBtn->setEnabled(true);
}

void UILayer::handleKeypress(cocos2d::enumKeyCodes key, bool down, double timestamp) {
    switch (key) {
        case CONTROLLER_B:
        case KEY_X:
            if (!down || m_editorMode)
                return;
            if (m_gameLayer->m_isPracticeMode)
                onDeleteCheck(nullptr);
            return;
        case KEY_Z:
        case CONTROLLER_X:
            if (!down) {
                m_checkpointBtnDown = false;
                return;
            }
            if (m_editorMode)
                return;
            if (!m_checkpointBtnDown) {
                m_checkpointBtnDown = true;
                if (m_gameLayer->m_isPracticeMode)
                    onCheck(nullptr);
            }
            return;
        case CONTROLLER_RB:
        case KEY_Space:
        case CONTROLLER_A:
        case KEY_W:
            if (down) {
                if (m_p1Jumping)
                    return;
                m_p1Jumping = true;
                m_gameLayer->queueButton(1, true, false, timestamp);
            } else {
                m_p1Jumping = false;
                m_gameLayer->queueButton(1, false, false, timestamp);
            }
            return;
        case CONTROLLER_LB:
            if (down) {
                if (m_p2Jumping)
                    return;
                m_p2Jumping = true;
                m_gameLayer->queueButton(1, true, true, timestamp);
            } else {
                m_p2Jumping = false;
                m_gameLayer->queueButton(1, false, true, timestamp);
            }
            return;
        case KEY_Left:
        case KEY_A:
        case CONTROLLER_Left:
        case CONTROLLER_LTHUMBSTICK_LEFT:
            m_gameLayer->queueButton(2, down, false, timestamp);
            return;
        case KEY_Right:
        case KEY_D:
        case CONTROLLER_Right:
        case CONTROLLER_LTHUMBSTICK_RIGHT:
            m_gameLayer->queueButton(3, down, false, timestamp);
            return;
        case CONTROLLER_RTHUMBSTICK_LEFT:
        case 1028:
            m_gameLayer->queueButton(3, down, true, timestamp);
            return;
        case CONTROLLER_RTHUMBSTICK_RIGHT:
        case (KEY_Clear | 0x400):
            m_gameLayer->queueButton(2, down, true, timestamp);
            return;
        case KEY_Escape:
        case CONTROLLER_Back:
            if (down && !m_editorMode)
                onPause(nullptr);
            return;
        case KEY_R:
            if (down && !m_editorMode && GameManager::sharedState()->getGameVariable("0163")) {
                if (GameManager::sharedState()->m_playLayer && GameManager::sharedState()->m_playLayer->canPauseGame() && m_pauseBtn->isEnabled()) {
                    m_p1TouchId = -1;
                    m_p2TouchId = -1;
                    resetUINodeState();
                    if (cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->m_bControlPressed)
                        GameManager::sharedState()->m_playLayer->fullReset();
                    else
                        GameManager::sharedState()->m_playLayer->resetLevel();
                }
            }
            return;
        case KEY_P:
            if (down && !m_editorMode && GameManager::sharedState()->getGameVariable("0163")) {
                if (GameManager::sharedState()->m_playLayer && GameManager::sharedState()->m_playLayer->m_isPracticeMode) {
                    GameManager::sharedState()->toggleGameVariable("0166");
                    GameManager::sharedState()->m_playLayer->updateDebugDrawSettings();
                }
            }
            return;
        default:
            return;
    }
}

void UILayer::onCheck(cocos2d::CCObject* sender) {
    if (GameManager::sharedState()->m_playLayer && GameManager::sharedState()->m_playLayer->m_isPracticeMode) {
        GameManager::sharedState()->m_playLayer->queueCheckpoint();
        GameManager::sharedState()->m_playLayer->m_player1->removePendingCheckpoint();
    }
}

void UILayer::onDeleteCheck(cocos2d::CCObject* sender) {
    if (GameManager::sharedState()->m_playLayer && GameManager::sharedState()->m_playLayer->m_isPracticeMode)
        GameManager::sharedState()->m_playLayer->removeCheckpoint(false);
}

void UILayer::onPause(cocos2d::CCObject* sender) {
    if (!GameManager::sharedState()->m_playLayer ||
        GameManager::sharedState()->m_playLayer->m_gameState.m_pauseCounter <= 2)
        doPause();
}

void UILayer::refreshDpad() {
    if (!m_inPlatformer)
        return;
    m_dpadType = GameManager::sharedState()->getGameVariable("0113");
    GameManager::sharedState()->prepareDPadSettings();
    int count = m_dualMode ? 4 : 1;
    for (int i = 0; i < count; i++) {
        GJUINode* node = (GJUINode*)m_uiNodes->objectAtIndex(i);
        UIButtonConfig* config = getDpadSetting(i, m_dualMode);
        node->loadFromConfig(config);
    }
}

void UILayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void UILayer::resetAllButtons() {
    m_p1Jumping = false;
    m_p1TouchId = -1;
    m_p2TouchId = -1;
    m_p2Jumping = false;
    m_checkpointBtnDown = false;
    resetUINodeState();
}

void UILayer::resetUINodeState() {
    if (m_uiNodes) {
        for (unsigned int i = 0; i < m_uiNodes->count(); i++) {
            GJUINode* node = (GJUINode*)m_uiNodes->objectAtIndex(i);
            node->resetState();
        }
    }
}

void UILayer::toggleCheckpointsMenu(bool visible) {
    m_checkpointMenu->setVisible(visible);
}

void UILayer::toggleMenuVisibility(bool visible) {
    resetUINodeState();
    cocos2d::CCNode* pauseImage = m_pauseBtn->getNormalImage();
    ((cocos2d::CCSprite*)pauseImage)->setOpacity(visible ? 75 : 0);
    updateUINodeVisibility(visible && m_inPlatformer);
}

void UILayer::togglePlatformerMode(bool platformer) {
    m_inPlatformer = platformer;
    refreshDpad();
    updateUINodeVisibility(m_inPlatformer);
}

void UILayer::updateCheckState() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint practicePos = GameManager::sharedState()->m_practicePos;
    if (practicePos.x == 0.0f && practicePos.y == 0.0f)
        practicePos = cocos2d::CCPoint(winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 40.0f);
    m_checkpointMenu->setPosition(practicePos);

    unsigned char opacity = (unsigned char)(GameManager::sharedState()->m_practiceOpacity * 255.0f);
    for (unsigned int i = 0; i < m_controllerButtons->count(); i++) {
        cocos2d::CCObject* obj = m_controllerButtons->objectAtIndex(i);
        if (auto sprite = dynamic_cast<cocos2d::CCSprite*>(obj))
            sprite->setOpacity(opacity);
        else if (auto label = dynamic_cast<cocos2d::CCLabelBMFont*>(obj))
            label->setOpacity(opacity);
    }
}

void UILayer::updateDualMode(bool dual) {
    if (m_dualMode != dual) {
        m_dualMode = dual;
        togglePlatformerMode(m_inPlatformer);
    }
}

void UILayer::updateUINodeVisibility(bool visible) {
    for (unsigned int i = 0; i < m_uiNodes->count(); i++) {
        cocos2d::CCObject* node = m_uiNodes->objectAtIndex(i);
        bool show = visible && (i == 0 || m_dualMode);
        ((GJUINode*)node)->setVisible(show);
    }
}

UILayer::~UILayer() {
}