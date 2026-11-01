#include "MoreOptionsLayer.h"
#include "FLAlertLayer.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "PlatformToolbox.h"
#include "FMODAudioEngine.h"
#include "KeybindingsLayer.h"
#include "ParentalOptionsLayer.h"
#include "GJSongBrowser.h"
#include "GJDropDownLayer.h"
#include "CCTextInputNode.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "ButtonSprite.h"
#include "CCScale9Sprite.h"

void MoreOptionsLayer::addToggle(const char* label, const char* key, const char* description)
{
    m_toggleCount++;

    int page = (int)floorf((float)(m_toggleCount / 10));
    if (page >= m_pageCount)
        m_pageCount = page;

    int keyID = atoi(key);
    GameManager* gm = GameManager::sharedState();
    bool value = gm->getGameVariable(key);

    m_objects->setObject(cocos2d::CCString::create(key), keyID);

    cocos2d::CCPoint pos = nextPosition(page);
    cocos2d::CCLayer* layer = layerForPage(page);

    CCMenuItemToggler* toggler = GameToolbox::createToggleButton(
        label, this, menu_selector(MoreOptionsLayer::onToggle),
        0, value, m_buttonMenu, pos, this, layer,
        0.8f, 0.4f, 130.0f, ccp(8.0f, 0.0f), "bigFont.fnt", false, false, nullptr);
    toggler->setTag(keyID);
    toggler->setSizeMult(1.0f);

    incrementCountForPage(page);

    cocos2d::CCArray* objects = objectsForPage(page);
    objects->addObject(toggler);
    toggler->setVisible(false);
    layer->setVisible(false);

    if (description) {
        cocos2d::CCString* descStr = cocos2d::CCString::create(description);
        const char* infoKeyStr = infoKey(keyID);
        m_variables->setObject(descStr, infoKeyStr);

        cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSprite->setScale(0.5f);
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
            infoSprite, nullptr, this, menu_selector(MoreOptionsLayer::onInfo));
        infoBtn->setSizeMult(2.0f);
        infoBtn->setTag(keyID);
        m_buttonMenu->addChild(infoBtn);
        infoBtn->setPosition(toggler->getPosition() + ccp(-18.0f, 16.0f));

        objects->addObject(infoBtn);
        infoBtn->setVisible(false);
    }
}

int MoreOptionsLayer::countForPage(int page)
{
    cocos2d::CCString* str = (cocos2d::CCString*)m_variables->valueForKey(pageKey(page));
    return str->intValue();
}

MoreOptionsLayer* MoreOptionsLayer::create()
{
    MoreOptionsLayer* ret = new MoreOptionsLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MoreOptionsLayer::dropDownLayerWillClose(GJDropDownLayer* layer)
{
    GameManager::sharedState()->fadeInMenuMusic();
}

void MoreOptionsLayer::goToPage(int page)
{
    if (m_pageCount < page)
        page = 0;
    else if (page < 0)
        page = m_pageCount;

    if (m_page == page)
        return;

    cocos2d::CCArray* oldObjects = (cocos2d::CCArray*)m_variables->objectForKey(objectKey(m_page));
    if (oldObjects) {
        for (unsigned int i = 0; i < oldObjects->count(); ++i) {
            cocos2d::CCObject* obj = oldObjects->objectAtIndex(i);
            ((cocos2d::CCNode*)obj)->setVisible(false);
        }
    }

    cocos2d::CCLayer* oldLayer = (cocos2d::CCLayer*)m_variables->objectForKey(layerKey(m_page));
    if (oldLayer)
        oldLayer->setVisible(false);

    cocos2d::CCArray* newObjects = (cocos2d::CCArray*)m_variables->objectForKey(objectKey(page));
    if (newObjects) {
        for (unsigned int i = 0; i < newObjects->count(); ++i) {
            cocos2d::CCObject* obj = newObjects->objectAtIndex(i);
            ((cocos2d::CCNode*)obj)->setVisible(true);
        }
    }

    cocos2d::CCLayer* newLayer = (cocos2d::CCLayer*)m_variables->objectForKey(layerKey(page));
    if (newLayer)
        newLayer->setVisible(true);

    m_page = page;
    m_leftBtn->setVisible(true);
    m_rightBtn->setVisible(true);

    m_offsetInput->setTouchEnabled(m_offsetInput->isVisible());

    std::string category = "Options";
    switch (m_page) {
        case 0: category = "Gameplay"; break;
        case 1:
        case 2: category = "Visual"; break;
        case 3: category = "Practice"; break;
        case 4: category = "Performance"; break;
        case 5: category = "Audio"; break;
        case 6: category = "Other"; break;
        default: break;
    }
    m_categoryLabel->setString(category.c_str());
}

void MoreOptionsLayer::incrementCountForPage(int page)
{
    int count = countForPage(page);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", count + 1);
    m_variables->setObject(str, pageKey(page));
}

const char* MoreOptionsLayer::infoKey(int index)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("info_%i", index);
    return str->getCString();
}

