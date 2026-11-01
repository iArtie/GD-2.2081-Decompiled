#include "Layers/EditLevelLayer.h"
#include "Models/GJGameLevel.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "Managers/LocalLevelManager.h"
#include "Layers/LevelEditorLayer.h"
#include "Layers/PlayLayer.h"
#include "Layers/LevelBrowserLayer.h"
#include "Layers/ShareLevelLayer.h"
#include "Popups/UploadActionPopup.h"
#include "Popups/SetFolderPopup.h"
#include "Layers/GameLevelOptionsLayer.h"
#include "Layers/LevelLeaderboard.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/TextArea.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "extension/CCScale9Sprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "Layers/FLAlertLayer.h"
#include "Managers/GJAccountManager.h"
#include <cctype>

EditLevelLayer* EditLevelLayer::create(GJGameLevel* level)
{
    EditLevelLayer* pRet = new EditLevelLayer();
    if (pRet && pRet->init(level))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

cocos2d::CCScene* EditLevelLayer::scene(GJGameLevel* level)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    EditLevelLayer* layer = EditLevelLayer::create(level);
    scene->addChild(layer);
    return scene;
}

bool EditLevelLayer::init(GJGameLevel* level)
{
    if (!cocos2d::CCLayer::init())
        return false;
    
    m_level = level;
    level->retain();
    m_descriptionPopup = nullptr;
    m_exiting = false;
    m_buttonMenu = nullptr;
    m_textInputs = cocos2d::CCArray::create();
    m_textInputs->retain();
    m_levelType = level->m_levelType;
    
    // Store level name
    if (!level->m_levelName.empty())
        m_levelName = level->m_levelName;
    else if (!level->m_tempName.empty())
        m_levelName = level->m_tempName;
    else
        m_levelName = "Unknown";
    
    // Set current level ID in GameManager
    GameManager::sharedState()->m_currentLevelID = level->m_M_ID;
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0, 0));
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().height);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().width);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccColor3B(102, 153, 255));
    
    // Side art
    cocos2d::CCSprite* sideArt1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt1->setAnchorPoint(cocos2d::CCPoint(0, 0));
    sideArt1->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    addChild(sideArt1, 1);
    
    cocos2d::CCSprite* sideArt2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt2->setAnchorPoint(cocos2d::CCPoint(1, 0));
    sideArt2->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    sideArt2->setFlipX(true);
    addChild(sideArt2, 1);
    
    // Level name input
    cocos2d::CCPoint namePos = cocos2d::CCPoint(centerX, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 30.0f);
    
    cocos2d::extension::CCScale9Sprite* nameBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0, 0, 80, 80));
    nameBg->setOpacity(255);
    nameBg->setContentSize(cocos2d::CCSize(365, 40));
    nameBg->setPosition(namePos);
    addChild(nameBg, -1);
    nameBg->setColor(cocos2d::ccColor3B(56, 141, 255));
    
    CCTextInputNode* nameInput = nullptr;
    
    // If level is uploaded or version > 1, show label instead of input
    if (level->m_levelVersion > 1 || level->m_isUploaded)
    {
        cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(m_levelName.c_str(), "bigFont.fnt");
        nameLabel->setPosition(namePos);
        addChild(nameLabel, 20);
    }
    else
    {
        nameInput = CCTextInputNode::create(300.0f, 50.0f, "Level Name", "Thonburi");
        nameInput->setDelegate(this);
        nameInput->setTag(1);
        nameInput->setPosition(namePos);
        nameInput->m_maxLabelLength = 20;
        nameInput->m_filterSwearWords = true;
        nameInput->setString(m_levelName);
        nameInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
        nameInput->setLabelPlaceholderScale(0.7f);
        addChild(nameInput, 18);
        m_textInputs->addObject(nameInput);
    }
    
    // Description input
    cocos2d::CCPoint descPos = cocos2d::CCPoint(centerX, centerY + 65.0f);
    
    cocos2d::extension::CCScale9Sprite* descBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0, 0, 80, 80));
    descBg->setOpacity(255);
    descBg->setContentSize(cocos2d::CCSize(365, 60));
    descBg->setPosition(descPos);
    addChild(descBg, -1);
    descBg->setColor(cocos2d::ccColor3B(56, 141, 255));
    
    CCTextInputNode* descInput = CCTextInputNode::create(365.0f, 50.0f, "Description [Optional]", "Thonburi");
    descInput->setDelegate(this);
    descInput->setTag(2);
    descInput->setPosition(descPos);
    descInput->m_maxLabelLength = 180;
    descInput->m_filterSwearWords = false;
    descInput->m_textField->setVisible(false);
    descInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");
    descInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
    addChild(descInput, 20);
    m_textInputs->addObject(descInput);
    
    // Load description
    std::string descText = level->getUnpackedLevelDescription();
    if (!descText.empty())
        descInput->setString(descText);
    
    // Menu buttons
    m_buttonMenu = cocos2d::CCMenu::create();
    addChild(m_buttonMenu);
    
    // Edit button
    cocos2d::CCSprite* editSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
    CCMenuItemSpriteExtra* editBtn = CCMenuItemSpriteExtra::create(
        editSprite, nullptr, this, menu_selector(EditLevelLayer::onEdit));
    m_buttonMenu->addChild(editBtn);
    
    // Play button
    cocos2d::CCSprite* playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    CCMenuItemSpriteExtra* playBtn = CCMenuItemSpriteExtra::create(
        playSprite, nullptr, this, menu_selector(EditLevelLayer::onPlay));
    m_buttonMenu->addChild(playBtn);
    
    // Share button
    cocos2d::CCSprite* shareSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_shareBtn_001.png");
    CCMenuItemSpriteExtra* shareBtn = CCMenuItemSpriteExtra::create(
        shareSprite, nullptr, this, menu_selector(EditLevelLayer::onShare));
    m_buttonMenu->addChild(shareBtn);
    
    m_buttonMenu->alignItemsHorizontallyWithPadding(20.0f);
    m_buttonMenu->setPosition(cocos2d::CCPoint(m_buttonMenu->getPosition().x, centerY - 25.0f));
    
    // Update description button for uploaded levels
    if (level->m_levelVersion > 1 || level->m_isUploaded)
    {
        cocos2d::CCSprite* updateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        CCMenuItemSpriteExtra* updateBtn = CCMenuItemSpriteExtra::create(
            updateSprite, nullptr, this, menu_selector(EditLevelLayer::onUpdateDescription));
        m_buttonMenu->addChild(updateBtn);
        updateBtn->setPosition(cocos2d::CCPoint(30.0f, descPos.y));
    }
    
    // Right side buttons
    cocos2d::CCMenu* sideMenu = cocos2d::CCMenu::create();
    addChild(sideMenu);
    
    cocos2d::CCPoint sidePos = cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 30.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 30.0f
    );
    
    // Delete button
    cocos2d::CCSprite* deleteSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
    CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSprite, nullptr, this, menu_selector(EditLevelLayer::confirmDelete));
    sideMenu->addChild(deleteBtn);
    deleteBtn->setPosition(sideMenu->convertToNodeSpace(sidePos));
    
    // Help button
    cocos2d::CCSprite* helpSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_helpBtn_001.png");
    float scale = deleteSprite->getContentSize().width / helpSprite->getContentSize().width;
    helpSprite->setScale(scale);
    CCMenuItemSpriteExtra* helpBtn = CCMenuItemSpriteExtra::create(
        helpSprite, nullptr, this, menu_selector(EditLevelLayer::onHelp));
    sideMenu->addChild(helpBtn);
    helpBtn->setPosition(deleteBtn->getPosition() + cocos2d::CCPoint(0, -60.0f));
    
    // Duplicate button
    cocos2d::CCSprite* dupSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png");
    CCMenuItemSpriteExtra* dupBtn = CCMenuItemSpriteExtra::create(
        dupSprite, nullptr, this, menu_selector(EditLevelLayer::confirmClone));
    sideMenu->addChild(dupBtn);
    dupBtn->setPosition(helpBtn->getPosition() + cocos2d::CCPoint(0, -60.0f));
    
    // Move to top button
    cocos2d::CCSprite* moveSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_orderUpBtn_001.png");
    CCMenuItemSpriteExtra* moveBtn = CCMenuItemSpriteExtra::create(
        moveSprite, nullptr, this, menu_selector(EditLevelLayer::confirmMoveToTop));
    sideMenu->addChild(moveBtn);
    moveBtn->setPosition(dupBtn->getPosition() + cocos2d::CCPoint(0, -60.0f));
    
    // Folder button
    cocos2d::CCSprite* folderSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
    
    m_folderLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", level->m_levelFolder)->getCString(), "bigFont.fnt");
    m_folderLabel->setPosition(
        folderSprite->getContentSize().width * 0.5f,
        folderSprite->getContentSize().height * 0.5f + 1.0f
    );
    m_folderLabel->limitLabelWidth(24.0f, 0.55f, 0.0f);
    folderSprite->addChild(m_folderLabel, 1);
    
    CCMenuItemSpriteExtra* folderBtn = CCMenuItemSpriteExtra::create(
        folderSprite, nullptr, this, menu_selector(EditLevelLayer::onSetFolder));
    folderBtn->setSizeMult(1.2f);
    sideMenu->addChild(folderBtn);
    folderBtn->setPosition(sideMenu->convertToNodeSpace(cocos2d::CCPoint(30.0f, descPos.y - 60.0f)));
    
    // Level Leaderboard for platformer levels
    if (GJGameLevel::isPlatformer(level))
    {
        cocos2d::CCSprite* leaderboardSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_levelLeaderboardBtn_001.png");
        leaderboardSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* leaderboardBtn = CCMenuItemSpriteExtra::create(
            leaderboardSprite, nullptr, this, menu_selector(EditLevelLayer::onLevelLeaderboard));
        sideMenu->addChild(leaderboardBtn);
        leaderboardBtn->setPosition(folderBtn->getPosition() + cocos2d::CCPoint(0, -50.0f));
    }
    
    // Back button
    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(EditLevelLayer::onBack));
    backBtn->setSizeMult(1.6f);
    
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f
    ));
    
    // Bottom menu buttons
    cocos2d::CCMenu* bottomMenu = cocos2d::CCMenu::create();
    addChild(bottomMenu, 2);
    
    // Info button
    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite, nullptr, this, menu_selector(EditLevelLayer::onLevelInfo));
    infoBtn->setSizeMult(2.0f);
    bottomMenu->addChild(infoBtn);
    bottomMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 30.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f
    ));
    
    // Guidelines button
    cocos2d::CCSprite* guidelinesSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
    guidelinesSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* guidelinesBtn = CCMenuItemSpriteExtra::create(
        guidelinesSprite, nullptr, this, menu_selector(EditLevelLayer::onGuidelines));
    guidelinesBtn->setSizeMult(1.2f);
    bottomMenu->addChild(guidelinesBtn);
    guidelinesBtn->setPosition(bottomMenu->convertToNodeSpace(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 30.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f
    )));
    
    // Options button
    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, this, menu_selector(EditLevelLayer::onLevelOptions));
    bottomMenu->addChild(optionsBtn);
    optionsBtn->setPosition(bottomMenu->convertToNodeSpace(cocos2d::CCPoint(30.0f, 70.0f)));
    
    // Controller support
    if (PlatformToolbox::isControllerConnected())
    {
        GameToolbox::addBackButton(this, backBtn);
        cocos2d::CCSprite* controllerSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        playSprite->addChild(controllerSprite);
        controllerSprite->setPosition(cocos2d::CCPoint(
            playSprite->getContentSize().width * 0.5f,
            playSprite->getContentSize().height * 0.5f - 40.0f
        ));
    }
    
    setupLevelInfo();
    
    return true;
}

