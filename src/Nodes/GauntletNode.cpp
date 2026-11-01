#include "Nodes/GauntletNode.h"
#include "Models/GJMapPack.h"
#include "Sprites/GJChestSprite.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "cocos2d/extensions/CCScale9Sprite.h"

GauntletNode* GauntletNode::create(GJMapPack* gauntlet)
{
    GauntletNode* instance = new GauntletNode();
    if (instance && instance->init(gauntlet))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GauntletNode::init(GJMapPack* gauntlet)
{
    if (!cocos2d::CCNode::init())
        return false;

    if (gauntlet)
    {
        m_gauntlet = gauntlet;
        m_gauntlet->retain();
    }
    m_locked = false;
    generateNode();
    return true;
}

std::string GauntletNode::frameForType(GauntletType type)
{
    switch (type)
    {
        case Fire:      return "island_fire_001.png";
        case Ice:       return "island_ice_001.png";
        case Poison:    return "island_poison_001.png";
        case Shadow:    return "island_shadow_001.png";
        case Lava:      return "island_lava_001.png";
        case Bonus:     return "island_bonus_001.png";
        case Chaos:     return "island_chaos_001.png";
        case Demon:     return "island_demon_001.png";
        case Time:      return "island_time_001.png";
        case Crystal:   return "island_crystal_001.png";
        case Magic:     return "island_magic_001.png";
        case Spike:     return "island_spike_001.png";
        case Monster:   return "island_monster_001.png";
        case Doom:      return "island_doom_001.png";
        case Death:     return "island_death_001.png";
        case Forest:    return "island_new01_001.png";
        case Rune:      return "island_new02_001.png";
        case Force:     return "island_new03_001.png";
        case Spooky:    return "island_new04_001.png";
        case Dragon:    return "island_new05_001.png";
        case Water:     return "island_new06_001.png";
        case Haunted:   return "island_new07_001.png";
        case Acid:      return "island_new08_001.png";
        case Witch:     return "island_new09_001.png";
        case Power:     return "island_new10_001.png";
        case Potion:    return "island_new11_001.png";
        case Snake:     return "island_new12_001.png";
        case Toxic:     return "island_new13_001.png";
        case Halloween: return "island_new14_001.png";
        case Treasure:  return "island_new15_001.png";
        case Ghost:     return "island_new16_001.png";
        case Spider:    return "island_new17_001.png";
        case Gem:       return "island_new18_001.png";
        case Inferno:   return "island_new19_001.png";
        case Portal:    return "island_new20_001.png";
        case Strange:   return "island_new21_001.png";
        case Fantasy:   return "island_new22_001.png";
        case Christmas: return "island_new23_001.png";
        case Surprise:  return "island_new24_001.png";
        case Mystery:   return "island_new25_001.png";
        case Cursed:    return "island_new26_001.png";
        case Cyborg:    return "island_new27_001.png";
        case Castle:    return "island_new28_001.png";
        case Grave:     return "island_new29_001.png";
        case Temple:    return "island_new30_001.png";
        case World:     return "island_new31_001.png";
        case Galaxy:    return "island_new32_001.png";
        case Universe:  return "island_new33_001.png";
        case Discord:   return "island_new34_001.png";
        case Split:     return "island_new35_001.png";
        case NCS_I:     return "island_ncs01_001.png";
        case NCS_II:    return "island_ncs02_001.png";
        case Space:     return "island_space_001.png";
        case Cosmos:    return "island_cosmos_001.png";
        case Random:    return "island_random_001.png";
        case Chance:    return "island_chance_001.png";
        case Future:    return "island_future_001.png";
        case Utopia:    return "island_utopia_001.png";
        case Cinema:    return "island_cinema_001.png";
        case Love:      return "island_love_001.png";
        default:        return "island_fire_001.png";
    }
}

std::string GauntletNode::nameForType(GauntletType type)
{
    switch (type)
    {
        case Fire:      return "Fire";
        case Ice:       return "Ice";
        case Poison:    return "Poison";
        case Shadow:    return "Shadow";
        case Lava:      return "Lava";
        case Bonus:     return "Bonus";
        case Chaos:     return "Chaos";
        case Demon:     return "Demon";
        case Time:      return "Time";
        case Crystal:   return "Crystal";
        case Magic:     return "Magic";
        case Spike:     return "Spike";
        case Monster:   return "Monster";
        case Doom:      return "Doom";
        case Death:     return "Death";
        case Forest:    return "Forest";
        case Rune:      return "Rune";
        case Force:     return "Force";
        case Spooky:    return "Spooky";
        case Dragon:    return "Dragon";
        case Water:     return "Water";
        case Haunted:   return "Haunted";
        case Acid:      return "Acid";
        case Witch:     return "Witch";
        case Power:     return "Power";
        case Potion:    return "Potion";
        case Snake:     return "Snake";
        case Toxic:     return "Toxic";
        case Halloween: return "Halloween";
        case Treasure:  return "Treasure";
        case Ghost:     return "Ghost";
        case Spider:    return "Spider";
        case Gem:       return "Gem";
        case Inferno:   return "Inferno";
        case Portal:    return "Portal";
        case Strange:   return "Strange";
        case Fantasy:   return "Fantasy";
        case Christmas: return "Christmas";
        case Surprise:  return "Surprise";
        case Mystery:   return "Mystery";
        case Cursed:    return "Cursed";
        case Cyborg:    return "Cyborg";
        case Castle:    return "Castle";
        case Grave:     return "Grave";
        case Temple:    return "Temple";
        case World:     return "World";
        case Galaxy:    return "Galaxy";
        case Universe:  return "Universe";
        case Discord:   return "Discord";
        case Split:     return "Split";
        case NCS_I:     return "NCS I";
        case NCS_II:    return "NCS II";
        case Space:     return "Space";
        case Cosmos:    return "Cosmos";
        case Random:    return "Random";
        case Chance:    return "Chance";
        case Future:    return "Future";
        case Utopia:    return "Utopia";
        case Cinema:    return "Cinema";
        case Love:      return "Love";
        default:        return "Unknown";
    }
}

void GauntletNode::generateNode()
{
    removeAllChildrenWithCleanup(false);

    if (!m_gauntlet)
        return;

    GauntletType type = m_gauntlet->m_packID;
    std::string name = nameForType(type);
    std::string frame = frameForType(type);

    // Create info and reward nodes
    m_gauntletInfoNode = cocos2d::CCNode::create();
    m_gauntletInfoNode->setContentSize(cocos2d::CCSize(110, 234));
    addChild(m_gauntletInfoNode, 1);

    m_rewardNode = cocos2d::CCNode::create();
    m_rewardNode->setContentSize(cocos2d::CCSize(110, 234));
    addChild(m_rewardNode, 2);
    m_rewardNode->setVisible(false);

    // Set colors based on gauntlet type
    cocos2d::ccColor3B labelColor = cocos2d::ccc3(255, 255, 255);
    cocos2d::ccColor3B bgColor = cocos2d::ccc3(255, 255, 255);
    float iconYOffset = 0.0f;
    float labelScale = 0.7f;

    switch (type)
    {
        case Fire:
            labelColor = cocos2d::ccc3(255, 200, 0);
            break;
        case Ice:
            labelColor = cocos2d::ccc3(0, 150, 255);
            bgColor = cocos2d::ccc3(125, 200, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Poison:
            labelColor = cocos2d::ccc3(0, 255, 0);
            bgColor = cocos2d::ccc3(100, 0, 100);
            iconYOffset = 0.0f;
            labelScale = 0.62f;
            break;
        case Shadow:
            labelColor = cocos2d::ccc3(0, 200, 255);
            bgColor = cocos2d::ccc3(50, 0, 50);
            iconYOffset = 0.0f;
            labelScale = 0.62f;
            break;
        case Lava:
            labelColor = cocos2d::ccc3(200, 0, 0);
            bgColor = cocos2d::ccc3(0, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Bonus:
            labelColor = cocos2d::ccc3(255, 200, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Chaos:
            labelColor = cocos2d::ccc3(100, 100, 100);
            bgColor = cocos2d::ccc3(25, 25, 25);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Demon:
        case Death:
            labelColor = cocos2d::ccc3(50, 50, 50);
            bgColor = cocos2d::ccc3(200, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Time:
            labelColor = cocos2d::ccc3(255, 255, 0);
            bgColor = cocos2d::ccc3(0, 0, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Crystal:
            labelColor = cocos2d::ccc3(0, 255, 255);
            bgColor = cocos2d::ccc3(50, 0, 100);
            iconYOffset = 0.0f;
            labelScale = 0.6f;
            break;
        case Magic:
            labelColor = cocos2d::ccc3(255, 125, 0);
            bgColor = cocos2d::ccc3(50, 0, 50);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Spike:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(100, 0, 50);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Monster:
            labelColor = cocos2d::ccc3(0, 200, 0);
            bgColor = cocos2d::ccc3(50, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.55f;
            break;
        case Doom:
            labelColor = cocos2d::ccc3(255, 200, 100);
            bgColor = cocos2d::ccc3(25, 50, 100);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Forest:
            labelColor = cocos2d::ccc3(255, 0, 0);
            bgColor = cocos2d::ccc3(0, 200, 0);
            iconYOffset = 2.0f;
            labelScale = 0.7f;
            break;
        case Rune:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(100, 0, 50);
            iconYOffset = 1.0f;
            labelScale = 0.7f;
            break;
        case Force:
            labelColor = cocos2d::ccc3(0, 0, 255);
            bgColor = cocos2d::ccc3(0, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Spooky:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(50, 0, 50);
            iconYOffset = -2.0f;
            labelScale = 0.7f;
            break;
        case Dragon:
            labelColor = cocos2d::ccc3(0, 255, 0);
            bgColor = cocos2d::ccc3(200, 0, 0);
            iconYOffset = 3.0f;
            labelScale = 0.7f;
            break;
        case Water:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 50, 100);
            iconYOffset = 3.0f;
            labelScale = 0.7f;
            break;
        case Haunted:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 0, 100);
            iconYOffset = 0.0f;
            labelScale = 0.55f;
            break;
        case Acid:
            labelColor = cocos2d::ccc3(255, 255, 0);
            bgColor = cocos2d::ccc3(100, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Witch:
            labelColor = cocos2d::ccc3(255, 0, 255);
            bgColor = cocos2d::ccc3(50, 200, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Power:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 0, 100);
            iconYOffset = 2.0f;
            labelScale = 0.7f;
            break;
        case Potion:
            labelColor = cocos2d::ccc3(255, 0, 255);
            bgColor = cocos2d::ccc3(100, 0, 50);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Snake:
            labelColor = cocos2d::ccc3(255, 0, 0);
            bgColor = cocos2d::ccc3(0, 200, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Toxic:
            labelColor = cocos2d::ccc3(0, 255, 0);
            bgColor = cocos2d::ccc3(100, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Halloween:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(100, 50, 0);
            iconYOffset = 3.0f;
            labelScale = 0.48f;
            break;
        case Treasure:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 0, 255);
            iconYOffset = 3.0f;
            labelScale = 0.55f;
            break;
        case Ghost:
            labelColor = cocos2d::ccc3(255, 0, 0);
            bgColor = cocos2d::ccc3(50, 50, 50);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Spider:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 255, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Gem:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 0, 255);
            iconYOffset = 2.0f;
            labelScale = 0.7f;
            break;
        case Inferno:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 0, 0);
            iconYOffset = 6.0f;
            labelScale = 0.6f;
            break;
        case Portal:
            labelColor = cocos2d::ccc3(255, 0, 255);
            bgColor = cocos2d::ccc3(0, 0, 255);
            iconYOffset = 5.0f;
            labelScale = 0.7f;
            break;
        case Strange:
            labelColor = cocos2d::ccc3(255, 0, 255);
            bgColor = cocos2d::ccc3(100, 0, 100);
            iconYOffset = -4.0f;
            labelScale = 0.55f;
            break;
        case Fantasy:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 200, 255);
            iconYOffset = 0.0f;
            labelScale = 0.6f;
            break;
        case Christmas:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(0, 255, 0);
            iconYOffset = 0.0f;
            labelScale = 0.5f;
            break;
        case Surprise:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.55f;
            break;
        case Mystery:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.6f;
            break;
        case Cursed:
            labelColor = cocos2d::ccc3(0, 255, 0);
            bgColor = cocos2d::ccc3(50, 0, 50);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Cyborg:
            labelColor = cocos2d::ccc3(50, 50, 50);
            bgColor = cocos2d::ccc3(0, 0, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Castle:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = -2.0f;
            labelScale = 0.7f;
            break;
        case Grave:
            labelColor = cocos2d::ccc3(0, 255, 0);
            bgColor = cocos2d::ccc3(255, 0, 0);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Temple:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 0, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case World:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Galaxy:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Universe:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Discord:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Split:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case NCS_I:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case NCS_II:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Space:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Cosmos:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Random:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Chance:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Future:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Utopia:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Cinema:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        case Love:
            labelColor = cocos2d::ccc3(255, 200, 0);
            bgColor = cocos2d::ccc3(255, 255, 255);
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
        default:
            iconYOffset = 0.0f;
            labelScale = 0.7f;
            break;
    }

    // Create background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_squareB_01.png",
        cocos2d::CCRect(0, 0, 86, 86)
    );
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(110, 234));
    addChild(bg, -1);
    bg->setColor(bgColor);

    // Main title label
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    addChild(titleLabel, 2);
    titleLabel->limitLabelWidth(75.0f, labelScale, 0.0f);
    titleLabel->setPosition(cocos2d::CCPoint(0, 94));
    titleLabel->setColor(labelColor);

    // Subtitle shadow
    cocos2d::CCLabelBMFont* subtitleShadow = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    addChild(subtitleShadow, 2);
    subtitleShadow->setScale(0.45f);
    subtitleShadow->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0, -19));
    subtitleShadow->setColor(labelColor);

    // Title shadow
    cocos2d::CCLabelBMFont* titleShadow = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    addChild(titleShadow, 1);
    titleShadow->setScale(titleLabel->getScale());
    titleShadow->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(2, -3));
    titleShadow->setColor(cocos2d::ccc3(0, 0, 0));
    titleShadow->setOpacity(60);

    // Subtitle shadow 2
    cocos2d::CCLabelBMFont* subtitleShadow2 = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    addChild(subtitleShadow2, 1);
    subtitleShadow2->setScale(subtitleShadow->getScale());
    subtitleShadow2->setPosition(subtitleShadow->getPosition() + cocos2d::CCPoint(2, -3));
    subtitleShadow2->setColor(cocos2d::ccc3(0, 0, 0));
    subtitleShadow2->setOpacity(60);

    if (m_locked)
    {
        // Locked state
        bg->setColor(cocos2d::ccc3(80, 80, 80));
        cocos2d::ccColor3B lockedColor = cocos2d::ccc3(175, 175, 175);
        titleLabel->setColor(lockedColor);
        subtitleShadow->setColor(lockedColor);

        // Lock icon
        cocos2d::CCSprite* lockIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        lockIcon->setScale(1.75f);
        m_gauntletInfoNode->addChild(lockIcon, 2);
        lockIcon->setPosition(cocos2d::CCPoint(0, iconYOffset + 16));

        // Unlock button
        cocos2d::CCSprite* unlockBtn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_unlockBtn_001.png");
        m_gauntletInfoNode->addChild(unlockBtn, 2);
        unlockBtn->setPosition(cocos2d::CCPoint(0, -41));

        // Video button
        cocos2d::CCSprite* videoBtn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_adVideoBtn_001.png");
        m_gauntletInfoNode->addChild(videoBtn, 2);
        videoBtn->setPosition(unlockBtn->getPosition() + cocos2d::CCPoint(0, -31));

        // Watch text
        cocos2d::CCLabelBMFont* watchLabel = cocos2d::CCLabelBMFont::create("Watch", "bigFont.fnt");
        watchLabel->setScale(0.3f);
        watchLabel->setColor(cocos2d::ccc3(255, 200, 0));
        m_gauntletInfoNode->addChild(watchLabel, 2);
        watchLabel->setPosition(videoBtn->getPosition() + cocos2d::CCPoint(0, -25));
    }
    else
    {
        // Unlocked state - icon
        cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
        m_gauntletInfoNode->addChild(icon, 2);
        icon->setPosition(cocos2d::CCPoint(0, iconYOffset + 16));

        // Icon shadow
        cocos2d::CCSprite* iconShadow = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
        m_gauntletInfoNode->addChild(iconShadow, 1);
        iconShadow->setPosition(icon->getPosition() + cocos2d::CCPoint(0, -10));
        iconShadow->setScaleY(1.2f);
        iconShadow->setColor(cocos2d::ccc3(0, 0, 0));
        iconShadow->setOpacity(50);

        // Progress label
        int completed = m_gauntlet->completedMaps();
        std::string progress = cocos2d::CCString::createWithFormat("%i/5", completed)->getCString();
        cocos2d::CCLabelBMFont* progressLabel = cocos2d::CCLabelBMFont::create(progress.c_str(), "bigFont.fnt");
        m_gauntletInfoNode->addChild(progressLabel, 2);
        progressLabel->setScale(0.4f);
        progressLabel->setPosition(icon->getPosition() + cocos2d::CCPoint(0, -40 - iconYOffset));

        // Progress shadow
        cocos2d::CCLabelBMFont* progressShadow = cocos2d::CCLabelBMFont::create(progress.c_str(), "bigFont.fnt");
        m_gauntletInfoNode->addChild(progressShadow, 1);
        progressShadow->setScale(progressLabel->getScale());
        progressShadow->setPosition(progressLabel->getPosition() + cocos2d::CCPoint(2, -3));
        progressShadow->setColor(cocos2d::ccc3(0, 0, 0));
        progressShadow->setOpacity(60);

        // Check if pack is completed
        if (m_gauntlet->hasCompletedMapPack())
        {
            // Completion icon
            cocos2d::CCSprite* checkIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            addChild(checkIcon, 5);
            checkIcon->setScale(1.1f);
            checkIcon->setPosition(cocos2d::CCPoint(0, -72));

            // Completion shadow
            cocos2d::CCSprite* checkShadow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            addChild(checkShadow, 1);
            checkShadow->setScale(1.1f);
            checkShadow->setPosition(checkIcon->getPosition() + cocos2d::CCPoint(2, -3));
            checkShadow->setColor(cocos2d::ccc3(0, 0, 0));
            checkShadow->setOpacity(60);

            // Check for reward chest
            std::string rewardKey = GameStatsManager::sharedState()->getGauntletRewardKey(type);
            bool chestUnlocked = GameStatsManager::sharedState()->isSpecialChestUnlocked(rewardKey);

            if (!chestUnlocked)
            {
                // Show reward node
                m_gauntletInfoNode->setVisible(false);
                m_rewardNode->setVisible(true);

                // Reward label
                cocos2d::CCLabelBMFont* rewardLabel = cocos2d::CCLabelBMFont::create("Reward", "bigFont.fnt");
                rewardLabel->setScale(0.65f);
                m_rewardNode->addChild(rewardLabel, 2);
                rewardLabel->setPosition(cocos2d::CCPoint(0, 43));

                // Reward shadow
                cocos2d::CCLabelBMFont* rewardShadow = cocos2d::CCLabelBMFont::create("Reward", "bigFont.fnt");
                rewardShadow->setScale(rewardLabel->getScale());
                m_rewardNode->addChild(rewardShadow, 1);
                rewardShadow->setPosition(rewardLabel->getPosition() + cocos2d::CCPoint(2, -3));
                rewardShadow->setColor(cocos2d::ccc3(0, 0, 0));
                rewardShadow->setOpacity(60);

                // Chest sprite
                GJChestSprite* chest = GJChestSprite::create(2);
                m_rewardNode->addChild(chest, 2);
                chest->setScale(0.6f);
                chest->setPosition(cocos2d::CCPoint(0, rewardLabel->getPosition().y - 46));

                // Chest shadow
                GJChestSprite* chestShadow = GJChestSprite::create(2);
                m_rewardNode->addChild(chestShadow, 1);
                chestShadow->setScale(chest->getScale());
                chestShadow->setPosition(chest->getPosition() + cocos2d::CCPoint(2, -3));
                cocos2d::CCObject* child = chestShadow->getChildren()->objectAtIndex(0);
                child->setColor(cocos2d::ccc3(0, 0, 0));
                child->setOpacity(60);
            }
        }
        else
        {
            // Not completed - show chest hint
            cocos2d::CCLabelBMFont* hintLabel = cocos2d::CCLabelBMFont::create("Chest", "bigFont.fnt");
            hintLabel->setScale(0.45f);
            m_gauntletInfoNode->addChild(hintLabel, 2);
            hintLabel->setPosition(cocos2d::CCPoint(0, -47));

            // Hint shadow
            cocos2d::CCLabelBMFont* hintShadow = cocos2d::CCLabelBMFont::create("Chest", "bigFont.fnt");
            hintShadow->setScale(0.45f);
            m_gauntletInfoNode->addChild(hintShadow, 1);
            hintShadow->setPosition(hintLabel->getPosition() + cocos2d::CCPoint(2, -3));
            hintShadow->setColor(cocos2d::ccc3(0, 0, 0));
            hintShadow->setOpacity(60);

            // Small chest icon
            GJChestSprite* chest = GJChestSprite::create(2);
            m_gauntletInfoNode->addChild(chest, 2);
            chest->setScale(0.4f);
            chest->setPosition(cocos2d::CCPoint(0, hintLabel->getPosition().y - 31.5f));

            // Small chest shadow
            GJChestSprite* chestShadow = GJChestSprite::create(2);
            m_gauntletInfoNode->addChild(chestShadow, 1);
            chestShadow->setScale(chest->getScale());
            chestShadow->setPosition(chest->getPosition() + cocos2d::CCPoint(2, -3));
            cocos2d::CCObject* child = chestShadow->getChildren()->objectAtIndex(0);
            child->setColor(cocos2d::ccc3(0, 0, 0));
            child->setOpacity(60);
        }
    }
}

void GauntletNode::onClaimReward()
{
    m_gauntletInfoNode->setVisible(true);
    m_rewardNode->setVisible(false);
}

void GauntletNode::onUnlock()
{
    m_locked = false;
    generateNode();

    // Unlock animation
    cocos2d::extension::CCScale9Sprite* flash = cocos2d::extension::CCScale9Sprite::create(
        "GJ_squareB_01.png",
        cocos2d::CCRect(0, 0, 86, 86)
    );
    flash->setOpacity(255);
    flash->setContentSize(cocos2d::CCSize(110, 234));
    addChild(flash, 100);
    flash->setColor(cocos2d::ccc3(255, 200, 0));
    flash->setBlendAdditive(true);
    flash->runAction(cocos2d::CCSequence::create(
        cocos2d::CCEaseIn::create(cocos2d::CCFadeOut::create(0.8f), 1.5f),
        cocos2d::CCCallFunc::create(flash, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup)),
        nullptr
    ));
}

void GauntletNode::showUnlockAnimation()
{
    cocos2d::extension::CCScale9Sprite* glow = cocos2d::extension::CCScale9Sprite::create(
        "GJ_squareB_01.png",
        cocos2d::CCRect(0, 0, 86, 86)
    );
    glow->setOpacity(0);
    glow->setContentSize(cocos2d::CCSize(110, 234));
    addChild(glow, 100);
    glow->setColor(cocos2d::ccc3(255, 200, 0));
    glow->setBlendAdditive(true);
    glow->runAction(cocos2d::CCEaseIn::create(cocos2d::CCFadeTo::create(2.0f, 150), 2.0f));
}

GauntletNode::~GauntletNode()
{
    if (m_gauntlet)
        m_gauntlet->release();
}