bool MoreOptionsLayer::init()
{
    cocos2d::ccColor4B color = ccc4(0, 0, 0, 0);
    if (!FLAlertLayer::initWithColor(color, 150))
        return false;

    GameManager* gm = GameManager::sharedState();
    if (gm->getGameVariable("0168"))
        m_noElasticity = true;

    FLAlertLayer::incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(m_mainLayer);

    cocos2d::CCRect rect(0.0f, 0.0f, 80.0f, 80.0f);
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    bg->setContentSize(cocos2d::CCSize(400.0f, 280.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    m_categoryLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_categoryLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 140.0f - 16.0f));
    m_mainLayer->addChild(m_categoryLabel);
    m_categoryLabel->setScale(0.7f);

    m_joystickConnected = -1;

    m_objects = cocos2d::CCDictionary::create();
    m_objects->retain();

    m_variables = cocos2d::CCDictionary::create();
    m_variables->retain();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    addToggle("Auto-Retry", "0026", "Restarts level upon death automatically.");
    addToggle("Enable Faster Reset", "0052", "Restarts in 0.5s instead of 1.0s upon death.");
    addToggle("Click Between Steps", "0177", "Toggles substeps on, allowing you to click in-between the default normal steps. This may affect performance.");
    addToggle("Click On Steps", "0176", "Allows you to click on every step, regardless of your refresh rate. This is beneficial for users below 240 Hz.");
    addToggle("Flip 2-Player\nControls", "0010", "Flips which side controls which player during 2-player mode.");
    addToggle("Always Limit\nControls", "0011", "Limits player 1 controls to one side even when dual mode is inactive.");
    offsetToNextPage();
    addToggle("Hide Attempts", "0135", "Hides the attempt counter when playing levels.");
    addToggle("Flip Pause Button", "0015", "Flips the location of the pause button.");
    addToggle("Disable Portal Guide", "0129", "Disables extra indicators on portals.");
    addToggle("Enable Orb Guide", "0130", "Enables extra indicators on orbs.");
    addToggle("Disable Orb Scale", "0140", "Disables the scaling effect on all orbs.");
    addToggle("Disable Trigger\nOrb Scale", "0141", "Disables the scaling effect on only trigger orbs.");
    addToggle("Disable Shake", "0172", "Disables shake effects.");
    addToggle("Disable Explosion\nShake", "0014", "Disables the shake effect that happens upon death.");
    addToggle("Disable Gravity\nEffect", "0072", "Disables the effect that happens upon changing gravity.");
    addToggle("Default Mini Icon", "0060", "Sets player icon in mini mode to default.");
    addToggle("Switch Spider\nTeleport Color", "0061", "Toggles between main and secondary color for the teleport effect in spider mode.");
    addToggle("Switch Dash\nFire Color", "0062", "Toggles between main and secondary color for the fire effect from dash orbs.");
    addToggle("Switch Wave\nTrail Color", "0096", "Toggles between main and secondary color for the trail in wave mode.");
    addToggle("Hide Playtest Text", "0174", "Hides text in the top left when using start positions or ignore damage.");
    addToggle("Hitbox On Death", "0179", "Shows hitboxes upon death in both normal and practice mode.");
    offsetToNextPage();
    addToggle("Hide Attempts", "0134", "Hides the attempt counter when playing levels in practice mode.");
    addToggle("Enable Auto-Checkpoints", "0027", "Places checkpoints automatically in practice mode.");
    addToggle("Enable Quick\nCheckpoints", "0068", "Tries to place checkpoints more often in practice mode.");
    addToggle("Enable Death Effect", "0100", "Shows death effects in practice mode.");
    addToggle("Enable Normal Music\nIn Editor", "0125", "Plays normal music in sync to editor levels in practice mode.");
    addToggle("Show Hitboxes", "0166", "Shows hitboxes while in practice mode.");
    addToggle("Disable Player Hitbox", "0171", "Disables the player's hitbox in practice mode\n(if hitboxes are shown).");
    offsetToNextPage();
    addToggle("Enable Smooth Fix", "0023", "Makes some optimizations that can reduce lag. Disable if game speed becomes inconsistent.");
    addToggle("Increase Draw Capacity", "0066", "Increases draw capacity for batch nodes at level start. Can improve performance on some levels, but may cause issues on low-end devices.");
    addToggle("Enable Low Detail", "0108", "Enables low detail mode on levels that support it automatically.");
    addToggle("Disable High Object\nAlert", "0082", "Removes the alert shown when starting levels with a high object count.");
    addToggle("Enable Extra LDM", "0136", "Removes glow and enter effects while in low detail mode. Levels without LDM show LDM Lite.");
    addToggle("Increase Maximum\nLevels", "0042", "Increases maximum locally saved levels from 10 to 100. This refers to level data, not statistics. Enabling this can make your save file considerably larger, so keeping the option off is recommended for quicker saving.");
    addToggle("Disable Level Saving", "0119", "Saves level statistics as usual, but levels need to be redownloaded every time you restart the game. Makes saving and loading faster.");
    addToggle("Save Gauntlets", "0127", "Saves gauntlet levels locally so they do not have to be redownloaded. Increases save time but helpful if you have poor connection.");
    addToggle("Disable Shader\nAnti-Aliasing", "0155", "Disables anti-aliasing on shader effects.");
    offsetToNextPage();
    addToggle("Disable Song Alert", "0083", "Removes the alert shown when starting levels without the song downloaded.");
    addToggle("No Song Limit", "0018", "Stops automatic deletion of custom songs. This is done by default to save space.");
    addToggle("Reduce Quality\n(Read Info)", "0142", "Lowers audio sampling rate from 44100 Hz to 24000 Hz. Requires restarting to take effect.");
    addToggle("Audio Fix 01", "0159", "Increases the audio buffer size, which may fix certain issues. Do not enable if audio is working fine. Causes a slight more audio delay. Requires restarting to take effect.");
    offsetToNextPage();
    addToggle("More Comments", "0094", "Shows more comments per page. Why not?");
    addToggle("Load Comments", "0090", "Loads comments automatically.");
    addToggle("New Completed Filter", "0073", "Makes completed levels filter based only on percentage from update 2.1. Useful to rebeat levels for Mana Orbs.");
    addToggle("Increase Local Levels\nPer Page", "0093", "Increases created and saved levels per page from 10 to 20.");
    addToggle("Manual Level Order", "0084", "Places new levels last in the saved levels list. Useful if you want to manually move levels to the top.");
    addToggle("Percentage Decimals", "0126", "Shows decimals in level progress.");
    addToggle("Show Leaderboard\nPercentage", "0099", "Toggles viewing the leaderboard percentage you have on levels. To upload your level progress to the level leaderboard, you need to replay levels completed before 2.11.");
    addToggle("Do Not...", "0095", "Does not do anything... Well, nothing useful.");
    addToggle("Confirm Exit", "0167", "Adds an extra confirmation window when exiting levels.");
    addToggle("Fast\nMenu", "0168", "Makes transitions between menu pages faster.");

    cocos2d::CCArray* page5Objects = objectsForPage(5);
    cocos2d::CCSprite* songsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
    CCMenuItemSpriteExtra* songsBtn = CCMenuItemSpriteExtra::create(
        songsSprite, nullptr, this, menu_selector(MoreOptionsLayer::onSongBrowser));
    songsBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(songsBtn);
    songsBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 200.0f - 36.0f, winSize.height * 0.5f - 140.0f + 36.0f + 2.0f)));
    page5Objects->addObject(songsBtn);
    songsBtn->setVisible(false);

    cocos2d::CCNode* debugNode = ButtonSprite::create(
        "DEBUG", 0, false, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* debugBtn = CCMenuItemSpriteExtra::create(
        debugNode, nullptr, this, menu_selector(MoreOptionsLayer::onFMODDebug));
    m_buttonMenu->addChild(debugBtn);
    debugBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 40.0f, winSize.height * 0.5f - 140.0f + 30.0f)));
    page5Objects->addObject(debugBtn);
    debugBtn->setVisible(false);

    cocos2d::CCArray* page6Objects = objectsForPage(6);
    cocos2d::CCSprite* parentalSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_profileButton_001.png");
    parentalSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* parentalBtn = CCMenuItemSpriteExtra::create(
        parentalSprite, nullptr, this, menu_selector(MoreOptionsLayer::onParental));
    parentalBtn->setSizeMult(1.2f);
    m_buttonMenu->addChild(parentalBtn);
    parentalBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 200.0f - 36.0f, winSize.height * 0.5f - 140.0f + 36.0f)));
    page6Objects->addObject(parentalBtn);
    parentalBtn->setVisible(false);

    cocos2d::CCPoint offsetPos = ccp(
        winSize.width * 0.5f - 200.0f + 75.0f,
        winSize.height * 0.5f - 140.0f + 35.0f);

    cocos2d::CCLabelBMFont* offsetLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    offsetLabel->setScale(0.4f);
    offsetLabel->setPosition(offsetPos + ccp(0.0f, 25.0f));
    m_mainLayer->addChild(offsetLabel);
    page5Objects->addObject(offsetLabel);
    offsetLabel->setVisible(false);

    cocos2d::extension::CCScale9Sprite* offsetBG = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    offsetBG->setOpacity(100);
    offsetBG->setContentSize(cocos2d::CCSize(100.0f, 30.0f));
    m_mainLayer->addChild(offsetBG, -1);
    offsetBG->setPosition(offsetPos);
    page5Objects->addObject(offsetBG);
    offsetBG->setVisible(false);

    m_offsetInput = CCTextInputNode::create(100.0f, 50.0f, "Offset", "Thonburi");
    m_mainLayer->addChild(m_offsetInput, 20);
    m_offsetInput->m_delegate = this;
    m_offsetInput->setPosition(offsetPos);
    m_offsetInput->setMaxLabelScale(0.7f);
    m_offsetInput->setMaxLabelWidth(200.0f);
    m_offsetInput->m_textField->setVisible(false);
    m_offsetInput->m_maxLabelLength = 4;
    m_offsetInput->setAllowedChars("0123456789-");
    m_offsetInput->setLabelPlaceholderColor(ccc3(120, 170, 240));
    m_offsetInput->setLabelPlaceholderScale(0.5f);
    page5Objects->addObject(m_offsetInput);
    m_offsetInput->setVisible(false);

    if (FMODAudioEngine::sharedEngine()->m_musicOffset) {
        cocos2d::CCString* offsetStr = cocos2d::CCString::createWithFormat(
            "%i", FMODAudioEngine::sharedEngine()->m_musicOffset);
        m_offsetInput->setString(offsetStr->getCString());
    }

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(MoreOptionsLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f - 200.0f + 5.0f, winSize.height * 0.5f + 140.0f - 5.0f)));

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_leftBtn = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(MoreOptionsLayer::onPrevPage));
    m_buttonMenu->addChild(m_leftBtn);
    m_leftBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 20.0f, winSize.height * 0.5f)));
    m_leftBtn->setSizeMult(1.5f);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_rightBtn = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(MoreOptionsLayer::onNextPage));
    m_buttonMenu->addChild(m_rightBtn);
    m_rightBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 20.0f, winSize.height * 0.5f)));
    m_rightBtn->setSizeMult(1.5f);

    goToPage(0);

    return true;
}

