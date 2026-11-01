#include "EditorOptionsLayer.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "EditorUI.h"
#include "GJGameLevel.h"
#include "cocos2d.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "GameToolbox.h"

EditorOptionsLayer* EditorOptionsLayer::create()
{
    EditorOptionsLayer* pRet = new EditorOptionsLayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool EditorOptionsLayer::init()
{
    m_gap = 36.0f;
    m_maxLabelScale = 0.4f;
    return GJOptionsLayer::init(true);
}

void EditorOptionsLayer::setupOptions()
{
    m_togglesPerPage = 12;
    m_offset = 15.0f;
    
    // Add all editor toggle options
    GJOptionsLayer::addGVToggle(
        this,
        "Draw Trigger Boxes",
        "0044",
        "Draws the hitbox for touch triggered objects."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Playtest No UI",
        "0046",
        "Hides part of the editor's user interface during playtesting."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Playtest Music",
        "0002",
        "Plays music during playtesting."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Effect Lines",
        "0043",
        "Shows a vertical line for triggers to indicate where they are activated."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Duration Lines",
        "0058",
        "Shows a horizontal line for triggers to indicate how long they are active."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Grid On Top",
        "0039",
        "Shows the grid on top of objects in editor."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Playtest No Grid",
        "0079",
        "Hides grid during playtesting."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Hide Background",
        "0078",
        "Hides background in editor."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Enable Link Controls",
        "0097",
        "Enables option to link objects together in the editor. Linked objects are always selected and deleted together."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Hold To Swipe",
        "0057",
        "Enables swiping after holding down anywhere on the grid for a short duration. Useful when selecting objects without the need to toggle swiping manually."
    );
    
    GJOptionsLayer::offsetToNextPage(this);
    
    GJOptionsLayer::addGVToggle(
        this,
        "Swipe Cycle Mode",
        "0059",
        "Cycles through touched objects instead of adding to selection when tapping in the same position while swiping is enabled. Useful if you want to select multiple objects that are mixed with other objects."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Layer Locking",
        "0103",
        "Allows you to click on the editor layer number to lock and unlock a layer."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Show Clicks",
        "0149",
        "Shows a marker during playtesting when the player clicks."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Auto-Pause",
        "0150",
        "Pauses gameplay when starting from a start position to prepare gameplay. Click a second time to actually start playing. This is used to handle start lag in larger levels."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Hide Path",
        "0152",
        "Hides the player path from playtesting."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Start Optimization",
        "0151",
        "Tries to speed up start position loading in editor. Experimental."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Hide Particle Icons",
        "0137",
        "Hides particle icons while preview mode is enabled."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Increase Undo/Redo",
        "0013",
        "Increases maximum undo and redo limit from 200 to 1000 in editor."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Playtest Smooth Fix",
        "0102",
        "Enables smooth fix when playtesting in the editor."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Increase Scale",
        "0112",
        "Increases maximum object scale to 4x and minimum scale to 0.25x. Note that objects larger than 6x grid size may disappear near the screen edge. This may be fixed in the future."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Disable Paste State Groups",
        "0156",
        "Makes it so that pasting state no longer updates the group info of the target object(s). Group state can be pasted directly on the edit group page instead."
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Small Warp Buttons",
        "0169",
        "Warp control buttons are 50% smaller. (Restart editor)"
    );
    
    GJOptionsLayer::addGVToggle(
        this,
        "Static Trace Arrows",
        "0181",
        "Disable the arrows showing directed connections when using Trace In/Out."
    );
    
    // Get current values from GameManager
    GameManager* gm = GameManager::sharedState();
    m_buttonsPerRow = gm->getIntGameVariable("0049");
    m_buttonRows = gm->getIntGameVariable("0050");
    
    cocos2d::CCArray* objects = GJOptionsLayer::objectsForPage(this, 0);
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Buttons Per Row control
    cocos2d::CCPoint perRowPos = cocos2d::CCPoint(centerX - 100.0f, centerY - 115.0f);
    
    // Label: "Buttons per row"
    cocos2d::CCLabelBMFont* perRowLabel = cocos2d::CCLabelBMFont::create("Buttons per row", "bigFont.fnt");
    perRowLabel->setScale(0.5f);
    m_mainLayer->addChild(perRowLabel, 2);
    perRowLabel->setPosition(perRowPos + cocos2d::CCPoint(0, 30.0f));
    objects->addObject(perRowLabel);
    
    // Value label
    m_buttonsPerRowLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", m_buttonsPerRow)->getCString(),
        "bigFont.fnt"
    );
    m_mainLayer->addChild(m_buttonsPerRowLabel, 2);
    m_buttonsPerRowLabel->setPosition(perRowPos);
    objects->addObject(m_buttonsPerRowLabel);
    
    // Left arrow (decrease)
    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    leftArrow->setFlipX(true);
    leftArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, this, menu_selector(EditorOptionsLayer::onButtonsPerRow));
    leftBtn->setTag(0);
    leftBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(leftBtn);
    leftBtn->setPosition(m_buttonMenu->convertToNodeSpace(perRowPos + cocos2d::CCPoint(-40.0f, 0.0f)));
    objects->addObject(leftBtn);
    
    // Right arrow (increase)
    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    rightArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, this, menu_selector(EditorOptionsLayer::onButtonsPerRow));
    rightBtn->setTag(1);
    rightBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(rightBtn);
    rightBtn->setPosition(m_buttonMenu->convertToNodeSpace(perRowPos + cocos2d::CCPoint(40.0f, 0.0f)));
    objects->addObject(rightBtn);
    
    // Button Rows control
    cocos2d::CCPoint rowsPos = cocos2d::CCPoint(centerX + 100.0f, centerY - 115.0f);
    
    // Label: "Button rows"
    cocos2d::CCLabelBMFont* rowsLabel = cocos2d::CCLabelBMFont::create("Button rows", "bigFont.fnt");
    rowsLabel->setScale(0.5f);
    m_mainLayer->addChild(rowsLabel, 2);
    rowsLabel->setPosition(rowsPos + cocos2d::CCPoint(0, 30.0f));
    objects->addObject(rowsLabel);
    
    // Value label
    m_buttonRowsLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", m_buttonRows)->getCString(),
        "bigFont.fnt"
    );
    m_mainLayer->addChild(m_buttonRowsLabel, 2);
    m_buttonRowsLabel->setPosition(rowsPos);
    objects->addObject(m_buttonRowsLabel);
    
    // Left arrow (decrease)
    cocos2d::CCSprite* rowsLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    rowsLeft->setFlipX(true);
    rowsLeft->setScale(0.8f);
    CCMenuItemSpriteExtra* rowsLeftBtn = CCMenuItemSpriteExtra::create(
        rowsLeft, nullptr, this, menu_selector(EditorOptionsLayer::onButtonRows));
    rowsLeftBtn->setTag(0);
    rowsLeftBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(rowsLeftBtn);
    rowsLeftBtn->setPosition(m_buttonMenu->convertToNodeSpace(rowsPos + cocos2d::CCPoint(-40.0f, 0.0f)));
    objects->addObject(rowsLeftBtn);
    
    // Right arrow (increase)
    cocos2d::CCSprite* rowsRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    rowsRight->setScale(0.8f);
    CCMenuItemSpriteExtra* rowsRightBtn = CCMenuItemSpriteExtra::create(
        rowsRight, nullptr, this, menu_selector(EditorOptionsLayer::onButtonRows));
    rowsRightBtn->setTag(1);
    rowsRightBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(rowsRightBtn);
    rowsRightBtn->setPosition(m_buttonMenu->convertToNodeSpace(rowsPos + cocos2d::CCPoint(40.0f, 0.0f)));
    objects->addObject(rowsRightBtn);
}

