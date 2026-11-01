#include "EditorUI.h"
#include "FLAlertLayer.h"
#include "LevelEditorLayer.h"
#include "GameObject.h"
#include "EffectGameObject.h"
#include "CreateMenuItem.h"
#include "EditButtonBar.h"
#include "GJRotationControl.h"
#include "GJScaleControl.h"
#include "GJTransformControl.h"
#include "ColorSelectLiveOverlay.h"
#include "HSVLiveOverlay.h"
#include "Slider.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "ButtonSprite.h"
#include "UndoObject.h"
#include "GJSmartTemplate.h"
#include "SetIDPopup.h"
#include "GameManager.h"
#include "GJGameLevel.h"
#include "ObjectToolbox.h"
#include "CCCircleWave.h"
#include "FMODAudioEngine.h"
#include "EditorPauseLayer.h"
#include "LevelSettingsLayer.h"
#include "CustomizeObjectLayer.h"
#include "SetupTriggerPopup.h"
#include "SetGroupIDLayer.h"
#include "SetColorIDPopup.h"
#include "FindObjectPopup.h"
#include "GJSpecialColorSelect.h"
#include "GameToolbox.h"

EditorUI* EditorUI::create(LevelEditorLayer* editorLayer) {
    EditorUI* instance = new EditorUI();
    if (instance && instance->init(editorLayer)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool EditorUI::init(LevelEditorLayer* editorLayer)
{
    if (!CCLayer::init()) {
        return false;
    }

    // Store editor layer reference
    m_editorLayer = editorLayer;

    // Store initial time
    struct timespec ts;
    clock_gettime(0, &ts);
    m_keyTime = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;

    // Enable input handlers
    setTouchEnabled(true);
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    setMouseEnabled(true);

    // Get window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    // Create UI items array
    m_uiItems = CCArray::create();
    m_uiItems->retain();

    // Check if increased scale is enabled
    m_increaseScale = GameManager::sharedState()->getGameVariable("0112");

    // Create position slider
    Slider* slider = Slider::create(
        this,
        menu_selector(EditorUI::sliderChanged),
        0,
        "sliderBar.png",
        "slidergroove_02.png",
        "GJ_moveBtn.png",
        "GJ_moveSBtn.png",
        0.9f
    );
    m_positionSlider = slider;
    slider->setBarVisibility(false);
    addChild(m_positionSlider, 2);

    // Position slider at top center
    float screenTop = CCDirector::sharedDirector()->getScreenTop();
    m_positionSlider->setPosition(ccp(winSize.width * 0.5f + 30.0f, screenTop - 20.0f));
    m_uiItems->addObject(m_positionSlider);

    // Create rotation control
    GJRotationControl* rotationControl = GJRotationControl::create();
    m_rotationControl = rotationControl;
    rotationControl->retain();
    m_editorLayer->m_objectLayer->addChild(m_rotationControl, 10000);
    m_rotationControl->m_delegate = &m_rotationDelegate;
    deactivateRotationControl();

    // Create scale control
    GJScaleControl* scaleControl = GJScaleControl::create();
    m_scaleControl = scaleControl;
    scaleControl->retain();
    m_editorLayer->m_objectLayer->addChild(m_scaleControl, 10000);
    m_scaleControl->m_delegate = &m_scaleDelegate;
    deactivateScaleControl();

    // Create transform control
    GJTransformControl* transformControl = GJTransformControl::create();
    m_transformControl = transformControl;
    transformControl->retain();
    m_editorLayer->m_objectLayer->addChild(m_transformControl, 10000);
    m_transformControl->m_delegate = &m_transformDelegate;
    deactivateTransformControl();

    // Create transform node
    m_transformNode = CCNode::create();
    m_editorLayer->addChild(m_transformNode);
    m_transformChild = CCNode::create();
    m_transformNode->addChild(m_transformChild);

    // Create toolbar background
    CCSprite* bgSprite = CCSprite::create("edit_barBG_001.png");
    float barWidth = winSize.width + 2.0f;
    bgSprite->setScaleX(barWidth / bgSprite->getContentSize().width);
    bgSprite->setAnchorPoint(ccp(0, 0));
    bgSprite->setPosition(ccp(CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
                               CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    bgSprite->setZOrder(-1);
    
    m_toolbarHeight = bgSprite->getContentSize().height * bgSprite->getScaleY();
    m_constrainedHeight = m_toolbarHeight + 140.0f;
    addChild(bgSprite, 1);
    m_uiItems->addObject(bgSprite);

    // Create selection arrays
    m_selectedObjects = CCArray::create();
    m_selectedObjects->retain();
    
    m_snapPositions = CCArray::create();
    m_snapPositions->retain();
    
    m_deleteObjects = CCArray::create();
    m_deleteObjects->retain();

    // Set initial mode
    m_selectedMode = 0;

    // ========================================================================
    // CREATE MODE BUTTONS (Build, Edit, Delete)
    // ========================================================================
    CCMenu* modeMenu = CCMenu::create();
    addChild(modeMenu, 2);

    m_buildModeBtn = getModeBtn("edit_buildBtn_001.png", 2);
    modeMenu->addChild(m_buildModeBtn);

    m_editModeBtn = getModeBtn("edit_editBtn_001.png", 3);
    modeMenu->addChild(m_editModeBtn);

    m_deleteModeBtn = getModeBtn("edit_deleteBtn_001.png", 1);
    modeMenu->addChild(m_deleteModeBtn);

    // Position mode buttons
    float screenLeft = CCDirector::sharedDirector()->getScreenLeft();
    float screenBottom = CCDirector::sharedDirector()->getScreenBottom();
    CCPoint basePos = ccp(screenLeft + 42.0f, screenBottom + 75.0f);
    CCPoint nodePos = modeMenu->convertToNodeSpace(basePos);
    
    m_buildModeBtn->setPosition(nodePos);
    m_editModeBtn->setPosition(ccp(nodePos.x, nodePos.y - 29.0f));
    m_deleteModeBtn->setPosition(ccp(nodePos.x, nodePos.y - 58.0f));

    // Vertical line separator
    CCSprite* vLine = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
    addChild(vLine, 2);
    vLine->setPosition(ccp(screenLeft + 85.0f, screenBottom + m_toolbarHeight * 0.5f));
    m_uiItems->addObject(vLine);

    // ========================================================================
    // CREATE TOGGLE BUTTONS (Swipe, Free Move, Snap, Rotate)
    // ========================================================================
    m_swipeBtn = getSpriteButton("edit_swipeBtn_001.png", menu_selector(EditorUI::toggleSwipe), modeMenu, 1.0f);
    m_freeMoveBtn = getSpriteButton("edit_freeMoveBtn_001.png", menu_selector(EditorUI::toggleFreeMove), modeMenu, 1.0f);
    m_snapBtn = getSpriteButton("edit_snapBtn_001.png", menu_selector(EditorUI::toggleSnap), modeMenu, 1.0f);
    m_rotateBtn = getSpriteButton("edit_enableRotateBtn_001.png", menu_selector(EditorUI::toggleEnableRotate), modeMenu, 1.0f);

    // Position toggle buttons on right side
    float screenRight = CCDirector::sharedDirector()->getScreenRight();
    CCPoint rotatePos = ccp(screenRight - 25.0f, screenBottom + m_toolbarHeight - 26.0f);
    CCPoint rotateNodePos = modeMenu->convertToNodeSpace(rotatePos);
    m_rotateBtn->setPosition(rotateNodePos);

    m_snapBtn->setPosition(ccp(rotateNodePos.x, rotateNodePos.y - 43.0f));
    m_swipeBtn->setPosition(ccp(rotateNodePos.x - 44.0f, rotateNodePos.y));
    m_freeMoveBtn->setPosition(ccp(rotateNodePos.x - 44.0f, rotateNodePos.y - 43.0f));

    // Second vertical line
    CCSprite* vLine2 = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
    addChild(vLine2, 2);
    vLine2->setPosition(ccp(screenRight - 96.0f, screenBottom + m_toolbarHeight * 0.5f));
    m_uiItems->addObject(vLine2);

    // ========================================================================
    // CREATE ACTION BUTTONS (Undo, Redo, Trash, Playback, Playtest, Zoom)
    // ========================================================================
    
    // Undo button
    CCSprite* undoSp = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
    m_undoBtn = CCMenuItemSpriteExtra::create(undoSp, this, menu_selector(EditorUI::undoLastAction));
    modeMenu->addChild(m_undoBtn);
    CCPoint undoPos = ccp(screenLeft + 25.0f, CCDirector::sharedDirector()->getScreenTop() - 20.0f);
    m_undoBtn->setPosition(modeMenu->convertToNodeSpace(undoPos));

    // Redo button
    CCSprite* redoSp = CCSprite::createWithSpriteFrameName("GJ_redoBtn_001.png");
    m_redoBtn = CCMenuItemSpriteExtra::create(redoSp, this, menu_selector(EditorUI::redoLastAction));
    modeMenu->addChild(m_redoBtn);
    m_redoBtn->setPosition(ccp(m_undoBtn->getPositionX() + 50.0f, m_undoBtn->getPositionY()));

    // Trash button
    CCSprite* trashSp = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    trashSp->setScale(0.9f);
    m_trashBtn = CCMenuItemSpriteExtra::create(trashSp, this, menu_selector(EditorUI::onDeleteSelected));
    modeMenu->addChild(m_trashBtn);
    m_trashBtn->setPosition(ccp(m_redoBtn->getPositionX() + 55.0f, m_redoBtn->getPositionY()));

    // Playback button
    CCSprite* playSp = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png");
    m_playbackBtn = CCMenuItemSpriteExtra::create(playSp, this, menu_selector(EditorUI::onPlayback));
    modeMenu->addChild(m_playbackBtn);
    m_playbackBtn->setPosition(ccp(m_undoBtn->getPositionX(), m_undoBtn->getPositionY() - 45.0f));

    // Playtest button
    CCSprite* testSp = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
    m_playtestBtn = CCMenuItemSpriteExtra::create(testSp, this, menu_selector(EditorUI::onPlaytest));
    modeMenu->addChild(m_playtestBtn);
    m_playtestBtn->setPosition(ccp(m_playbackBtn->getPositionX(), m_playbackBtn->getPositionY() - 45.0f));

    // Playtest stop button
    CCSprite* stopSp = CCSprite::createWithSpriteFrameName("GJ_stopEditorBtn_001.png");
    m_playtestStopBtn = CCMenuItemSpriteExtra::create(stopSp, this, menu_selector(EditorUI::onStopPlaytest));
    modeMenu->addChild(m_playtestStopBtn);
    m_playtestStopBtn->setPosition(ccp(m_playtestBtn->getPositionX() + 50.0f, m_playtestBtn->getPositionY()));
    m_playtestStopBtn->setVisible(false);

    // Zoom buttons
    CCSprite* zoomInSp = CCSprite::createWithSpriteFrameName("GJ_zoomInBtn_001.png");
    zoomInSp->setScale(0.8f);
    CCMenuItemSpriteExtra* zoomInBtn = CCMenuItemSpriteExtra::create(zoomInSp, this, menu_selector(EditorUI::zoomIn));
    modeMenu->addChild(zoomInBtn);
    zoomInBtn->setPosition(ccp(m_playtestBtn->getPositionX(), m_playtestBtn->getPositionY() - 45.0f));

    CCSprite* zoomOutSp = CCSprite::createWithSpriteFrameName("GJ_zoomOutBtn_001.png");
    zoomOutSp->setScale(0.8f);
    CCMenuItemSpriteExtra* zoomOutBtn = CCMenuItemSpriteExtra::create(zoomOutSp, this, menu_selector(EditorUI::zoomOut));
    modeMenu->addChild(zoomOutBtn);
    zoomOutBtn->setPosition(ccp(zoomInBtn->getPositionX(), zoomInBtn->getPositionY() - 35.0f));

    // ========================================================================
    // CREATE STICKY CONTROLS
    // ========================================================================
    CCPoint stickyPos = zoomInBtn->getPosition() + ccp(34.0f, -17.5f);

    CCSprite* enableSp = CCSprite::createWithSpriteFrameName("gj_linkEnableBtn_001.png");
    enableSp->setScale(0.83f);
    m_enableLinkBtn = CCMenuItemSpriteExtra::create(enableSp, this, menu_selector(EditorUI::onStickyToggle));
    modeMenu->addChild(m_enableLinkBtn);
    m_enableLinkBtn->setPosition(stickyPos + ccp(0.0f, 25.0f));

    CCSprite* linkSp = CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png");
    linkSp->setScale(0.83f);
    m_linkBtn = CCMenuItemSpriteExtra::create(linkSp, this, menu_selector(EditorUI::onGroupSticky));
    modeMenu->addChild(m_linkBtn);
    m_linkBtn->setPosition(stickyPos);

    CCSprite* unlinkSp = CCSprite::createWithSpriteFrameName("gj_linkBtnOff_001.png");
    unlinkSp->setScale(0.83f);
    m_unlinkBtn = CCMenuItemSpriteExtra::create(unlinkSp, this, menu_selector(EditorUI::onUngroupSticky));
    modeMenu->addChild(m_unlinkBtn);
    m_unlinkBtn->setPosition(stickyPos + ccp(0.0f, -25.0f));

    // Initialize sticky controls state
    m_stickyControlsEnabled = GameManager::sharedState()->getGameVariable("0097");
    m_linkControlsDisabled = GameManager::sharedState()->getGameVariable("0180");
    if (!m_stickyControlsEnabled) {
        m_linkBtn->setEnabled(false);
        m_linkBtn->setVisible(false);
        m_unlinkBtn->setEnabled(false);
        m_unlinkBtn->setVisible(false);
        m_enableLinkBtn->setEnabled(false);
        m_enableLinkBtn->setVisible(false);
    }

    // ========================================================================
    // SETUP MENUS
    // ========================================================================
    setupDeleteMenu();
    setupCreateMenu();
    setupEditMenu();
    resetUI();

    // ========================================================================
    // PAUSE BUTTON
    // ========================================================================
    CCSprite* pauseSp = CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png");
    CCMenuItemSpriteExtra* pauseBtn = CCMenuItemSpriteExtra::create(pauseSp, this, menu_selector(EditorUI::onPause));
    pauseBtn->m_volume = 0.0f;
    CCMenu* pauseMenu = CCMenu::create(pauseBtn, nullptr);
    addChild(pauseMenu, 10);
    pauseMenu->setPosition(ccp(CCDirector::sharedDirector()->getScreenRight() - 20.0f,
                                CCDirector::sharedDirector()->getScreenTop() - 20.0f));

    // Set initial mode to Build
    toggleMode(m_buildModeBtn);

    // Position game layer
    float screenLeft2 = CCDirector::sharedDirector()->getScreenLeft();
    m_editorLayer->m_objectLayer->setPosition(ccp(screenLeft2, m_constrainedHeight + 30.0f));
    constrainGameLayerPosition(-1.0f, -0.5f);
    updateSlider();

    // ========================================================================
    // CREATE EDITOR TOOLBAR BUTTONS
    // ========================================================================
    CCMenu* toolMenu = (CCMenu*)pauseMenu;

    // Options button
    CCSprite* optionsSp = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(optionsSp, this, menu_selector(EditorUI::onSettings));
    toolMenu->addChild(optionsBtn);
    CCPoint optionsPos = ccp(CCDirector::sharedDirector()->getScreenRight() - 70.0f,
                              CCDirector::sharedDirector()->getScreenTop() - 20.0f);
    optionsBtn->setPosition(toolMenu->convertToNodeSpace(optionsPos));

    // Position for toolbar buttons
    CCPoint toolPos = ccp(CCDirector::sharedDirector()->getScreenRight() - 24.0f,
                           CCDirector::sharedDirector()->getScreenTop() - 60.0f);
    toolPos = toolMenu->convertToNodeSpace(toolPos);

    // Copy/Paste buttons
    CCSprite* dupSp = CCSprite::createWithSpriteFrameName("GJ_duplicateObjectBtn2_001.png");
    dupSp->setScale(1.0f);
    m_copyPasteBtn = CCMenuItemSpriteExtra::create(dupSp, this, menu_selector(EditorUI::onDuplicate));
    toolMenu->addChild(m_copyPasteBtn);
    m_copyPasteBtn->setPosition(toolPos);
    m_copyPasteBtn->setSizeMult(1.2f);

    CCSprite* copySp = CCSprite::createWithSpriteFrameName("GJ_copyBtn2_001.png");
    copySp->setScale(1.0f);
    m_copyBtn = CCMenuItemSpriteExtra::create(copySp, this, menu_selector(EditorUI::onCopy));
    toolMenu->addChild(m_copyBtn);
    m_copyBtn->setPosition(toolPos + ccp(-76.0f, 0.0f));
    m_copyBtn->setSizeMult(1.2f);

    CCSprite* pasteSp = CCSprite::createWithSpriteFrameName("GJ_pasteBtn2_001.png");
    pasteSp->setScale(1.0f);
    m_pasteBtn = CCMenuItemSpriteExtra::create(pasteSp, this, menu_selector(EditorUI::onPaste));
    toolMenu->addChild(m_pasteBtn);
    m_pasteBtn->setPosition(toolPos + ccp(-38.0f, 0.0f));
    m_pasteBtn->setSizeMult(1.2f);

    // Edit buttons
    CCSprite* hsvSp = CCSprite::createWithSpriteFrameName("GJ_editHSVBtn2_001.png");
    hsvSp->setScale(1.0f);
    m_editHSVBtn = CCMenuItemSpriteExtra::create(hsvSp, this, menu_selector(EditorUI::onEditColor));
    toolMenu->addChild(m_editHSVBtn);
    m_editHSVBtn->setPosition(toolPos + ccp(-76.0f, -105.0f));
    m_editHSVBtn->setSizeMult(1.2f);

    CCSprite* specialSp = CCSprite::createWithSpriteFrameName("GJ_editObjBtn4_001.png");
    specialSp->setScale(1.0f);
    m_editSpecialBtn = CCMenuItemSpriteExtra::create(specialSp, this, menu_selector(EditorUI::editObject2));
    toolMenu->addChild(m_editSpecialBtn);
    m_editSpecialBtn->setPosition(toolPos + ccp(-76.0f, -35.0f));
    m_editSpecialBtn->setSizeMult(1.2f);

    CCSprite* editSp = CCSprite::createWithSpriteFrameName("GJ_editObjBtn3_001.png");
    editSp->setScale(1.0f);
    m_editObjectBtn = CCMenuItemSpriteExtra::create(editSp, this, menu_selector(EditorUI::editObject));
    toolMenu->addChild(m_editObjectBtn);
    m_editObjectBtn->setPosition(toolPos + ccp(0.0f, -35.0f));
    m_editObjectBtn->setSizeMult(1.2f);

    // Deselect button
    CCSprite* deselectSp = CCSprite::createWithSpriteFrameName("GJ_deSelBtn2_001.png");
    deselectSp->setScale(1.0f);
    m_deselectBtn = CCMenuItemSpriteExtra::create(deselectSp, this, menu_selector(EditorUI::onDeselectAll));
    toolMenu->addChild(m_deselectBtn);
    m_deselectBtn->setPosition(toolPos + ccp(0.0f, -105.0f));
    m_deselectBtn->setSizeMult(1.2f);

    // Group ID button
    CCSprite* groupSp = CCSprite::createWithSpriteFrameName("GJ_groupIDBtn2_001.png");
    groupSp->setScale(1.0f);
    m_editGroupBtn = CCMenuItemSpriteExtra::create(groupSp, this, menu_selector(EditorUI::editGroup));
    toolMenu->addChild(m_editGroupBtn);
    m_editGroupBtn->setPosition(toolPos + ccp(-38.0f, -35.0f));
    m_editGroupBtn->setSizeMult(1.2f);

    // Copy State button
    CCSprite* copyStateSp = CCSprite::createWithSpriteFrameName("GJ_copyStateBtn_001.png");
    copyStateSp->setScale(1.0f);
    m_copyValuesBtn = CCMenuItemSpriteExtra::create(copyStateSp, this, menu_selector(EditorUI::onCopyState));
    toolMenu->addChild(m_copyValuesBtn);
    m_copyValuesBtn->setPosition(toolPos + ccp(-76.0f, -70.0f));
    m_copyValuesBtn->setSizeMult(1.2f);

    // Paste State button
    CCSprite* pasteStateSp = CCSprite::createWithSpriteFrameName("GJ_pasteStateBtn_001.png");
    pasteStateSp->setScale(1.0f);
    m_pasteStateBtn = CCMenuItemSpriteExtra::create(pasteStateSp, this, menu_selector(EditorUI::onPasteState));
    toolMenu->addChild(m_pasteStateBtn);
    m_pasteStateBtn->setPosition(toolPos + ccp(-38.0f, -70.0f));
    m_pasteStateBtn->setSizeMult(1.2f);

    // Paste Color button
    CCSprite* pasteColorSp = CCSprite::createWithSpriteFrameName("GJ_pasteColorBtn_001.png");
    pasteColorSp->setScale(1.0f);
    m_pasteColorBtn = CCMenuItemSpriteExtra::create(pasteColorSp, this, menu_selector(EditorUI::onPasteColor));
    toolMenu->addChild(m_pasteColorBtn);
    m_pasteColorBtn->setPosition(toolPos + ccp(0.0f, -70.0f));
    m_pasteColorBtn->setSizeMult(1.2f);

    // Go to Layer button
    CCSprite* goLayerSp = CCSprite::createWithSpriteFrameName("GJ_goToLayerBtn_001.png");
    goLayerSp->setScale(1.0f);
    m_goToLayerBtn = CCMenuItemSpriteExtra::create(goLayerSp, this, menu_selector(EditorUI::onGoToLayer));
    toolMenu->addChild(m_goToLayerBtn);
    m_goToLayerBtn->setPosition(toolPos + ccp(-38.0f, -105.0f));
    m_goToLayerBtn->setSizeMult(1.2f);

    // ========================================================================
    // LAYER NAVIGATION CONTROLS
    // ========================================================================
    CCPoint layerPos = ccp(CCDirector::sharedDirector()->getScreenRight() - 46.0f,
                            CCDirector::sharedDirector()->getScreenBottom() + 123.0f);

    // Next layer button
    CCSprite* nextSp = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextSp->setFlipX(true);
    nextSp->setScale(0.6f);
    m_layerNextBtn = CCMenuItemSpriteExtra::create(nextSp, this, menu_selector(EditorUI::onGroupUp));
    toolMenu->addChild(m_layerNextBtn);
    CCPoint nextPos = layerPos + ccp(30.0f, 0.0f);
    m_layerNextBtn->setPosition(toolMenu->convertToNodeSpace(nextPos));
    m_layerNextBtn->setSizeMult(1.2f);

    // Previous layer button
    CCSprite* prevSp = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    prevSp->setScale(0.6f);
    m_layerPrevBtn = CCMenuItemSpriteExtra::create(prevSp, this, menu_selector(EditorUI::onGroupDown));
    toolMenu->addChild(m_layerPrevBtn);
    CCPoint prevPos = layerPos + ccp(-30.0f, 0.0f);
    m_layerPrevBtn->setPosition(toolMenu->convertToNodeSpace(prevPos));
    m_layerPrevBtn->setSizeMult(1.2f);

    // Go to base layer button
    CCSprite* baseSp = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    baseSp->setScale(0.54f);
    baseSp->setOpacity(175);
    m_goToBaseBtn = CCMenuItemSpriteExtra::create(baseSp, this, menu_selector(EditorUI::onGoToBaseLayer));
    toolMenu->addChild(m_goToBaseBtn);
    CCPoint basePos2 = layerPos + ccp(-58.0f, 0.0f);
    m_goToBaseBtn->setPosition(toolMenu->convertToNodeSpace(basePos2));
    m_goToBaseBtn->setSizeMult(1.2f);

    // Layer label
    m_currentLayerLabel = CCLabelBMFont::create();
    addChild(m_currentLayerLabel);
    m_currentLayerLabel->setScale(0.6f);
    m_currentLayerLabel->setPosition(layerPos);
    m_uiItems->addObject(m_currentLayerLabel);

    // Layer lock sprite
    CCSprite* lockSp = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
    lockSp->setScale(0.5f);
    m_layerLockSprite = lockSp;
    m_layerLockSprite->setPosition(m_currentLayerLabel->getPosition() + ccp(12.0f, 5.0f));
    addChild(m_layerLockSprite, 2);
    m_uiItems->addObject(m_layerLockSprite);
    m_layerLockSprite->setVisible(false);

    // Lock layer button (invisible)
    CCSprite* lockBtnSp = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    lockBtnSp->setOpacity(0);
    lockBtnSp->setScaleY(0.8f);
    CCMenuItemSpriteExtra* lockBtn = CCMenuItemSpriteExtra::create(lockBtnSp, this, menu_selector(EditorUI::onLockLayer));
    toolMenu->addChild(lockBtn);
    lockBtn->setPosition(m_layerNextBtn->getPosition() + ccp(-24.5f, 1.0f));

    // Update labels
    updateGroupIDLabel();
    updateButtons();

    // ========================================================================
    // OBJECT INFO LABEL
    // ========================================================================
    m_objectInfoLabel = CCLabelBMFont::create();
    m_objectInfoLabel->setScale(0.6f);
    m_objectInfoLabel->setAnchorPoint(ccp(0.0f, 1.0f));
    addChild(m_objectInfoLabel);
    m_objectInfoLabel->setPosition(ccp(52.0f, winSize.height - 50.0f));

    // Initialize touch IDs
    m_rotationTouchID = -1;
    m_scaleTouchID = -1;
    m_touchID = -1;
    m_transformTouchID = -1;

    // Initialize toggle states
    m_swipeEnabled = !GameManager::sharedState()->getGameVariable("0003");
    m_freeMoveEnabled = !GameManager::sharedState()->getGameVariable("0004");
    GameManager::sharedState()->toggleGameVariable("0008");
    GameManager::sharedState()->toggleGameVariable("0007");

    // Set initial toggle states
    toggleSwipe(nullptr);
    toggleFreeMove(nullptr);
    toggleSnap(nullptr);
    toggleEnableRotate(nullptr);

    // Add music download delegate
    MusicDownloadManager::sharedState()->addMusicDownloadDelegate(&m_musicDelegate);

    // Setup transform control and info label
    setupTransformControl();
    toggleObjectInfoLabel();

    // Add all UI items to the list for visibility toggling
    m_uiItems->addObjectsFromArray(modeMenu->getChildren());
    CCArray* toolChildren = toolMenu->getChildren();
    m_uiItems->addObjectsFromArray(toolChildren);

    // Remove buttons that shouldn't be affected by UI hide
    m_uiItems->removeObject(pauseBtn);
    m_uiItems->removeObject(m_playtestBtn);
    m_uiItems->removeObject(m_playtestStopBtn);

    return true;
}

EditorUI::~EditorUI() {
    CC_SAFE_RELEASE(m_selectedObjects);
    CC_SAFE_RELEASE(m_tabsArray);
    CC_SAFE_RELEASE(m_createButtonArray);
    CC_SAFE_RELEASE(m_customObjectButtonArray);
    CC_SAFE_RELEASE(m_createButtonBars);
    CC_SAFE_RELEASE(m_uiItems);
    CC_SAFE_RELEASE(m_snapPositions);
    CC_SAFE_RELEASE(m_deleteObjects);
}

void EditorUI::setupCreateMenu() {
    // Initialize arrays
    m_selectedTab = -1;
    m_selectedObjectIndex = 0;
    
    m_createButtonArray = CCArray::create();
    m_createButtonArray->retain();
    
    m_customObjectButtonArray = CCArray::create();
    m_customObjectButtonArray->retain();
    
    m_createButtonBars = CCArray::create();
    m_createButtonBars->retain();
    
    m_tabsArray = CCArray::create();
    m_tabsArray->retain();
    
    m_tabsMenu = CCMenu::create();
    addChild(m_tabsMenu, 100);
    
    // Get grid configuration
    GameManager* gm = GameManager::sharedState();
    int rows = gm->getIntGameVariable("0049");
    int cols = gm->getIntGameVariable("0050");
    
    // Clamp minimum values
    if (rows <= 5) {
        rows = 6;
        gm->setIntGameVariable("0049", 6);
    }
    if (cols <= 1) {
        cols = 2;
        gm->setIntGameVariable("0050", 2);
    }
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float bottom = CCDirector::sharedDirector()->getScreenBottom();
    CCPoint barPosition = ccp(winSize.width * 0.5f - 5.0f, bottom + m_toolbarHeight - 6.0f);
    
    // Tab icon names
    CCArray* tabIcons = CCArray::create();
    tabIcons->addObject(CCString::create("square_01_001.png"));          // Tab 0: Blocks
    tabIcons->addObject(CCString::create("blockOutline_01_001.png"));     // Tab 1: Outlines
    tabIcons->addObject(CCString::create("triangle_a_02_001.png"));       // Tab 2: Triangles
    tabIcons->addObject(CCString::create("spike_01_001.png"));            // Tab 3: Spikes
    tabIcons->addObject(CCString::create("persp_outline_01_001.png"));    // Tab 4: Perspective
    tabIcons->addObject(CCString::create("ring_01_001.png"));             // Tab 5: Rings
    tabIcons->addObject(CCString::create("GJBeast01_01_001.png"));        // Tab 6: Beasts
    tabIcons->addObject(CCString::create("pixelb_03_01_001.png"));        // Tab 7: Pixel
    tabIcons->addObject(CCString::create("particle_01_001.png"));         // Tab 8: Particles
    tabIcons->addObject(CCString::create("pixelitem_001_001.png"));       // Tab 9: Items
    tabIcons->addObject(CCString::create("d_spikes_01_001.png"));         // Tab 10: D Spikes
    tabIcons->addObject(CCString::create("sawblade_02_001.png"));         // Tab 11: Sawblades
    tabIcons->addObject(CCString::create("edit_eTintCol01Btn_001.png"));  // Tab 12: Colors
    tabIcons->addObject(CCString::create("custom"));                      // Tab 13: Custom
    
    // ========================================================================
    // TAB 0: SOLIDS (Blocks)
    // ========================================================================
    CCArray* solidsItems = CCArray::create();
    solidsItems->addObject(getCreateBtn(1, 4));    // Block
    solidsItems->addObject(getCreateBtn(83, 4));   // Block
    solidsItems->addObject(getCreateBtn(2, 4));    // Block
    solidsItems->addObject(getCreateBtn(3, 4));    // Block
    solidsItems->addObject(getCreateBtn(4, 4));    // Block
    solidsItems->addObject(getCreateBtn(5, 4));    // Block
    solidsItems->addObject(getCreateBtn(502, 4));  // Block
    solidsItems->addObject(getCreateBtn(6, 4));    // Block
    solidsItems->addObject(getCreateBtn(7, 4));    // Block
    solidsItems->addObject(getCreateBtn(69, 4));   // Block
    solidsItems->addObject(getCreateBtn(70, 4));   // Block
    solidsItems->addObject(getCreateBtn(71, 4));   // Block
    solidsItems->addObject(getCreateBtn(72, 4));   // Block
    solidsItems->addObject(getCreateBtn(73, 4));   // Block
    solidsItems->addObject(getCreateBtn(74, 4));   // Block
    solidsItems->addObject(getCreateBtn(75, 4));   // Block
    solidsItems->addObject(getCreateBtn(76, 4));   // Block
    solidsItems->addObject(getCreateBtn(77, 4));   // Block
    solidsItems->addObject(getCreateBtn(78, 4));   // Block
    solidsItems->addObject(getCreateBtn(80, 4));   // Block
    solidsItems->addObject(getCreateBtn(81, 4));   // Block
    solidsItems->addObject(getCreateBtn(82, 4));   // Block
    solidsItems->addObject(getCreateBtn(90, 4));   // Block
    solidsItems->addObject(getCreateBtn(91, 4));   // Block
    solidsItems->addObject(getCreateBtn(92, 4));   // Block
    solidsItems->addObject(getCreateBtn(93, 4));   // Block
    solidsItems->addObject(getCreateBtn(94, 4));   // Block
    solidsItems->addObject(getCreateBtn(95, 4));   // Block
    solidsItems->addObject(getCreateBtn(96, 4));   // Block
    solidsItems->addObject(getCreateBtn(116, 4));  // Block
    solidsItems->addObject(getCreateBtn(117, 4));  // Block
    solidsItems->addObject(getCreateBtn(118, 4));  // Block
    solidsItems->addObject(getCreateBtn(119, 4));  // Block
    solidsItems->addObject(getCreateBtn(120, 4));  // Block
    solidsItems->addObject(getCreateBtn(121, 4));  // Block
    solidsItems->addObject(getCreateBtn(122, 4));  // Block
    solidsItems->addObject(getCreateBtn(207, 4));  // Block
    solidsItems->addObject(getCreateBtn(208, 4));  // Block
    solidsItems->addObject(getCreateBtn(209, 4));  // Block
    solidsItems->addObject(getCreateBtn(210, 4));  // Block
    solidsItems->addObject(getCreateBtn(211, 4));  // Block
    solidsItems->addObject(getCreateBtn(212, 4));  // Block
    solidsItems->addObject(getCreateBtn(213, 4));  // Block
    solidsItems->addObject(getCreateBtn(1820, 4)); // Block
    solidsItems->addObject(getCreateBtn(1821, 4)); // Block
    solidsItems->addObject(getCreateBtn(1823, 4)); // Block
    solidsItems->addObject(getCreateBtn(1824, 4)); // Block
    solidsItems->addObject(getCreateBtn(1825, 4)); // Block
    solidsItems->addObject(getCreateBtn(1826, 4)); // Block
    solidsItems->addObject(getCreateBtn(1827, 4)); // Block
    solidsItems->addObject(getCreateBtn(1828, 4)); // Block
    solidsItems->addObject(getCreateBtn(255, 4));  // Block
    solidsItems->addObject(getCreateBtn(256, 4));  // Block
    solidsItems->addObject(getCreateBtn(257, 4));  // Block
    solidsItems->addObject(getCreateBtn(258, 4));  // Block
    solidsItems->addObject(getCreateBtn(259, 4));  // Block
    solidsItems->addObject(getCreateBtn(260, 4));  // Block
    solidsItems->addObject(getCreateBtn(261, 4));  // Block
    solidsItems->addObject(getCreateBtn(263, 4));  // Block
    solidsItems->addObject(getCreateBtn(264, 4));  // Block
    solidsItems->addObject(getCreateBtn(265, 4));  // Block
    solidsItems->addObject(getCreateBtn(266, 4));  // Block
    solidsItems->addObject(getCreateBtn(267, 4));  // Block
    solidsItems->addObject(getCreateBtn(268, 4));  // Block
    solidsItems->addObject(getCreateBtn(269, 4));  // Block
    solidsItems->addObject(getCreateBtn(270, 4));  // Block
    solidsItems->addObject(getCreateBtn(271, 4));  // Block
    solidsItems->addObject(getCreateBtn(272, 4));  // Block
    solidsItems->addObject(getCreateBtn(273, 4));  // Block
    solidsItems->addObject(getCreateBtn(274, 4));  // Block
    solidsItems->addObject(getCreateBtn(275, 4));  // Block
    solidsItems->addObject(getCreateBtn(1142, 4)); // Block
    solidsItems->addObject(getCreateBtn(1140, 4)); // Block
    solidsItems->addObject(getCreateBtn(1141, 4)); // Block
    solidsItems->addObject(getCreateBtn(1143, 4)); // Block
    solidsItems->addObject(getCreateBtn(1144, 4)); // Block
    solidsItems->addObject(getCreateBtn(1149, 4)); // Block
    solidsItems->addObject(getCreateBtn(1145, 4)); // Block
    solidsItems->addObject(getCreateBtn(1146, 4)); // Block
    solidsItems->addObject(getCreateBtn(1147, 4)); // Block
    solidsItems->addObject(getCreateBtn(1153, 4)); // Block
    solidsItems->addObject(getCreateBtn(1148, 4)); // Block
    solidsItems->addObject(getCreateBtn(1150, 4)); // Block
    solidsItems->addObject(getCreateBtn(1151, 4)); // Block
    solidsItems->addObject(getCreateBtn(1152, 4)); // Block
    solidsItems->addObject(getCreateBtn(476, 4));  // Block
    solidsItems->addObject(getCreateBtn(477, 4));  // Block
    solidsItems->addObject(getCreateBtn(478, 4));  // Block
    solidsItems->addObject(getCreateBtn(479, 4));  // Block
    solidsItems->addObject(getCreateBtn(480, 4));  // Block
    solidsItems->addObject(getCreateBtn(481, 4));  // Block
    solidsItems->addObject(getCreateBtn(482, 4));  // Block
    solidsItems->addObject(getCreateBtn(485, 4));  // Block
    solidsItems->addObject(getCreateBtn(486, 4));  // Block
    solidsItems->addObject(getCreateBtn(487, 4));  // Block
    solidsItems->addObject(getCreateBtn(488, 4));  // Block
    solidsItems->addObject(getCreateBtn(489, 4));  // Block
    solidsItems->addObject(getCreateBtn(490, 4));  // Block
    solidsItems->addObject(getCreateBtn(491, 4));  // Block
    solidsItems->addObject(getCreateBtn(644, 4));  // Block
    solidsItems->addObject(getCreateBtn(650, 4));  // Block
    solidsItems->addObject(getCreateBtn(641, 4));  // Block
    solidsItems->addObject(getCreateBtn(1075, 4)); // Block
    solidsItems->addObject(getCreateBtn(646, 4));  // Block
    solidsItems->addObject(getCreateBtn(642, 4));  // Block
    solidsItems->addObject(getCreateBtn(739, 4));  // Block
    solidsItems->addObject(getCreateBtn(643, 4));  // Block
    solidsItems->addObject(getCreateBtn(645, 4));  // Block
    solidsItems->addObject(getCreateBtn(647, 4));  // Block
    solidsItems->addObject(getCreateBtn(648, 4));  // Block
    solidsItems->addObject(getCreateBtn(649, 4));  // Block
    solidsItems->addObject(getCreateBtn(1077, 4)); // Block
    solidsItems->addObject(getCreateBtn(1298, 4)); // Block
    solidsItems->addObject(getCreateBtn(1076, 4)); // Block
    solidsItems->addObject(getCreateBtn(658, 4));  // Block
    solidsItems->addObject(getCreateBtn(722, 4));  // Block
    solidsItems->addObject(getCreateBtn(659, 4));  // Block
    solidsItems->addObject(getCreateBtn(734, 4));  // Block
    solidsItems->addObject(getCreateBtn(752, 4));  // Block
    solidsItems->addObject(getCreateBtn(753, 4));  // Block
    solidsItems->addObject(getCreateBtn(754, 4));  // Block
    solidsItems->addObject(getCreateBtn(755, 4));  // Block
    solidsItems->addObject(getCreateBtn(756, 4));  // Block
    solidsItems->addObject(getCreateBtn(757, 4));  // Block
    solidsItems->addObject(getCreateBtn(758, 4));  // Block
    solidsItems->addObject(getCreateBtn(759, 4));  // Block
    solidsItems->addObject(getCreateBtn(903, 4));  // Block
    solidsItems->addObject(getCreateBtn(904, 4));  // Block
    solidsItems->addObject(getCreateBtn(905, 4));  // Block
    solidsItems->addObject(getCreateBtn(911, 4));  // Block
    solidsItems->addObject(getCreateBtn(1045, 4)); // Block
    solidsItems->addObject(getCreateBtn(1046, 4)); // Block
    solidsItems->addObject(getCreateBtn(1294, 4)); // Block
    solidsItems->addObject(getCreateBtn(1295, 4)); // Block
    solidsItems->addObject(getCreateBtn(952, 4));  // Block
    solidsItems->addObject(getCreateBtn(953, 4));  // Block
    solidsItems->addObject(getCreateBtn(954, 4));  // Block
    solidsItems->addObject(getCreateBtn(955, 4));  // Block
    solidsItems->addObject(getCreateBtn(956, 4));  // Block
    solidsItems->addObject(getCreateBtn(957, 4));  // Block
    solidsItems->addObject(getCreateBtn(958, 4));  // Block
    solidsItems->addObject(getCreateBtn(959, 4));  // Block
    solidsItems->addObject(getCreateBtn(974, 4));  // Block
    solidsItems->addObject(getCreateBtn(975, 4));  // Block
    solidsItems->addObject(getCreateBtn(976, 4));  // Block
    solidsItems->addObject(getCreateBtn(977, 4));  // Block
    solidsItems->addObject(getCreateBtn(1047, 4)); // Block
    solidsItems->addObject(getCreateBtn(1048, 4)); // Block
    solidsItems->addObject(getCreateBtn(1296, 4)); // Block
    solidsItems->addObject(getCreateBtn(1297, 4)); // Block
    solidsItems->addObject(getCreateBtn(1299, 4)); // Block
    solidsItems->addObject(getCreateBtn(1300, 4)); // Block
    solidsItems->addObject(getCreateBtn(1301, 4)); // Block
    solidsItems->addObject(getCreateBtn(1302, 4)); // Block
    solidsItems->addObject(getCreateBtn(1303, 4)); // Block
    solidsItems->addObject(getCreateBtn(1304, 4)); // Block
    solidsItems->addObject(getCreateBtn(1162, 4)); // Block
    solidsItems->addObject(getCreateBtn(1163, 4)); // Block
    solidsItems->addObject(getCreateBtn(1164, 4)); // Block
    solidsItems->addObject(getCreateBtn(1165, 4)); // Block
    solidsItems->addObject(getCreateBtn(1166, 4)); // Block
    solidsItems->addObject(getCreateBtn(1167, 4)); // Block
    solidsItems->addObject(getCreateBtn(1168, 4)); // Block
    solidsItems->addObject(getCreateBtn(1169, 4)); // Block
    solidsItems->addObject(getCreateBtn(1170, 4)); // Block
    solidsItems->addObject(getCreateBtn(1171, 4)); // Block
    solidsItems->addObject(getCreateBtn(1172, 4)); // Block
    solidsItems->addObject(getCreateBtn(1173, 4)); // Block
    solidsItems->addObject(getCreateBtn(1174, 4)); // Block
    solidsItems->addObject(getCreateBtn(1175, 4)); // Block
    solidsItems->addObject(getCreateBtn(1176, 4)); // Block
    solidsItems->addObject(getCreateBtn(1177, 4)); // Block
    solidsItems->addObject(getCreateBtn(1178, 4)); // Block
    solidsItems->addObject(getCreateBtn(1179, 4)); // Block
    solidsItems->addObject(getCreateBtn(1180, 4)); // Block
    solidsItems->addObject(getCreateBtn(1181, 4)); // Block
    solidsItems->addObject(getCreateBtn(1182, 4)); // Block
    solidsItems->addObject(getCreateBtn(1183, 4)); // Block
    solidsItems->addObject(getCreateBtn(1184, 4)); // Block
    solidsItems->addObject(getCreateBtn(1185, 4)); // Block
    solidsItems->addObject(getCreateBtn(1186, 4)); // Block
    solidsItems->addObject(getCreateBtn(1322, 4)); // Block
    solidsItems->addObject(getCreateBtn(1191, 4)); // Block
    solidsItems->addObject(getCreateBtn(1192, 4)); // Block
    solidsItems->addObject(getCreateBtn(1193, 4)); // Block
    solidsItems->addObject(getCreateBtn(1197, 4)); // Block
    solidsItems->addObject(getCreateBtn(1196, 4)); // Block
    solidsItems->addObject(getCreateBtn(1194, 4)); // Block
    solidsItems->addObject(getCreateBtn(1195, 4)); // Block
    
    EditButtonBar* solidsBar = EditButtonBar::create(solidsItems, barPosition, CCSize(356.0f, 220.0f), 0, true, rows);
    addChild(solidsBar, 10);
    m_createButtonBars->addObject(solidsBar);
    
    // ========================================================================
    // TAB 1: BLOCK OUTLINES
    // ========================================================================
    CCArray* outlinesItems = CCArray::create();
    outlinesItems->addObject(getCreateBtn(467, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(1338, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1339, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(472, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(473, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(474, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(661, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(662, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(663, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(664, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(1154, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1155, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1156, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1157, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1158, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(475, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(1210, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1202, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1203, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1204, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1209, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1205, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1206, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1207, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1208, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1227, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1220, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1221, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1222, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1226, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1223, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1224, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1225, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1260, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1261, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1262, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1263, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1264, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1265, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(146, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(147, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(206, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(204, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(673, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(674, 4));  // Outline
    outlinesItems->addObject(getCreateBtn(1340, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1341, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1342, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1343, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1344, 4)); // Outline
    outlinesItems->addObject(getCreateBtn(1345, 4)); // Outline
    
    EditButtonBar* outlinesBar = EditButtonBar::create(outlinesItems, barPosition, CCSize(356.0f, 220.0f), 1, true, rows);
    addChild(outlinesBar, 10);
    m_createButtonBars->addObject(outlinesBar);
    
    // ========================================================================
    // TAB 2: SLOPES
    // ========================================================================
    CCArray* slopesItems = CCArray::create();
    slopesItems->addObject(getCreateBtn(1743, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1744, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1745, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1746, 4)); // Slope
    slopesItems->addObject(getCreateBtn(305, 4));  // Slope
    slopesItems->addObject(getCreateBtn(307, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1747, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1748, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1749, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1750, 4)); // Slope
    slopesItems->addObject(getCreateBtn(371, 4));  // Slope
    slopesItems->addObject(getCreateBtn(372, 4));  // Slope
    slopesItems->addObject(getCreateBtn(373, 4));  // Slope
    slopesItems->addObject(getCreateBtn(374, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1906, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1907, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1908, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1909, 4)); // Slope
    slopesItems->addObject(getCreateBtn(294, 4));  // Slope
    slopesItems->addObject(getCreateBtn(295, 4));  // Slope
    slopesItems->addObject(getCreateBtn(296, 4));  // Slope
    slopesItems->addObject(getCreateBtn(297, 4));  // Slope
    slopesItems->addObject(getCreateBtn(326, 4));  // Slope
    slopesItems->addObject(getCreateBtn(327, 4));  // Slope
    slopesItems->addObject(getCreateBtn(328, 4));  // Slope
    slopesItems->addObject(getCreateBtn(329, 4));  // Slope
    slopesItems->addObject(getCreateBtn(349, 4));  // Slope
    slopesItems->addObject(getCreateBtn(351, 4));  // Slope
    slopesItems->addObject(getCreateBtn(762, 4));  // Slope
    slopesItems->addObject(getCreateBtn(763, 4));  // Slope
    slopesItems->addObject(getCreateBtn(764, 4));  // Slope
    slopesItems->addObject(getCreateBtn(765, 4));  // Slope
    slopesItems->addObject(getCreateBtn(766, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1033, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1034, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1035, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1036, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1041, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1042, 4)); // Slope
    slopesItems->addObject(getCreateBtn(771, 4));  // Slope
    slopesItems->addObject(getCreateBtn(772, 4));  // Slope
    slopesItems->addObject(getCreateBtn(773, 4));  // Slope
    slopesItems->addObject(getCreateBtn(774, 4));  // Slope
    slopesItems->addObject(getCreateBtn(775, 4));  // Slope
    slopesItems->addObject(getCreateBtn(960, 4));  // Slope
    slopesItems->addObject(getCreateBtn(961, 4));  // Slope
    slopesItems->addObject(getCreateBtn(964, 4));  // Slope
    slopesItems->addObject(getCreateBtn(965, 4));  // Slope
    slopesItems->addObject(getCreateBtn(966, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1037, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1038, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1039, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1040, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1043, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1044, 4)); // Slope
    slopesItems->addObject(getCreateBtn(969, 4));  // Slope
    slopesItems->addObject(getCreateBtn(970, 4));  // Slope
    slopesItems->addObject(getCreateBtn(971, 4));  // Slope
    slopesItems->addObject(getCreateBtn(972, 4));  // Slope
    slopesItems->addObject(getCreateBtn(973, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1305, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1306, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1307, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1308, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1309, 4)); // Slope
    slopesItems->addObject(getCreateBtn(681, 4));  // Slope
    slopesItems->addObject(getCreateBtn(682, 4));  // Slope
    slopesItems->addObject(getCreateBtn(683, 4));  // Slope
    slopesItems->addObject(getCreateBtn(684, 4));  // Slope
    slopesItems->addObject(getCreateBtn(685, 4));  // Slope
    slopesItems->addObject(getCreateBtn(686, 4));  // Slope
    slopesItems->addObject(getCreateBtn(687, 4));  // Slope
    slopesItems->addObject(getCreateBtn(688, 4));  // Slope
    slopesItems->addObject(getCreateBtn(689, 4));  // Slope
    slopesItems->addObject(getCreateBtn(690, 4));  // Slope
    slopesItems->addObject(getCreateBtn(691, 4));  // Slope
    slopesItems->addObject(getCreateBtn(692, 4));  // Slope
    slopesItems->addObject(getCreateBtn(324, 4));  // Slope
    slopesItems->addObject(getCreateBtn(325, 4));  // Slope
    slopesItems->addObject(getCreateBtn(358, 4));  // Slope
    slopesItems->addObject(getCreateBtn(693, 4));  // Slope
    slopesItems->addObject(getCreateBtn(694, 4));  // Slope
    slopesItems->addObject(getCreateBtn(695, 4));  // Slope
    slopesItems->addObject(getCreateBtn(696, 4));  // Slope
    slopesItems->addObject(getCreateBtn(697, 4));  // Slope
    slopesItems->addObject(getCreateBtn(698, 4));  // Slope
    slopesItems->addObject(getCreateBtn(699, 4));  // Slope
    slopesItems->addObject(getCreateBtn(700, 4));  // Slope
    slopesItems->addObject(getCreateBtn(701, 4));  // Slope
    slopesItems->addObject(getCreateBtn(702, 4));  // Slope
    slopesItems->addObject(getCreateBtn(703, 4));  // Slope
    slopesItems->addObject(getCreateBtn(704, 4));  // Slope
    slopesItems->addObject(getCreateBtn(705, 4));  // Slope
    slopesItems->addObject(getCreateBtn(706, 4));  // Slope
    slopesItems->addObject(getCreateBtn(707, 4));  // Slope
    slopesItems->addObject(getCreateBtn(708, 4));  // Slope
    slopesItems->addObject(getCreateBtn(713, 4));  // Slope
    slopesItems->addObject(getCreateBtn(714, 4));  // Slope
    slopesItems->addObject(getCreateBtn(730, 4));  // Slope
    slopesItems->addObject(getCreateBtn(731, 4));  // Slope
    slopesItems->addObject(getCreateBtn(715, 4));  // Slope
    slopesItems->addObject(getCreateBtn(716, 4));  // Slope
    slopesItems->addObject(getCreateBtn(732, 4));  // Slope
    slopesItems->addObject(getCreateBtn(733, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1187, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1188, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1189, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1190, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1325, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1326, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1198, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1199, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1200, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1201, 4)); // Slope
    slopesItems->addObject(getCreateBtn(826, 4));  // Slope
    slopesItems->addObject(getCreateBtn(827, 4));  // Slope
    slopesItems->addObject(getCreateBtn(828, 4));  // Slope
    slopesItems->addObject(getCreateBtn(829, 4));  // Slope
    slopesItems->addObject(getCreateBtn(830, 4));  // Slope
    slopesItems->addObject(getCreateBtn(831, 4));  // Slope
    slopesItems->addObject(getCreateBtn(832, 4));  // Slope
    slopesItems->addObject(getCreateBtn(833, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1091, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1107, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1092, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1108, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1093, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1094, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1109, 4)); // Slope
    slopesItems->addObject(getCreateBtn(877, 4));  // Slope
    slopesItems->addObject(getCreateBtn(878, 4));  // Slope
    slopesItems->addObject(getCreateBtn(888, 4));  // Slope
    slopesItems->addObject(getCreateBtn(889, 4));  // Slope
    slopesItems->addObject(getCreateBtn(895, 4));  // Slope
    slopesItems->addObject(getCreateBtn(896, 4));  // Slope
    slopesItems->addObject(getCreateBtn(1256, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1257, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1258, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1259, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1014, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1015, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1016, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1017, 4)); // Slope
    slopesItems->addObject(getCreateBtn(1018, 4)); // Slope
    
    EditButtonBar* slopesBar = EditButtonBar::create(slopesItems, barPosition, CCSize(356.0f, 220.0f), 2, true, rows);
    addChild(slopesBar, 10);
    m_createButtonBars->addObject(slopesBar);
    
    // ========================================================================
    // TAB 3: HAZARDS (Spikes)
    // ========================================================================
    CCArray* hazardsItems = CCArray::create();
    hazardsItems->addObject(getCreateBtn(8, 4));    // Spike
    hazardsItems->addObject(getCreateBtn(39, 4));   // Spike
    hazardsItems->addObject(getCreateBtn(103, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(392, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(216, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(217, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(218, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(458, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(144, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(205, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(145, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(459, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(177, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(178, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(179, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(1889, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1890, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1891, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1892, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1715, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1719, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1720, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1721, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(135, 4));  // Spike
    hazardsItems->addObject(getCreateBtn(1711, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1712, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1713, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1714, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1717, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1718, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1716, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1723, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1724, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1722, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1725, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1726, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1727, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1728, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1729, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1730, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1731, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1732, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(1733, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(3610, 4)); // Spike
    hazardsItems->addObject(getCreateBtn(3611, 4)); // Spike
    
    EditButtonBar* hazardsBar = EditButtonBar::create(hazardsItems, barPosition, CCSize(356.0f, 220.0f), 3, true, rows);
    addChild(hazardsBar, 10);
    m_createButtonBars->addObject(hazardsBar);
    
    // ========================================================================
    // TAB 4: 3D BLOCKS
    // ========================================================================
    CCArray* threeDItems = CCArray::create();
    threeDItems->addObject(getCreateBtn(506, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(507, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(508, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(509, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(510, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(511, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(512, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(513, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(514, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(515, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(516, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(517, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(518, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(519, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(520, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(521, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(522, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(523, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(524, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(525, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(526, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(527, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(528, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(529, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(530, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(531, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(532, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(533, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(534, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(535, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(536, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(537, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(538, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(539, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(540, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(541, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(542, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(543, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(544, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(545, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(546, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(547, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(548, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(549, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(550, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(551, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(552, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(553, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(554, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(555, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(556, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(557, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(558, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(559, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(560, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(561, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(562, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(563, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(564, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(565, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(566, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(567, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(568, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(569, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(570, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(571, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(572, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(573, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(574, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(575, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(576, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(577, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(578, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(579, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(580, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(581, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(582, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(583, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(584, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(585, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(586, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(587, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(588, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(589, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(590, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(591, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(592, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(593, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(594, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(595, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(596, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(597, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(598, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(599, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(600, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(601, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(602, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(603, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(604, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(605, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(606, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(607, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(608, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(609, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(610, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(611, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(612, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(613, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(614, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(615, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(616, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(617, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(618, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(619, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(620, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(621, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(622, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(623, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(624, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(625, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(626, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(627, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(628, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(629, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(630, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(631, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(632, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(633, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(634, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(635, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(636, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(637, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(638, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(639, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(640, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(943, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(944, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(945, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(946, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(947, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(948, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(949, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(950, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(951, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(1024, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1025, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1026, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1027, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1028, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1029, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1030, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1031, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1032, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(980, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(981, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(982, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(983, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(984, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(985, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(986, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(987, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(988, 4));  // 3D Block
    threeDItems->addObject(getCreateBtn(1063, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1064, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1065, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1066, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1067, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1068, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1069, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1070, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1071, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1552, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1553, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1554, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1555, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1556, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1557, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1558, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1559, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1560, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1529, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1530, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1531, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1532, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1533, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1534, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1535, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1536, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1537, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1538, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1539, 4)); // 3D Block
    threeDItems->addObject(getCreateBtn(1540, 4)); // 3D Block
    
    EditButtonBar* threeDBar = EditButtonBar::create(threeDItems, barPosition, CCSize(356.0f, 220.0f), 4, true, rows);
    addChild(threeDBar, 10);
    m_createButtonBars->addObject(threeDBar);
    
    // ========================================================================
    // TAB 5: ORBS & PORTALS
    // ========================================================================
    CCArray* orbsItems = CCArray::create();
    orbsItems->addObject(getCreateBtn(35, 4));     // Orb
    orbsItems->addObject(getCreateBtn(140, 4));    // Orb
    orbsItems->addObject(getCreateBtn(1332, 4));   // Orb
    orbsItems->addObject(getCreateBtn(67, 4));     // Portal
    orbsItems->addObject(getCreateBtn(3005, 4));   // Portal
    orbsItems->addObject(getCreateBtn(36, 4));     // Orb
    orbsItems->addObject(getCreateBtn(141, 4));    // Orb
    orbsItems->addObject(getCreateBtn(1333, 4));   // Orb
    orbsItems->addObject(getCreateBtn(84, 4));     // Portal
    orbsItems->addObject(getCreateBtn(1022, 4));   // Portal
    orbsItems->addObject(getCreateBtn(1330, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1704, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1751, 4));   // Orb
    orbsItems->addObject(getCreateBtn(3004, 4));   // Orb
    orbsItems->addObject(getCreateBtn(3027, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1594, 4));   // Orb
    orbsItems->addObject(getCreateBtn(10, 4));     // Portal
    orbsItems->addObject(getCreateBtn(11, 4));     // Portal
    orbsItems->addObject(getCreateBtn(2926, 4));   // Portal
    orbsItems->addObject(getCreateBtn(12, 4));     // Portal
    orbsItems->addObject(getCreateBtn(13, 4));     // Portal
    orbsItems->addObject(getCreateBtn(47, 4));     // Portal
    orbsItems->addObject(getCreateBtn(111, 4));    // Portal
    orbsItems->addObject(getCreateBtn(660, 4));    // Portal
    orbsItems->addObject(getCreateBtn(745, 4));    // Portal
    orbsItems->addObject(getCreateBtn(45, 4));     // Portal
    orbsItems->addObject(getCreateBtn(46, 4));     // Portal
    orbsItems->addObject(getCreateBtn(99, 4));     // Portal
    orbsItems->addObject(getCreateBtn(101, 4));    // Portal
    orbsItems->addObject(getCreateBtn(286, 4));    // Portal
    orbsItems->addObject(getCreateBtn(287, 4));    // Portal
    orbsItems->addObject(getCreateBtn(747, 4));    // Portal
    orbsItems->addObject(getCreateBtn(2902, 4));   // Portal
    orbsItems->addObject(getCreateBtn(2064, 4));   // Portal
    orbsItems->addObject(getCreateBtn(1331, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1933, 4));   // Portal
    orbsItems->addObject(getCreateBtn(200, 4));    // Portal
    orbsItems->addObject(getCreateBtn(201, 4));    // Portal
    orbsItems->addObject(getCreateBtn(202, 4));    // Portal
    orbsItems->addObject(getCreateBtn(203, 4));    // Portal
    orbsItems->addObject(getCreateBtn(1334, 4));   // Orb
    orbsItems->addObject(getCreateBtn(2063, 4));   // Portal
    orbsItems->addObject(getCreateBtn(914, 4));    // Portal
    orbsItems->addObject(getCreateBtn(1755, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1813, 4));   // Portal
    orbsItems->addObject(getCreateBtn(1829, 4));   // Orb
    orbsItems->addObject(getCreateBtn(1859, 4));   // Orb
    orbsItems->addObject(getCreateBtn(2069, 4));   // Portal
    orbsItems->addObject(getCreateBtn(3645, 4));   // Portal
    orbsItems->addObject(getCreateBtn(2866, 4));   // Portal
    
    EditButtonBar* orbsBar = EditButtonBar::create(orbsItems, barPosition, CCSize(356.0f, 220.0f), 5, true, rows);
    addChild(orbsBar, 10);
    m_createButtonBars->addObject(orbsBar);
    
    // ========================================================================
    // TAB 6: MONSTERS
    // ========================================================================
    CCArray* monstersItems = CCArray::create();
    monstersItems->addObject(getCreateBtn(2065, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1586, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1700, 4)); // Monster
    monstersItems->addObject(getCreateBtn(918, 4));  // Monster
    monstersItems->addObject(getCreateBtn(1327, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1328, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1584, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2012, 4)); // Monster
    monstersItems->addObject(getCreateBtn(919, 4));  // Monster
    monstersItems->addObject(getCreateBtn(1050, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1051, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1052, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3000, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3001, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3002, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1228, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1053, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1054, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1592, 4)); // Monster
    monstersItems->addObject(getCreateBtn(920, 4));  // Monster
    monstersItems->addObject(getCreateBtn(923, 4));  // Monster
    monstersItems->addObject(getCreateBtn(924, 4));  // Monster
    monstersItems->addObject(getCreateBtn(921, 4));  // Monster
    monstersItems->addObject(getCreateBtn(1582, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1583, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1591, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1593, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1849, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1850, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1851, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1852, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1853, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1854, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1855, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1858, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1856, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1697, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1698, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1699, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1857, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1860, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1516, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1518, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1517, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1519, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1618, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1839, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1840, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1841, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1842, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1936, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1937, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1938, 4)); // Monster
    monstersItems->addObject(getCreateBtn(1939, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2020, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2021, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2022, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2023, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2024, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2025, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2026, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2027, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2028, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2029, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2030, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2031, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2032, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2033, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2034, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2035, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2036, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2037, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2038, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2039, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2040, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2041, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2042, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2043, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2044, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2045, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2046, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2047, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2048, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2049, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2050, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2051, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2052, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2053, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2054, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2055, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2864, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2865, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2867, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2868, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2869, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2870, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2871, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2872, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2873, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2874, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2875, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2876, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2877, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2878, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2879, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2880, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2881, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2882, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2883, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2884, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2885, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2886, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2887, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2888, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2889, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2890, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2891, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2892, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2893, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2894, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2222, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2223, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2245, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2246, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2605, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2606, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2629, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2630, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2694, 4)); // Monster
    monstersItems->addObject(getCreateBtn(2695, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3119, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3120, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3121, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3219, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3302, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3303, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3304, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3305, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3482, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3483, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3484, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3485, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3486, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3487, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3488, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3489, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3490, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3491, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3492, 4)); // Monster
    monstersItems->addObject(getCreateBtn(3493, 4)); // Monster
    monstersItems->addObject(getCreateBtn(4211, 4)); // Monster
    monstersItems->addObject(getCreateBtn(4300, 4)); // Monster
    
    EditButtonBar* monstersBar = EditButtonBar::create(monstersItems, barPosition, CCSize(356.0f, 220.0f), 6, true, rows);
    addChild(monstersBar, 10);
    m_createButtonBars->addObject(monstersBar);
    
    // ========================================================================
    // TAB 7: PIXEL ART
    // ========================================================================
    CCArray* pixelArtItems = CCArray::create();
    pixelArtItems->addObject(getCreateBtn(2083, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2084, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2085, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2086, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2087, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2088, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2089, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2078, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2079, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2080, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2081, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2082, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2093, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2094, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2095, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2096, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2120, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2121, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2097, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2070, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2071, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2072, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2073, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2074, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2075, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2076, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2092, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2077, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2090, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2091, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2098, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2099, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2100, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2101, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2102, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2103, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2104, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2105, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2106, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2107, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2457, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2458, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2108, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2109, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2110, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2111, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2456, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2112, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2113, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2114, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2115, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2116, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2117, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2118, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2119, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2122, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2123, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2125, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2124, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2126, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2127, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2128, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2129, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2130, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2131, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2132, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2133, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2134, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2135, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2136, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2137, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2138, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2139, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2140, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2141, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2142, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2143, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2144, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2145, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2146, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2147, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2148, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2149, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2150, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2151, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2152, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2153, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2626, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2154, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2155, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2156, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2157, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2158, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2159, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2160, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2161, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2162, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2163, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2164, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2165, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2166, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2167, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2168, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2169, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2170, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2171, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2172, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2173, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2174, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2175, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2176, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2177, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2178, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2179, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2180, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2181, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2182, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2183, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2184, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2185, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2186, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2187, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2188, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2189, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2190, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2191, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2192, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2193, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2194, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2195, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2196, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2197, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2198, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2199, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2200, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2201, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2202, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2203, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2204, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2205, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2206, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2207, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2208, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2209, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2210, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2211, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2212, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2213, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2214, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2215, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2216, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2217, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2218, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2219, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2220, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2221, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2224, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2225, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2226, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2227, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2228, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2229, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2230, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2231, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2232, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2233, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2234, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2235, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2236, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2237, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2238, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3540, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3541, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3542, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3543, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2239, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2240, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2241, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2242, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2243, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2244, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2249, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2250, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2251, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2252, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2253, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2254, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2255, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2256, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2257, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2258, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2259, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2260, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2261, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2262, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2263, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2264, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2265, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2266, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2267, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2268, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2269, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2270, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2271, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2272, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2273, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2274, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2275, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2276, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2277, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2278, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2279, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2280, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2281, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2282, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2283, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2284, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2285, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2286, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2287, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2288, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2289, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2290, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2291, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2292, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2293, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2294, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2295, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2296, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2297, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2298, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2299, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2300, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2301, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2302, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2303, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2304, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2305, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2306, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2307, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2308, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2309, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2310, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2311, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2312, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2313, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2314, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2315, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2316, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2317, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2318, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2319, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2320, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2321, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2322, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2323, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2324, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2325, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2326, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2327, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2328, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2329, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2330, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2331, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2332, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2333, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2334, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2335, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2336, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2337, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2338, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2339, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2340, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2341, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2342, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2343, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2344, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2345, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2346, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2347, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2348, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2349, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2350, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2351, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2352, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2353, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2354, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2355, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2356, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2357, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2358, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2359, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2360, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2361, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2362, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2363, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2364, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2365, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2366, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2367, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2368, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2369, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2370, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2371, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2372, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2373, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2633, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2374, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2375, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2376, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2377, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2378, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2379, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2380, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2384, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2381, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2382, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2383, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2385, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2386, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2387, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2388, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2389, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2390, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2391, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2392, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2634, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2635, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2636, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2393, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2394, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2395, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2396, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2397, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2398, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2399, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2400, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2401, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2402, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2403, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2404, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2405, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2406, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2407, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2408, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2409, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2410, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2411, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2412, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2413, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2414, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2627, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2696, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2697, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2698, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2699, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2700, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2415, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2628, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2416, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2417, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2418, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2419, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2420, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2421, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2422, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2423, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2424, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2425, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2426, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2427, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2428, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2429, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2430, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2462, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4368, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2463, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4369, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2464, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4370, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2465, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4371, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2466, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2467, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4372, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2468, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4373, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2469, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4374, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2470, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4375, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2471, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4376, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2472, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4377, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2473, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4378, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2474, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4379, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2475, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2476, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2477, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2478, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2479, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2480, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2481, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2482, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2483, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2484, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2485, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2486, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2487, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2488, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4380, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2489, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4381, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2490, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4382, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2491, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4383, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2492, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2493, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2494, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2495, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2496, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2497, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2498, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2499, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2500, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2501, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2502, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2503, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2504, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2505, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2506, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2507, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2508, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2509, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2510, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2511, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2512, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2513, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2531, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2514, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2515, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2516, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2517, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2518, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2519, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2520, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2521, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2522, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2523, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2524, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2525, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2526, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2527, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2528, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2529, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2530, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2534, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2535, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2536, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2537, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2538, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2539, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2540, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2541, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2542, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2543, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2544, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2545, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2546, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2547, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2548, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2549, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2550, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2551, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2552, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2553, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2554, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2631, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2555, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2556, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2557, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2558, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2559, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2560, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2632, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2561, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2562, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2563, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2564, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2565, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2566, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2567, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2568, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2569, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2570, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2571, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2572, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2573, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2574, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2575, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2576, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2577, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2578, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2579, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2580, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2581, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2582, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2583, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2584, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2585, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2586, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2587, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2588, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2589, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2590, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2591, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2592, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2593, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2594, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2595, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2596, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2597, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2598, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2599, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2600, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2601, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2602, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2603, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2604, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2693, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2607, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2608, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2609, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2610, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2611, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2612, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2613, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2614, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2615, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2616, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2617, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2618, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2619, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2620, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2621, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2622, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2623, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2624, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2625, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2637, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2638, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2639, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2640, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2641, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2642, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2643, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2644, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2645, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2646, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2647, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2648, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2649, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2650, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2651, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2652, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2653, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2654, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2655, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2656, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2657, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2658, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2659, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2660, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2661, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2662, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2663, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2664, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2665, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2666, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2667, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2668, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2669, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2670, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2671, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2672, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2673, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2674, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2675, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2676, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2677, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2678, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2679, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2680, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2681, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2682, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2683, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2684, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2685, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2686, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2687, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2688, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2689, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2690, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2691, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2692, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3101, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3102, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3103, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3104, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3105, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3106, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3107, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3108, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3109, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3110, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3111, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3112, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3113, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3114, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3115, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3116, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3117, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3118, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3122, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3123, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3124, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3125, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3126, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3127, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3128, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3129, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3130, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3131, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3132, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3133, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3134, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3135, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3136, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3137, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3138, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3139, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3140, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3141, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3142, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3143, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3144, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3145, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3146, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3147, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3148, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3149, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3150, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3151, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3152, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3153, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3154, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3155, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3156, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3157, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3158, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3159, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3160, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3161, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3162, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3163, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3164, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3165, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3166, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3167, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3168, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3169, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3170, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3171, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3172, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3173, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3174, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3175, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3176, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3177, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3178, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3179, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3180, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3181, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3182, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3183, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3184, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3185, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3186, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3187, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3188, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3189, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3190, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3191, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3192, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3193, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3194, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3195, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3196, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3197, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3198, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3199, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3200, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3201, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3202, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3203, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3204, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3205, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3206, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3207, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3208, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3209, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3210, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3211, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3212, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3213, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3214, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3215, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3216, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3217, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3218, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3220, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3221, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3222, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3223, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3224, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3225, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3226, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3227, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3228, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3229, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3230, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3231, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3232, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3233, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3234, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3235, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3236, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3237, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3238, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3239, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3240, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3241, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3242, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3243, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3244, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3245, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3246, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3247, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3248, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3249, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3250, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3251, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3252, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3253, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3254, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3255, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3256, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3257, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3258, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3259, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3260, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3261, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3262, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3263, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3264, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3265, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3266, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3267, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3268, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3269, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3270, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3271, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3272, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3273, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3274, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3275, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3276, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3277, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3278, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3279, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3280, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3281, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3282, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3283, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3284, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3285, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3286, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3287, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3288, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3289, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3290, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3291, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3292, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3293, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3294, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3295, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3296, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3297, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3298, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3299, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3300, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3301, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3306, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3307, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3308, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3309, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3310, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3311, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3312, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3318, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3319, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3320, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3321, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3322, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3323, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3324, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3325, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3313, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3314, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3315, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3316, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3317, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3326, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3327, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3328, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3329, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3330, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3331, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3332, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3333, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3335, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3336, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3337, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3338, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3339, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3340, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3341, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3342, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3343, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3344, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3345, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3346, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3347, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3348, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3349, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3350, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3351, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3352, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3353, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3354, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3355, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3356, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3357, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3358, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3359, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3360, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3361, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3362, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3363, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3364, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3365, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3366, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3367, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3368, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3369, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3370, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3371, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3372, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3373, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3374, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3375, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3376, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3377, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3378, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3379, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3380, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3381, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3382, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3383, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3384, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3385, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3386, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3387, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3388, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3389, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3390, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3391, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3392, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3393, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3394, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3395, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3396, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3397, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3398, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3399, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3400, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3401, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3402, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3403, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3404, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3405, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3406, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3407, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3408, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3409, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3410, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3411, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3412, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3413, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3414, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3415, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3416, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3417, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3418, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3419, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3420, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3421, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3422, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3423, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3424, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3425, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3426, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3427, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3428, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3429, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3430, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3431, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3432, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3433, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3434, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3435, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3436, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3437, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3438, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3439, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3440, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3441, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3442, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3443, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3444, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3445, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3446, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3447, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3448, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3449, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3450, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3334, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3451, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3538, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3452, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3453, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3539, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3454, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3455, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3456, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3457, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3458, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3459, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3460, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3461, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3462, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3463, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3464, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3465, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3466, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3467, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3468, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3469, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3470, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3471, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3472, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3473, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3474, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3475, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3476, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3477, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3478, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3479, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3480, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3481, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3494, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3495, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3496, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3497, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3498, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3499, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3500, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3501, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3502, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3503, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3504, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3505, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3506, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3507, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3508, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3509, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3510, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3511, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3512, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3513, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3514, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3515, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3516, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3517, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3518, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3519, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3520, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3521, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3522, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3523, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3524, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3525, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3526, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3527, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3528, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3529, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3530, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3531, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3532, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3533, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3534, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3535, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3536, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3537, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3544, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3545, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3546, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3547, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3548, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3549, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3550, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3551, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3552, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3553, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3554, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3555, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3556, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3557, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3558, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3559, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3560, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3561, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3562, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3563, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3564, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3565, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3566, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3567, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3568, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3569, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3570, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3571, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3572, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3573, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3574, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3575, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3576, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3577, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3578, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3579, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3580, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3581, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3582, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3583, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3584, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3585, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3586, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3587, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3588, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3589, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3590, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3591, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3592, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3593, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3594, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3595, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3596, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3597, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3598, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3599, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3700, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3701, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3702, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3703, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3704, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3705, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3706, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3707, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3708, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3709, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3710, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3711, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3712, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3713, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3714, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3715, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3716, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3717, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3718, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3719, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3720, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3721, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3722, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3723, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3724, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3725, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3726, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3727, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3728, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3729, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3730, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3731, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3732, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3733, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3734, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3735, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3736, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3737, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3738, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3739, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3740, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3741, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3742, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3743, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3744, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3745, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3746, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3747, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3748, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3749, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3750, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3751, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3752, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3753, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3754, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3755, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3756, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3757, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3758, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3759, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3760, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3761, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3762, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3763, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3764, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3765, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3766, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3767, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3768, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3769, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3770, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3771, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3772, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3773, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3774, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3775, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3776, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3777, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3778, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3779, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3780, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3781, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3782, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3783, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3784, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3785, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3786, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3787, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3788, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3789, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3790, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3791, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3792, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4000, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4004, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4001, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4005, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4002, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4006, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4003, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4007, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2452, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2453, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4384, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4385, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4008, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4010, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4009, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4011, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2454, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2455, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4012, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4013, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4014, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4015, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4016, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4017, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4018, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4019, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4020, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4021, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4022, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4023, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4024, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4030, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4031, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4025, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4032, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4033, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4026, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2434, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2435, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4027, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2436, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2437, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4028, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2438, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2439, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4029, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4214, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2440, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4215, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2441, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4216, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2442, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4217, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2443, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4218, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2444, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2451, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4219, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2445, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4220, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2446, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4221, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4222, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2447, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4223, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2448, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4224, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2449, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4225, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2450, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4034, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4035, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4036, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4037, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4038, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4039, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4040, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4041, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4042, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4043, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4044, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4045, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4046, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4047, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4048, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4049, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4050, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4051, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4052, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4053, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4054, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4055, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4056, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4057, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4058, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4059, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4062, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4063, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4064, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4065, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4066, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4067, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4068, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4069, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4070, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4071, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4072, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4073, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4074, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4075, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4076, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4234, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4235, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4236, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4237, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4077, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4078, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4079, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4080, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4081, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4082, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4083, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4084, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4085, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4086, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4087, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4088, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4089, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4090, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4091, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4092, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4093, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4094, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4095, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4096, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4097, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4098, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4099, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4100, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4101, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4102, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4103, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4104, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4105, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4106, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4107, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4108, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4109, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4110, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4111, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4112, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4113, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4114, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4115, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4116, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4117, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4118, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4119, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4120, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4121, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4123, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4124, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4125, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4126, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4127, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4128, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4129, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4130, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4131, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4132, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4133, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4134, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4135, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4136, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4137, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4138, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4139, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4140, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4141, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4142, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4143, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4144, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4145, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4146, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4147, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4148, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4149, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4150, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4151, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4152, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4153, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4154, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4155, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4156, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4157, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4158, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4159, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4160, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4161, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4162, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4163, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4164, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4165, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4166, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4167, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4168, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4169, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4170, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4171, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4172, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4173, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4174, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4175, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4176, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4177, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4178, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4179, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4180, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4181, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4182, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4183, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4184, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4185, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4186, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4187, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4188, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4189, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4190, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4191, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4192, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4193, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4194, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4195, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4196, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4197, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4198, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4199, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4200, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4201, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4202, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4203, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4204, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4205, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4206, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4207, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4208, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4209, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4210, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4212, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4213, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4226, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4227, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4228, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4229, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4230, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4231, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4232, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4233, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2532, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2533, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4299, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4122, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3793, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3794, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3795, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3796, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3797, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3798, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3799, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4238, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4239, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4240, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4241, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4242, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4243, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4244, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4245, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4246, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4247, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4248, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4249, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4250, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4251, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4252, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4253, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4254, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4256, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4257, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4258, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4255, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4264, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4259, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4260, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4261, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4060, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4061, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4262, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4263, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4265, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4266, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4267, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4268, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4269, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4270, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4271, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2247, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2248, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4272, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4273, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2433, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4274, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4275, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4276, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4277, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4278, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4279, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4280, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4281, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4282, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4283, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4284, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4285, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2431, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(2432, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4286, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4287, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4288, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4289, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4290, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4291, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4292, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4293, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4294, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4295, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4296, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4297, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4298, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4301, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4302, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4303, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4304, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4305, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4306, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4307, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4308, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4309, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4310, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4311, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4312, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4313, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4314, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4315, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4316, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4317, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4318, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4319, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4320, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4321, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4322, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4323, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4324, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4325, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4326, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4327, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4328, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4329, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4330, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4331, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4332, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4333, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4334, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4335, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4336, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4337, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4338, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4339, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4340, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4341, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4342, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4343, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4344, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4345, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4346, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4347, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4348, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4349, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4350, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4351, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4352, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4353, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4354, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4355, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4356, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4357, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4358, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4359, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4360, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4361, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4362, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4363, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4364, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4365, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4366, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(4367, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3092, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3093, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3094, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3095, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3096, 4)); // Pixel Art
    pixelArtItems->addObject(getCreateBtn(3097, 4)); // Pixel Art
    
    EditButtonBar* pixelArtBar = EditButtonBar::create(pixelArtItems, barPosition, CCSize(356.0f, 220.0f), 7, true, rows);
    addChild(pixelArtBar, 10);
    m_createButtonBars->addObject(pixelArtBar);
    
    // ========================================================================
    // TAB 8: PARTICLES
    // ========================================================================
    CCArray* particleItems = CCArray::create();
    particleItems->addObject(getCreateBtn(3801, 4)); // Particle
    particleItems->addObject(getCreateBtn(3802, 4)); // Particle
    particleItems->addObject(getCreateBtn(3803, 4)); // Particle
    particleItems->addObject(getCreateBtn(3804, 4)); // Particle
    particleItems->addObject(getCreateBtn(3805, 4)); // Particle
    particleItems->addObject(getCreateBtn(3806, 4)); // Particle
    particleItems->addObject(getCreateBtn(3807, 4)); // Particle
    particleItems->addObject(getCreateBtn(3808, 4)); // Particle
    particleItems->addObject(getCreateBtn(3809, 4)); // Particle
    particleItems->addObject(getCreateBtn(3810, 4)); // Particle
    particleItems->addObject(getCreateBtn(3811, 4)); // Particle
    particleItems->addObject(getCreateBtn(3812, 4)); // Particle
    particleItems->addObject(getCreateBtn(3813, 4)); // Particle
    particleItems->addObject(getCreateBtn(3814, 4)); // Particle
    particleItems->addObject(getCreateBtn(3815, 4)); // Particle
    particleItems->addObject(getCreateBtn(3816, 4)); // Particle
    particleItems->addObject(getCreateBtn(3817, 4)); // Particle
    particleItems->addObject(getCreateBtn(3818, 4)); // Particle
    particleItems->addObject(getCreateBtn(3819, 4)); // Particle
    particleItems->addObject(getCreateBtn(3820, 4)); // Particle
    particleItems->addObject(getCreateBtn(3821, 4)); // Particle
    particleItems->addObject(getCreateBtn(3822, 4)); // Particle
    particleItems->addObject(getCreateBtn(3823, 4)); // Particle
    particleItems->addObject(getCreateBtn(3824, 4)); // Particle
    particleItems->addObject(getCreateBtn(3825, 4)); // Particle
    particleItems->addObject(getCreateBtn(3826, 4)); // Particle
    particleItems->addObject(getCreateBtn(3827, 4)); // Particle
    particleItems->addObject(getCreateBtn(3828, 4)); // Particle
    particleItems->addObject(getCreateBtn(3829, 4)); // Particle
    particleItems->addObject(getCreateBtn(3830, 4)); // Particle
    particleItems->addObject(getCreateBtn(3831, 4)); // Particle
    particleItems->addObject(getCreateBtn(3832, 4)); // Particle
    particleItems->addObject(getCreateBtn(3833, 4)); // Particle
    particleItems->addObject(getCreateBtn(3834, 4)); // Particle
    particleItems->addObject(getCreateBtn(3835, 4)); // Particle
    particleItems->addObject(getCreateBtn(3836, 4)); // Particle
    particleItems->addObject(getCreateBtn(3837, 4)); // Particle
    particleItems->addObject(getCreateBtn(3838, 4)); // Particle
    particleItems->addObject(getCreateBtn(3839, 4)); // Particle
    particleItems->addObject(getCreateBtn(3840, 4)); // Particle
    particleItems->addObject(getCreateBtn(3841, 4)); // Particle
    particleItems->addObject(getCreateBtn(3842, 4)); // Particle
    particleItems->addObject(getCreateBtn(3843, 4)); // Particle
    particleItems->addObject(getCreateBtn(3844, 4)); // Particle
    particleItems->addObject(getCreateBtn(3845, 4)); // Particle
    particleItems->addObject(getCreateBtn(3846, 4)); // Particle
    particleItems->addObject(getCreateBtn(3847, 4)); // Particle
    particleItems->addObject(getCreateBtn(3848, 4)); // Particle
    particleItems->addObject(getCreateBtn(3849, 4)); // Particle
    particleItems->addObject(getCreateBtn(3850, 4)); // Particle
    particleItems->addObject(getCreateBtn(3851, 4)); // Particle
    particleItems->addObject(getCreateBtn(3852, 4)); // Particle
    particleItems->addObject(getCreateBtn(3853, 4)); // Particle
    particleItems->addObject(getCreateBtn(3854, 4)); // Particle
    particleItems->addObject(getCreateBtn(3855, 4)); // Particle
    particleItems->addObject(getCreateBtn(3856, 4)); // Particle
    particleItems->addObject(getCreateBtn(3857, 4)); // Particle
    particleItems->addObject(getCreateBtn(3858, 4)); // Particle
    particleItems->addObject(getCreateBtn(3859, 4)); // Particle
    particleItems->addObject(getCreateBtn(3860, 4)); // Particle
    particleItems->addObject(getCreateBtn(3861, 4)); // Particle
    particleItems->addObject(getCreateBtn(3862, 4)); // Particle
    particleItems->addObject(getCreateBtn(3863, 4)); // Particle
    particleItems->addObject(getCreateBtn(3864, 4)); // Particle
    particleItems->addObject(getCreateBtn(3865, 4)); // Particle
    particleItems->addObject(getCreateBtn(3866, 4)); // Particle
    particleItems->addObject(getCreateBtn(3867, 4)); // Particle
    particleItems->addObject(getCreateBtn(3868, 4)); // Particle
    particleItems->addObject(getCreateBtn(3869, 4)); // Particle
    particleItems->addObject(getCreateBtn(3870, 4)); // Particle
    particleItems->addObject(getCreateBtn(3871, 4)); // Particle
    particleItems->addObject(getCreateBtn(3872, 4)); // Particle
    particleItems->addObject(getCreateBtn(3873, 4)); // Particle
    particleItems->addObject(getCreateBtn(3874, 4)); // Particle
    particleItems->addObject(getCreateBtn(3875, 4)); // Particle
    particleItems->addObject(getCreateBtn(3876, 4)); // Particle
    particleItems->addObject(getCreateBtn(3877, 4)); // Particle
    particleItems->addObject(getCreateBtn(3878, 4)); // Particle
    particleItems->addObject(getCreateBtn(3879, 4)); // Particle
    particleItems->addObject(getCreateBtn(3880, 4)); // Particle
    particleItems->addObject(getCreateBtn(3881, 4)); // Particle
    particleItems->addObject(getCreateBtn(3882, 4)); // Particle
    particleItems->addObject(getCreateBtn(3883, 4)); // Particle
    particleItems->addObject(getCreateBtn(3884, 4)); // Particle
    particleItems->addObject(getCreateBtn(3885, 4)); // Particle
    particleItems->addObject(getCreateBtn(3886, 4)); // Particle
    particleItems->addObject(getCreateBtn(3887, 4)); // Particle
    particleItems->addObject(getCreateBtn(3888, 4)); // Particle
    particleItems->addObject(getCreateBtn(3889, 4)); // Particle
    particleItems->addObject(getCreateBtn(3890, 4)); // Particle
    particleItems->addObject(getCreateBtn(3891, 4)); // Particle
    particleItems->addObject(getCreateBtn(3892, 4)); // Particle
    particleItems->addObject(getCreateBtn(3893, 4)); // Particle
    particleItems->addObject(getCreateBtn(3894, 4)); // Particle
    particleItems->addObject(getCreateBtn(3895, 4)); // Particle
    particleItems->addObject(getCreateBtn(3896, 4)); // Particle
    particleItems->addObject(getCreateBtn(3897, 4)); // Particle
    particleItems->addObject(getCreateBtn(3898, 4)); // Particle
    particleItems->addObject(getCreateBtn(3899, 4)); // Particle
    particleItems->addObject(getCreateBtn(3900, 4)); // Particle
    particleItems->addObject(getCreateBtn(3901, 4)); // Particle
    particleItems->addObject(getCreateBtn(3902, 4)); // Particle
    particleItems->addObject(getCreateBtn(3903, 4)); // Particle
    particleItems->addObject(getCreateBtn(3904, 4)); // Particle
    particleItems->addObject(getCreateBtn(3905, 4)); // Particle
    particleItems->addObject(getCreateBtn(3906, 4)); // Particle
    particleItems->addObject(getCreateBtn(3907, 4)); // Particle
    particleItems->addObject(getCreateBtn(3908, 4)); // Particle
    particleItems->addObject(getCreateBtn(3909, 4)); // Particle
    particleItems->addObject(getCreateBtn(3910, 4)); // Particle
    particleItems->addObject(getCreateBtn(3911, 4)); // Particle
    particleItems->addObject(getCreateBtn(3912, 4)); // Particle
    particleItems->addObject(getCreateBtn(3913, 4)); // Particle
    particleItems->addObject(getCreateBtn(3914, 4)); // Particle
    particleItems->addObject(getCreateBtn(3915, 4)); // Particle
    particleItems->addObject(getCreateBtn(3916, 4)); // Particle
    particleItems->addObject(getCreateBtn(3917, 4)); // Particle
    particleItems->addObject(getCreateBtn(3918, 4)); // Particle
    particleItems->addObject(getCreateBtn(3919, 4)); // Particle
    particleItems->addObject(getCreateBtn(3920, 4)); // Particle
    particleItems->addObject(getCreateBtn(3921, 4)); // Particle
    particleItems->addObject(getCreateBtn(3922, 4)); // Particle
    particleItems->addObject(getCreateBtn(3923, 4)); // Particle
    particleItems->addObject(getCreateBtn(3924, 4)); // Particle
    particleItems->addObject(getCreateBtn(3925, 4)); // Particle
    particleItems->addObject(getCreateBtn(3926, 4)); // Particle
    particleItems->addObject(getCreateBtn(3927, 4)); // Particle
    particleItems->addObject(getCreateBtn(3928, 4)); // Particle
    particleItems->addObject(getCreateBtn(3929, 4)); // Particle
    particleItems->addObject(getCreateBtn(3930, 4)); // Particle
    particleItems->addObject(getCreateBtn(3931, 4)); // Particle
    particleItems->addObject(getCreateBtn(3932, 4)); // Particle
    particleItems->addObject(getCreateBtn(3933, 4)); // Particle
    particleItems->addObject(getCreateBtn(3934, 4)); // Particle
    particleItems->addObject(getCreateBtn(3935, 4)); // Particle
    particleItems->addObject(getCreateBtn(3936, 4)); // Particle
    particleItems->addObject(getCreateBtn(3937, 4)); // Particle
    particleItems->addObject(getCreateBtn(3938, 4)); // Particle
    particleItems->addObject(getCreateBtn(3939, 4)); // Particle
    particleItems->addObject(getCreateBtn(3940, 4)); // Particle
    particleItems->addObject(getCreateBtn(3941, 4)); // Particle
    particleItems->addObject(getCreateBtn(3942, 4)); // Particle
    particleItems->addObject(getCreateBtn(3943, 4)); // Particle
    particleItems->addObject(getCreateBtn(3944, 4)); // Particle
    particleItems->addObject(getCreateBtn(3945, 4)); // Particle
    particleItems->addObject(getCreateBtn(3946, 4)); // Particle
    particleItems->addObject(getCreateBtn(3947, 4)); // Particle
    particleItems->addObject(getCreateBtn(3948, 4)); // Particle
    particleItems->addObject(getCreateBtn(3949, 4)); // Particle
    particleItems->addObject(getCreateBtn(3950, 4)); // Particle
    particleItems->addObject(getCreateBtn(3951, 4)); // Particle
    particleItems->addObject(getCreateBtn(3952, 4)); // Particle
    particleItems->addObject(getCreateBtn(3953, 4)); // Particle
    particleItems->addObject(getCreateBtn(3954, 4)); // Particle
    particleItems->addObject(getCreateBtn(3955, 4)); // Particle
    particleItems->addObject(getCreateBtn(3956, 4)); // Particle
    particleItems->addObject(getCreateBtn(3957, 4)); // Particle
    particleItems->addObject(getCreateBtn(3958, 4)); // Particle
    particleItems->addObject(getCreateBtn(3959, 4)); // Particle
    particleItems->addObject(getCreateBtn(3960, 4)); // Particle
    particleItems->addObject(getCreateBtn(3961, 4)); // Particle
    particleItems->addObject(getCreateBtn(3962, 4)); // Particle
    particleItems->addObject(getCreateBtn(3963, 4)); // Particle
    particleItems->addObject(getCreateBtn(3964, 4)); // Particle
    particleItems->addObject(getCreateBtn(3965, 4)); // Particle
    particleItems->addObject(getCreateBtn(3966, 4)); // Particle
    particleItems->addObject(getCreateBtn(3967, 4)); // Particle
    particleItems->addObject(getCreateBtn(3968, 4)); // Particle
    particleItems->addObject(getCreateBtn(3969, 4)); // Particle
    particleItems->addObject(getCreateBtn(3970, 4)); // Particle
    particleItems->addObject(getCreateBtn(3971, 4)); // Particle
    particleItems->addObject(getCreateBtn(3972, 4)); // Particle
    particleItems->addObject(getCreateBtn(3973, 4)); // Particle
    particleItems->addObject(getCreateBtn(3974, 4)); // Particle
    particleItems->addObject(getCreateBtn(3975, 4)); // Particle
    particleItems->addObject(getCreateBtn(3976, 4)); // Particle
    particleItems->addObject(getCreateBtn(3977, 4)); // Particle
    particleItems->addObject(getCreateBtn(3978, 4)); // Particle
    particleItems->addObject(getCreateBtn(3979, 4)); // Particle
    particleItems->addObject(getCreateBtn(3980, 4)); // Particle
    particleItems->addObject(getCreateBtn(3981, 4)); // Particle
    particleItems->addObject(getCreateBtn(3982, 4)); // Particle
    particleItems->addObject(getCreateBtn(3983, 4)); // Particle
    particleItems->addObject(getCreateBtn(3984, 4)); // Particle
    particleItems->addObject(getCreateBtn(3985, 4)); // Particle
    particleItems->addObject(getCreateBtn(3986, 4)); // Particle
    particleItems->addObject(getCreateBtn(3987, 4)); // Particle
    particleItems->addObject(getCreateBtn(3988, 4)); // Particle
    particleItems->addObject(getCreateBtn(3989, 4)); // Particle
    particleItems->addObject(getCreateBtn(3990, 4)); // Particle
    particleItems->addObject(getCreateBtn(3991, 4)); // Particle
    particleItems->addObject(getCreateBtn(3992, 4)); // Particle
    particleItems->addObject(getCreateBtn(3993, 4)); // Particle
    particleItems->addObject(getCreateBtn(3994, 4)); // Particle
    particleItems->addObject(getCreateBtn(3995, 4)); // Particle
    particleItems->addObject(getCreateBtn(3996, 4)); // Particle
    particleItems->addObject(getCreateBtn(3997, 4)); // Particle
    particleItems->addObject(getCreateBtn(3998, 4)); // Particle
    particleItems->addObject(getCreateBtn(3999, 4)); // Particle
    particleItems->addObject(getCreateBtn(3646, 4)); // Particle
    particleItems->addObject(getCreateBtn(3647, 4)); // Particle
    particleItems->addObject(getCreateBtn(3648, 4)); // Particle
    particleItems->addObject(getCreateBtn(3649, 4)); // Particle
    particleItems->addObject(getCreateBtn(3650, 4)); // Particle
    particleItems->addObject(getCreateBtn(3651, 4)); // Particle
    particleItems->addObject(getCreateBtn(3652, 4)); // Particle
    particleItems->addObject(getCreateBtn(3653, 4)); // Particle
    particleItems->addObject(getCreateBtn(3654, 4)); // Particle
    particleItems->addObject(getCreateBtn(3656, 4)); // Particle
    particleItems->addObject(getCreateBtn(3657, 4)); // Particle
    particleItems->addObject(getCreateBtn(3658, 4)); // Particle
    particleItems->addObject(getCreateBtn(3659, 4)); // Particle
    
    EditButtonBar* particleBar = EditButtonBar::create(particleItems, barPosition, CCSize(356.0f, 220.0f), 8, true, rows);
    addChild(particleBar, 10);
    m_createButtonBars->addObject(particleBar);
    
    // ========================================================================
    // TAB 9: PIXEL ITEMS
    // ========================================================================
    CCArray* pixelItems = CCArray::create();
    pixelItems->addObject(getCreateBtn(1329, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1275, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1276, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1587, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1588, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1589, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1590, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1598, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1599, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(1614, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(3601, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4401, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4402, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4403, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4404, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4405, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4406, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4407, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4408, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4409, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4410, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4411, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4412, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4413, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4414, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4415, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4416, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4417, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4418, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4419, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4420, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4421, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4422, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4423, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4424, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4425, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4426, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4427, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4428, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4429, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4430, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4431, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4432, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4433, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4434, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4435, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4436, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4437, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4438, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4439, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4440, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4441, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4442, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4443, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4444, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4445, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4446, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4447, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4448, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4449, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4450, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4451, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4452, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4453, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4454, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4455, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4456, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4457, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4458, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4459, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4460, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4461, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4462, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4463, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4464, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4465, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4466, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4467, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4468, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4469, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4470, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4471, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4472, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4473, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4474, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4475, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4476, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4477, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4478, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4479, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4480, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4481, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4482, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4483, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4484, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4485, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4486, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4487, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4488, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4538, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4489, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4490, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4491, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4492, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4493, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4494, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4495, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4496, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4497, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4537, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4498, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4499, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4500, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4501, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4502, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4503, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4504, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4505, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4506, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4507, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4508, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4509, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4510, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4511, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4512, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4513, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4514, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4515, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4516, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4517, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4518, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4519, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4520, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4521, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4522, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4523, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4524, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4525, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4526, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4527, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4528, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4529, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4530, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4531, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4532, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4533, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4534, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4535, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4536, 4)); // Pixel Item
    pixelItems->addObject(getCreateBtn(4539, 4)); // Pixel Item
    
    EditButtonBar* pixelItemsBar = EditButtonBar::create(pixelItems, barPosition, CCSize(356.0f, 220.0f), 9, true, rows);
    addChild(pixelItemsBar, 10);
    m_createButtonBars->addObject(pixelItemsBar);
    
    // ========================================================================
    // TAB 10: DECORATIVE SPIKES
    // ========================================================================
    CCArray* decoSpikesItems = CCArray::create();
    decoSpikesItems->addObject(getCreateBtn(41, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(110, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(106, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(107, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(498, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(499, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(129, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(130, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(131, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(128, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(123, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(124, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(125, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(126, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(127, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(134, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(190, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(151, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(152, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(153, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(225, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(226, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(500, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(501, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(229, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(230, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(237, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(238, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(239, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(240, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(241, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(231, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(232, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(283, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(284, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(285, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(279, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(280, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(233, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(281, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(282, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(234, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(235, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(277, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(278, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(245, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(246, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(906, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(414, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(406, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(407, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(408, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(907, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(908, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(909, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(910, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(940, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(941, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(942, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(409, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(410, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(411, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(412, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(413, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1756, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(453, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(454, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(455, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(456, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(457, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1001, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1002, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1003, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1004, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1005, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(668, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(669, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(670, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(671, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(672, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(738, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1737, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1738, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1739, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1740, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1741, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1742, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(719, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(721, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(990, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(992, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(916, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(917, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(925, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(926, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(936, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(937, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(938, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(939, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1120, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1122, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1123, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1124, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1125, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1126, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1127, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1132, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1133, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1134, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1135, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1136, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1137, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1241, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1242, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1243, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1244, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1245, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1246, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1138, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1139, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1310, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1311, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1312, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1313, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1314, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1315, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1316, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1317, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1318, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1319, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1320, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1596, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1597, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1600, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1601, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1843, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1602, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1603, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1604, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1605, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1606, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1607, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1608, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1609, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1610, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1753, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1754, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1757, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1830, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1764, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1765, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1766, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1767, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1768, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1835, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1836, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1837, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1838, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1844, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1845, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1846, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1847, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1848, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1893, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1897, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1896, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1898, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1894, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1895, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1899, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1900, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1901, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1902, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3621, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3622, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3623, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3624, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3625, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3626, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3627, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3628, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3629, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3630, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3631, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3632, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3633, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3634, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3635, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3636, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3637, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3638, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(3639, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(18, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(19, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(20, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(21, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(48, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(49, 4));   // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(113, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(114, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(115, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(157, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(158, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(159, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(227, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(228, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(242, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(419, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(420, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(767, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(448, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(449, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(450, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(451, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(452, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1292, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1010, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1009, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1271, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1272, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1760, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1761, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1887, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(503, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(505, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(504, 4));  // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1273, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1274, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1291, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1758, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1759, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1886, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1011, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1013, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1012, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1269, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1270, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1293, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1762, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1763, 4)); // Deco Spike
    decoSpikesItems->addObject(getCreateBtn(1888, 4)); // Deco Spike
    
    EditButtonBar* decoSpikesBar = EditButtonBar::create(decoSpikesItems, barPosition, CCSize(356.0f, 220.0f), 10, true, rows);
    addChild(decoSpikesBar, 10);
    m_createButtonBars->addObject(decoSpikesBar);
    
    // ========================================================================
    // TAB 11: SAW BLADES
    // ========================================================================
    CCArray* sawBladeItems = CCArray::create();
    sawBladeItems->addObject(getCreateBtn(1705, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1706, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1707, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1708, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1709, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1710, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1734, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1735, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1736, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(186, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(187, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(188, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(740, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(741, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(742, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(678, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(679, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(680, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1619, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1620, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1701, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1702, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1703, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(183, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(184, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(185, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(85, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(86, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(87, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(97, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(137, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(138, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(139, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(154, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(155, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(156, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(180, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(181, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(182, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(222, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(223, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(224, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(375, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(376, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(377, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(378, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1521, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1522, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1523, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1524, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1525, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1526, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1527, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1528, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(394, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(395, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(396, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(997, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(998, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(999, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1000, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1019, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1020, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1021, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1055, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1056, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1057, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1058, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1059, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1060, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1061, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1752, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1831, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1832, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1833, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(1834, 4)); // Saw Blade
    sawBladeItems->addObject(getCreateBtn(15, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(16, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(17, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(50, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(51, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(52, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(53, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(54, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(60, 4));   // Saw Blade
    sawBladeItems->addObject(getCreateBtn(148, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(149, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(405, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(132, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(460, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(494, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(133, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(136, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(150, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(236, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(497, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(495, 4));  // Saw Blade
    sawBladeItems->addObject(getCreateBtn(496, 4));  // Saw Blade
    
    EditButtonBar* sawBladeBar = EditButtonBar::create(sawBladeItems, barPosition, CCSize(356.0f, 220.0f), 11, true, rows);
    addChild(sawBladeBar, 10);
    m_createButtonBars->addObject(sawBladeBar);
    
    // ========================================================================
    // TAB 12: TINT & DECORATION
    // ========================================================================
    CCArray* tintDecoItems = CCArray::create();
    tintDecoItems->addObject(getCreateBtn(31, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(899, 4));  // Tint
    tintDecoItems->addObject(getCreateBtn(901, 4));  // Tint
    tintDecoItems->addObject(getCreateBtn(1616, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1006, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1007, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1049, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1268, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1346, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2067, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1347, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1520, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1585, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3033, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1814, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3016, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3660, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3661, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3032, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3006, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3007, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3008, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3009, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3010, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3011, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3012, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3013, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3014, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3015, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3024, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3029, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3030, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3031, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1595, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1611, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1811, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1817, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3614, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3615, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3617, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3619, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3620, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3641, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1912, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2068, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3607, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3608, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3618, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1913, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1914, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1916, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2901, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2015, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2062, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2925, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2016, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1917, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2900, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1934, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3605, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3602, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3603, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3604, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1935, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2999, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3606, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3612, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1615, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3613, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3662, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1815, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3609, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3640, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1816, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3643, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1812, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(33, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(32, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(1613, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1612, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1818, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1819, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3600, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(1932, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2899, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3642, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2903, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2066, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3022, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2904, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2905, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2907, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2909, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2910, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2911, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2912, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2913, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2914, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2915, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2916, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2917, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2919, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2920, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2921, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2922, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2923, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(2924, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(22, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(24, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(23, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(25, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(26, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(27, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(28, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(55, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(56, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(57, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(58, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(59, 4));   // Tint
    tintDecoItems->addObject(getCreateBtn(1915, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3017, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3018, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3019, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3020, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3021, 4)); // Tint
    tintDecoItems->addObject(getCreateBtn(3023, 4)); // Tint
    
    EditButtonBar* tintDecoBar = EditButtonBar::create(tintDecoItems, barPosition, CCSize(356.0f, 220.0f), 12, true, rows);
    addChild(tintDecoBar, 10);
    m_createButtonBars->addObject(tintDecoBar);
    
    // ========================================================================
    // TAB 13: CUSTOM OBJECTS
    // ========================================================================
    tabIcons->addObject(CCString::create("custom"));
    CCArray* customItems = createCustomItems();
    EditButtonBar* customBar = EditButtonBar::create(customItems, barPosition, CCSize(356.0f, 220.0f), 13, true, rows);
    addChild(customBar, 10);
    m_customTabBar = customBar;
    m_createButtonBars->addObject(customBar);
    
    // ========================================================================
    // BUILD TAB SWITCHER BUTTONS
    // ========================================================================
    for (unsigned int i = 0; i < tabIcons->count(); ++i) {
        CCString* iconStr = (CCString*)tabIcons->objectAtIndex(i);
        const char* iconName = iconStr->getCString();
        
        CCSprite* tabOff = CCSprite::createWithSpriteFrameName("GJ_tabOff_001.png");
        CCSprite* tabOn = CCSprite::createWithSpriteFrameName("GJ_tabOn_001.png");
        tabOff->setScale(1.0f);
        tabOn->setScale(tabOff->getScale());
        
        float maxIconHeight = tabOff->getContentSize().height - 4.0f;
        float maxIconWidth = tabOff->getContentSize().width - 8.0f;
        
        CCSprite* iconOff = nullptr;
        CCSprite* iconOn = nullptr;
        
        if (strcmp(iconName, "custom") == 0) {
            // Custom tab - use placeholder texture
            CCLabelBMFont* placeholder = CCLabelBMFont::create();
            CCArray* children = placeholder->getChildren();
            CCSprite* glyph = (CCSprite*)children->objectAtIndex(0);
            iconOff = CCSprite::createWithTexture(glyph->getTexture());
            iconOn = CCSprite::createWithTexture(glyph->getTexture());
        } else {
            iconOff = CCSprite::createWithSpriteFrameName(iconName);
            iconOn = CCSprite::createWithSpriteFrameName(iconName);
        }
        
        tabOff->addChild(iconOff);
        tabOn->addChild(iconOn);
        
        CCPoint pos = tabOff->getContentSize() * 0.5f;
        pos.y -= 1.0f;
        iconOff->setPosition(pos);
        iconOn->setPosition(pos);
        
        float scale = MIN(maxIconWidth / iconOff->getContentSize().width, 
                         maxIconHeight / iconOff->getContentSize().height);
        iconOff->setScale(scale);
        iconOn->setScale(scale);
        
        tabOff->setOpacity(106);
        iconOff->setOpacity(106);
        
        CCMenuItemToggler* toggle = CCMenuItemToggler::create(tabOff, tabOn, this, 
                                                               menu_selector(EditorUI::onSelectBuildTab));
        toggle->m_notClickable = true;
        toggle->setTag(i);
        m_tabsMenu->addChild(toggle);
        m_tabsArray->addObject(toggle);
    }
    
    // Align tabs
    m_tabsMenu->alignItemsHorizontallyWithPadding(2.0f);
    for (unsigned int i = 0; i < m_tabsArray->count(); ++i) {
        CCMenuItemToggler* toggle = (CCMenuItemToggler*)m_tabsArray->objectAtIndex(i);
        toggle->setSizeMult(1.2f);
    }
    
    CCPoint menuPos = m_tabsMenu->getPosition();
    menuPos.y += 14.0f;
    m_tabsMenu->setPosition(menuPos);
    
    // Restore last pages for each bar
    for (unsigned int i = 0; i < m_createButtonBars->count(); ++i) {
        EditButtonBar* bar = (EditButtonBar*)m_createButtonBars->objectAtIndex(i);
        int lastPage = m_editorLayer->m_level->getLastBuildPageForTab(i);
        bar->goToPage(lastPage);
    }
    
    // Restore current selection
    selectBuildTab(m_editorLayer->m_level->m_lastBuildTab);
    m_createButtonBar->goToPage(m_editorLayer->m_level->m_lastBuildPage);
    m_editorLayer->m_currentLayer = m_editorLayer->m_level->m_lastBuildGroupID;
    updateCreateMenu(false);
}

void EditorUI::setupDeleteMenu() {
    m_deleteMenu = CCMenu::create();
    addChild(m_deleteMenu, 2);
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float screenBottom = CCDirector::sharedDirector()->getScreenBottom();
    CCPoint menuPos(winSize.width * 0.5f, screenBottom + m_toolbarHeight * 0.5f);
    m_deleteMenu->setPosition(menuPos);
    
    // Delete button
    CCPoint offset(0.0f, -1.0f);
    float scale = 0.9f;
    m_deleteButton = getSpriteButton("edit_delBtn_001.png", menu_selector(EditorUI::onDeleteSelected), 
                                     0, m_deleteMenu, 4, &offset, scale);
    
    // Delete all button
    ButtonSprite* deleteAllSprite = ButtonSprite::create();
    CCSprite* deleteIcon = CCSprite::createWithSpriteFrameName("GJ_deleteAllIcon_001.png");
    deleteAllSprite->addChild(deleteIcon);
    deleteIcon->setPosition(ccp(0.0f, 8.0f));
    
    m_deleteAllSprite = CCSprite::create();
    deleteAllSprite->addChild(m_deleteAllSprite);
    m_deleteAllSprite->setPosition(ccp(0.0f, -8.0f));
    
    m_deleteAllButton = getSpriteButton(deleteAllSprite, menu_selector(EditorUI::onDeleteAll));
    CCPoint allPos = m_deleteButton->getPosition();
    allPos.y -= 40.0f;
    m_deleteAllButton->setPosition(allPos);
    
    // Filter buttons
    m_deleteFilterNone = getSpriteButton("deleteFilter_none_001.png", menu_selector(EditorUI::onUpdateDeleteFilter), 
                                         0, m_deleteMenu, 4, &offset, scale);
    m_deleteFilterNone->setTag(0);
    
    m_deleteFilterStatic = getSpriteButton("deleteFilter_static_001.png", menu_selector(EditorUI::onUpdateDeleteFilter),
                                           0, m_deleteMenu, 4, &offset, scale);
    m_deleteFilterStatic->setTag(2);
    
    m_deleteFilterDetails = getSpriteButton("deleteFilter_details_001.png", menu_selector(EditorUI::onUpdateDeleteFilter),
                                            0, m_deleteMenu, 4, &offset, scale);
    m_deleteFilterDetails->setTag(1);
    
    // Custom filter sprite
    ButtonSprite* customFilterSprite = ButtonSprite::createWithSpriteFrameName("deleteFilter_custom_001.png");
    m_customDeleteSprite = CCSprite::create();
    customFilterSprite->addChild(m_customDeleteSprite, -1);
    m_customDeleteSprite->setPosition(ccp(0.0f, -4.0f));
    m_deleteFilterCustom = getSpriteButton(customFilterSprite, menu_selector(EditorUI::onUpdateDeleteFilter));
    m_deleteFilterCustom->setTag(3);
    
    // Position filter buttons
    CCPoint basePos = m_deleteButton->getPosition();
    basePos.x = menuPos.x - 108.0f;
    basePos.y -= 18.0f;
    
    m_deleteFilterNone->setPosition(ccp(basePos.x - 40.0f, basePos.y));
    m_deleteFilterStatic->setPosition(ccp(basePos.x, basePos.y));
    m_deleteFilterDetails->setPosition(ccp(basePos.x - 40.0f, basePos.y - 40.0f));
    m_deleteFilterCustom->setPosition(ccp(basePos.x, basePos.y - 40.0f));
    
    // Info button
    CCSprite* infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSprite, 0, this, 
                                                                    menu_selector(EditorUI::onDeleteInfo));
    infoBtn->setSizeMult(1.5f);
    m_deleteMenu->addChild(infoBtn);
    infoBtn->setPosition(ccp(basePos.x + 160.0f, basePos.y + 10.0f));
    
    // Group filter button
    int groupID = GameManager::sharedState()->getIntGameVariable("0133");
    CCString* groupStr = CCString::createWithFormat("%i", groupID);
    m_deleteGroupSprite = (ButtonSprite*)ButtonSprite::create(groupStr->getCString(), "bigFont.fnt", 
                                                              "GJ_button_04.png", 40.0f);
    m_deleteGroupSprite->setScale(0.9f);
    CCMenuItemSpriteExtra* groupBtn = CCMenuItemSpriteExtra::create(m_deleteGroupSprite, 0, this,
                                                                    menu_selector(EditorUI::onGroupIDFilter));
    m_deleteMenu->addChild(groupBtn);
    groupBtn->setPosition(ccp(basePos.x - 80.0f, basePos.y));
    
    // Color filter button
    int colorID = GameManager::sharedState()->getIntGameVariable("0139");
    CCString* colorStr = CCString::createWithFormat("%i", colorID);
    m_deleteColorSprite = (ButtonSprite*)ButtonSprite::create(colorStr->getCString(), "bigFont.fnt",
                                                              "GJ_button_04.png", 40.0f);
    m_deleteColorSprite->setScale(0.9f);
    CCMenuItemSpriteExtra* colorBtn = CCMenuItemSpriteExtra::create(m_deleteColorSprite, 0, this,
                                                                    menu_selector(EditorUI::onColorFilter));
    m_deleteMenu->addChild(colorBtn);
    colorBtn->setPosition(ccp(basePos.x - 80.0f, basePos.y - 40.0f));
    
    // Reset and find buttons
    CCMenuItemSpriteExtra* resetBtn = getSpriteButton("edit_delBtn_001.png", menu_selector(EditorUI::onResetSpecialFilter),
                                                      0, m_deleteMenu, 4, &offset, scale);
    resetBtn->setPosition(ccp(basePos.x - 120.0f, basePos.y - 40.0f));
    
    CCMenuItemSpriteExtra* findBtn = getSpriteButton("edit_findBtn_001.png", menu_selector(EditorUI::onFindObject),
                                                     0, m_deleteMenu, 4, &offset, scale);
    findBtn->setPosition(ccp(basePos.x - 120.0f, basePos.y));
}

void EditorUI::setupEditMenu() {
    createMoveMenu();
    updateEditMenu();
}

void EditorUI::setupTransformControl() {
    // Empty - placeholder for transform control setup
}

void EditorUI::createMoveMenu()
{
    CCDirector* director = CCDirector::sharedDirector();
    director->getWinSize();
    
    // Create dictionary for edit buttons
    m_editButtonDict = CCDictionary::create();
    m_editButtonDict->retain();
    
    // Create array for menu items
    CCArray* items = CCArray::create();
    
    // ========================================================================
    // MOVEMENT BUTTONS (Various directions and step sizes)
    // ========================================================================
    
    // Small step buttons (1 grid unit)
    CCMenuItemSpriteExtra* upBtn = getSpriteButton("edit_upBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    upBtn->setTag(3);  // Up
    items->addObject(upBtn);
    
    CCMenuItemSpriteExtra* downBtn = getSpriteButton("edit_downBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    downBtn->setTag(4);  // Down
    items->addObject(downBtn);
    
    CCMenuItemSpriteExtra* leftBtn = getSpriteButton("edit_leftBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    leftBtn->setTag(1);  // Left
    items->addObject(leftBtn);
    
    CCMenuItemSpriteExtra* rightBtn = getSpriteButton("edit_rightBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    rightBtn->setTag(2);  // Right
    items->addObject(rightBtn);
    
    // Medium step buttons (5 grid units)
    CCMenuItemSpriteExtra* upBtn2 = getSpriteButton("edit_upBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    upBtn2->setTag(7);  // Up (big)
    items->addObject(upBtn2);
    
    CCMenuItemSpriteExtra* downBtn2 = getSpriteButton("edit_downBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    downBtn2->setTag(8);  // Down (big)
    items->addObject(downBtn2);
    
    CCMenuItemSpriteExtra* leftBtn2 = getSpriteButton("edit_leftBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    leftBtn2->setTag(5);  // Left (big)
    items->addObject(leftBtn2);
    
    CCMenuItemSpriteExtra* rightBtn2 = getSpriteButton("edit_rightBtn2_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    rightBtn2->setTag(6);  // Right (big)
    items->addObject(rightBtn2);
    
    // Large step buttons (10 grid units)
    CCMenuItemSpriteExtra* upBtn3 = getSpriteButton("edit_upBtn3_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    upBtn3->setTag(11);  // Up (extra)
    items->addObject(upBtn3);
    
    CCMenuItemSpriteExtra* downBtn3 = getSpriteButton("edit_downBtn3_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    downBtn3->setTag(12);  // Down (extra)
    items->addObject(downBtn3);
    
    CCMenuItemSpriteExtra* leftBtn3 = getSpriteButton("edit_leftBtn3_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    leftBtn3->setTag(9);  // Left (extra)
    items->addObject(leftBtn3);
    
    CCMenuItemSpriteExtra* rightBtn3 = getSpriteButton("edit_rightBtn3_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    rightBtn3->setTag(10);  // Right (extra)
    items->addObject(rightBtn3);
    
    // Tiny step buttons (0.5 grid units)
    CCMenuItemSpriteExtra* upBtnTiny = getSpriteButton("edit_upBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    upBtnTiny->setTag(15);  // Up (tiny)
    items->addObject(upBtnTiny);
    
    CCMenuItemSpriteExtra* downBtnTiny = getSpriteButton("edit_downBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    downBtnTiny->setTag(16);  // Down (tiny)
    items->addObject(downBtnTiny);
    
    CCMenuItemSpriteExtra* leftBtnTiny = getSpriteButton("edit_leftBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    leftBtnTiny->setTag(13);  // Left (tiny)
    items->addObject(leftBtnTiny);
    
    CCMenuItemSpriteExtra* rightBtnTiny = getSpriteButton("edit_rightBtn_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    rightBtnTiny->setTag(14);  // Right (tiny)
    items->addObject(rightBtnTiny);
    
    // Scale the tiny step button images down
    upBtnTiny->getNormalImage()->setScale(0.8f);
    downBtnTiny->getNormalImage()->setScale(0.8f);
    leftBtnTiny->getNormalImage()->setScale(0.8f);
    rightBtnTiny->getNormalImage()->setScale(0.8f);
    
    // Half step buttons (2.5 grid units)
    CCMenuItemSpriteExtra* upBtn5 = getSpriteButton("edit_upBtn5_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    upBtn5->setTag(19);  // Up (half)
    items->addObject(upBtn5);
    
    CCMenuItemSpriteExtra* downBtn5 = getSpriteButton("edit_downBtn5_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    downBtn5->setTag(20);  // Down (half)
    items->addObject(downBtn5);
    
    CCMenuItemSpriteExtra* leftBtn5 = getSpriteButton("edit_leftBtn5_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    leftBtn5->setTag(17);  // Left (half)
    items->addObject(leftBtn5);
    
    CCMenuItemSpriteExtra* rightBtn5 = getSpriteButton("edit_rightBtn5_001.png", menu_selector(EditorUI::moveObjectCall), nullptr, 0.9f);
    rightBtn5->setTag(18);  // Right (half)
    items->addObject(rightBtn5);
    
    // ========================================================================
    // TRANSFORM BUTTONS (Flip, Rotate, Scale)
    // ========================================================================
    
    // Flip X
    CCMenuItemSpriteExtra* flipXBtn = getSpriteButton("edit_flipXBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    flipXBtn->setTag(21);
    items->addObject(flipXBtn);
    
    // Flip Y
    CCMenuItemSpriteExtra* flipYBtn = getSpriteButton("edit_flipYBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    flipYBtn->setTag(22);
    items->addObject(flipYBtn);
    
    // Rotate CW (90 degrees)
    CCMenuItemSpriteExtra* rotateCWBtn = getSpriteButton("edit_cwBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    rotateCWBtn->setTag(23);
    items->addObject(rotateCWBtn);
    
    // Rotate CCW (90 degrees)
    CCMenuItemSpriteExtra* rotateCCWBtn = getSpriteButton("edit_ccwBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    rotateCCWBtn->setTag(24);
    items->addObject(rotateCCWBtn);
    
    // Rotate 45 degrees right
    CCMenuItemSpriteExtra* rotate45RBtn = getSpriteButton("edit_rotate45rBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    rotate45RBtn->setTag(25);
    items->addObject(rotate45RBtn);
    
    // Store in dictionary
    CCString* key25 = CCString::createWithFormat("%i", 25);
    m_editButtonDict->setObject(key25->getCString(), rotate45RBtn);
    
    // Rotate 45 degrees left
    CCMenuItemSpriteExtra* rotate45LBtn = getSpriteButton("edit_rotate45lBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    rotate45LBtn->setTag(26);
    items->addObject(rotate45LBtn);
    
    CCString* key26 = CCString::createWithFormat("%i", 26);
    m_editButtonDict->setObject(key26->getCString(), rotate45LBtn);
    
    // Free Rotate
    CCMenuItemSpriteExtra* freeRotateBtn = getSpriteButton("edit_freeRotateBtn_001.png", menu_selector(EditorUI::activateRotationControl), nullptr, 0.9f);
    freeRotateBtn->setTag(27);
    items->addObject(freeRotateBtn);
    
    CCString* key27 = CCString::createWithFormat("%i", 27);
    m_editButtonDict->setObject(key27->getCString(), freeRotateBtn);
    
    // Rotate Snap
    CCMenuItemSpriteExtra* rotateSnapBtn = getSpriteButton("edit_rotateSnapBtn_001.png", menu_selector(EditorUI::transformObjectCall), nullptr, 0.9f);
    rotateSnapBtn->setTag(28);
    items->addObject(rotateSnapBtn);
    
    CCString* key28 = CCString::createWithFormat("%i", 28);
    m_editButtonDict->setObject(key28->getCString(), rotateSnapBtn);
    
    // Scale X only
    CCMenuItemSpriteExtra* scaleXBtn = getSpriteButton("edit_scaleBtn_001.png", menu_selector(EditorUI::activateScaleControl), nullptr, 0.9f);
    scaleXBtn->setTag(29);
    items->addObject(scaleXBtn);
    
    CCString* key29 = CCString::createWithFormat("%i", 29);
    m_editButtonDict->setObject(key29->getCString(), scaleXBtn);
    
    // Scale XY uniform
    CCMenuItemSpriteExtra* scaleXYBtn = getSpriteButton("edit_scaleXYBtn_001.png", menu_selector(EditorUI::activateScaleControl), nullptr, 0.9f);
    scaleXYBtn->setTag(30);
    items->addObject(scaleXYBtn);
    
    CCString* key30 = CCString::createWithFormat("%i", 30);
    m_editButtonDict->setObject(key30->getCString(), scaleXYBtn);
    
    // Transform/Warp
    CCMenuItemSpriteExtra* warpBtn = getSpriteButton("edit_warpBtn_001.png", menu_selector(EditorUI::activateTransformControl), nullptr, 0.9f);
    warpBtn->setTag(31);
    items->addObject(warpBtn);
    
    CCString* key31 = CCString::createWithFormat("%i", 31);
    m_editButtonDict->setObject(key31->getCString(), warpBtn);
    
    // ========================================================================
    // CREATE THE EDIT BUTTON BAR
    // ========================================================================
    
    GameManager* gm = GameManager::sharedState();
    int rows = gm->getIntGameVariable("0049");
    int cols = gm->getIntGameVariable("0050");
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float screenBottom = CCDirector::sharedDirector()->getScreenBottom();
    
    CCPoint barPos(winSize.width * 0.5f - 5.0f, screenBottom + m_toolbarHeight - 6.0f);
    
    m_editButtonBar = EditButtonBar::create(items, barPos, CCSize(356.0f, 220.0f), 0, 0, rows);
    addChild(m_editButtonBar, 10);
}

void EditorUI::updateCreateMenu(bool selectTab) {
    if (m_selectedMode == 2) {
        m_createButtonBar->setVisible(true);
        m_tabsMenu->setVisible(true);
        
        // Enable all buttons
        for (unsigned int i = 0; i < m_createButtonArray->count(); ++i) {
            CreateMenuItem* btn = (CreateMenuItem*)m_createButtonArray->objectAtIndex(i);
            enableButton(btn);
        }
        
        for (unsigned int i = 0; i < m_customObjectButtonArray->count(); ++i) {
            CreateMenuItem* btn = (CreateMenuItem*)m_customObjectButtonArray->objectAtIndex(i);
            enableButton(btn);
        }
        
        // Find and disable the selected button
        for (unsigned int i = 0; i < m_createButtonArray->count(); ++i) {
            CreateMenuItem* btn = (CreateMenuItem*)m_createButtonArray->objectAtIndex(i);
            if (btn->m_objectID == m_selectedObjectIndex) {
                disableButton(btn);
                if (selectTab) {
                    selectBuildTab(btn->m_tabIndex);
                    m_createButtonBar->goToPage(btn->m_pageIndex);
                }
                return;
            }
        }
        
        for (unsigned int i = 0; i < m_customObjectButtonArray->count(); ++i) {
            CreateMenuItem* btn = (CreateMenuItem*)m_customObjectButtonArray->objectAtIndex(i);
            if (btn->m_objectID == m_selectedObjectIndex) {
                disableButton(btn);
                return;
            }
        }
    } else {
        m_createButtonBar->setVisible(false);
        m_tabsMenu->setVisible(false);
    }
}

void EditorUI::updateDeleteMenu() {
    m_deleteMenu->setVisible(m_selectedMode == 1);
    updateDeleteButtons();
}

void EditorUI::updateEditMenu() {
    m_editButtonBar->setVisible(m_selectedMode == 3);
}

void EditorUI::enableButton(CreateMenuItem* button) {
    CCNode* node = button->getNormalImage();
    if (node) {
        node->setColor(ccc3(255, 255, 255));
    }
}

void EditorUI::disableButton(CreateMenuItem* button) {
    CCNode* node = button->getNormalImage();
    if (node) {
        node->setColor(ccc3(127, 127, 127));
    }
}

CCMenuItemSpriteExtra* EditorUI::getSpriteButton(const char* frameName, cocos2d::SEL_MenuHandler handler,
                                                  cocos2d::CCObject* target, int menuTag, float scale) {
    CCSprite* sprite = CCSprite::createWithSpriteFrameName(frameName);
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(sprite, target, handler);
    if (menuTag) {
        btn->setTag(menuTag);
    }
    btn->setScale(scale);
    return btn;
}

void EditorUI::selectBuildTab(int tab) {
    if (m_selectedTab == tab) return;
    
    int count = m_createButtonBars->count();
    if (tab < 0 || tab >= count - 1) {
        tab = count - 1;
    }
    
    m_selectedTab = tab;
    
    // Show only the selected tab's bar
    for (unsigned int i = 0; i < m_createButtonBars->count(); ++i) {
        EditButtonBar* bar = (EditButtonBar*)m_createButtonBars->objectAtIndex(i);
        bar->setVisible(i == tab);
        if (i == tab) {
            m_createButtonBar = bar;
        }
    }
    
    // Toggle tab buttons
    for (unsigned int i = 0; i < m_tabsArray->count(); ++i) {
        CCMenuItemToggler* toggle = (CCMenuItemToggler*)m_tabsArray->objectAtIndex(i);
        toggle->toggle(i == tab);
    }
}

CreateMenuItem* EditorUI::getCreateBtn(int id, int bg) {
    const char* bgFrame = "GJ_button_01.png";
    switch (bg) {
        case 2: bgFrame = "GJ_button_02.png"; break;
        case 3: bgFrame = "GJ_button_03.png"; break;
        case 4: bgFrame = "GJ_button_04.png"; break;
        case 5: bgFrame = "GJ_button_05.png"; break;
        case 6: bgFrame = "GJ_button_06.png"; break;
    }
    
    GameObject* obj = GameObject::createWithKey(id);
    if (!obj) return nullptr;
    
    // Set up sprite
    ObjectToolbox* toolbox = ObjectToolbox::sharedState();
    const char* frameName = toolbox->intKeyToFrame(id);
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName(frameName);
    if (!sprite) {
        sprite = CCSprite::create(frameName);
    }
    
    // Create button
    CreateMenuItem* btn = CreateMenuItem::create(sprite, nullptr, this, 
                                                 menu_selector(EditorUI::onCreateButton));
    btn->m_objectID = id;
    btn->setScale(0.9f);
    btn->m_baseScale = 0.9f;
    
    m_createButtonArray->addObject(btn);
    
    return btn;
}

cocos2d::CCArray* EditorUI::createCustomItems() {
    CCArray* items = CCArray::create();
    
    GameManager* gm = GameManager::sharedState();
    CCArray* keys = gm->getOrderedCustomObjectKeys();
    
    for (unsigned int i = 0; i < keys->count(); ++i) {
        CCString* key = (CCString*)keys->objectAtIndex(i);
        int id = key->intValue();
        std::string data = gm->stringForCustomObject(id);
        
        if (!data.empty()) {
            CreateMenuItem* btn = menuItemFromObjectString(data, id);
            if (btn) {
                items->addObject(btn);
            }
        }
    }
    
    // Add custom tab controls if not already created
    if (!m_customTabControls) {
        m_customTabControls = CCArray::create();
        m_customTabControls->retain();
        
        // Up button
        CCSprite* upSprite = CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
        CCMenuItemSpriteExtra* upBtn = getSpriteButton(upSprite, menu_selector(EditorUI::orderUpCustomItem),
                                                        0, 0, 0.9f);
        m_customTabControls->addObject(upBtn);
        
        // Down button
        CCSprite* downSprite = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
        CCMenuItemSpriteExtra* downBtn = getSpriteButton(downSprite, menu_selector(EditorUI::orderDownCustomItem),
                                                          0, 0, 0.9f);
        m_customTabControls->addObject(downBtn);
        
        // Add button
        CCSprite* addSprite = CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png");
        CCMenuItemSpriteExtra* addBtn = getSpriteButton(addSprite, menu_selector(EditorUI::onNewCustomItem),
                                                        0, 0, 0.9f);
        m_customTabControls->addObject(addBtn);
        
        // Delete button
        CCSprite* delSprite = CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");
        CCMenuItemSpriteExtra* delBtn = getSpriteButton(delSprite, menu_selector(EditorUI::onDeleteCustomItem),
                                                        0, 0, 0.9f);
        m_customTabControls->addObject(delBtn);
    }
    
    items->addObjectsFromArray(m_customTabControls);
    
    return items;
}

CreateMenuItem* EditorUI::menuItemFromObjectString(std::string str, int id) {
    // Parse object string and create a button
    ButtonSprite* sprite = spriteFromObjectString(str, 1, 0, 100, 0, 0, 0);
    if (!sprite) return nullptr;
    
    float size = MAX(sprite->getContentSize().width, sprite->getContentSize().height);
    sprite->setScale(40.0f / size);
    
    CCNode* node = ButtonSprite::create(sprite, "GJ_button_04.png", 32.0f);
    sprite->setPosition(ccp(0.0f, -1.0f));
    
    CreateMenuItem* btn = CreateMenuItem::create(node, nullptr, this,
                                                  menu_selector(EditorUI::onCreateButton));
    btn->m_objectID = id;
    btn->setScale(0.9f);
    btn->m_baseScale = 0.9f;
    
    m_customObjectButtonArray->addObject(btn);
    
    return btn;
}

void EditorUI::onCreateButton(cocos2d::CCObject* sender) {
    CreateMenuItem* btn = (CreateMenuItem*)sender;
    m_selectedObjectIndex = btn->m_objectID;
    updateCreateMenu(false);
    updateGridNodeSize();
}

void EditorUI::onSelectBuildTab(cocos2d::CCObject* sender) {
    int tab = sender->getTag();
    selectBuildTab(tab);
}

void EditorUI::updateDeleteButtons() {
    int filter = GameManager::sharedState()->getIntGameVariable("0005");
    
    // Update filter button highlights
    updateButtonHighlight(m_deleteFilterNone, filter == 0);
    updateButtonHighlight(m_deleteFilterStatic, filter == 2);
    updateButtonHighlight(m_deleteFilterDetails, filter == 1);
    updateButtonHighlight(m_deleteFilterCustom, filter == 3);
    
    // Update custom filter preview
    if (filter == 3 && m_selectedObject) {
        ObjectToolbox* toolbox = ObjectToolbox::sharedState();
        const char* frame = toolbox->intKeyToFrame(m_selectedObject->m_objectID);
        CCSpriteFrame* spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame);
        m_customDeleteSprite->setDisplayFrame(spriteFrame);
        m_customDeleteSprite->setVisible(true);
        
        // Scale to fit
        float w = m_customDeleteSprite->getContentSize().width;
        float h = m_customDeleteSprite->getContentSize().height;
        float scale = 20.0f / MAX(w, h);
        m_customDeleteSprite->setScale(MIN(scale, 1.0f));
    } else {
        m_customDeleteSprite->setVisible(false);
    }
}

void EditorUI::updateButtonHighlight(CCMenuItemSpriteExtra* btn, bool highlighted) {
    ButtonSprite* sprite = (ButtonSprite*)btn->getNormalImage();
    if (sprite) {
        const char* frame = highlighted ? "GJ_button_02.png" : "GJ_button_04.png";
        sprite->updateBGImage(frame);
    }
}

void EditorUI::onUpdateDeleteFilter(cocos2d::CCObject* sender) {
    int filter = sender->getTag();
    if (filter >= 0 && filter <= 3) {
        GameManager::sharedState()->setIntGameVariable("0005", filter);
        if (filter == 3 && m_selectedObject) {
            GameManager::sharedState()->setIntGameVariable("0006", m_selectedObject->m_objectID);
        } else if (filter != 3) {
            GameManager::sharedState()->setIntGameVariable("0006", 0);
        }
        updateDeleteButtons();
    }
}

void EditorUI::updateGridNodeSize() {
    // Update grid size based on selected object(s)
    if (m_selectedMode == 2) {
        // Use selected object index
        updateGridNodeSize(m_selectedObjectIndex);
    } else if (m_selectedObject) {
        m_selectedObjectIndex = m_selectedObject->m_objectID;
        updateGridNodeSize(m_selectedObjectIndex);
    } else if (m_selectedObjects->count() > 0) {
        // Multiple objects - check if they share the same grid size
        float size = -1.0f;
        int id = -1;
        for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
            float objSize = ObjectToolbox::sharedState()->gridNodeSizeForKey(obj->m_objectID);
            if (size == -1.0f) {
                size = objSize;
                id = obj->m_objectID;
            } else if (objSize != size) {
                id = -1;
                break;
            }
        }
        updateGridNodeSize(id);
    } else {
        m_selectedObjectIndex = -1;
        updateGridNodeSize(-1);
    }
}

void EditorUI::updateGridNodeSize(int objectID) {
    // Implementation depends on grid node management
    // This would update the grid size in the editor layer
}

void EditorUI::onPause(cocos2d::CCObject* sender) {
    if (!m_isPaused) {
        // Save state
        m_editorLayer->m_level->m_lastCameraPos = m_editorLayer->m_objectLayer->getPosition();
        m_editorLayer->m_level->m_lastEditorZoom = m_editorLayer->m_objectLayer->getScale();
        m_editorLayer->m_level->m_lastBuildPage = m_createButtonBar->getPage();
        m_editorLayer->m_level->m_lastBuildGroupID = m_editorLayer->m_currentLayer;
        m_editorLayer->m_level->m_lastBuildTab = m_selectedTab;
        
        // Save pages for each tab
        for (unsigned int i = 0; i < m_createButtonBars->count(); ++i) {
            EditButtonBar* bar = (EditButtonBar*)m_createButtonBars->objectAtIndex(i);
            m_editorLayer->m_level->setLastBuildPageForTab(bar->getPage(), i);
        }
        
        // Show pause layer
        EditorPauseLayer* pauseLayer = EditorPauseLayer::create(m_editorLayer);
        getParent()->addChild(pauseLayer, 100);
        toggleLockUI(true);
    }
}

void EditorUI::toggleLockUI(bool lockUI) {
    m_isPaused = lockUI;
}

void EditorUI::onDeleteSelected(cocos2d::CCObject* sender) {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return;
    
    CCArray* objectsToDelete = CCArray::create();
    
    // Collect deletable objects
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        if (shouldDeleteObject(obj)) {
            objectsToDelete->addObject(obj);
        }
    }
    
    if (m_selectedObject && shouldDeleteObject(m_selectedObject)) {
        objectsToDelete->addObject(m_selectedObject);
    }
    
    // Delete objects
    for (unsigned int i = 0; i < objectsToDelete->count(); ++i) {
        GameObject* obj = (GameObject*)objectsToDelete->objectAtIndex(i);
        if (obj->m_objectID == 747) {
            deselectTargetPortals();
        }
        deleteObject(obj, true);
    }
    
    // Add to undo
    if (objectsToDelete->count() > 0) {
        UndoObject* undo = UndoObject::createWithArray(objectsToDelete, DeleteMulti);
        LevelEditorLayer::addToUndoList(m_editorLayer, undo, 0);
    }
    
    updateSpecialUIElements();
    updateButtons();
    tryUpdateTimeMarkers();
}

bool EditorUI::shouldDeleteObject(GameObject* object) {
    if (!object || object->m_objectID == 749) return false;
    
    int filter = GameManager::sharedState()->getIntGameVariable("0005");
    switch (filter) {
        case 0: // None - delete all
            return true;
        case 1: // Static - blocks, slopes
            return object->getType() == Decoration || object->getType() == Slope;
        case 2: // Details - portals, triggers, etc.
            return object->getType() != Decoration && object->getType() != Slope;
        case 3: // Custom - only selected object type
            return GameManager::sharedState()->getIntGameVariable("0006") == object->m_objectID;
        default:
            return true;
    }
}

void EditorUI::deleteObject(GameObject* object, bool noUndo) {
    if (!object) return;
    
    object->m_wasSelected = object->m_isSelected;
    deselectObject();
    LevelEditorLayer::removeObject(m_editorLayer, object, noUndo);
    deactivateRotationControl();
    deactivateScaleControl();
    deactivateTransformControl();
}

void EditorUI::deselectAll() {
    deselectObject();
    
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        GameObject::deselectObject(obj);
    }
    
    m_selectedObjects->removeAllObjects();
    updateButtons();
    deactivateRotationControl();
    deactivateScaleControl();
    deactivateTransformControl();
    updateObjectInfoLabel();
}

void EditorUI::deselectObject() {
    if (m_selectedObject) {
        GameObject::deselectObject(m_selectedObject);
        m_selectedObject = nullptr;
        m_canActivateControls = true;
    }
}

void EditorUI::updateButtons() {
    toggleEditObjectButton();
    toggleDuplicateButton();
    updateDeleteMenu();
    toggleSpecialEditButtons();
    
    bool hasSelection = (m_selectedObject || m_selectedObjects->count() > 0);
    
    // Update deselect button
    m_deselectBtn->setEnabled(hasSelection);
    m_deselectBtn->setColor(hasSelection ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    // Update undo/redo
    m_undoBtn->setEnabled(LevelEditorLayer::hasAction(m_editorLayer, 1));
    m_redoBtn->setEnabled(LevelEditorLayer::hasAction(m_editorLayer, 0));
    
    // Update delete buttons
    m_deleteButton->setEnabled(hasSelection);
    if (!hasSelection) {
        m_deleteButton->setColor(ccc3(127, 127, 127));
    }
    
    // Update edit group button
    m_editGroupBtn->setEnabled(hasSelection);
    m_editGroupBtn->setColor(hasSelection ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    // Update go to layer button
    bool hasLayer = false;
    if (m_selectedObject) {
        hasLayer = true;
    } else if (m_selectedObjects->count() > 0) {
        int layer1 = editorLayerForArray(m_selectedObjects, false);
        int layer2 = editorLayerForArray(m_selectedObjects, true);
        hasLayer = (layer1 > 0 || layer2 > 0);
    }
    m_goToLayerBtn->setEnabled(hasLayer);
    m_goToLayerBtn->setColor(hasLayer ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    // Update sticky controls
    updateStickyControls();
}

void EditorUI::toggleDuplicateButton() {
    bool hasSelection = (m_selectedObject || m_selectedObjects->count() > 0);
    
    m_copyPasteBtn->setEnabled(hasSelection);
    m_copyPasteBtn->setColor(hasSelection ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    m_copyBtn->setEnabled(hasSelection);
    m_copyBtn->setColor(hasSelection ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    bool hasClipboard = (GameManager::sharedState()->m_editorClipboard.length() > 0);
    m_pasteBtn->setEnabled(hasClipboard);
    m_pasteBtn->setColor(hasClipboard ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
}

void EditorUI::toggleEditObjectButton() {
    bool usable = editButtonUsable();
    m_editObjectBtn->setEnabled(usable);
    m_editObjectBtn->setColor(usable ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    bool usable2 = editButton2Usable();
    m_editSpecialBtn->setEnabled(usable2);
    m_editSpecialBtn->setColor(usable2 ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
    
    updateEditColorButton();
}

bool EditorUI::editButtonUsable() {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return false;
    
    // Check if any selected object is editable
    if (m_selectedObject) {
        if (m_selectedObject->canChangeCustomColor()) return true;
        if (m_selectedObject->isBasicTrigger()) return true;
    }
    
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        if (obj->canChangeCustomColor()) return true;
        if (obj->isBasicTrigger()) return true;
    }
    
    return false;
}

bool EditorUI::editButton2Usable() {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return false;
    
    // Check for advanced edit capabilities
    if (m_selectedObject) {
        if (m_selectedObject->getType() == Smart) return true;
        if (m_selectedObject->getHasRotateAction()) return true;
    }
    
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        if (obj->getType() == Smart) return true;
    }
    
    return false;
}

int EditorUI::editorLayerForArray(cocos2d::CCArray* objects, bool layer2) {
    if (!objects || objects->count() == 0) return -1;
    
    int result = -1;
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = (GameObject*)objects->objectAtIndex(i);
        int layer = layer2 ? obj->m_editorLayer2 : obj->m_editorLayer;
        if (result == -1) {
            result = layer;
        } else if (result != layer) {
            return 0; // Mixed layers
        }
    }
    return result;
}

void EditorUI::updateEditColorButton() {
    bool usable = editColorButtonUsable();
    m_editHSVBtn->setEnabled(usable);
    m_editHSVBtn->setColor(usable ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
}

bool EditorUI::editColorButtonUsable() {
    return (m_selectedObject || m_selectedObjects->count() > 0);
}

void EditorUI::updateObjectInfoLabel()
{
    GameManager* gm = GameManager::sharedState();
    if (!gm->getGameVariable("0041")) {
        return;
    }

    // Initialize string builder
    std::string infoText;

    // Get selected objects info
    CCArray* selectedObjects = m_selectedObjects;
    GameObject* selectedObj = m_selectedObject;

    int mainColor = -2;      // -2 = unset, -10001 = mixed
    int secondaryColor = -2;
    int zOrder = -1;
    int layer1 = -1;
    int layer2 = -1;
    ZLayer zLayer = B3;
    int objectCount = 0;

    // Process selection
    if (selectedObjects && selectedObjects->count() > 0) {
        objectCount = selectedObjects->count();
        bool mainColorMixed = false;
        bool secondaryColorMixed = false;
        bool zLayerMixed = false;
        bool zOrderMixed = false;
        bool layer1Mixed = false;
        bool layer2Mixed = false;

        for (unsigned int i = 0; i < selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)selectedObjects->objectAtIndex(i);

            // Main color
            GJSpriteColor* color = obj->getRelativeSpriteColor(1);
            if (color) {
                int c = color->getColorMode();
                if (!mainColorMixed) {
                    if (mainColor == -2) {
                        mainColor = c;
                    } else if (mainColor != c) {
                        mainColorMixed = true;
                        mainColor = -10001;
                    }
                }
            }

            // Secondary color
            GJSpriteColor* color2 = obj->getRelativeSpriteColor(2);
            if (color2) {
                int c = color2->getColorMode();
                if (!secondaryColorMixed) {
                    if (secondaryColor == -2) {
                        secondaryColor = c;
                    } else if (secondaryColor != c) {
                        secondaryColorMixed = true;
                        secondaryColor = -10001;
                    }
                }
            }

            // Z layer
            ZLayer layer = obj->getObjectZLayer();
            if (!zLayerMixed) {
                if (zLayer == B3) {
                    zLayer = layer;
                } else if (zLayer != layer) {
                    zLayerMixed = true;
                    zLayer = -10001;
                }
            }

            // Z order
            int order = obj->getObjectZOrder();
            if (!zOrderMixed) {
                if (zOrder == -1) {
                    zOrder = order;
                } else if (zOrder != order) {
                    zOrderMixed = true;
                    zOrder = -10001;
                }
            }

            // Layer 1
            int l1 = obj->m_editorLayer;
            if (!layer1Mixed) {
                if (layer1 == -1) {
                    layer1 = l1;
                } else if (layer1 != l1) {
                    layer1Mixed = true;
                    layer1 = -10001;
                }
            }

            // Layer 2
            int l2 = obj->m_editorLayer2;
            if (!layer2Mixed) {
                if (layer2 == -1) {
                    layer2 = l2;
                } else if (layer2 != l2) {
                    layer2Mixed = true;
                    layer2 = -10001;
                }
            }
        }
    } else if (selectedObj) {
        objectCount = 0;
        GJSpriteColor* color = selectedObj->getRelativeSpriteColor(1);
        mainColor = color ? color->getColorMode() : -2;

        GJSpriteColor* color2 = selectedObj->getRelativeSpriteColor(2);
        secondaryColor = color2 ? color2->getColorMode() : -2;

        zLayer = selectedObj->getObjectZLayer();
        zOrder = selectedObj->getObjectZOrder();
        layer1 = selectedObj->m_editorLayer;
        layer2 = selectedObj->m_editorLayer2;
    } else {
        m_objectInfoLabel->setString(" ");
        return;
    }

    // Build color info
    std::string color1Str = "?";
    std::string color2Str = "?";

    if (mainColor != -10001) {
        if (mainColor >= 0 && mainColor <= 999) {
            color1Str = GameToolbox::intToString(mainColor);
        } else if (mainColor > 999) {
            color1Str = GJSpecialColorSelect::textForColorIdx(mainColor);
        } else if (mainColor == -2) {
            color1Str = "-";
        }
    }

    if (secondaryColor != -10001) {
        if (secondaryColor >= 0 && secondaryColor <= 999) {
            color2Str = GameToolbox::intToString(secondaryColor);
        } else if (secondaryColor > 999) {
            color2Str = GJSpecialColorSelect::textForColorIdx(secondaryColor);
        } else if (secondaryColor == -2) {
            color2Str = "-";
        }
    }

    // Build color string
    if (mainColor == -10001 || secondaryColor == -10001) {
        infoText += "C: ";
        infoText += (mainColor == -10001) ? "?" : color1Str;
        infoText += "\n";
    } else if (mainColor == -2 || secondaryColor == -2) {
        infoText += "C: ";
        infoText += (secondaryColor == -2) ? color1Str : color2Str;
        infoText += "\n";
    } else {
        infoText += "C1: ";
        infoText += color1Str;
        infoText += "\nC2: ";
        infoText += color2Str;
        infoText += "\n";
    }

    // Build group IDs
    if (selectedObjects && selectedObjects->count() > 0) {
        // Collect group IDs from all selected objects
        std::map<int, int> groupCounts;

        for (unsigned int i = 0; i < selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)selectedObjects->objectAtIndex(i);
            int groupCount = obj->m_groupCount;
            for (int j = 0; j < groupCount; ++j) {
                int gid = obj->getGroupID(j);
                groupCounts[gid]++;
            }
        }

        if (!groupCounts.empty()) {
            infoText += "G: ";
            int count = 0;
            for (auto it = groupCounts.begin(); it != groupCounts.end(); ++it) {
                if (count > 0) infoText += ",";

                int gid = it->first;
                int numInGroup = it->second;

                // Check if this group has a parent
                bool hasParent = false;
                for (unsigned int i = 0; i < selectedObjects->count(); ++i) {
                    GameObject* obj = (GameObject*)selectedObjects->objectAtIndex(i);
                    GameManager* gm2 = GameManager::sharedState();
                    if (obj == GJBaseGameLayer::getGroupParent(gm2->m_levelEditorLayer, gid)) {
                        hasParent = true;
                        break;
                    }
                }

                char buf[32];
                if (hasParent) {
                    snprintf(buf, sizeof(buf), "%i*", gid);
                } else {
                    snprintf(buf, sizeof(buf), "%i", gid);
                }
                infoText += buf;

                if (++count >= 5) {
                    infoText += "...";
                    break;
                }
            }
            infoText += "\n";
        }
    }

    // Z layer and order
    const char* zLayerStr = "?";
    if (zLayer != -10001) {
        switch (zLayer) {
            case B5: zLayerStr = "B5"; break;
            case B4: zLayerStr = "B4"; break;
            case B3: zLayerStr = "B3"; break;
            case B2: zLayerStr = "B2"; break;
            case B1: zLayerStr = "B1"; break;
            case T1: zLayerStr = "T1"; break;
            case T2: zLayerStr = "T2"; break;
            case T3: zLayerStr = "T3"; break;
            case T4: zLayerStr = "T4"; break;
            default: zLayerStr = "?"; break;
        }
    }

    std::string zOrderStr = (zOrder == -10001) ? "?" : GameToolbox::intToString(zOrder);
    infoText += "Layer: ";
    infoText += zLayerStr;
    infoText += "\nZ: ";
    infoText += zOrderStr;
    infoText += "\n";

    // Editor layers
    std::string layer1Str, layer2Str;
    if (layer1 > 0 && layer1 != -10001) {
        layer1Str = GameToolbox::intToString(layer1);
    }
    if (layer2 > 0 && layer2 != -10001) {
        if (!layer1Str.empty()) {
            layer2Str = ", " + GameToolbox::intToString(layer2);
        } else {
            layer2Str = GameToolbox::intToString(layer2);
        }
    }

    if (!layer1Str.empty() || !layer2Str.empty()) {
        infoText += "EL: ";
        infoText += layer1Str;
        infoText += layer2Str;
        infoText += "\n";
    }

    // Effect order info (if selected object is an effect)
    if (selectedObj && selectedObj->m_classType == Effect) {
        if (selectedObj->m_orderChannel != 0 || selectedObj->m_orderIndex != 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Order: %i,%i\n", selectedObj->m_orderChannel, selectedObj->m_orderIndex);
            infoText += buf;
        }

        // Position and size
        CCPoint pos = selectedObj->getPosition();
        CCSize size = selectedObj->getContentSize();
        char buf[64];
        snprintf(buf, sizeof(buf), "%.01f %.01f, %.01f %.01f\n", pos.x, pos.y, size.width, size.height);
        infoText += buf;
    }

    // Object count
    if (objectCount > 1) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Objects: %i", objectCount);
        infoText += buf;
    }

    // Set label text
    m_objectInfoLabel->setString(infoText.c_str());
}


void EditorUI::updateStickyControls() {
    m_stickyControlsEnabled = GameManager::sharedState()->getGameVariable("0097");
    m_linkControlsDisabled = GameManager::sharedState()->getGameVariable("0180");
    
    m_enableLinkBtn->setEnabled(m_stickyControlsEnabled);
    m_enableLinkBtn->setVisible(m_stickyControlsEnabled);
    m_linkBtn->setEnabled(m_stickyControlsEnabled);
    m_linkBtn->setVisible(m_stickyControlsEnabled);
    m_unlinkBtn->setEnabled(m_stickyControlsEnabled);
    m_unlinkBtn->setVisible(m_stickyControlsEnabled);
}

void EditorUI::onCopy(cocos2d::CCObject* sender) {
    doCopyObjects(false);
}

void EditorUI::doCopyObjects(bool withColor) {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return;
    if (m_selectedObject && m_selectedObject->m_objectID == 749) return;
    
    CCArray* objects = m_selectedObjects;
    if (!objects || objects->count() == 0) {
        objects = CCArray::createWithObject(m_selectedObject);
    }
    
    std::string clipboard = copyObjects(objects, withColor, false);
    GameManager::sharedState()->m_editorClipboard = clipboard;
    GameManager::sharedState()->m_copiedObjectCount = objects->count();
    updateButtons();
}

void EditorUI::onPaste(cocos2d::CCObject* sender) {
    doPasteObjects(false);
}

void EditorUI::doPasteObjects(bool withColor) {
    if (GameManager::sharedState()->m_editorClipboard.empty()) return;
    
    int currentCount = m_editorLayer->m_objectCount.random - m_editorLayer->m_objectCount.seed;
    int newCount = currentCount + GameManager::sharedState()->m_copiedObjectCount;
    
    if (newCount > 80000 && !m_editorLayer->m_level->m_unlimitedObjectsEnabled) {
        showMaxError();
        return;
    }
    if (newCount > 40000 && !m_editorLayer->m_level->m_highObjectsEnabled) {
        showMaxBasicError();
        return;
    }
    
    CCArray* pasted = pasteObjects(GameManager::sharedState()->m_editorClipboard, withColor, false);
    if (pasted && pasted->count() > 0) {
        // Center pasted objects on screen
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        CCPoint center = m_editorLayer->m_objectLayer->convertToNodeSpace(
            ccp(winSize.width * 0.5f, m_toolbarHeight * 0.5f + winSize.height * 0.5f));
        center = getGridSnappedPos(center);
        repositionObjectsToCenter(pasted, center);
    }
    
    updateButtons();
    updateObjectInfoLabel();
}

std::string EditorUI::copyObjects(cocos2d::CCArray* objects, bool copyColors, bool sort) {
    if (!objects) return "";
    
    if (sort) {
        // Sort objects
    }
    
    std::string result;
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = (GameObject*)objects->objectAtIndex(i);
        if (obj->m_objectID == 749) continue;
        
        std::string objStr = obj->getSaveString();
        result += objStr;
        result += ";";
        
        if (copyColors) {
            // Include color data
            int mainColor = obj->getMainColorMode();
            int secondaryColor = obj->getSecondaryColorMode();
            // Add color data to result
        }
    }
    
    return result;
}

cocos2d::CCArray* EditorUI::pasteObjects(std::string str, bool withColor, bool noUndo) {
    if (str.empty()) return nullptr;
    
    CCArray* objects = LevelEditorLayer::createObjectsFromString(
        m_editorLayer, str, true, m_editorLayer->m_level->m_unlimitedObjectsEnabled);
    
    if (!objects) return nullptr;
    
    if (!noUndo) {
        UndoObject* undo = UndoObject::createWithArray(objects, Paste);
        LevelEditorLayer::addToUndoList(m_editorLayer, undo, 0);
    }
    
    if (objects->count() > 0) {
        deselectAll();
        if (objects->count() == 1) {
            selectObject((GameObject*)objects->objectAtIndex(0), false);
        } else {
            for (unsigned int i = 0; i < objects->count(); ++i) {
                GameObject* obj = (GameObject*)objects->objectAtIndex(i);
                m_selectedObjects->addObject(obj);
                obj->selectObject(ccc3(255, 0, 0));
            }
        }
        GJBaseGameLayer::assignNewStickyGroups(m_editorLayer, objects);
    }
    
    return objects;
}

void EditorUI::onDuplicate(cocos2d::CCObject* sender) {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return;
    
    int currentCount = m_editorLayer->m_objectCount.random - m_editorLayer->m_objectCount.seed;
    int newCount = currentCount + m_selectedObjects->count();
    
    if (newCount > 80000 && !m_editorLayer->m_level->m_unlimitedObjectsEnabled) {
        showMaxError();
        return;
    }
    if (newCount > 40000 && !m_editorLayer->m_level->m_highObjectsEnabled) {
        showMaxBasicError();
        return;
    }
    
    CCArray* objects = m_selectedObjects;
    if (!objects || objects->count() == 0) {
        objects = CCArray::createWithObject(m_selectedObject);
    }
    
    std::string clipboard = copyObjects(objects, false, false);
    m_editorLayer->m_keepEditorLayer = true;
    pasteObjects(clipboard, false, false);
    m_editorLayer->m_keepEditorLayer = false;
    
    tryUpdateTimeMarkers();
    updateButtons();
    updateObjectInfoLabel();
}

void EditorUI::updateSpecialUIElements() {
    updateObjectInfoLabel();
    updateScaleControl();
}

void EditorUI::updateScaleControl() {
    // Update scale control bounds based on selection
    if (m_scaleControl && m_scaleControl->isVisible()) {
        bool isTrigger = false;
        if (m_selectedObject) {
            isTrigger = m_selectedObject->isTrigger();
        } else if (m_selectedObjects->count() > 0) {
            for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
                GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
                if (!obj->isTrigger()) {
                    isTrigger = false;
                    break;
                }
                isTrigger = true;
            }
        }
        
        if (m_increaseScale) {
            m_scaleControl->m_upperBound = 4.0f;
            m_scaleControl->m_lowerBound = 0.25f;
        } else {
            m_scaleControl->m_lowerBound = 0.5f;
            m_scaleControl->m_upperBound = isTrigger ? 4.0f : 2.0f;
        }
        
        m_scaleControl->loadValues(m_selectedObject, m_selectedObjects, &m_objectEditorStates);
    }
}

void EditorUI::deactivateRotationControl() {
    m_rotationTouchID = -1;
    if (m_rotationControl && m_rotationControl->isVisible()) {
        m_rotationControl->setVisible(false);
        m_rotationControl->finishTouch();
    }
}

void EditorUI::deactivateScaleControl() {
    if (m_scaleControl && m_scaleControl->isVisible()) {
        m_scaleControl->setVisible(false);
        m_scaleControl->finishTouch();
        m_scaleControl->m_upperBound = 2.0f;
        m_scaleControl->m_lowerBound = 0.5f;
    }
    updateEditButtonColor(m_scaleControl->m_senderTag, ccc3(255, 255, 255));
    m_scaleControl->m_senderTag = 0;
}

void EditorUI::deactivateTransformControl() {
    if (m_transformControl && m_transformControl->isVisible()) {
        m_transformControl->setVisible(false);
        m_transformControl->finishTouch();
    }
    updateEditButtonColor(31, ccc3(255, 255, 255));
}

void EditorUI::activateRotationControl(cocos2d::CCObject* sender) {
    if ((!m_selectedObject && m_selectedObjects->count() == 0) || 
        (m_rotationControl->isVisible() && !m_canActivateControls)) {
        return;
    }
    
    deactivateTransformControl();
    m_canActivateControls = false;
    
    CCPoint pivot = (m_selectedObjects->count() > 0) ? 
        getGroupCenter(m_selectedObjects, false) : 
        m_selectedObject->getPosition();
    
    m_pivotPoint = pivot;
    m_rotationControl->setPosition(pivot);
    m_rotationControl->setScale(1.0f / m_editorLayer->m_objectLayer->getScale());
    m_rotationControl->setVisible(true);
}

void EditorUI::activateScaleControl(cocos2d::CCObject* sender) {
    if ((!m_selectedObject && m_selectedObjects->count() == 0)) return;
    
    deactivateTransformControl();
    int tag = sender->getTag();
    if (m_scaleControl->isVisible() && tag != m_scaleControl->m_senderTag) {
        deactivateScaleControl();
    }
    
    if (!m_scaleControl->isVisible() || m_canActivateControls) {
        updateEditButtonColor(tag, ccc3(168, 168, 255));
        m_canActivateControls = false;
        
        m_pivotPoint = (m_selectedObjects->count() > 0) ?
            getGroupCenter(m_selectedObjects, false) :
            m_selectedObject->getPosition();
        
        m_scaleControl->m_senderTag = tag;
        m_scaleControl->setPosition(m_pivotPoint + ccp(0.0f, 40.0f));
        m_scaleControl->setScale(1.0f / m_editorLayer->m_objectLayer->getScale());
        m_scaleControl->setVisible(true);
        updateScaleControl();
    }
}

void EditorUI::activateTransformControl(cocos2d::CCObject* sender) {
    if ((!m_selectedObject && m_selectedObjects->count() == 0)) return;
    
    deactivateScaleControl();
    deactivateRotationControl();
    
    if (m_transformControl->isVisible()) {
        deactivateTransformControl();
        return;
    }
    
    if (!m_transformControl->isVisible() || m_canActivateControls) {
        m_transformState.m_transformReset = true;
        updateEditButtonColor(31, ccc3(168, 168, 255));
        m_canActivateControls = false;
        
        m_pivotPoint = (m_selectedObjects->count() > 0) ?
            getGroupCenter(m_selectedObjects, false) :
            m_selectedObject->getPosition();
        
        m_transformControl->setPosition(m_pivotPoint);
        m_transformControl->scaleButtons(1.0f / m_editorLayer->m_objectLayer->getScale());
        m_transformControl->setVisible(true);
        m_transformControl->updateAnchorSprite(m_pivotPoint - m_transformControl->getPosition());
    }
}

void EditorUI::updateEditButtonColor(int tag, cocos2d::ccColor3B color) {
    // Update color of edit button with given tag
    CCString* key = CCString::createWithFormat("%i", tag);
    CCObject* obj = m_editButtonDict->objectForKey(key->getCString());
    if (obj) {
        CCMenuItemSpriteExtra* btn = (CCMenuItemSpriteExtra*)obj;
        btn->setColor(color);
    }
}

void EditorUI::toggleSpecialEditButtons() {
    // Check if any selected object can rotate freely
    bool canRotateFree = false;
    if (m_selectedObject) {
        canRotateFree = m_selectedObject->canRotateFree();
    } else if (m_selectedObjects->count() > 0) {
        canRotateFree = true;
        for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
            if (!obj->canRotateFree()) {
                canRotateFree = false;
                break;
            }
        }
    }
    
    // Update rotation-related buttons
    for (int i = 0; i < 3; ++i) {
        CCString* key = CCString::createWithFormat("%i", 27 + i); // free rotate, snap, scale buttons
        CCObject* obj = m_editButtonDict->objectForKey(key->getCString());
        if (obj) {
            CCMenuItemSpriteExtra* btn = (CCMenuItemSpriteExtra*)obj;
            btn->setEnabled(canRotateFree);
            btn->setColor(canRotateFree ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
        }
    }
    
    if (!canRotateFree) {
        deactivateRotationControl();
    } else if (GameManager::sharedState()->getGameVariable("0007")) {
        activateRotationControl(nullptr);
    }
}

void EditorUI::toggleEnableRotate(cocos2d::CCObject* sender) {
    bool enabled = GameManager::sharedState()->getGameVariable("0007");
    GameManager::sharedState()->setGameVariable("0007", !enabled);
    
    ButtonSprite* sprite = (ButtonSprite*)m_rotateBtn->getNormalImage();
    sprite->updateBGImage(enabled ? "GJ_button_01.png" : "GJ_button_02.png");
    
    if (enabled) {
        deactivateRotationControl();
    } else {
        toggleSpecialEditButtons();
    }
}

void EditorUI::toggleMode(cocos2d::CCObject* sender) {
    int mode = sender->getTag();
    if (m_selectedMode == mode) return;
    
    m_selectedMode = mode;
    resetUI();
    
    // Update button images
    const char* frame = nullptr;
    const char* activeFrame = nullptr;
    switch (mode) {
        case 1:
            frame = "edit_deleteBtn_001.png";
            activeFrame = "edit_deleteSBtn_001.png";
            break;
        case 2:
            frame = "edit_buildBtn_001.png";
            activeFrame = "edit_buildSBtn_001.png";
            break;
        case 3:
            frame = "edit_editBtn_001.png";
            activeFrame = "edit_editSBtn_001.png";
            break;
    }
    
    // Update the active mode button
    CCMenuItemSpriteExtra* btn = nullptr;
    switch (mode) {
        case 1: btn = m_deleteModeBtn; break;
        case 2: btn = m_buildModeBtn; break;
        case 3: btn = m_editModeBtn; break;
    }
    if (btn) {
        CCNode* node = btn->getNormalImage();
        if (node) {
            // Set sprite frame to active version
            node->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(activeFrame));
        }
    }
}

void EditorUI::resetUI() {
    // Reset mode button images
    updateModeButton(m_deleteModeBtn, "edit_deleteBtn_001.png");
    updateModeButton(m_buildModeBtn, "edit_buildBtn_001.png");
    updateModeButton(m_editModeBtn, "edit_editBtn_001.png");
    
    updateDeleteMenu();
    updateCreateMenu(false);
    updateEditMenu();
    updateGridNodeSize();
}

void EditorUI::updateModeButton(CCMenuItemSpriteExtra* btn, const char* frameName) {
    if (!btn) return;
    CCNode* node = btn->getNormalImage();
    if (!node) return;
    
    CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName);
    node->setDisplayFrame(frame);
}

void EditorUI::onDelete(cocos2d::CCObject* sender) {
    CCPoint clickPos = m_clickAtPosition;
    CCArray* objects = LevelEditorLayer::objectsAtPosition(m_editorLayer, clickPos);
    if (!objects) return;
    
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = getCycledObject(objects, true);
        if (!obj) break;
        
        if (shouldDeleteObject(obj)) {
            m_deleteObjects->addObject(obj);
        }
    }
    
    // Delete objects
    if (m_deleteObjects->count() > 0) {
        UndoObject* undo = UndoObject::createWithArray(m_deleteObjects, DeleteMulti);
        LevelEditorLayer::addToUndoList(m_editorLayer, undo, 0);
    }
    
    m_deleteObjects->removeAllObjects();
    updateSpecialUIElements();
    tryUpdateTimeMarkers();
}

GameObject* EditorUI::getCycledObject(cocos2d::CCArray* objects, bool dontSkipSelected) {
    if (!objects || objects->count() == 0) return nullptr;
    
    GameObject* best = nullptr;
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = (GameObject*)objects->objectAtIndex(i);
        if (!canSelectObject(obj)) continue;
        if (dontSkipSelected && obj->m_isSelected) continue;
        
        if (!best || obj->m_objectID >= best->m_objectID) {
            best = obj;
        }
    }
    
    if (best) {
        best->m_cycledObjectIndex = m_cycledObjectIndex;
    }
    return best;
}

bool EditorUI::canSelectObject(GameObject* object) {
    if (!object) return false;
    
    if (GameManager::sharedState()->getGameVariable("0064")) {
        int filterType = GameManager::sharedState()->getIntGameVariable("0005");
        if (filterType == 3) {
            int filterID = GameManager::sharedState()->getIntGameVariable("0006");
            if (filterID != object->m_objectID) return false;
        }
    }
    
    int groupFilter = GameManager::sharedState()->getIntGameVariable("0133");
    if (groupFilter > 0 && !object->belongsToGroup(groupFilter)) return false;
    
    int colorFilter = GameManager::sharedState()->getIntGameVariable("0139");
    if (colorFilter > 0) {
        int mainColor = object->getMainColorMode();
        int secondaryColor = object->getSecondaryColorMode();
        if (colorFilter != mainColor && colorFilter != secondaryColor) return false;
    }
    
    return true;
}

void EditorUI::onGroupIDFilter(cocos2d::CCObject* sender) {
    int current = GameManager::sharedState()->getIntGameVariable("0133");
    SetIDPopup* popup = SetIDPopup::create(current, 0, 9999, "GroupID Filter", "OK", true);
    popup->setTag(1);
    popup->m_delegate = &m_setIDPopupDelegate;
    popup->show();
}

void EditorUI::onColorFilter(cocos2d::CCObject* sender) {
    int current = GameManager::sharedState()->getIntGameVariable("0139");
    SetColorIDPopup* popup = SetColorIDPopup::create(current);
    popup->setTag(2);
    popup->m_delegate = &m_setIDPopupDelegate;
    popup->show();
}

void EditorUI::setIDPopupClosed(SetIDPopup* popup, int value) {
    int tag = popup->getTag();
    if (tag == 1) {
        GameManager::sharedState()->setIntGameVariable("0133", value);
        m_deleteGroupSprite->setString(CCString::createWithFormat("%i", value)->getCString());
    } else if (tag == 2) {
        GameManager::sharedState()->setIntGameVariable("0139", value);
        std::string text;
        if (value > 999) {
            text = GJSpecialColorSelect::textForColorIdx(value);
        } else {
            text = GameToolbox::intToString(value);
        }
        m_deleteColorSprite->setString(text.c_str());
    } else if (tag == 3) {
        findAndSelectObject(value, true);
    }
}

void EditorUI::findAndSelectObject(int id, bool useUniqueID) {
    GameObject* obj = nullptr;
    if (useUniqueID) {
        obj = LevelEditorLayer::findGameObject(m_editorLayer, id);
    } else {
        obj = GJBaseGameLayer::tryGetObject(m_editorLayer, id);
    }
    
    if (obj) {
        selectObject(obj, false);
        centerCameraOnObject(obj);
        playCircleAnim(obj->getPosition(), 50.0f, 0.8f);
    }
}

void EditorUI::centerCameraOnObject(GameObject* object) {
    if (!object) return;
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float scale = m_editorLayer->m_objectLayer->getScale();
    
    CCPoint pos = object->getPosition();
    float x = winSize.width * 0.5f - pos.x * scale;
    float y = winSize.height * 0.5f - pos.y * scale;
    
    m_editorLayer->m_objectLayer->setPosition(ccp(x, y));
    constrainGameLayerPosition();
}

void EditorUI::constrainGameLayerPosition(float minX, float minY)
{
    // Get current position
    CCPoint currentPos = m_editorLayer->m_objectLayer->getPosition();
    
    // Reset to origin to calculate bounds
    m_editorLayer->m_objectLayer->setPosition(ccp(0, 0));
    
    // Get screen boundaries
    CCDirector* director = CCDirector::sharedDirector();
    float screenLeft = director->getScreenLeft();
    float screenTop = director->getScreenTop();
    float screenRight = director->getScreenRight();
    
    // Calculate min bounds (top-left)
    CCPoint minWorldPos(screenLeft, m_constrainedHeight);
    CCPoint minOffset(minX * 30.0f - 30.0f, minY * 30.0f + 90.0f);
    CCPoint minNodePos = m_editorLayer->m_objectLayer->convertToWorldSpace(minOffset);
    CCPoint minBounds = m_editorLayer->convertToNodeSpace(minNodePos);
    
    // Calculate max bounds (bottom-right)
    LevelSettingsObject* settings = m_editorLayer->m_levelSettings;
    int maxSection = settings->m_dynamicLevelHeight ? 1000 : 80;
    CCPoint maxWorldPos(screenRight, screenTop);
    CCPoint maxOffset(240030.0f, maxSection * 30.0f + 90.0f + 30.0f);
    CCPoint maxNodePos = m_editorLayer->m_objectLayer->convertToWorldSpace(maxOffset);
    CCPoint maxBounds = m_editorLayer->convertToNodeSpace(maxNodePos);
    
    // Clamp position
    float x = currentPos.x;
    float y = currentPos.y;
    
    x = fmaxf(x, minBounds.x);
    x = fminf(x, maxBounds.x);
    y = fmaxf(y, minBounds.y);
    y = fminf(y, maxBounds.y);
    
    // Apply constrained position
    m_editorLayer->m_objectLayer->setPosition(ccp(x, y));
}

void EditorUI::constrainGameLayerPosition()
{
    constrainGameLayerPosition(-100.0f, -100.0f);
}

void EditorUI::playCircleAnim(cocos2d::CCPoint position, float radius, float duration) {
    CCCircleWave* wave = CCCircleWave::create(0, 1.0f, position.x, position.y, 0, false);
    wave->setColor(ccc3(255, 255, 255));
    addChild(wave);
    wave->m_animDuration = 1;
}

void EditorUI::selectObject(GameObject* object, bool ignoreFilter) {
    if (!ignoreFilter && !canSelectObject(object)) return;
    
    deselectAll();
    m_selectedObject = object;
    object->selectObject(ccc3(255, 0, 0));
    m_canActivateControls = true;
    checkLiveColorSelect();
}

void EditorUI::selectObjects(cocos2d::CCArray* objects, bool ignoreFilter) {
    if (!objects || objects->count() == 0) return;
    
    // Filter objects if needed
    if (!ignoreFilter) {
        for (unsigned int i = 0; i < objects->count(); ++i) {
            GameObject* obj = (GameObject*)objects->objectAtIndex(i);
            if (!canSelectObject(obj)) {
                objects->removeObject(obj);
                --i;
            }
        }
        if (objects->count() == 0) return;
    }
    
    // Add objects to selection
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = (GameObject*)objects->objectAtIndex(i);
        if (!m_selectedObjects->containsObject(obj)) {
            m_selectedObjects->addObject(obj);
            obj->selectObject(ccc3(255, 0, 0));
            m_canActivateControls = true;
        }
    }
}

void EditorUI::onDeselectAll(cocos2d::CCObject* sender) {
    if (m_selectedObject || m_selectedObjects->count() > 0) {
        createUndoSelectObject(false);
    }
    deselectAll();
}

void EditorUI::createUndoSelectObject(bool redo) {
    CCArray* selected = getSelectedObjects();
    UndoObject* undo = UndoObject::createWithArray(selected, Select);
    undo->m_redo = redo;
    LevelEditorLayer::addToUndoList(m_editorLayer, undo, false);
}

cocos2d::CCArray* EditorUI::getSelectedObjects() {
    CCArray* result = CCArray::create();
    
    if (m_selectedObject && m_selectedObject->m_objectID != 749) {
        result->addObject(m_selectedObject);
    }
    
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        if (obj->m_objectID != 749) {
            result->addObject(obj);
        }
    }
    
    return result;
}

void EditorUI::deselectTargetPortals() {
    // Deselect any portal objects (ID 749)
    if (m_selectedObject && m_selectedObject->m_objectID == 749) {
        deselectObject();
    }
    
    CCArray* toRemove = CCArray::create();
    for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
        GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
        if (obj->m_objectID == 749) {
            toRemove->addObject(obj);
        }
    }
    
    for (unsigned int i = 0; i < toRemove->count(); ++i) {
        GameObject* obj = (GameObject*)toRemove->objectAtIndex(i);
        m_selectedObjects->removeObject(obj);
        deselectObject();
    }
}

void EditorUI::onEditColor(cocos2d::CCObject* sender) {
    if (!editColorButtonUsable()) return;
    
    if (m_editHSVBtn->getTag() == 1) {
        editHSV();
    } else {
        editColor();
    }
    updateEditColorButton();
}

void EditorUI::editColor()
{
    ColorAction* mainColor = nullptr;
    ColorAction* detailColor = nullptr;
    EffectGameObject* object = nullptr;
    
    getEditColorTargets(&mainColor, &detailColor, &object);
    
    if (mainColor != nullptr || object != nullptr)
    {
        closeLiveColorSelect();
        closeLiveHSVSelect();
        
        m_colorOverlay = nullptr;
        
        ColorSelectLiveOverlay* overlay = nullptr;
        if (object != nullptr)
        {
            overlay = ColorSelectLiveOverlay::createWithObject(object);
        }
        else
        {
            overlay = ColorSelectLiveOverlay::createWithActions(mainColor, detailColor);
        }
        
        m_colorOverlay = overlay;
        overlay->m_delegate = &m_colorSelectDelegate;
        overlay->m_scene = this;
        overlay->show();
        
        // Deselect objects if editing color actions (not a specific object)
        if (object == nullptr)
        {
            if (m_selectedObject)
            {
                GameObject::deselectObject(m_selectedObject);
            }
            else
            {
                for (unsigned int i = 0; i < m_selectedObjects->count(); ++i)
                {
                    GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
                    GameObject::deselectObject(obj);
                }
            }
        }
    }
}

void EditorUI::editHSV()
{
    closeLiveHSVSelect();
    closeLiveColorSelect();
    
    CCArray* selectedObjects = nullptr;
    
    if (m_selectedObject)
    {
        selectedObjects = nullptr;
        GameObject::deselectObject(m_selectedObject);
    }
    else
    {
        selectedObjects = getSelectedObjects();
        
        if (m_selectedObjects)
        {
            ccArray* data = m_selectedObjects->data;
            if (data->num > 0)
            {
                GameObject** arr = (GameObject**)data->arr;
                GameObject** end = arr + data->num;
                for (GameObject** it = arr; it < end; ++it)
                {
                    GameObject* obj = *it;
                    if (obj)
                    {
                        GameObject::deselectObject(obj);
                    }
                }
            }
        }
    }
    
    HSVLiveOverlay* overlay = HSVLiveOverlay::create(m_selectedObject, selectedObjects);
    m_hsvOverlay = overlay;
    overlay->m_delegate = &m_colorSelectDelegate;
    overlay->m_scene = this;
    overlay->show();
}

void EditorUI::editObject(cocos2d::CCObject* sender)
{
    if (!editButtonUsable()) {
        return;
    }
    
    closeLiveColorSelect();
    
    RotateGameplayGameObject* selectedObj = (RotateGameplayGameObject*)m_selectedObject;
    int objectID = -1;
    int objType = 0;
    int typeVal = 0;
    int isSpawnable = 0;
    bool isBasicEnterEffect = false;
    bool isAreaTrigger = false;
    
    if (selectedObj) {
        objectID = selectedObj->m_objectID;
        objType = 0;
        typeVal = 0;
        isSpawnable = 0;
        isBasicEnterEffect = GameObject::isBasicEnterEffect(selectedObj->m_objectID);
    } else if (m_selectedObjects && m_selectedObjects->count() > 0) {
        isBasicEnterEffect = true;
        objectID = -1;
        objType = 1;
        typeVal = 1;
        isSpawnable = 1;
        int objTypeVal = -1;
        
        for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
            
            if (typeVal) {
                if (!GameObject::isBasicTrigger(obj)) {
                    typeVal = 0;
                }
            }
            
            if (objectID) {
                if (objectID == -1) {
                    objectID = obj->m_objectID;
                } else if (objectID != obj->m_objectID) {
                    objectID = 0;
                }
            }
            
            if (objTypeVal != -1) {
                if (objTypeVal == -1) {
                    objTypeVal = obj->getType();
                } else if (objTypeVal != obj->getType()) {
                    objTypeVal = 0;
                }
            }
            
            if (isSpawnable) {
                if (!GameObject::isSpawnableTrigger(obj)) {
                    isSpawnable = 0;
                }
            }
            
            if (isAreaTrigger) {
                isAreaTrigger = GameObject::isBasicEnterEffect(obj->m_objectID);
            }
        }
    } else {
        objectID = -1;
        typeVal = 0;
        isSpawnable = 0;
        isBasicEnterEffect = false;
    }
    
    // Handle coin object
    if (objectID == 142) {
        SetupCoinLayer* popup = SetupCoinLayer::create((EffectGameObject*)selectedObj, m_selectedObjects);
        popup->show();
        return;
    }
    
    // Handle start position object
    if (selectedObj && selectedObj->m_objectID == 31) {
        LevelSettingsObject* settings = *(LevelSettingsObject**)&selectedObj->m_groundDirection;
        settings->m_platformerMode = m_editorLayer->m_levelSettings->m_platformerMode;
        LevelSettingsLayer* layer = LevelSettingsLayer::create(settings, nullptr);
        layer->show();
        return;
    }
    
    // Main switch based on object ID
    switch (objectID) {
        case 2900: {
            SetupRotateGameplayPopup* popup = SetupRotateGameplayPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2901: {
            SetupGameplayOffsetPopup* popup = SetupGameplayOffsetPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2899: {
            SetupOptionsTriggerPopup* popup = SetupOptionsTriggerPopup::create((GameOptionsTrigger*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3642: {
            SetupAudioLineGuidePopup* popup = SetupAudioLineGuidePopup::create((AudioLineGuideGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3022: {
            SetupTeleportPopup* popup = SetupTeleportPopup::create(
                (TeleportPortalObject*)selectedObj,
                m_selectedObjects,
                3022,
                m_editorLayer->m_levelSettings->m_platformerMode);
            popup->show();
            return;
        }
        case 3006: {
            SetupAreaMoveTriggerPopup* popup = SetupAreaMoveTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3008: {
            SetupAreaTransformTriggerPopup* popup = SetupAreaTransformTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3007: {
            SetupAreaRotateTriggerPopup* popup = SetupAreaRotateTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3009: {
            SetupAreaFadeTriggerPopup* popup = SetupAreaFadeTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3010: {
            SetupAreaTintTriggerPopup* popup = SetupAreaTintTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // Area animation triggers (3011-3015)
    if ((unsigned int)(objectID - 3011) <= 4) {
        SetupAreaAnimTriggerPopup* popup = SetupAreaAnimTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects, objectID);
        popup->show();
        return;
    }
    
    // Stop area trigger
    if (objectID == 3024) {
        std::string title = "Stop Area Trigger";
        std::string label = "EffectID:";
        SetTargetIDLayer* popup = SetTargetIDLayer::create(
            (SetTargetIDLayer*)selectedObj,
            (EffectGameObject*)m_selectedObjects,
            title,
            label,
            0,
            9999,
            3024);
        popup->show();
        return;
    }
    
    // Area trigger
    if (isAreaTrigger) {
        SetupAreaTriggerPopup* popup = SetupAreaTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
        popup->show();
        return;
    }
    
    // Enter effect triggers (3017-3023)
    if ((unsigned int)(objectID - 3017) <= 4 || objectID == 3023) {
        SetupEnterEffectPopup* popup = SetupEnterEffectPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects, objectID);
        popup->show();
        return;
    }
    
    // Basic enter effect
    if (isBasicEnterEffect) {
        SetupEnterTriggerPopup* popup = SetupEnterTriggerPopup::create((EnterEffectObject*)selectedObj, m_selectedObjects);
        popup->show();
        return;
    }
    
    // Shader effects (2904-2924)
    if ((unsigned int)(objectID - 2904) <= 1 ||
        objectID == 2907 ||
        (unsigned int)(objectID - 2909) <= 8 ||
        (unsigned int)(objectID - 2919) <= 5) {
        SetupShaderEffectPopup* popup = SetupShaderEffectPopup::create(selectedObj, m_selectedObjects, objectID);
        popup->show();
        return;
    }
    
    // Art switch triggers (3029-3031)
    if ((unsigned int)(objectID - 3029) <= 2) {
        SetupArtSwitchPopup* popup = SetupArtSwitchPopup::create((ArtTriggerGameObject*)selectedObj, m_selectedObjects, objectID);
        popup->show();
        return;
    }
    
    // Individual object handlers
    switch (objectID) {
        case 2925: {
            SetupCameraModePopup* popup = SetupCameraModePopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1007: {
            SetupOpacityPopup* popup = SetupOpacityPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1268: {
            SetupSpawnPopup* popup = SetupSpawnPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1006: {
            SetupPulsePopup* popup = SetupPulsePopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 899: {
            ColorSelectPopup* popup = ColorSelectPopup::create((ColorSelectPopup*)selectedObj, (EffectGameObject*)m_selectedObjects);
            popup->show();
            return;
        }
        case 1049:
        case 1812: {
            SetupObjectTogglePopup* popup = SetupObjectTogglePopup::create(
                (EffectGameObject*)selectedObj,
                m_selectedObjects,
                m_editorLayer->m_levelSettings->m_platformerMode);
            popup->show();
            return;
        }
        case 1520: {
            SetupShakePopup* popup = SetupShakePopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1585: {
            SetupAnimationPopup* popup = SetupAnimationPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3033: {
            SetupKeyframeAnimPopup* popup = SetupKeyframeAnimPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3032: {
            SetupKeyframePopup* popup = SetupKeyframePopup::create((KeyframeGameObject*)selectedObj, m_selectedObjects, m_editorLayer);
            popup->show();
            return;
        }
        case 3600: {
            SetupPlatformerEndPopup* popup = SetupPlatformerEndPopup::create((EndTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 901: {
            SetupMoveCommandPopup* popup = SetupMoveCommandPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1346: {
            SetupRotateCommandPopup* popup = SetupRotateCommandPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1347: {
            GJFollowCommandLayer* popup = GJFollowCommandLayer::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1814: {
            GJPFollowCommandLayer* popup = GJPFollowCommandLayer::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3016: {
            SetupAdvFollowPopup* popup = SetupAdvFollowPopup::create((AdvancedFollowTriggerObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3660: {
            SetupAdvFollowEditPhysicsPopup* popup = SetupAdvFollowEditPhysicsPopup::create((AdvancedFollowEditObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3661: {
            SetupAdvFollowRetargetPopup* popup = SetupAdvFollowRetargetPopup::create((AdvancedFollowEditObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2067: {
            SetupTransformPopup* popup = SetupTransformPopup::create((TransformTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1934: {
            SetupSongTriggerPopup* popup = SetupSongTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3602: {
            SetupSFXPopup* popup = SetupSFXPopup::create((SFXTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3603: {
            SetupSFXEditPopup* popup = SetupSFXEditPopup::create((SFXTriggerGameObject*)selectedObj, m_selectedObjects, false);
            popup->show();
            return;
        }
        case 3605: {
            SetupSFXEditPopup* popup = SetupSFXEditPopup::create((SFXTriggerGameObject*)selectedObj, m_selectedObjects, true);
            popup->show();
            return;
        }
        case 3604: {
            SetupEventLinkPopup* popup = SetupEventLinkPopup::create((EventLinkTrigger*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1935: {
            SetupTimeWarpPopup* popup = SetupTimeWarpPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2015: {
            SetupCameraRotatePopup2* popup = SetupCameraRotatePopup2::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2016: {
            SetupCameraGuidePopup* popup = SetupCameraGuidePopup::create((CameraTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2069:
        case 3645: {
            SetupForceBlockPopup* popup = SetupForceBlockPopup::create((ForceBlockGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2066: {
            SetupGravityTriggerPopup* popup = SetupGravityTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // Advanced random trigger
    if (selectedObj && selectedObj->m_objectID == 2068) {
        SetupRandAdvTriggerPopup* popup = SetupRandAdvTriggerPopup::create((RandTriggerGameObject*)selectedObj, m_selectedObjects);
        popup->show();
        return;
    }
    
    // Sequence trigger
    if (selectedObj && selectedObj->m_objectID == 3607) {
        SetupSequenceTriggerPopup* popup = SetupSequenceTriggerPopup::create((SequenceTriggerGameObject*)selectedObj);
        popup->show();
        return;
    }
    
    // More object handlers
    switch (objectID) {
        case 1816: {
            CollisionBlockPopup* popup = CollisionBlockPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 2999: {
            SetupMGTrigger* popup = SetupMGTrigger::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3606:
        case 3612: {
            SetupBGSpeedTrigger* popup = SetupBGSpeedTrigger::create(selectedObj, m_selectedObjects, objectID);
            popup->show();
            return;
        }
        case 3613: {
            UIObjectSettingsPopup* popup = UIObjectSettingsPopup::create((UISettingsGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3662: {
            std::string title = "Visibility Link";
            std::string label = "GroupID:";
            SetTargetIDLayer* popup = SetTargetIDLayer::create(
                (SetTargetIDLayer*)selectedObj,
                (EffectGameObject*)m_selectedObjects,
                title,
                label,
                0,
                9999,
                3662);
            popup->show();
            return;
        }
        case 1932: {
            SetupPlayerControlPopup* popup = SetupPlayerControlPopup::create((PlayerControlGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3655: {
            SetupObjectControlPopup* popup = SetupObjectControlPopup::create((ObjectControlGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // Multi triggers (200-203, 1334)
    if ((unsigned int)(objectID - 200) <= 3 || objectID == 1334) {
        MultiTriggerPopup* popup = MultiTriggerPopup::create(selectedObj, m_selectedObjects);
        popup->show();
        return;
    }
    
    // More triggers
    if (selectedObj) {
        int id = selectedObj->m_objectID;
        if (id == 1931) {
            SetupEndPopup* popup = SetupEndPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (id == 1595) {
            SetupTouchTogglePopup* popup = SetupTouchTogglePopup::create(selectedObj, nullptr);
            popup->show();
            return;
        }
        if (id == 1611) {
            SetupCountTriggerPopup* popup = SetupCountTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (objectID == 1811) {
            SetupInstantCountPopup* popup = SetupInstantCountPopup::create((CountTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // More trigger handlers
    switch (objectID) {
        case 1817: {
            SetupPickupTriggerPopup* popup = SetupPickupTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3614: {
            SetupTimerTriggerPopup* popup = SetupTimerTriggerPopup::create((TimerTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3615: {
            SetupTimerEventTriggerPopup* popup = SetupTimerEventTriggerPopup::create((TimerTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3617: {
            SetupTimerControlTriggerPopup* popup = SetupTimerControlTriggerPopup::create(
                (TimerTriggerGameObject*)selectedObj,
                m_selectedObjects);
            popup->show();
            return;
        }
        case 3618: {
            SetupResetTriggerPopup* popup = SetupResetTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3619: {
            SetupItemEditTriggerPopup* popup = SetupItemEditTriggerPopup::create((ItemTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3620: {
            SetupItemCompareTriggerPopup* popup = SetupItemCompareTriggerPopup::create(
                (ItemTriggerGameObject*)selectedObj,
                m_selectedObjects);
            popup->show();
            return;
        }
        case 3641: {
            SetupPersistentItemTriggerPopup* popup = SetupPersistentItemTriggerPopup::create(
                (ItemTriggerGameObject*)selectedObj,
                m_selectedObjects);
            popup->show();
            return;
        }
        case 1914: {
            SetupStaticCameraPopup* popup = SetupStaticCameraPopup::create((CameraTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // More object-specific handlers (1912, 1913, 2062, 1916, configurable portal)
    if (selectedObj) {
        int id = selectedObj->m_objectID;
        if (id == 1912) {
            SetupRandTriggerPopup* popup = SetupRandTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (id == 1913) {
            SetupZoomTriggerPopup* popup = SetupZoomTriggerPopup::create(selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (id == 2062) {
            SetupCameraEdgePopup* popup = SetupCameraEdgePopup::create((CameraTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (id == 1916) {
            SetupCameraOffsetTrigger* popup = SetupCameraOffsetTrigger::create((CameraTriggerGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        if (GameObject::isConfigurablePortal(selectedObj)) {
            SetupCameraModePopup* popup = SetupCameraModePopup::create((EffectGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
    }
    
    // Effect triggers
    switch (objectID) {
        case 1616: {
            SetupStopTriggerPopup* popup = SetupStopTriggerPopup::create((EffectGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3608: {
            SetupSpawnParticlePopup* popup = SetupSpawnParticlePopup::create((EffectGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 1815: {
            SetupCollisionTriggerPopup* popup = SetupCollisionTriggerPopup::create((EffectGameObject*)selectedObj, m_selectedObjects);
            popup->show();
            return;
        }
        case 3609: {
            SetupInstantCollisionTriggerPopup* popup = SetupInstantCollisionTriggerPopup::create(
                (EffectGameObject*)selectedObj,
                m_selectedObjects);
            popup->show();
            return;
        }
        case 3640: {
            SetupCollisionStateTriggerPopup* popup = SetupCollisionStateTriggerPopup::create(
                (EffectGameObject*)selectedObj,
                m_selectedObjects);
            popup->show();
            return;
        }
        case 3643: {
            SetupObjectTogglePopup* popup = SetupObjectTogglePopup::create(
                (EffectGameObject*)selectedObj,
                m_selectedObjects,
                m_editorLayer->m_levelSettings->m_platformerMode);
            popup->show();
            return;
        }
    }
    
    // Default: triggers or custom object editor
    if (!typeVal) {
        // Check if we should use CustomizeObjectLayer or EditTriggersPopup
        if (m_selectedObject && 
            !GameObject::isBasicTrigger(m_selectedObject) &&
            (!(isSpawnable || objectID == 1917) || objectID == 2903)) {
            
            if (m_selectedObject && !GameObject::canChangeCustomColor(m_selectedObject)) {
                ColorSelectPopup* popup = ColorSelectPopup::create(
                    (ColorSelectPopup*)m_selectedObject,
                    (EffectGameObject*)m_selectedObjects);
                popup->show();
                return;
            }
        } else if (!(isSpawnable || objectID == 1917) || objectID == 2903) {
            // Use CustomizeObjectLayer
            if (m_selectedObjects->count() > 0 || 
                (m_selectedObject && GameObject::canChangeCustomColor(m_selectedObject))) {
                CustomizeObjectLayer* popup = CustomizeObjectLayer::create(m_selectedObject, m_selectedObjects);
                popup->show();
            }
            return;
        }
    }
    
    // Default: EditTriggersPopup
    EditTriggersPopup* popup = EditTriggersPopup::create((EffectGameObject*)m_selectedObject, m_selectedObjects);
    popup->show();
}



void EditorUI::editObject2(cocos2d::CCObject* sender) {
    editObjectSpecial(0);
}

void EditorUI::editObject3(cocos2d::CCObject* sender) {
    editObjectSpecial(1);
}

void EditorUI::editObjectSpecial(int type)
{
    CCArray* objects = m_selectedObjects;
    GameObject* obj = m_selectedObject;
    
    int objectID = -1;
    int classType = -1;
    int objectType = -1;
    getGroupInfo(obj, objects, &objectID, &classType, &objectType);
    
    bool hasRotateAction = false;
    if (obj) {
        hasRotateAction = obj->getHasRotateAction();
    } else if (objects && objects->count() > 0) {
        hasRotateAction = true;
        for (unsigned int i = 0; i < objects->count(); ++i) {
            GameObject* o = (GameObject*)objects->objectAtIndex(i);
            if (!o->canAllowMultiActivate()) {
                hasRotateAction = false;
                break;
            }
        }
    }
    
    // Check if it's a smart block
    if (objectID != -1 && classType == 5) {
        CCArray* smartBlocks = nullptr;
        if (objects) {
            smartBlocks = CCArray::create();
            for (unsigned int i = 0; i < objects->count(); ++i) {
                GameObject* o = (GameObject*)objects->objectAtIndex(i);
                if (o->m_classType == Smart) {
                    smartBlocks->addObject(o);
                }
            }
        }
        SetupSmartBlockLayer* popup = SetupSmartBlockLayer::create((SmartGameObject*)obj, smartBlocks);
        popup->show();
        return;
    }
    
    // Check if any selected object is a smart block
    if ((obj && obj->m_classType == Smart) || arrayContainsClass(objects, 5)) {
        CCArray* smartBlocks = nullptr;
        if (objects) {
            smartBlocks = CCArray::create();
            for (unsigned int i = 0; i < objects->count(); ++i) {
                GameObject* o = (GameObject*)objects->objectAtIndex(i);
                if (o->m_classType == Smart) {
                    smartBlocks->addObject(o);
                }
            }
        }
        SetupSmartBlockLayer* popup = SetupSmartBlockLayer::create((SmartGameObject*)obj, smartBlocks);
        popup->show();
        return;
    }
    
    LevelSettingsObject* settings = m_editorLayer->m_levelSettings;
    bool isPlatformer = m_editorLayer->m_isPlatformer;
    
    // Dash ring (platformer only)
    if (isPlatformer && (objectID == 1751 || objectID == 1704)) {
        SetupDashRingPopup* popup = SetupDashRingPopup::create((DashRingObject*)obj, objects);
        popup->show();
        return;
    }
    
    // Gradient trigger
    if (objectID == 2903) {
        SetupGradientPopup* popup = SetupGradientPopup::create((GradientTriggerObject*)obj, objects);
        popup->show();
        return;
    }
    
    // Object-specific handlers
    switch (objectID) {
        case 2063: { // Checkpoint
            SetupCheckpointPopup* popup = SetupCheckpointPopup::create((CheckpointGameObject*)obj, objects);
            popup->show();
            return;
        }
        case 747:   // Portal
        case 1751:  // Dash ring (non-platformer)
        case 3022:  // Teleport portal
        case 3027:  // Teleport portal
        case 3645:  // Teleport portal
        case 3004:  // Teleport portal
        case 3026:  // Teleport portal
        case 3028:  // Teleport portal
        case 3034:  // Teleport portal
        case 3038:  // Teleport portal
        case 3042:  // Teleport portal
        case 3046:  // Teleport portal
        case 3035:  // Teleport portal
        case 3039:  // Teleport portal
        case 3043:  // Teleport portal
        case 3047:  // Teleport portal
        case 3036:  // Teleport portal
        case 3040:  // Teleport portal
        case 3044:  // Teleport portal
        case 3048:  // Teleport portal
        case 3037:  // Teleport portal
        case 3041:  // Teleport portal
        case 3045:  // Teleport portal
        case 3049:  // Teleport portal
        case 3050:  // Teleport portal
        case 3051:  // Teleport portal
        case 3052:  // Teleport portal
        case 3053:  // Teleport portal
        case 3054:  // Teleport portal
        case 3067:  // Teleport portal
        case 3055:  // Teleport portal
        case 3068:  // Teleport portal
        case 3056:  // Teleport portal
        case 3069:  // Teleport portal
        case 3057:  // Teleport portal
        case 3070:  // Teleport portal
        case 3058:  // Teleport portal
        case 3071:  // Teleport portal
        case 3059:  // Teleport portal
        case 3072:  // Teleport portal
        case 3060:  // Teleport portal
        case 3073:  // Teleport portal
        case 3061:  // Teleport portal
        case 3074:  // Teleport portal
        case 3062:  // Teleport portal
        case 3075:  // Teleport portal
        case 3063:  // Teleport portal
        case 3076:  // Teleport portal
        case 3064:  // Teleport portal
        case 3077:  // Teleport portal
        case 3065:  // Teleport portal
        case 3078:  // Teleport portal
        case 3066:  // Teleport portal
        case 3079:  // Teleport portal
        case 3090:  // Teleport portal
        case 3091:  // Teleport portal
        case 3080:  // Teleport portal
        case 3081:  // Teleport portal
        case 3082:  // Teleport portal
        case 3083:  // Teleport portal
        case 3084:  // Teleport portal
        case 3085:  // Teleport portal
        case 3087:  // Teleport portal
        case 3086:  // Teleport portal
        case 3089:  // Teleport portal
        case 3088:  // Teleport portal
        case 2911:  // Teleport portal
        {
            SetupTeleportPopup* popup = SetupTeleportPopup::create(
                (TeleportPortalObject*)obj,
                objects,
                objectID,
                settings->m_platformerMode);
            popup->show();
            return;
        }
        case 1615: { // Item ID
            SetItemIDLayer* popup = SetItemIDLayer::create((EffectGameObject*)obj, objects);
            popup->show();
            return;
        }
        case 2069: { // Object toggle
            SetupObjectTogglePopup* popup = SetupObjectTogglePopup::create(
                (EffectGameObject*)obj,
                objects,
                settings->m_platformerMode);
            popup->show();
            return;
        }
    }
    
    // Can allow multi activate
    if (canAllowMultiActivate(obj, objects)) {
        EditGameObjectPopup* popup = EditGameObjectPopup::create(
            (EffectGameObject*)obj,
            objects,
            settings->m_platformerMode);
        popup->show();
        return;
    }
    
    // Has rotate action
    if (hasRotateAction) {
        SetupRotatePopup* popup = SetupRotatePopup::create((EnhancedGameObject*)obj, objects);
        popup->show();
        return;
    }
    
    bool isInteractObject = (type != 1);
    bool allInteract = true;
    
    // Check if all selected objects are interact objects (type 30)
    if (isInteractObject) {
        for (unsigned int i = 0; i < objects->count(); ++i) {
            GameObject* o = (GameObject*)objects->objectAtIndex(i);
            if (o->getObjectType() != 30) {
                allInteract = false;
                break;
            }
        }
        if (allInteract) {
            SetupInteractObjectPopup* popup = SetupInteractObjectPopup::create((EffectGameObject*)obj, objects);
            popup->show();
            return;
        }
    }
    
    // Check for synced animation
    if (obj && obj->getHasSyncedAnimation()) {
        SetupAnimSettingsPopup* popup = SetupAnimSettingsPopup::create((EffectGameObject*)obj, nullptr);
        popup->show();
        return;
    }
    
    // Check for smart blocks in selection
    if (classType == 5 || (obj && obj->m_classType == Smart)) {
        SetupSmartBlockLayer* popup = SetupSmartBlockLayer::create((SmartGameObject*)obj, objects);
        popup->show();
        return;
    }
    
    // Check if all objects are of the same type and not monsters
    bool allSameType = true;
    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* o = (GameObject*)objects->objectAtIndex(i);
        if (o->m_objectID != 2065) {
            allSameType = false;
            break;
        }
    }
    
    if (!allSameType) {
        // Check if any object has synced animation
        for (unsigned int i = 0; i < objects->count(); ++i) {
            GameObject* o = (GameObject*)objects->objectAtIndex(i);
            if (o->canAllowMultiActivate()) {
                // Check if all objects can rotate
                bool allCanRotate = true;
                for (unsigned int j = 0; j < objects->count(); ++j) {
                    GameObject* o2 = (GameObject*)objects->objectAtIndex(j);
                    if (!o2->getHasRotateAction()) {
                        allCanRotate = false;
                        break;
                    }
                }
                if (allCanRotate) {
                    SetupRotatePopup* popup = SetupRotatePopup::create((EnhancedGameObject*)obj, objects);
                    popup->show();
                    return;
                }
                return;
            }
        }
        
        // All objects are the same type - use animation settings
        SetupAnimSettingsPopup* popup = SetupAnimSettingsPopup::create((EffectGameObject*)obj, objects);
        popup->show();
        return;
    }
    
    // Particle objects (monsters)
    if (objects->count() > 0) {
        CreateParticlePopup* popup = CreateParticlePopup::create(
            (CreateParticlePopup*)obj,
            (ParticleGameObject*)objects,
            (CCArray*)objectID);
        popup->show();
    }
}

void EditorUI::editGroup(cocos2d::CCObject* sender) {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return;
    
    SetGroupIDLayer* layer = SetGroupIDLayer::create(m_selectedObject, m_selectedObjects);
    layer->show();
}

void EditorUI::onGroupSticky(cocos2d::CCObject* sender) {
    if (m_selectedObjects->count() == 0) return;
    GJBaseGameLayer::groupStickyObjects(m_editorLayer, m_selectedObjects);
    updateButtons();
}

void EditorUI::onUngroupSticky(cocos2d::CCObject* sender) {
    if (m_selectedObjects->count() == 0) return;
    GJBaseGameLayer::ungroupStickyObjects(m_editorLayer, m_selectedObjects);
    updateButtons();
}

void EditorUI::onStickyToggle(cocos2d::CCObject* sender) {
    GameManager::sharedState()->toggleGameVariable("0180");
    m_linkControlsDisabled = GameManager::sharedState()->getGameVariable("0180");
    updateButtons();
}

void EditorUI::onCopyState(cocos2d::CCObject* sender) {
    if (m_selectedObject) {
        LevelEditorLayer::copyObjectState(m_editorLayer, m_selectedObject);
        updateButtons();
    }
}

void EditorUI::onPasteState(cocos2d::CCObject* sender) {
    if (m_selectedObject || m_selectedObjects->count() > 0) {
        LevelEditorLayer::pasteAttributeState(m_editorLayer, m_selectedObject, m_selectedObjects);
        updateObjectInfoLabel();
    }
}

void EditorUI::onPasteColor(cocos2d::CCObject* sender) {
    if (m_selectedObject || m_selectedObjects->count() > 0) {
        LevelEditorLayer::pasteColorState(m_editorLayer, m_selectedObject, m_selectedObjects);
        updateObjectInfoLabel();
    }
}

void EditorUI::onGoToLayer(cocos2d::CCObject* sender) {
    int layer = -1;
    if (m_selectedObject) {
        layer = m_selectedObject->m_editorLayer2;
        if (layer == 0 || layer == m_editorLayer->m_currentLayer) {
            layer = m_selectedObject->m_editorLayer;
        }
    } else if (m_selectedObjects->count() > 0) {
        int layer1 = editorLayerForArray(m_selectedObjects, false);
        int layer2 = editorLayerForArray(m_selectedObjects, true);
        if (layer1 > 0 && layer2 > 0) {
            layer = (m_editorLayer->m_currentLayer == layer2) ? layer1 : layer2;
        } else if (layer1 > 0) {
            layer = layer1;
        } else if (layer2 > 0) {
            layer = layer2;
        }
    }
    
    if (layer != -1) {
        m_editorLayer->m_currentLayer = layer;
        updateGroupIDLabel();
    }
}

void EditorUI::onGoToBaseLayer(cocos2d::CCObject* sender) {
    m_editorLayer->m_currentLayer = -1;
    updateGroupIDLabel();
}

void EditorUI::onGroupUp(cocos2d::CCObject* sender) {
    m_editorLayer->m_currentLayer++;
    updateGroupIDLabel();
}

void EditorUI::onGroupDown(cocos2d::CCObject* sender) {
    if (m_editorLayer->m_currentLayer > -1) {
        m_editorLayer->m_currentLayer--;
    }
    updateGroupIDLabel();
}

void EditorUI::updateGroupIDLabel() {
    const char* text = (m_editorLayer->m_currentLayer < 0) ? "All" :
        CCString::createWithFormat("%i", m_editorLayer->m_currentLayer)->getCString();
    
    m_currentLayerLabel->setString(text);
    m_currentLayerLabel->limitLabelWidth(38.0f, 0.6f, 0.0f);
    
    bool locked = LevelEditorLayer::isLayerLocked(m_editorLayer, m_editorLayer->m_currentLayer);
    m_layerLockSprite->setVisible(locked);
    m_currentLayerLabel->setColor(locked ? ccc3(255, 150, 0) : ccc3(255, 255, 255));
    
    updateGroupIDBtn2();
}

void EditorUI::updateGroupIDBtn2() {
    bool show = (m_editorLayer->m_currentLayer != -1);
    m_goToBaseBtn->setVisible(show);
}

void EditorUI::onLockLayer(cocos2d::CCObject* sender) {
    if (GameManager::sharedState()->getGameVariable("0103")) {
        LevelEditorLayer::toggleLockActiveLayer(m_editorLayer);
        updateGroupIDLabel();
    }
}

void EditorUI::onPlaytest(cocos2d::CCObject* sender) {
    if (m_isPaused) return;
    
    if (m_editorLayer->m_playbackMode == Playing) {
        m_isPlayingMusic = false;
    } else {
        GJBaseGameLayer::pauseAudio(m_editorLayer);
        m_editorLayer->m_audioPaused = true;
        m_isPlayingMusic = false;
        toggleMode(m_editModeBtn);
    }
    
    deselectAll();
    
    switch (m_editorLayer->m_playbackMode) {
        case Playing:
            LevelEditorLayer::onPausePlaytest(m_editorLayer);
            break;
        case Paused:
            LevelEditorLayer::onResumePlaytest(m_editorLayer);
            break;
        case Not:
            m_editorZoom = m_editorLayer->m_objectLayer->getScale();
            LevelEditorLayer::onPlaytest(m_editorLayer);
            break;
    }
    
    showUI(m_editorLayer->m_playbackMode != Playing);
    updatePlaybackBtn();
    m_playtestStopBtn->setVisible(true);
}

void EditorUI::onStopPlaytest(cocos2d::CCObject* sender) {
    showUI(true);
    if (m_isPlayingMusic) {
        onPlayback(nullptr);
    }
    LevelEditorLayer::onStopPlaytest(m_editorLayer);
    playtestStopped();
}

void EditorUI::playtestStopped() {
    showUI(true);
    m_playerTouchID1 = -1;
    m_playerTouchID2 = -1;
    m_playtestStopBtn->setVisible(false);
    updatePlaybackBtn();
    updateZoom(m_editorZoom);
}

void EditorUI::updateZoom(float zoom) {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCNode* parent = m_editorLayer->getParent();
    float parentScale = parent->getScale();
    
    parent->setScale(1.0f);
    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);
    CCPoint nodePos = m_editorLayer->m_objectLayer->convertToNodeSpace(center);
    
    m_editorLayer->m_objectLayer->setScale(zoom);
    
    CCPoint worldPos = m_editorLayer->m_objectLayer->convertToWorldSpace(nodePos);
    CCPoint delta = center - worldPos;
    CCPoint currentPos = m_editorLayer->m_objectLayer->getPosition();
    m_editorLayer->m_objectLayer->setPosition(currentPos + delta);
    
    constrainGameLayerPosition();
    parent->setScale(parentScale);
    updateSlider();
    
    m_rotationControl->setScale(1.0f / zoom);
    m_transformControl->scaleButtons(1.0f / zoom);
}

void EditorUI::showUI(bool show) {
    if (GameManager::sharedState()->getGameVariable("0046")) {
        show = true;
    }
    
    for (unsigned int i = 0; i < m_uiItems->count(); ++i) {
        CCNode* node = (CCNode*)m_uiItems->objectAtIndex(i);
        node->setVisible(show);
    }
    
    if (m_createButtonBar) {
        m_createButtonBar->setVisible(show && m_selectedMode == 2);
    }
    m_editButtonBar->setVisible(show && m_selectedMode == 3);
    m_deleteMenu->setVisible(show && m_selectedMode == 1);
}

void EditorUI::updatePlaybackBtn() {
    bool canPlay = (m_editorLayer->m_playbackMode == Not);
    const char* frame = (canPlay && m_isPlayingMusic) ? "GJ_stopMusicBtn_001.png" : "GJ_playMusicBtn_001.png";
    
    CCNode* node = m_playbackBtn->getNormalImage();
    if (node) {
        CCSpriteFrame* spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame);
        node->setDisplayFrame(spriteFrame);
    }
    
    m_playbackBtn->setEnabled(canPlay || m_isPlayingMusic);
    m_playbackBtn->setColor((canPlay || m_isPlayingMusic) ? ccc3(255, 255, 255) : ccc3(127, 127, 127));
}

void EditorUI::onPlayback(cocos2d::CCObject* sender) {
    if (m_isPaused || m_editorLayer->m_playbackMode != Not) return;
    
    if (!m_isPlayingMusic) {
        GJBaseGameLayer::pauseAudio(m_editorLayer);
        LevelEditorLayer::resetPlayback(m_editorLayer);
        m_playbackStartWarp = 0.0f;
        m_playbackStartTime = 0.0f;
    } else {
        LevelEditorLayer::stopPlayback(m_editorLayer);
    }
    
    m_isPlayingMusic = !m_isPlayingMusic;
    updatePlaybackBtn();
    
    if (m_isPlayingMusic) {
        FMODAudioEngine::sharedEngine()->updateReverb(Generic, 0);
        LevelEditorLayer::updateGameObjects(m_editorLayer);
        GJBaseGameLayer::processQueuedAudioTriggers(m_editorLayer);
    }
}

void EditorUI::toggleSwipe(cocos2d::CCObject* sender) {
    m_swipeEnabled = !m_swipeEnabled;
    ButtonSprite* sprite = (ButtonSprite*)m_swipeBtn->getNormalImage();
    sprite->updateBGImage(m_swipeEnabled ? "GJ_button_02.png" : "GJ_button_01.png");
    GameManager::sharedState()->setGameVariable("0003", m_swipeEnabled);
}

void EditorUI::toggleFreeMove(cocos2d::CCObject* sender) {
    m_freeMoveEnabled = !m_freeMoveEnabled;
    ButtonSprite* sprite = (ButtonSprite*)m_freeMoveBtn->getNormalImage();
    sprite->updateBGImage(m_freeMoveEnabled ? "GJ_button_02.png" : "GJ_button_01.png");
    GameManager::sharedState()->setGameVariable("0004", m_freeMoveEnabled);
}

void EditorUI::toggleSnap(cocos2d::CCObject* sender) {
    bool enabled = GameManager::sharedState()->getGameVariable("0008");
    GameManager::sharedState()->setGameVariable("0008", !enabled);
    
    ButtonSprite* sprite = (ButtonSprite*)m_snapBtn->getNormalImage();
    sprite->updateBGImage(enabled ? "GJ_button_01.png" : "GJ_button_02.png");
}

void EditorUI::onSettings(cocos2d::CCObject* sender) {
    if (m_isPaused) return;
    if (!m_editorLayer->m_level || m_editorLayer->m_level->m_levelType != Editor) return;
    
    LevelSettingsLayer* layer = LevelSettingsLayer::create(m_editorLayer->m_levelSettings, m_editorLayer);
    layer->m_delegate = &m_editorLayer->LevelSettingsDelegate;
    layer->show();
}

void EditorUI::onDeleteAll(cocos2d::CCObject* sender) {
    deselectAll();
    LevelEditorLayer::removeAllObjects(m_editorLayer);
}

void EditorUI::undoLastAction(cocos2d::CCObject* object) {
    LevelEditorLayer::undoLastAction(m_editorLayer);
    updateButtons();
    if (m_rotationControl && m_rotationControl->isVisible()) {
        m_canActivateControls = true;
        activateRotationControl(nullptr);
    }
    tryUpdateTimeMarkers();
    updateSpecialUIElements();
}

void EditorUI::redoLastAction(cocos2d::CCObject* sender) {
    LevelEditorLayer::redoLastAction(m_editorLayer);
    updateButtons();
    if (m_rotationControl && m_rotationControl->isVisible()) {
        m_canActivateControls = true;
        activateRotationControl(nullptr);
    }
    tryUpdateTimeMarkers();
    updateSpecialUIElements();
}

void EditorUI::tryUpdateTimeMarkers() {
    // Implementation would update time markers - empty func from robtop code
}

void EditorUI::onFindObject(cocos2d::CCObject* sender) {
    FindObjectPopup* popup = FindObjectPopup::create();
    popup->setTag(3);
    popup->m_delegate = &m_setIDPopupDelegate;
    popup->show();
}

void EditorUI::onDeleteInfo(cocos2d::CCObject* sender) {
    const char* text = "<co>Only remove certain objects.</c>\n"
                       "<cy>Static:</c> Blocks, Spikes...\n"
                       "<cg>Details</c>: Details, portals, triggers...\n"
                       "<cl>Custom</c>: Selected object\n"
                       "<cr>None:</c> Disabled";
    
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Delete Filters", text);
    alert->show();
}

void EditorUI::onNewCustomItem(cocos2d::CCObject* sender) {
    if (!m_selectedObject && m_selectedObjects->count() == 0) return;
    
    if (m_selectedObjects->count() > 1000) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Max Limit", 
            "A custom object cannot contain more than <cg>1000</c> objects.");
        alert->show();
        return;
    }
    
    if (GameManager::sharedState()->m_customObjectDict->count() > 200) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Max Custom Objects",
            "You cannot create more than <cy>200</c> custom objects.");
        alert->show();
        return;
    }
    
    if (m_selectedObject && m_selectedObject->m_objectID == 749) return;
    
    CCArray* objects = m_selectedObjects;
    if (!objects || objects->count() == 0) {
        objects = CCArray::createWithObject(m_selectedObject);
    }
    
    std::string data = copyObjects(objects, false, false);
    if (!data.empty() && data.length() > 0) {
        GameManager::sharedState()->addNewCustomObject(data);
        m_selectedObjectIndex = 0;
        reloadCustomItems();
    }
}

void EditorUI::onDeleteCustomItem(cocos2d::CCObject* sender) {
    if (m_selectedObjectIndex < 0) return;
    GameManager::sharedState()->removeCustomObject(m_selectedObjectIndex);
    m_selectedObjectIndex = 0;
    reloadCustomItems();
}

void EditorUI::orderUpCustomItem(cocos2d::CCObject* sender) {
    if (m_selectedObjectIndex < 0) return;
    int newIndex = GameManager::sharedState()->reorderKey(m_selectedObjectIndex, true);
    if (newIndex != m_selectedObjectIndex) {
        reloadCustomItems();
        m_selectedObjectIndex = newIndex;
        updateCreateMenu(false);
    }
}

void EditorUI::orderDownCustomItem(cocos2d::CCObject* sender) {
    if (m_selectedObjectIndex < 0) return;
    int newIndex = GameManager::sharedState()->reorderKey(m_selectedObjectIndex, false);
    if (newIndex != m_selectedObjectIndex) {
        reloadCustomItems();
        m_selectedObjectIndex = newIndex;
        updateCreateMenu(false);
    }
}

void EditorUI::reloadCustomItems() {
    int tab = GameManager::sharedState()->getIntGameVariable("0049");
    int page = GameManager::sharedState()->getIntGameVariable("0050");
    
    CCArray* items = createCustomItems();
    m_customTabBar->loadFromItems(items, tab, page, true);
}

void EditorUI::onResetSpecialFilter(cocos2d::CCObject* sender) {
    GameManager::sharedState()->setIntGameVariable("0133", 0);
    m_deleteGroupSprite->setString("0");
    GameManager::sharedState()->setIntGameVariable("0139", 0);
    m_deleteColorSprite->setString("0");
}

// Additional implementations for remaining methods...

void EditorUI::checkLiveColorSelect()
{
    // Check if color overlay is active
    if (m_colorOverlay)
    {
        closeLiveColorSelect();
        editColor();
        
        // Reselect objects after color editing
        if (m_selectedObject)
        {
            m_selectedObject->selectObject(ccc3(255, 0, 0));
        }
        else if (m_selectedObjects && m_selectedObjects->count() > 0)
        {
            for (unsigned int i = 0; i < m_selectedObjects->count(); ++i)
            {
                GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
                obj->selectObject(ccc3(255, 0, 0));
            }
        }
        
        // Schedule deselection after delay
        stopActionByTag(124);
        CCSequence* sequence = CCSequence::create(
            CCDelayTime::create(0.1f),
            CCCallFunc::create(this, callfunc_selector(EditorUI::deselectObjectsColor)),
            nullptr
        );
        sequence->setTag(124);
        runAction(sequence);
        return;
    }
    
    // Check if HSV overlay is active
    if (m_hsvOverlay)
    {
        closeLiveHSVSelect();
        editHSV();
        
        // Reselect objects after HSV editing
        if (m_selectedObject)
        {
            m_selectedObject->selectObject(ccc3(255, 0, 0));
        }
        else if (m_selectedObjects && m_selectedObjects->count() > 0)
        {
            for (unsigned int i = 0; i < m_selectedObjects->count(); ++i)
            {
                GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
                obj->selectObject(ccc3(255, 0, 0));
            }
        }
        
        // Schedule deselection after delay
        stopActionByTag(124);
        CCSequence* sequence = CCSequence::create(
            CCDelayTime::create(0.1f),
            CCCallFunc::create(this, callfunc_selector(EditorUI::deselectObjectsColor)),
            nullptr
        );
        sequence->setTag(124);
        runAction(sequence);
    }
}

void EditorUI::closeLiveColorSelect() {
    if (m_colorOverlay) {
        m_colorOverlay->closeColorSelect(false);
        m_colorOverlay = nullptr;
    }
}

void EditorUI::closeLiveHSVSelect() {
    if (m_hsvOverlay) {
        m_hsvOverlay->closeColorSelect(false);
        m_hsvOverlay = nullptr;
    }
}

void EditorUI::colorSelectClosed(cocos2d::CCNode* popup) {
    if (m_colorOverlay == popup) {
        bool closed = m_colorOverlay->m_closeButtonPressed;
        m_colorOverlay = nullptr;
        resetSelectedObjectsColor();
        if (closed) {
            updateEditColorButton();
        }
    } else if (m_hsvOverlay == popup) {
        bool closed = m_hsvOverlay->m_unkBool3;
        m_hsvOverlay = nullptr;
        resetSelectedObjectsColor();
        if (closed) {
            updateEditColorButton();
        }
    }
}

void EditorUI::resetSelectedObjectsColor() {
    if (m_selectedObject) {
        m_selectedObject->selectObject(ccc3(255, 0, 0));
    } else {
        for (unsigned int i = 0; i < m_selectedObjects->count(); ++i) {
            GameObject* obj = (GameObject*)m_selectedObjects->objectAtIndex(i);
            obj->selectObject(ccc3(255, 0, 0));
        }
    }
}

void EditorUI::toggleObjectInfoLabel() {
    bool show = GameManager::sharedState()->getGameVariable("0041");
    m_objectInfoLabel->setVisible(show);
    if (show) {
        updateObjectInfoLabel();
    }
}