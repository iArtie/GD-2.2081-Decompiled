#include "LevelOptionsLayer2.h"
#include "LevelSettingsObject.h"
#include "GJOptionsLayer.h"
#include "cocos2d.h"

LevelOptionsLayer2* LevelOptionsLayer2::create(LevelSettingsObject* object) {
    LevelOptionsLayer2* layer = new LevelOptionsLayer2();
    if (layer && layer->init(object)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool LevelOptionsLayer2::init(LevelSettingsObject* object) {
    if (!LevelOptionsLayer::init(object)) {
        return false;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    label->setScale(0.7f);
    label->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        m_height * 0.5f + winSize.height * 0.5f - 20.0f
    ));
    m_mainLayer->addChild(label, 1);

    return true;
}

void LevelOptionsLayer2::setupOptions() {
    addToggle(
        "Allow Multi-Rotation",
        8,
        m_settingsObject->m_allowMultiRotation,
        "Allows multiple rotation actions to run on the same target group. This was not possible before 2.2."
    );

    addToggle(
        "Enable 2.2 Changes",
        17,
        m_settingsObject->m_enable22Changes,
        "Legacy compatibility pre 2.2. Should be enabled on new levels."
    );

    addToggle(
        "Allow Static-Rotate",
        18,
        m_settingsObject->m_allowStaticRotate,
        "Allow rotating static objects in actions and animations. There is currently no support for rotating static objects other than 90 degree steps, but use this option to allow it anyway. Hitboxes will not match the art. Can be useful in some cases."
    );

    addToggle(
        "Enable Player Squeeze",
        9,
        m_settingsObject->m_enablePlayerSqueeze,
        "Destroys player when squeezed between two objects. This is on by default but only for levels after update 2.2 for legacy reasons. Cannot be disabled for platformer levels."
    );

    addToggle(
        "Fix Gravity Bug",
        10,
        m_settingsObject->m_fixGravityBug,
        "Fixes an old bug that makes flipped gravity act differently than normal gravity. Old levels can break with this enabled."
    );

    addToggle(
        "Fix Negative Scale",
        11,
        m_settingsObject->m_fixNegativeScale,
        "Makes negative scaled objects collide correctly. Disabled on old levels for compatibility."
    );

    addToggle(
        "Fix Robot Jump",
        12,
        m_settingsObject->m_fixRobotJump,
        "Fixes robot jumps being bugged on slopes and/or pads."
    );

    addToggle(
        "Dynamic Level Height",
        14,
        m_settingsObject->m_dynamicLevelHeight,
        "Increases max level height dynamically (the same way as max length). This option is always enabled in platformer mode."
    );

    addToggle(
        "Sort Groups",
        15,
        m_settingsObject->m_sortGroups,
        "Makes sure that spawn groups are triggered in the correct order (left to right)."
    );

    addToggle(
        "Fix Radius Collision",
        16,
        m_settingsObject->m_fixRadiusCollision,
        "Replaces the player hitbox with a circle (instead of a square) for interactions with circular hitboxes. This fixes some issues where collisions would not register."
    );

    addToggle(
        "Reverse Sync",
        19,
        m_settingsObject->m_reverseSync,
        "Slows down or speeds up the player when reversing direction using orb or pads to keep music in sync."
    );

    addToggle(
        "Decrease Boost Slide",
        22,
        m_settingsObject->m_decreaseBoostSlide,
        "Slide less after dashing or being launched by moving blocks in platformer mode."
    );

    if (!m_settingsObject->m_enableImpulseFix) {
        addToggle("Enable Impulse Fix", 23, false, "Fix moving platform impulse bugs in platformer mode.");
    }
}

LevelOptionsLayer2::~LevelOptionsLayer2() {
}