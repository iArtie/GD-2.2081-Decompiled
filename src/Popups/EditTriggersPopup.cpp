#include "Popups/EditTriggersPopup.h"
#include "UI/InfoAlertButton.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"
#include "CCDirector.h"

EditTriggersPopup* EditTriggersPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    EditTriggersPopup* instance = new EditTriggersPopup();
    if (instance && instance->init(object, objects)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool EditTriggersPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    // Initialize base SetupTriggerPopup with size 330x120
    if (!SetupTriggerPopup::init(this, object, objects, 330.0f, 120.0f, true)) {
        return false;
    }

    // Determine start values for the trigger
    determineStartValues();

    // Get window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    // Create title label
    CCLabelBMFont* titleLabel = CCLabelBMFont::create();
    addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(ccp(
        winSize.width * 0.5f,
        m_height * 0.5f + winSize.height * 0.5f - 18.0f
    ));

    // Create OK button
    CCNode* okButton = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okMenuItem = CCMenuItemSpriteExtra::create(
        okButton,
        this,
        menu_selector(EditTriggersPopup::onClose)
    );
    okMenuItem->setTag(1320);
    m_buttonMenu->addChild(okMenuItem);

    // Create Help button
    std::string helpTitle = "Help";
    std::string helpDesc = 
        "<cg>Edit attributes of the selected triggers.</c>\n"
        "<cy>Touch Triggers makes the object activate on player touch.</c>\n"
        "<cl>Spawn Triggered makes the triggers activate from a spawn trigger.</c>";
    
    InfoAlertButton* helpButton = InfoAlertButton::create(helpTitle, helpDesc, 1.0f);
    m_buttonMenu->addChild(helpButton);
    
    // Position help button at top-right
    float posX = 18.0f - (m_width * 0.5f - winSize.width * 0.5f);
    float posY = m_height * 0.5f + winSize.height * 0.5f - 18.0f;
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(ccp(posX, posY)));

    // Initialize joystick state
    m_joystickConnected = -1;

    // Create default multi-trigger items
    SetupTriggerPopup::createMultiTriggerItemsDefault(this);

    return true;
}

EditTriggersPopup::~EditTriggersPopup()
{
    // Cleanup handled by base class
}