void MoreOptionsLayer::keyBackClicked()
{
    onClose(nullptr);
}

cocos2d::CCLayer* MoreOptionsLayer::layerForPage(int page)
{
    cocos2d::CCLayer* layer = (cocos2d::CCLayer*)m_variables->objectForKey(layerKey(page));
    if (layer)
        return layer;

    layer = cocos2d::CCLayer::create();
    m_mainLayer->addChild(layer);
    m_variables->setObject(layer, layerKey(page));
    return layer;
}

const char* MoreOptionsLayer::layerKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("layer_%i", page);
    return str->getCString();
}

cocos2d::CCPoint MoreOptionsLayer::nextPosition(int page)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint posA(winSize.width * 0.5f - 160.0f, winSize.height * 0.5f + 80.0f);
    cocos2d::CCPoint posB(winSize.width * 0.5f + 32.0f, posA.y);

    int count = countForPage(page);
    cocos2d::CCPoint result = ((count + 1) & 1) ? posA : posB;

    if (count + 1 > 2) {
        float rows = (float)count * 0.5f;
        result.y -= (float)(int)(48 * (int)rows);
    }
    return result;
}

const char* MoreOptionsLayer::objectKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("object_%i", page);
    return str->getCString();
}

cocos2d::CCArray* MoreOptionsLayer::objectsForPage(int page)
{
    cocos2d::CCArray* arr = (cocos2d::CCArray*)m_variables->objectForKey(objectKey(page));
    if (arr)
        return arr;

    arr = cocos2d::CCArray::create();
    m_variables->setObject(arr, objectKey(page));
    return arr;
}