void EditorOptionsLayer::onButtonsPerRow(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    int newValue = (tag == 1) ? m_buttonsPerRow + 1 : m_buttonsPerRow - 1;
    
    if (newValue < 6) newValue = 6;
    if (newValue > 12) newValue = 12;
    
    m_buttonsPerRow = newValue;
    m_buttonsPerRowLabel->setString(
        cocos2d::CCString::createWithFormat("%i", newValue)->getCString()
    );
}

void EditorOptionsLayer::onButtonRows(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    int newValue = (tag == 1) ? m_buttonRows + 1 : m_buttonRows - 1;
    
    if (newValue < 2) newValue = 2;
    if (newValue > 3) newValue = 3;
    
    m_buttonRows = newValue;
    m_buttonRowsLabel->setString(
        cocos2d::CCString::createWithFormat("%i", newValue)->getCString()
    );
}

void EditorOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    GameManager* gm = GameManager::sharedState();
    int oldPerRow = gm->getIntGameVariable("0049");
    int oldRows = gm->getIntGameVariable("0050");
    
    // Save values if changed
    if (oldPerRow != m_buttonsPerRow || oldRows != m_buttonRows)
    {
        gm->setIntGameVariable("0049", m_buttonsPerRow);
        gm->setIntGameVariable("0050", m_buttonRows);
        
        if (GameManager::sharedState()->m_levelEditorLayer)
        {
            GameManager::sharedState()->m_levelEditorLayer->m_editorUI->m_reloadItems = true;
        }
    }
    
    // Update preview particles
    if (GameManager::sharedState()->m_levelEditorLayer)
    {
        GameManager::sharedState()->m_levelEditorLayer->updatePreviewParticles();
    }
    
    SetupTriggerPopup::onClose(this, sender);
}

EditorOptionsLayer::~EditorOptionsLayer()
{
    // Base class handles cleanup
}