void EditLevelLayer::keyBackClicked()
{
    onBack(nullptr);
}

void EditLevelLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == KEY_Space || key == CONTROLLER_Start)
        onPlay(nullptr);
    else
        cocos2d::CCLayer::keyDown(key, timestamp);
}

void EditLevelLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    int tag = layer->getTag(layer);
    
    if (tag == 4 && btn2)
    {
        onDelete();
        return;
    }
    if (tag == 7 && btn2)
    {
        onClone();
        return;
    }
    if (tag == 8 && btn2)
    {
        onMoveToTop();
        return;
    }
    if (tag == 10)
    {
        onShare(nullptr);
        return;
    }
    if (tag == 5)
    {
        if (!btn2)
        {
            onEdit(nullptr);
            return;
        }
        goto openEditorGuide;
    }
    if (tag == 9 && btn2)
    {
        closeTextInputs();
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;
        bool updated = GameLevelManager::sharedState()->updateDescription(
            m_level->m_levelID.random - m_level->m_levelID.seed,
            m_level->m_levelDesc
        );
        if (updated)
        {
            if (m_descriptionPopup)
            {
                m_descriptionPopup->setDelegate(nullptr);
                m_descriptionPopup = nullptr;
            }
            UploadActionPopup* popup = UploadActionPopup::create(this, "Updating description...");
            m_descriptionPopup = popup;
            popup->show();
        }
        return;
    }
    if (tag == 6 && btn2)
    {
openEditorGuide:
        GameManager::sharedState()->openEditorGuide();
    }
}

