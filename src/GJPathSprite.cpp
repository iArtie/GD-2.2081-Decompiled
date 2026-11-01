#include "GJPathSprite.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "CCSpriteGrayscale.h"
#include "CCSpriteWithHue.h"

GJPathSprite* GJPathSprite::create(int path) {
    auto sprite = new GJPathSprite();
    if (sprite && sprite->init(path)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool GJPathSprite::init(int path) {
    auto frame = cocos2d::CCString::createWithFormat("pathIcon_%02d_001.png", path);
    if (!cocos2d::CCSprite::initWithSpriteFrameName(frame->getCString())) {
        return false;
    }
    m_pathNumber = path;
    return true;
}

void GJPathSprite::addRankLabel(int path) {
    auto label = cocos2d::CCLabelBMFont::create(GameToolbox::intToString(path).c_str(), "bigFont.fnt");
    label->setScale(0.55f);
    addChild(label, 1);
    cocos2d::CCPoint pos = convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f));
    label->setPosition(pos + cocos2d::CCPoint(0.0f, 1.0f));
}

void GJPathSprite::addShardSprite() {
    const char* shardFrame = nullptr;
    
    switch (m_pathNumber) {
        case 1: shardFrame = "fireShardBig_001.png"; break;
        case 2: shardFrame = "iceShardBig_001.png"; break;
        case 3: shardFrame = "poisonShardBig_001.png"; break;
        case 4: shardFrame = "shadowShardBig_001.png"; break;
        case 5: shardFrame = "lavaShardBig_001.png"; break;
        case 6: shardFrame = "shard0201ShardBig_001.png"; break;
        case 7: shardFrame = "shard0202ShardBig_001.png"; break;
        case 8: shardFrame = "shard0203ShardBig_001.png"; break;
        case 9: shardFrame = "shard0204ShardBig_001.png"; break;
        case 10: shardFrame = "shard0205ShardBig_001.png"; break;
        default: return;
    }
    
    auto sprite = cocos2d::CCSprite::createWithSpriteFrameName(shardFrame);
    addChild(sprite, 1);
    sprite->setScale(0.55f);
    cocos2d::CCPoint pos = convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f));
    sprite->setPosition(pos + cocos2d::CCPoint(0.0f, 1.0f));
}

void GJPathSprite::changeToLockedArt() {
    auto frame = cocos2d::CCString::createWithFormat("pathIcon_%02d_001.png", m_pathNumber);
    
    auto grayscale = CCSpriteGrayscale::createWithSpriteFrameName(frame->getCString());
    addChild(grayscale, -1);
    cocos2d::CCPoint pos = convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f));
    grayscale->setPosition(pos);
    
    float luminance = (m_pathNumber >= 6) ? 3.0f : 1.0f;
    grayscale->setEvenLuminance(luminance);
    grayscale->setColor(cocos2d::ccc3(40, 40, 40));
    
    auto lock = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
    addChild(lock, 1);
    pos = convertToNodeSpace(cocos2d::CCPoint(0.0f, 0.0f));
    lock->setPosition(pos);
    lock->setScale(1.0f);
    lock->setColor(cocos2d::ccc3(200, 100, 100));
}

void GJPathSprite::updateState() {
    auto stats = GameStatsManager::sharedState();
    int itemId = m_pathNumber + 5;
    
    if (stats->isItemUnlocked(GJItem, itemId)) {
        int pathProgress = stats->getStatFromKey(static_cast<StatKey>(m_pathNumber + 29));
        int rank = pathProgress / 100;
        
        if (rank > 9) {
            addShardSprite();
        } else {
            addRankLabel(rank);
        }
    } else {
        changeToLockedArt();
    }
}

GJPathSprite::~GJPathSprite() {
}