#include "SetGroupIDLayer.h"
#include "GameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "EditorUI.h"
#include "GJBaseGameLayer.h"
#include "LevelSettings.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "InfoAlertButton.h"
#include "ConfigureValuePopup.h"
#include "SetupObjectOptionsPopup.h"
#include "SetupObjectOptions2Popup.h"
#include "GameToolbox.h"

static int groupIDCompare(const void* a, const void* b) {
    int valA = atoi((*(cocos2d::CCString**)a)->getCString());
    int valB = atoi((*(cocos2d::CCString**)b)->getCString());
    return valA - valB;
}

SetGroupIDLayer* SetGroupIDLayer::create(GameObject* object, cocos2d::CCArray* objects) {
    SetGroupIDLayer* layer = new SetGroupIDLayer();
    if (layer && layer->init(object, objects)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SetGroupIDLayer::init(GameObject* obj, cocos2d::CCArray* objs) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    m_editorLayerEdited = true;
    m_noElasticity = true;

    if (obj) {
        m_targetObject = obj;
    } else {
        m_targetObjects = objs;
        m_targetObjects->retain();
    }

    m_zLayerButtons = cocos2d::CCArray::create();
    m_zLayerButtons->retain();
    m_parentGroups = cocos2d::CCDictionary::create();
    m_parentGroups->retain();

    FLAlertLayer::incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    m_groupIDObjects = cocos2d::CCArray::create();
    m_groupIDObjects->retain();

    m_groupIDValue = GameManager::sharedState()->m_currentGroupID;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(390.0f, 310.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::extension::CCScale9Sprite* subBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", cocos2d::CCRect(0, 0, 80, 80));
    subBg->setOpacity(50);
    subBg->setContentSize(cocos2d::CCSize(360.0f, 68.0f));
    m_mainLayer->addChild(subBg, -1);
    subBg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 17.0f));

    cocos2d::CCLabelBMFont* l1Label = cocos2d::CCLabelBMFont::create("Editor L1", "bigFont.fnt");
    l1Label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 125.0f, winSize.height * 0.5f + 155.0f - 17.0f));
    m_mainLayer->addChild(l1Label);
    l1Label->setScale(0.65f);

    cocos2d::CCLabelBMFont* l2Label = cocos2d::CCLabelBMFont::create("Editor L2", "bigFont.fnt");
    l2Label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 155.0f - 17.0f));
    m_mainLayer->addChild(l2Label);
    l2Label->setScale(0.65f);

    cocos2d::CCLabelBMFont* groupLabel = cocos2d::CCLabelBMFont::create("Group", "bigFont.fnt");
    groupLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, l1Label->getPosition().y - 65.0f));
    m_mainLayer->addChild(groupLabel);
    groupLabel->setScale(0.65f);

    cocos2d::CCLabelBMFont* zLabel = cocos2d::CCLabelBMFont::create("Z", "bigFont.fnt");
    zLabel->setPosition(l1Label->getPosition() + cocos2d::CCPoint(250.0f, 0.0f));
    m_mainLayer->addChild(zLabel);
    zLabel->setScale(0.65f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_joystickConnected = false;

    ButtonSprite* okSprite = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, menu_selector(SetGroupIDLayer::onClose));
    m_buttonMenu->addChild(okButton);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 155.0f + 23.0f));

    cocos2d::CCSprite* plus1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    plus1->setScale(0.8f);
    CCMenuItemSpriteExtra* plus1Button = CCMenuItemSpriteExtra::create(
        plus1, nullptr, menu_selector(SetGroupIDLayer::onNextFreeEditorLayer1));
    m_buttonMenu->addChild(plus1Button);
    plus1Button->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, l1Label->getPosition()) + cocos2d::CCPoint(50.0f, 0.0f));

    cocos2d::CCSprite* plus2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    plus2->setScale(0.8f);
    CCMenuItemSpriteExtra* plus2Button = CCMenuItemSpriteExtra::create(
        plus2, nullptr, menu_selector(SetGroupIDLayer::onNextFreeEditorLayer2));
    m_buttonMenu->addChild(plus2Button);
    plus2Button->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, l2Label->getPosition()) + cocos2d::CCPoint(54.0f, 0.0f));

    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.35f);
    CCMenuItemSpriteExtra* optionsButton = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, menu_selector(SetGroupIDLayer::onGroupIDOptions));
    m_buttonMenu->addChild(optionsButton);
    optionsButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, groupLabel->getPosition()) + cocos2d::CCPoint(73.0f, 0.0f));

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Add</c> assigns up to 10 groups to the selected object(s).\n"
        " <cy>P</c> sets a single object as group parent.\n"
        "<cl>Editor L1 and L2</c> places the object(s) on an editor layer.\n"
        "<cp>Z layer</c> sets the draw layer of the object(s). T layers are above, while B layers are below the player.\n"
        "<cr>Z order</c> sets the draw order of the object(s) within the Z layer.\n"
        "<ca>CH</c> sets the channel of the object(s).\n"
        "<co>ORD</c> sets the order of the object(s).\n"
        "<cj>Extra</c> has more advanced options.\n"
        "<cg>Preview</c> makes touch toggled triggers preview in editor.\n"
        "<cy>Playback</c> starts playback from this object.",
        0.9f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f + 195.0f - 17.0f, winSize.height * 0.5f + 155.0f - 17.0f)));

    float textInputY = groupLabel->getPosition().y + 30.0f;

    m_editorLayerInput = SetGroupIDLayer::createTextInput(
        cocos2d::CCPoint(50.0f, 1.0f),
        cocos2d::CCPoint(l1Label->getPosition().x, l1Label->getPosition().y - 30.0f),
        5, "L1", 5.0f, 336);
    m_editorLayer2Input = SetGroupIDLayer::createTextInput(
        cocos2d::CCPoint(50.0f, 1.0f),
        cocos2d::CCPoint(l2Label->getPosition().x, l2Label->getPosition().y - 30.0f),
        6, "L2", 5.0f, 336);
    m_groupIDInput = SetGroupIDLayer::createTextInput(
        cocos2d::CCPoint(50.0f, 1.0f),
        cocos2d::CCPoint(groupLabel->getPosition().x, groupLabel->getPosition().y - 30.0f),
        1, (m_groupIDValue == -1) ? "Mixed" : "Num", 5.0f, 336);
    m_zOrderInput = createTextInput(
        cocos2d::CCPoint(50.0f, 1.0f),
        cocos2d::CCPoint(zLabel->getPosition().x, zLabel->getPosition().y - 30.0f),
        7, "Z", 5.0f, 336);

    ButtonSprite* nextFreeSprite = ButtonSprite::create(
        "Next Free", 0, 0, 0.7f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* nextFreeButton = CCMenuItemSpriteExtra::create(
        nextFreeSprite, nullptr, menu_selector(SetGroupIDLayer::onNextGroupID1));
    m_buttonMenu->addChild(nextFreeButton);
    nextFreeButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, m_groupIDInput->getPosition()) + cocos2d::CCPoint(-125.0f, 0.0f));

    ButtonSprite* addSprite = ButtonSprite::create(
        "Add", 0, 0, 0.7f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* addButton = CCMenuItemSpriteExtra::create(
        addSprite, nullptr, menu_selector(SetGroupIDLayer::onAddGroup));
    m_buttonMenu->addChild(addButton);
    addButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, m_groupIDInput->getPosition()) + cocos2d::CCPoint(105.0f, 0.0f));

    if (obj) {
        ButtonSprite* pSprite = ButtonSprite::create(
            "P", 0, 0, 0.7f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
        CCMenuItemSpriteExtra* pButton = CCMenuItemSpriteExtra::create(
            pSprite, nullptr, menu_selector(SetGroupIDLayer::onAddGroupParent));
        m_buttonMenu->addChild(pButton);
        pButton->setPosition(addButton->getPosition() + cocos2d::CCPoint(60.0f, 0.0f));
    }

    const char* zLayerNames[] = {"B5", "B4", "B3", "B2", "B1", "T1", "T2", "T3", "T4", "Default"};
    int zLayerValues[] = {-5, -3, -1, 1, 3, 5, 7, 9, 11, 0};

    cocos2d::CCArray* zButtons = cocos2d::CCArray::create();
    for (int i = 0; i < 10; i++) {
        ButtonSprite* layerSprite = ButtonSprite::create(
            zLayerNames[i], 0, 0, 0.3f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
        m_zLayerButtons->addObject(layerSprite);
        CCMenuItemSpriteExtra* layerButton = CCMenuItemSpriteExtra::create(
            layerSprite, nullptr, menu_selector(SetGroupIDLayer::onZLayer));
        layerButton->setTag(zLayerValues[i]);
        m_buttonMenu->addChild(layerButton);
        zButtons->addObject(layerButton);
    }

    cocos2d::CCPoint zButtonsPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        zLabel->getPosition() + cocos2d::CCPoint(0.0f, -28.0f));
    GameToolbox::alignItemsHorisontally(zButtons, 5.0f, zButtonsPos, false);

    determineStartValues();

    if (m_zLayerValue == -100) {
        if (m_targetObjects && m_targetObjects->count() > 0) {
            for (int i = 0; i < 2; i++) {
                const char* frame = i ? "GJ_colorBtn_001.png" : "GJ_colorBtn_001.png";
                cocos2d::CCSprite* shiftSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame);
                shiftSprite->setColor(i ? ccc3(0, 255, 0) : ccc3(255, 0, 0));
                shiftSprite->setOpacity(100);
                cocos2d::CCLabelBMFont* shiftLabel = cocos2d::CCLabelBMFont::create(i ? "+" : "-", "bigFont.fnt");
                shiftSprite->addChild(shiftLabel);
                shiftLabel->setScale(1.4f);
                shiftLabel->setPosition(cocos2d::CCPoint(2.0f, 2.5f));
                shiftSprite->setScale(0.3f);

                CCMenuItemSpriteExtra* shiftButton = CCMenuItemSpriteExtra::create(
                    shiftSprite, nullptr, menu_selector(SetGroupIDLayer::onZLayerShift));
                m_buttonMenu->addChild(shiftButton);
                shiftButton->setTag(i);
                shiftButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
                    zLabel->getPosition() + cocos2d::CCPoint(i ? 40.0f : -40.0f, -2.0f)));
                shiftButton->setSizeMult(2.0f);
            }
        }
    }

    updateGroupIDLabel();
    updateEditorLabel();
    updateEditorLabel2();
    updateZOrderLabel();
    updateZLayerButtons();
    updateGroupIDButtons();

    cocos2d::CCPoint copyPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f + 217.0f, winSize.height * 0.5f + 142.0f));

    ButtonSprite* copySprite = ButtonSprite::create("Copy", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    ButtonSprite* pasteSprite = ButtonSprite::create("Paste", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    ButtonSprite* extraSprite = ButtonSprite::create("Extra", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    ButtonSprite* extra2Sprite = ButtonSprite::create("Extra2", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    ButtonSprite* animSprite = ButtonSprite::create("Anim", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);

    CCMenuItemSpriteExtra* copyButton = CCMenuItemSpriteExtra::create(copySprite, nullptr, menu_selector(SetGroupIDLayer::onCopy));
    CCMenuItemSpriteExtra* pasteButton = CCMenuItemSpriteExtra::create(pasteSprite, nullptr, menu_selector(SetGroupIDLayer::onPaste));
    CCMenuItemSpriteExtra* extraButton = CCMenuItemSpriteExtra::create(extraSprite, nullptr, menu_selector(SetGroupIDLayer::onExtra));
    CCMenuItemSpriteExtra* extra2Button = CCMenuItemSpriteExtra::create(extra2Sprite, nullptr, menu_selector(SetGroupIDLayer::onExtra2));
    CCMenuItemSpriteExtra* animButton = CCMenuItemSpriteExtra::create(animSprite, nullptr, menu_selector(SetGroupIDLayer::onAnim));

    if (obj) {
        m_buttonMenu->addChild(copyButton);
        copyButton->setPosition(copyPos);
    }

    if (obj || LevelEditorLayer::canPasteState(GameManager::sharedState()->m_levelEditorLayer)) {
        m_buttonMenu->addChild(pasteButton);
        pasteButton->setPosition(copyPos + cocos2d::CCPoint(0.0f, -31.0f));
    }

    m_buttonMenu->addChild(extraButton);
    extraButton->setPosition(copyPos + cocos2d::CCPoint(0.0f, -62.0f));

    m_buttonMenu->addChild(extra2Button);
    extra2Button->setPosition(copyPos + cocos2d::CCPoint(0.0f, -93.0f));

    m_buttonMenu->addChild(animButton);
    animButton->setPosition(copyPos + cocos2d::CCPoint(0.0f, -124.0f));

    cocos2d::CCPoint toggleOffset = cocos2d::CCPoint(0.0f, 2.0f);
    cocos2d::CCPoint toggleStart = cocos2d::CCPoint(
        winSize.width * 0.5f + 217.0f,
        winSize.height * 0.5f + 142.0f - 164.0f);

    if (obj) {
        if (obj == (GameObject*)GameManager::sharedState()->m_levelEditorLayer->m_playbackObject) {
            GameToolbox::createToggleButton("Preview", menu_selector(SetGroupIDLayer::onToggleGuide),
                false, true, m_buttonMenu, toggleStart, this, this, 0.7f, 0.3f, 80.0f,
                toggleOffset, "goldFont.fnt", false, 0, 0);
        }
    }

    m_editorLayerEdited = false;

    return true;
}