void EditLevelLayer::closeTextInputs()
{
    for (unsigned int i = 0; i < m_textInputs->count(); i++)
    {
        CCTextInputNode* node = (CCTextInputNode*)m_textInputs->objectAtIndex(i);
        if (node)
            node->onClickTrackNode(false);
    }
}

void EditLevelLayer::confirmClone(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Clone Level",
        "Create a <cl>copy</c> of this <cg>level</c>?",
        "No",
        "Yes"
    );
    alert->setTag(7);
    alert->show();
}

void EditLevelLayer::confirmDelete(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Delete Level",
        "Are you sure you want to <cr>delete</c> this level?\n<cy>This action cannot be undone.</c>",
        "No",
        "Yes"
    );
    alert->setTag(4);
    alert->show();
}

void EditLevelLayer::confirmMoveToTop(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Move to Top",
        "Move this level to the top of the created levels list?",
        "No",
        "Yes"
    );
    alert->setTag(8);
    alert->show();
}

void EditLevelLayer::onBack(cocos2d::CCObject* sender)
{
    if (cocos2d::CCDirector::sharedDirector()->isTransitioning())
        return;
    
    closeTextInputs();
    verifyLevelName();
    setKeypadEnabled(false);
    
    cocos2d::CCScene* scene;
    GameLevelManager* manager = GameLevelManager::sharedState();
    
    if (manager->m_returnToLocalLevels)
    {
        manager->m_returnToLocalLevels = false;
        GJSearchObject* searchObj = GJSearchObject::create(GJSearchObject::Type::MyLevels);
        scene = LevelBrowserLayer::scene(searchObj);
    }
    else
    {
        std::string searchKey = manager->m_searchKey;
        scene = manager->getSearchScene(searchKey.c_str());
    }
    
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void EditLevelLayer::onClone()
{
    if (cocos2d::CCDirector::sharedDirector()->isTransitioning() || m_exiting)
        return;
    
    setKeypadEnabled(false);
    m_exiting = true;
    GameManager::sharedState()->m_sceneEnum = 2;
    
    GJGameLevel* newLevel = GameLevelManager::sharedState()->createNewLevel();
    newLevel->copyLevelInfo(m_level);
    
    int originalID = (m_level->m_originalLevel.random == m_level->m_originalLevel.seed) ? 0 :
                     m_level->m_originalLevel.random - m_level->m_originalLevel.seed;
    newLevel->m_originalLevel.value = originalID;
    newLevel->m_originalLevel.seed = (int)(((float)rand() * 4.6566e-10f) * 1000.0f);
    newLevel->m_originalLevel.random = originalID + newLevel->m_originalLevel.seed;
    
    cocos2d::CCScene* scene = EditLevelLayer::scene(newLevel);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void EditLevelLayer::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();
    m_descriptionPopup = nullptr;
}

void EditLevelLayer::onDelete()
{
    m_buttonMenu->setEnabled(false);
    GameLevelManager::sharedState()->deleteLevel(m_level);
    onBack(nullptr);
}

void EditLevelLayer::onEdit(cocos2d::CCObject* sender)
{
    if (cocos2d::CCDirector::sharedDirector()->isTransitioning() || m_exiting)
        return;
    
    closeTextInputs();
    setKeypadEnabled(false);
    m_exiting = true;
    GameManager::sharedState()->m_sceneEnum = 3;
    verifyLevelName();
    
    FMODAudioEngine::sharedEngine()->loadMusic(
        m_level->getAudioFileName(),
        1.0f, 0.0f, 1.0f, 1, false, false, false
    );
    
    cocos2d::CCScene* scene = LevelEditorLayer::scene(m_level, 0);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void EditLevelLayer::onGuidelines(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Upload Guidelines",
        "Hateful, abusive, or otherwise inappropriate content, whether it is in the level, name, or description may lead to <cr>deletion</c>. The same applies to levels abusing cheats or exploits. Stealing assets from other levels is <co>discouraged</c> and may stop a level from being rated depending on the context, severity, credit, and more. Levels verified with hacks cannot be rated.\n<cy>These guidelines and their enforcement are entirely at RobTop's discretion.</c>"
    );
    alert->show();
}

void EditLevelLayer::onHelp(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Editor Guide",
        "Do you want to open the editor guide now?",
        "No",
        "Yes"
    );
    alert->setTag(6);
    alert->show();
}

