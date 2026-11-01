#include "CreateGuidelinesLayer.h"
#include "LevelSettingsObject.h"
#include "FMODAudioEngine.h"
#include "ButtonSprite.h"
#include "TextArea.h"
#include "GameToolbox.h"
#include "extension/CCScale9Sprite.h"

CreateGuidelinesLayer* CreateGuidelinesLayer::create(CustomSongDelegate* delegate, AudioGuidelinesType type) {
    CreateGuidelinesLayer* pRet = new CreateGuidelinesLayer();
    if (pRet && pRet->init(delegate, type)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CreateGuidelinesLayer::init(CustomSongDelegate* delegate, AudioGuidelinesType type) {
    if (!FLAlertLayer::init(75)) {
        return false;
    }
    
    m_delegate = delegate;
    m_type = type;
    m_recording = false;
    m_elapsed = 0.0f;
    m_guidelines = 0;
    m_guidelineString = "";
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Arrays for toggling visibility
    m_nonRecordingObjects = cocos2d::CCArray::create();
    m_nonRecordingObjects->retain();
    m_recordingObjects = cocos2d::CCArray::create();
    m_recordingObjects->retain();
    
    // Button menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(440, 290));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Title
    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(centerX, centerY + 145.0f - 20.0f));
    title->setScale(0.8f);
    m_mainLayer->addChild(title);
    m_nonRecordingObjects->addObject(title);
    
    // Recording title (flashing)
    cocos2d::CCLabelBMFont* recordingTitle = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    recordingTitle->setPosition(cocos2d::CCPoint(centerX, centerY + 145.0f - 20.0f));
    recordingTitle->setScale(0.8f);
    recordingTitle->setColor(cocos2d::ccColor3B(255, 100, 0));
    m_mainLayer->addChild(recordingTitle);
    m_recordingObjects->addObject(recordingTitle);
    
    // Flashing animation for recording title
    cocos2d::CCAction* flashAction = cocos2d::CCRepeatForever::create(
        cocos2d::CCSequence::create(
            cocos2d::CCTintTo::create(1.0f, 255, 150, 0),
            cocos2d::CCTintTo::create(1.0f, 255, 100, 0),
            nullptr
        )
    );
    recordingTitle->runAction(flashAction);
    
    // Set titles based on type
    std::string titleText, descText1, descText2;
    if (type == BPMFinder) {
        titleText = "BPM Finder";
        descText1 = "- Find the BPM of a song.\n- Tap to the beat.\n- Recording starts on first tap.";
        descText2 = "- Tap on the screen to the beat.\n- Tap on the stop button to finish.";
    } else {
        titleText = "Guideline Creator";
        descText1 = "- Create guidelines for custom songs.\n"
                    "- Each tap will be shown as a line in the editor.\n"
                    "- Multiple recordings will be merged into one.\n"
                    "- Use the clear button to reset the guidelines.";
        descText2 = "- Tap on the screen to create markers.\n- Tap on the stop button to finish.";
    }
    title->setString(titleText.c_str());
    recordingTitle->setString(titleText.c_str());
    
    // Description text areas
    cocos2d::CCPoint titlePos = title->getPosition();
    
    TextArea* desc1 = TextArea::create(descText1, "goldFont.fnt", 1.0f, 9999.0f, 
                                        cocos2d::CCPoint(28, 0), 0.0f, false);
    desc1->setScale(0.5f);
    desc1->setAnchorPoint(cocos2d::CCPoint(0, 1));
    m_mainLayer->addChild(desc1);
    desc1->setPosition(cocos2d::CCPoint(titlePos.x - 155.0f, titlePos.y - 20.0f));
    m_nonRecordingObjects->addObject(desc1);
    
    TextArea* desc2 = TextArea::create(descText2, "goldFont.fnt", 1.0f, 9999.0f,
                                        cocos2d::CCPoint(28, 0), 0.0f, false);
    desc2->setScale(0.5f);
    desc2->setAnchorPoint(cocos2d::CCPoint(0, 1));
    m_mainLayer->addChild(desc2);
    desc2->setPosition(cocos2d::CCPoint(titlePos.x - 155.0f, titlePos.y - 20.0f));
    m_recordingObjects->addObject(desc2);
    
    // Clear button (not for BPM Finder)
    if (type != BPMFinder) {
        ButtonSprite* clearBtnSprite = ButtonSprite::create("Clear", 0, 0, 0.6f);
        CCMenuItemSpriteExtra* clearBtn = CCMenuItemSpriteExtra::create(
            clearBtnSprite, nullptr, this, menu_selector(CreateGuidelinesLayer::onClearGuidelines));
        m_buttonMenu->addChild(clearBtn);
        clearBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 75.0f, centerY)));
        m_nonRecordingObjects->addObject(clearBtn);
    }
    
    // Offset input
    cocos2d::CCPoint inputPos = cocos2d::CCPoint(centerX + 75.0f, centerY);
    
    // Background for input
    cocos2d::extension::CCScale9Sprite* inputBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0, 0, 40, 40));
    inputBg->setOpacity(100);
    inputBg->setContentSize(cocos2d::CCSize(100, 30));
    m_mainLayer->addChild(inputBg, -1);
    inputBg->setPosition(inputPos);
    m_nonRecordingObjects->addObject(inputBg);
    
    // Offset label
    cocos2d::CCLabelBMFont* offsetLabel = cocos2d::CCLabelBMFont::create("Offset", "bigFont.fnt");
    offsetLabel->setScale(0.4f);
    offsetLabel->setPosition(cocos2d::CCPoint(inputPos.x, inputPos.y + 25.0f));
    m_mainLayer->addChild(offsetLabel);
    m_nonRecordingObjects->addObject(offsetLabel);
    
    // Text input
    m_offsetInput = CCTextInputNode::create(100.0f, 50.0f, "Seconds", "Thonburi");
    m_offsetInput->setPosition(inputPos);
    m_offsetInput->setMaxLabelScale(0.7f);
    m_offsetInput->setMaxLabelWidth(200.0f);
    m_offsetInput->m_textField->setVisible(false);
    m_offsetInput->m_maxLabelLength = 3;
    m_offsetInput->setAllowedChars("0123456789.");
    m_offsetInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
    m_offsetInput->setLabelPlaceholderScale(0.5f);
    m_mainLayer->addChild(m_offsetInput, 20);
    m_nonRecordingObjects->addObject(m_offsetInput);
    
    // Record button
    ButtonSprite* recordSprite = ButtonSprite::create("Record");
    CCMenuItemSpriteExtra* recordBtn = CCMenuItemSpriteExtra::create(
        recordSprite, nullptr, this, menu_selector(CreateGuidelinesLayer::onRecord));
    recordBtn->setTag(1344);
    m_buttonMenu->addChild(recordBtn);
    recordBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX, centerY - 145.0f + 50.0f)));
    m_nonRecordingObjects->addObject(recordBtn);
    
    // Stop button
    ButtonSprite* stopSprite = ButtonSprite::create("Stop");
    CCMenuItemSpriteExtra* stopBtn = CCMenuItemSpriteExtra::create(
        stopSprite, nullptr, this, menu_selector(CreateGuidelinesLayer::onStop));
    m_buttonMenu->addChild(stopBtn);
    stopBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 220.0f - 40.0f, centerY + 145.0f - 25.0f)));
    m_recordingObjects->addObject(stopBtn);
    
    // Info label
    m_infoLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_infoLabel->setPosition(cocos2d::CCPoint(centerX, centerY - 145.0f + 90.0f));
    m_infoLabel->setScale(0.5f);
    m_infoLabel->setColor(cocos2d::ccColor3B(200, 200, 200));
    m_mainLayer->addChild(m_infoLabel);
    m_nonRecordingObjects->addObject(m_infoLabel);
    
    // Circle indicator
    m_circleSprite = cocos2d::CCSprite::createWithSpriteFrameName("d_ball_01_001.png");
    m_circleSprite->setPosition(cocos2d::CCPoint(centerX - 220.0f + 30.0f, centerY + 145.0f - 30.0f));
    m_circleSprite->setScale(0.8f);
    m_circleSprite->setColor(cocos2d::ccColor3B(255, 150, 0));
    m_mainLayer->addChild(m_circleSprite);
    m_recordingObjects->addObject(m_circleSprite);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(CreateGuidelinesLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    closeBtn->setTag(1304);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 220.0f + 10.0f, centerY + 145.0f - 10.0f)));
    m_nonRecordingObjects->addObject(closeBtn);
    
    // Info button (not for BPM Finder)
    if (type != BPMFinder) {
        cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
            infoSprite, nullptr, this, menu_selector(CreateGuidelinesLayer::onInfo));
        infoBtn->setSizeMult(2.0f);
        infoBtn->setTag(1336);
        m_buttonMenu->addChild(infoBtn);
        infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 220.0f - 17.0f, centerY + 145.0f - 17.0f)));
        m_nonRecordingObjects->addObject(infoBtn);
    }
    
    // Start in non-recording state
    toggleItems(false);
    
    return true;
}

void CreateGuidelinesLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (layer->getTag() == 1 && btn2) {
        doClearGuidelines();
    }
}

void CreateGuidelinesLayer::doClearGuidelines() {
    m_infoLabel->setString("Guidelines cleared");
    if (m_delegate) {
        LevelSettingsObject* settings = m_delegate->getLevelSettings();
        settings->m_guidelineString = "";
        settings->m_guidelinesUpdated = true;
    }
}

void CreateGuidelinesLayer::keyBackClicked() {
    if (!m_recording) {
        onClose(nullptr);
    }
}

void CreateGuidelinesLayer::keyUp(cocos2d::enumKeyCodes key, double timestamp) {
    // Empty implementation
}

void CreateGuidelinesLayer::onClearGuidelines(cocos2d::CCObject* sender) {
    FLAlertLayer* alert = FLAlertLayer::create(
        this, "Clear Guidelines", "<cr>Delete</c> the current <cl>guidelines</c>?", "No", "Yes");
    alert->setTag(1);
    alert->show();
}

void CreateGuidelinesLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CreateGuidelinesLayer::onInfo(cocos2d::CCObject* sender) {
    FLAlertLayer* alert = FLAlertLayer::create(
        nullptr,
        "Guidelines",
        "Guidelines can be used to make it easier to syncronize music and gameplay. Create your own custom song guidelines using this interface. Tap on record to start recording, then tap on the screen to create markings. Press on the stop button to stop recording and save your created guidelines."
    );
    alert->show();
}