void MoreOptionsLayer::offsetToNextPage()
{
    if (m_toggleCount % 10)
        m_toggleCount = m_toggleCount - m_toggleCount % 10 + 10;
}

void MoreOptionsLayer::onClose(cocos2d::CCObject* sender)
{
    m_offsetInput->onClickTrackNode(false);
    FMODAudioEngine::sharedEngine()->m_musicOffset = atoi(m_offsetInput->getString().c_str());

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MoreOptionsLayer::onFMODDebug(cocos2d::CCObject* sender)
{
    std::string status = FMODAudioEngine::sharedEngine()->getFMODStatus(0);
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "FMOD Debug", status);
    alert->show();
}

void MoreOptionsLayer::onGPSignIn(cocos2d::CCObject* sender)
{
    if (PlatformToolbox::isSignedInGooglePlay())
        toggleGP();
    else
        PlatformToolbox::signInGooglePlay();
}

void MoreOptionsLayer::onGPSignOut(cocos2d::CCObject* sender)
{
    if (PlatformToolbox::isSignedInGooglePlay()) {
        PlatformToolbox::signOutGooglePlay();
        GameManager::sharedState()->m_googlePlaySignedIn = false;
        m_gpSignInBtn->setVisible(true);
        m_gpSignOutBtn->setVisible(false);
    } else {
        toggleGP();
    }
}

