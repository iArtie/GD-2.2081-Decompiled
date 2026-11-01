#include "Popups/CollisionBlockPopup.h"
#include "Objects/EffectGameObject.h"
#include "Objects/GameObject.h"
#include "Layers/LevelEditorLayer.h"
#include "Managers/GameManager.h"
#include "UI/InfoAlertButton.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"
#include "Utils/GameToolbox.h"
#include "Platform/cocos2d.h"

CollisionBlockPopup* CollisionBlockPopup::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    CollisionBlockPopup* popup = new CollisionBlockPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void CollisionBlockPopup::determineStartValues() {
    if (m_gameObject) {
        m_blockID = m_gameObject->m_itemID;
        m_dynamicBlock = m_gameObject->m_isDynamicBlock;
    } else if (m_gameObjects && m_gameObjects->count() > 0) {
        m_blockID = -1;
        m_dynamicBlock = true;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            if (!obj[29].m_isDynamicBlock) {
                m_dynamicBlock = false;
            }

            int blockID = obj[29].m_itemID;
            if (m_blockID == -1) {
                m_blockID = blockID;
            } else if (m_blockID != blockID) {
                m_blockID = 0;
            }
        }
    }
}

bool CollisionBlockPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!FLAlertLayer::init(this, 150)) {
        return false;
    }

    m_disableDelegate = true;

    if (object) {
        m_gameObject = object;
        m_gameObject->retain();
    } else if (objects) {
        m_gameObjects = objects;
        m_gameObjects->retain();
    }

    determineStartValues();

    this->setOpacity(255);
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setContentSize(cocos2d::CCSize(300.0f, 200.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(title);
    title->setScale(0.7f);
    title->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 100.0f - 18.0f));

    // OK button
    cocos2d::CCNode* okBtn = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okBtn, nullptr, this, menu_selector(CollisionBlockPopup::onClose)
    );
    m_buttonMenu = cocos2d::CCMenu::create(okItem, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 100.0f + 24.0f));

    // Info button
    std::string helpTitle = "Help";
    std::string helpDesc = "Used for Collision and Instant Collision triggers.\n"
                           "Enable <cg>Dynamic Block</c> if you want it to collide with another. Dynamic Blocks check for collisions with other nearby collision blocks.";
    InfoAlertButton* infoBtn = InfoAlertButton::create(helpTitle, helpDesc, 1.0f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 150.0f + 18.0f, winSize.height * 0.5f + 100.0f - 18.0f)
    ));

    m_joystickConnected = -1;

    // Block ID input label
    cocos2d::CCPoint inputPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);

    cocos2d::CCLabelBMFont* inputLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(inputLabel);
    inputLabel->setScale(0.7f);
    inputLabel->setPosition(inputPos + cocos2d::CCPoint(0.0f, 34.0f));

    // Input background
    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png",
        cocos2d::CCRect(0, 0, 40, 40)
    );
    inputBg->setTag(100);
    inputBg->setColor(cocos2d::ccc3(0, 0, 0));
    inputBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(inputBg, -1);
    inputBg->setPosition(inputPos);

    // Text input
    m_blockIDInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_blockIDInput, 20);
    m_blockIDInput->setPosition(inputPos);
    m_blockIDInput->setMaxLabelScale(0.6f);
    m_blockIDInput->setMaxLabelWidth(200.0f);
    m_blockIDInput->m_textField->setVisible(false);
    m_blockIDInput->m_maxLabelLength = 6;
    m_blockIDInput->setAllowedChars("0123456789");
    m_blockIDInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_blockIDInput->setLabelPlaceholderScale(0.5f);
    m_blockIDInput->m_delegate = this;
    m_blockIDInput->m_numberInput = true;

    // Plus button
    cocos2d::CCSprite* plusSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    CCMenuItemSpriteExtra* plusBtn = CCMenuItemSpriteExtra::create(
        plusSprite, nullptr, this, menu_selector(CollisionBlockPopup::onNextItemID)
    );
    m_buttonMenu->addChild(plusBtn);
    plusBtn->setPosition(m_buttonMenu->convertToNodeSpace(inputPos + cocos2d::CCPoint(54.0f, 32.0f)));

    // Left arrow
    cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftSprite->setScale(1.1f);
    CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(
        leftSprite, nullptr, this, menu_selector(CollisionBlockPopup::onItemIDArrow)
    );
    leftBtn->setTag(1);
    leftBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(leftBtn);
    leftBtn->setPosition(m_buttonMenu->convertToNodeSpace(inputPos + cocos2d::CCPoint(-50.0f, 0.0f)));

    // Right arrow
    cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightSprite->setScale(1.1f);
    CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(
        rightSprite, nullptr, this, menu_selector(CollisionBlockPopup::onItemIDArrow)
    );
    rightBtn->setTag(2);
    rightBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(rightBtn);
    rightBtn->setPosition(m_buttonMenu->convertToNodeSpace(inputPos + cocos2d::CCPoint(50.0f, 0.0f)));

    updateTextInputLabel();

    // Dynamic Block toggle
    cocos2d::CCPoint togglePos = cocos2d::CCPoint(
        winSize.width * 0.5f - 150.0f + 20.0f,
        winSize.height * 0.5f - 100.0f + 25.0f
    );

    GameToolbox::createToggleButton(
        "Dynamic\nBlock",
        menu_selector(CollisionBlockPopup::onDynamicBlock),
        0,
        m_dynamicBlock,
        m_buttonMenu,
        togglePos,
        this,
        m_mainLayer,
        0.6f,
        0.35f,
        0.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt",
        0,
        0,
        0
    );

    m_disableDelegate = false;

    return true;
}