void CreateGuidelinesLayer::onRecord(cocos2d::CCObject* sender) {
    m_offsetInput->onClickTrackNode(false);
    float offset = atof(m_offsetInput->getString());
    
    toggleItems(true);
    m_offsetInput->setTouchEnabled(false);
    m_elapsed = offset;
    m_guidelineString = "";
    m_guidelines = 0;
    
    m_circleSprite->setColor(cocos2d::ccColor3B(255, 150, 0));
    m_circleSprite->setScale(0.8f);
    
    playMusic();
    m_recording = true;
    scheduleUpdate();
}

void CreateGuidelinesLayer::onStop(cocos2d::CCObject* sender) {
    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    toggleItems(false);
    m_offsetInput->setTouchEnabled(true);
    recordingDidStop();
    m_recording = false;
    unscheduleUpdate();
}

void CreateGuidelinesLayer::playMusic() {
    std::string filename = m_delegate->getSongFileName();
    FMODAudioEngine::sharedEngine()->loadAndPlayMusic(
        filename, (unsigned int)(m_elapsed * 1000.0f), 0);
}

void CreateGuidelinesLayer::recordingDidStop() {
    if (!m_guidelineString.empty()) {
        m_infoLabel->setString("Guidelines saved");
        
        LevelSettingsObject* settings = m_delegate->getLevelSettings();
        if (!settings->m_guidelineString.empty()) {
            std::string merged = getMergedRecordString(settings->m_guidelineString, m_guidelineString);
            settings->m_guidelineString = merged;
        } else {
            settings->m_guidelineString = m_guidelineString;
        }
        settings->m_guidelinesUpdated = true;
    } else {
        m_infoLabel->setString("");
    }
    
    if (m_guidelines > 1000) {
        m_infoLabel->setString("Maximum limit exceeded");
    }
    m_guidelines = 0;
}

void CreateGuidelinesLayer::registerTouch() {
    std::string timeStr = cocos2d::CCString::createWithFormat("%0.2f~0~", m_elapsed)->getCString();
    m_guidelineString += timeStr;
}

void CreateGuidelinesLayer::registerWithTouchDispatcher() {
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, 0, true);
}

void CreateGuidelinesLayer::toggleItems(bool visible) {
    bool hideNonRecording = !visible;
    
    // Hide non-recording objects
    for (unsigned int i = 0; i < m_nonRecordingObjects->count(); i++) {
        cocos2d::CCObject* obj = m_nonRecordingObjects->objectAtIndex(i);
        ((cocos2d::CCNode*)obj)->setVisible(hideNonRecording);
    }
    
    // Show recording objects
    for (unsigned int i = 0; i < m_recordingObjects->count(); i++) {
        cocos2d::CCObject* obj = m_recordingObjects->objectAtIndex(i);
        ((cocos2d::CCNode*)obj)->setVisible(visible);
    }
}

void CreateGuidelinesLayer::update(float dt) {
    m_elapsed += dt;
}

bool CreateGuidelinesLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_recording && m_guidelines <= 1000) {
        registerTouch();
        m_circleSprite->setColor(cocos2d::ccColor3B(255, 255, 0));
        m_circleSprite->setScale(0.9f);
        m_guidelines++;
        return true;
    }
    return true;
}

void CreateGuidelinesLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // Empty implementation
}

void CreateGuidelinesLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_recording) {
        m_circleSprite->setColor(cocos2d::ccColor3B(255, 150, 0));
        m_circleSprite->setScale(0.8f);
    }
}

CreateGuidelinesLayer::~CreateGuidelinesLayer() {
    CC_SAFE_RELEASE(m_nonRecordingObjects);
    CC_SAFE_RELEASE(m_recordingObjects);
}