void MoreOptionsLayer::onInfo(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCString* str = (cocos2d::CCString*)m_variables->valueForKey(infoKey(tag));
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Info", str->getCString());
    alert->show();
}

void MoreOptionsLayer::onKeybindings(cocos2d::CCObject* sender)
{
    KeybindingsLayer* layer = KeybindingsLayer::create();
    layer->show();
}

void MoreOptionsLayer::onNextPage(cocos2d::CCObject* sender)
{
    goToPage(m_page + 1);
}

void MoreOptionsLayer::onParental(cocos2d::CCObject* sender)
{
    ParentalOptionsLayer* layer = ParentalOptionsLayer::create();
    layer->show();
}

void MoreOptionsLayer::onPrevPage(cocos2d::CCObject* sender)
{
    goToPage(m_page - 1);
}

void MoreOptionsLayer::onSongBrowser(cocos2d::CCObject* sender)
{
    GJSongBrowser* browser = GJSongBrowser::create();
    browser->m_delegate = this;
    cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(browser, 106);
    browser->showLayer(false);
}

void MoreOptionsLayer::onToggle(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::CCString* str = (cocos2d::CCString*)m_objects->objectForKey(tag);
    if (!str)
        return;

    const char* key = str->getCString();
    GameManager* gm = GameManager::sharedState();
    bool value = gm->toggleGameVariable(key);

    std::string keyStr = key;

    if (keyStr == "0028") {
        gm = GameManager::sharedState();
        gm->m_disableThumbstick = gm->getGameVariable("0028");
        gm = GameManager::sharedState();
        PlatformToolbox::toggleMouseControl(!gm->getGameVariable("0028"));
        return;
    }

    if (keyStr == "0023") {
        gm = GameManager::sharedState();
        PlatformToolbox::toggleSmoothFix(gm->getGameVariable("0023"));
        return;
    }

    if (keyStr == "0032") {
        gm = GameManager::sharedState();
        PlatformToolbox::toggleForceTimer(gm->getGameVariable("0032"));
        return;
    }

    if (keyStr == "0115") {
        gm = GameManager::sharedState();
        bool show = gm->getGameVariable("0115");
        cocos2d::CCDirector::sharedDirector()->toggleShowFPS(show, "chatFont.fnt", ccp(0.0f, 0.0f));
        return;
    }

    if (keyStr == "0168") {
        cocos2d::CCDirector::sharedDirector()->m_bFastMenu = value;
        return;
    }
}

const char* MoreOptionsLayer::pageKey(int page)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("page_%i", page);
    return str->getCString();
}

void MoreOptionsLayer::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, ccp(0.0f, 0.0f));
    m_mainLayer->runAction(move);
}

void MoreOptionsLayer::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, ccp(0.0f, yOffset));
    m_mainLayer->runAction(move);
}

void MoreOptionsLayer::toggleGP()
{
    bool signedIn = PlatformToolbox::isSignedInGooglePlay();
    m_gpSignInBtn->setVisible(!signedIn);
    m_gpSignOutBtn->setVisible(signedIn);
}

MoreOptionsLayer::~MoreOptionsLayer()
{
    if (m_objects)
        m_objects->release();
    if (m_variables)
        m_variables->release();
}