void EditLevelLayer::onLevelInfo(cocos2d::CCObject* sender)
{
    int attempts = m_level->m_attempts.random - m_level->m_attempts.seed;
    int jumps = m_level->m_jumps.random - m_level->m_jumps.seed;
    int objects = m_level->m_objectCount.random - m_level->m_objectCount.seed;
    
    std::string infoText = "<cy>" + m_levelName + "</c>\n<cg>Total Attempts</c>: " + std::to_string(attempts) +
                           "\n<cl>Total Jumps</c>: " + std::to_string(jumps) +
                           "\n<cp>Objects</c>: " + std::to_string(objects);
    
    if (objects > 40000)
        infoText += " <cr>(high)</c>";
    
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Level Stats", infoText);
    alert->show();
}

void EditLevelLayer::onLevelLeaderboard(cocos2d::CCObject* sender)
{
    LevelLeaderboardMode mode = (LevelLeaderboardMode)GameManager::sharedState()->getIntGameVariable("0164");
    LevelLeaderboard* leaderboard = LevelLeaderboard::create(m_level, Local, mode);
    leaderboard->show();
}

void EditLevelLayer::onLevelOptions(cocos2d::CCObject* sender)
{
    GameLevelOptionsLayer* options = GameLevelOptionsLayer::create(m_level);
    options->show();
}