void CollisionBlockPopup::keyBackClicked() {
    onClose(nullptr);
}

void CollisionBlockPopup::onClose(cocos2d::CCObject* sender) {
    m_blockIDInput->m_delegate = nullptr;
    m_blockIDInput->onClickTrackNode(false);
    updateEditorLabel();

    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    this->setKeypadEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void CollisionBlockPopup::onDynamicBlock(cocos2d::CCObject* sender) {
    m_dynamicBlock = !m_dynamicBlock;

    if (m_gameObject) {
        m_gameObject->m_isDynamicBlock = m_dynamicBlock;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[29].m_isDynamicBlock = m_dynamicBlock;
        }
    }

    updateEditorLabel();
}

void CollisionBlockPopup::onItemIDArrow(cocos2d::CCObject* sender) {
    if (sender->getTag() == 1) {
        m_blockID--;
    } else {
        m_blockID++;
    }

    updateItemID();
    updateEditorLabel();
    updateTextInputLabel();
}

void CollisionBlockPopup::onNextItemID(cocos2d::CCObject* sender) {
    std::unordered_set<int> usedIDs;
    m_blockID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeBlockID(&usedIDs);

    updateItemID();
    updateEditorLabel();
    updateTextInputLabel();
}

void CollisionBlockPopup::show() {
    if (m_ZOrder == 0) {
        m_ZOrder = 105;
    }

    if (!m_scene) {
        m_scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    }

    m_scene->addChild(this, m_ZOrder);
}

void CollisionBlockPopup::textChanged(CCTextInputNode* node) {
    if (m_disableDelegate) {
        return;
    }

    std::string text = node->getString();
    m_blockID = atoi(text.c_str());

    updateItemID();
    updateEditorLabel();
}

void CollisionBlockPopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void CollisionBlockPopup::textInputReturn(CCTextInputNode* node) {
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, 0)));
}

void CollisionBlockPopup::textInputShouldOffset(CCTextInputNode* node, float yOffset) {
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, yOffset)));
}

void CollisionBlockPopup::updateEditorLabel() {
    if (m_gameObject) {
        if (!m_gameObjects) {
            m_gameObjects = cocos2d::CCArray::create();
        }
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(i));
            LevelEditorLayer::updateObjectLabel(obj);
        }
    }
}

void CollisionBlockPopup::updateItemID() {
    if (m_blockID < 0) m_blockID = 0;
    if (m_blockID > 9999) m_blockID = 9999;

    if (m_gameObject) {
        m_gameObject->m_itemID = m_blockID;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[29].m_itemID = m_blockID;
        }
    }
}

void CollisionBlockPopup::updateTextInputLabel() {
    m_disableDelegate = true;

    CCString* str = CCString::createWithFormat("%i", m_blockID);
    m_blockIDInput->setString(str->getCString());

    m_disableDelegate = false;
}

CollisionBlockPopup::~CollisionBlockPopup() {
    CC_SAFE_RELEASE_NULL(m_gameObject);
    CC_SAFE_RELEASE_NULL(m_gameObjects);
}