void EditLevelLayer::onMoveToTop()
{
    LocalLevelManager::sharedState()->moveLevelToTop(m_level);
}

void EditLevelLayer::onPlay(cocos2d::CCObject* sender)
{
    if (cocos2d::CCDirector::sharedDirector()->isTransitioning() || m_exiting)
        return;
    
    closeTextInputs();
    setKeypadEnabled(false);
    m_exiting = true;
    
    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);
    verifyLevelName();
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(EditLevelLayer::playStep2)),
        nullptr
    ));
}

void EditLevelLayer::playStep2()
{
    FMODAudioEngine::sharedEngine()->loadMusic(
        m_level->getAudioFileName(),
        1.0f, 0.0f, 1.0f, 1, false, false, false
    );
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(EditLevelLayer::playStep3)),
        nullptr
    ));
}

void EditLevelLayer::playStep3()
{
    GameManager::sharedState()->m_sceneEnum = 3;
    cocos2d::CCScene* scene = PlayLayer::scene(m_level, false, false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void EditLevelLayer::onSetFolder(cocos2d::CCObject* sender)
{
    if (!m_level)
        return;
    
    SetFolderPopup* popup = SetFolderPopup::create(m_level->m_levelFolder, true, "Set Folder");
    popup->setDelegate(this);
    popup->show();
}

void EditLevelLayer::onShare(cocos2d::CCObject* sender)
{
    GJAccountManager* accountManager = GJAccountManager::sharedState();
    
    if (accountManager->m_accountID - accountManager->m_unkInt1 <= 0)
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            nullptr,
            "Account Needed",
            "You need an <cl>account</c> to share <cg>levels</c>. You can create one for free from settings in the main menu."
        );
        alert->show();
        return;
    }
    
    // Check if level has content
    if (m_level->m_levelString.empty())
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            nullptr,
            "Empty",
            "You cannot share an empty level."
        );
        alert->show();
        return;
    }
    
    // Check if level has name
    if (m_levelName.empty())
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            nullptr,
            "No Name",
            "You must name your level before sharing it."
        );
        alert->show();
        return;
    }
    
    // Show guidelines if not seen before
    if (!GameManager::sharedState()->getGameVariable("0157"))
    {
        GameManager::sharedState()->setGameVariable("0157", true);
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Upload Guidelines",
            "Hateful, abusive, or otherwise inappropriate content, whether it is in the level, name, or description may lead to <cr>deletion</c>. The same applies to levels abusing cheats or exploits. Stealing assets from other levels is <co>discouraged</c> and may stop a level from being rated depending on the context, severity, credit, and more. Levels verified with hacks cannot be rated.\n<cy>These guidelines and their enforcement are entirely at RobTop's discretion.</c>"
        );
        alert->setTag(10);
        alert->show();
        return;
    }
    
    closeTextInputs();
    ShareLevelLayer* shareLayer = ShareLevelLayer::create(m_level);
    shareLayer->show();
}

void EditLevelLayer::onTest(cocos2d::CCObject* sender)
{
    closeTextInputs();
    GameManager::sharedState()->m_sceneEnum = 3;
    cocos2d::CCScene* scene = PlayLayer::scene(m_level, false, false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void EditLevelLayer::onUpdateDescription(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "Update Level Desc",
        "Do you want to <cg>update</c> the level <cl>description</c>?",
        "No",
        "Yes"
    );
    alert->setTag(9);
    alert->show();
}

void EditLevelLayer::setIDPopupClosed(SetIDPopup* popup, int value)
{
    if (value < 0) value = 0;
    if (value >= 1000) value = 999;
    
    m_level->m_levelFolder = value;
    m_folderLabel->setString(cocos2d::CCString::createWithFormat("%i", value)->getCString());
    m_folderLabel->limitLabelWidth(24.0f, 0.55f, 0.0f);
}

void EditLevelLayer::setupLevelInfo()
{
    // Setup level info UI (length, song, verified status, etc.)
    // This would display level metadata in the UI
}

void EditLevelLayer::textChanged(CCTextInputNode* node)
{
    int tag = node->getTag(tag);
    
    if (tag == 1)
    {
        const char* text = node->getString();
        m_level->m_levelName = text;
    }
    else if (tag == 2)
    {
        const char* text = node->getString();
        updateDescText(text);
    }
}

void EditLevelLayer::textInputClosed(CCTextInputNode* node)
{
    int tag = node->getTag();
    
    if (tag == 1)
    {
        const char* text = node->getString();
        m_level->m_levelName = text;
        node->updateLabel(text);
    }
    else if (tag == 2)
    {
        const char* text = node->getString();
        m_level->m_levelDesc = LevelTools::base64EncodeString(text);
        updateDescText(text);
    }
}

void EditLevelLayer::textInputOpened(CCTextInputNode* node)
{
    int tag = node->getTag();
    
    if (tag == 1)
    {
        if (node->getString() && strcmp(node->getString(), "") == 0)
        {
            node->setString(" ");
        }
    }
}

void EditLevelLayer::updateDescText(const char* text)
{
    // Update description display
}

void EditLevelLayer::uploadActionFailed(int id, int response)
{
    if (!m_level || id != m_level->m_levelID.random - m_level->m_levelID.seed)
        return;
    
    if (m_descriptionPopup)
    {
        m_descriptionPopup->showFailMessage("Failed. Please try again later.");
        m_descriptionPopup->setDelegate(nullptr);
        m_descriptionPopup = nullptr;
    }
}

void EditLevelLayer::uploadActionFinished(int id, int response)
{
    if (!m_level || id != m_level->m_levelID.random - m_level->m_levelID.seed || !m_descriptionPopup)
        return;
    
    m_descriptionPopup->showSuccessMessage("Description updated!");
    m_descriptionPopup->setDelegate(nullptr);
    m_descriptionPopup = nullptr;
}

void EditLevelLayer::verifyLevelName()
{
    if (m_levelName.empty())
    {
        if (!m_level->m_tempName.empty())
            m_level->m_levelName = m_level->m_tempName;
        else
            m_level->m_levelName = "Unknown";
    }
    
    // Check for duplicate names
    int rev = 0;
    cocos2d::CCArray* levels = LocalLevelManager::sharedState()->getAllLevelsWithName(m_level->m_levelName);
    
    if (levels && levels->count() > 1)
    {
        for (unsigned int i = 0; i < levels->count(); i++)
        {
            GJGameLevel* lvl = (GJGameLevel*)levels->objectAtIndex(i);
            if (lvl && lvl->m_levelRev > rev)
                rev = lvl->m_levelRev;
        }
        m_level->m_levelRev = rev + 1;
    }
    else
    {
        m_level->m_levelRev = 0;
    }
}

EditLevelLayer::~EditLevelLayer()
{
    CC_SAFE_RELEASE(m_level);
    CC_SAFE_RELEASE(m_textInputs);
}