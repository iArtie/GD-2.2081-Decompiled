#include "GJRewardItem.h"
#include "GJRewardObject.h"
#include "GameStatsManager.h"
#include "DS_Dictionary.h"

GJRewardItem* GJRewardItem::create() {
    auto item = new GJRewardItem();
    if (item && item->init(0, 0, "")) {
        item->autorelease();
        return item;
    }
    delete item;
    return nullptr;
}

GJRewardItem* GJRewardItem::createWithCoder(DS_Dictionary* dict) {
    auto item = GJRewardItem::create();
    item->dataLoaded(dict);
    return item;
}

GJRewardItem* GJRewardItem::createWithObject(GJRewardType type, GJRewardObject* object) {
    auto array = cocos2d::CCArray::createWithObject(object);
    return createWithObjects(type, array);
}

GJRewardItem* GJRewardItem::createWithObjects(GJRewardType type, cocos2d::CCArray* objects) {
    auto item = GJRewardItem::create();
    item->m_rewardType = type;
    if (objects != item->m_rewardObjects) {
        if (objects) {
            objects->retain();
        }
        if (item->m_rewardObjects) {
            item->m_rewardObjects->release();
        }
        item->m_rewardObjects = objects;
    }
    return item;
}

bool GJRewardItem::canEncode() {
    return true;
}

void GJRewardItem::dataLoaded(DS_Dictionary* dict) {
    m_chestID = dict->getIntegerForKey("1");
    m_rewardType = static_cast<GJRewardType>(dict->getIntegerForKey("2"));
    
    auto array = dict->getArrayForKey("3");
    if (array != m_rewardObjects) {
        if (array) {
            array->retain();
        }
        if (m_rewardObjects) {
            m_rewardObjects->release();
        }
        m_rewardObjects = array;
    }
}

void GJRewardItem::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 8);
    dict->setIntegerForKey("1", m_chestID);
    dict->setIntegerForKey("2", static_cast<int>(m_rewardType));
    dict->setArrayForKey("3", m_rewardObjects);
}

bool GJRewardItem::init(int chestID, int timeRemaining, const std::string& rewardStr) {
    m_chestID = chestID;
    m_timeRemaining = timeRemaining;
    m_rewardObjects = cocos2d::CCArray::create();
    m_rewardObjects->retain();
    
    if (rewardStr.empty()) {
        return true;
    }
    
    auto parts = splitString(rewardStr, ",");
    if (parts.size() > 3) {
        int type1 = std::stoi(parts[0]);
        int type2 = std::stoi(parts[1]);
        int count1 = std::stoi(parts[2]);
        int count2 = std::stoi(parts[3]);
        
        SpecialRewardItem shardType = SpecialRewardItem::FireShard;
        if (count1 == count2) {
            shardType = SpecialRewardItem::IceShard;
            count2 = 0;
        }
        
        auto rewards = cocos2d::CCArray::create();
        
        if (type1 > 0) {
            rewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(type1), 0));
        }
        if (type2 > 0) {
            rewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(type2), 0));
        }
        if (count1 > 0) {
            rewards->addObject(GJRewardObject::create(shardType, count1));
        }
        if (count2 > 0) {
            rewards->addObject(GJRewardObject::create(SpecialRewardItem::FireShard, count2));
        }
        
        if (rewards->count() > 0 && rewards != m_rewardObjects) {
            if (rewards) {
                rewards->retain();
            }
            if (m_rewardObjects) {
                m_rewardObjects->release();
            }
            m_rewardObjects = rewards;
        }
    }
    
    return true;
}

SpecialRewardItem GJRewardItem::getNextShardType(SpecialRewardItem type) {
    static SpecialRewardItem shardOrder[] = {
        SpecialRewardItem::IceShard,
        SpecialRewardItem::PoisonShard,
        SpecialRewardItem::ShadowShard,
        SpecialRewardItem::LavaShard,
        SpecialRewardItem::EarthShard,
        SpecialRewardItem::BloodShard,
        SpecialRewardItem::MetalShard,
        SpecialRewardItem::LightShard,
        SpecialRewardItem::SoulShard,
        SpecialRewardItem::FireShard
    };
    
    int idx = static_cast<int>(type) - 1;
    if (idx >= 0 && idx < 10) {
        return shardOrder[idx];
    }
    return SpecialRewardItem::FireShard;
}

SpecialRewardItem GJRewardItem::getRandomShardType() {
    static SpecialRewardItem shardTypes[] = {
        SpecialRewardItem::FireShard,
        SpecialRewardItem::IceShard,
        SpecialRewardItem::PoisonShard,
        SpecialRewardItem::ShadowShard,
        SpecialRewardItem::LavaShard,
        SpecialRewardItem::EarthShard,
        SpecialRewardItem::BloodShard,
        SpecialRewardItem::MetalShard,
        SpecialRewardItem::LightShard,
        SpecialRewardItem::SoulShard
    };
    
    int idx = static_cast<int>(rand() * 4.6566e-10f * 10.0f);
    if (idx >= 0 && idx < 10) {
        return shardTypes[idx];
    }
    return SpecialRewardItem::FireShard;
}

SpecialRewardItem GJRewardItem::getRandomNonMaxShardType() {
    auto stats = GameStatsManager::sharedState();
    SpecialRewardItem shardType = getRandomShardType();
    
    for (int attempt = 0; attempt < 10; ++attempt) {
        std::string statKey = rewardItemToStat(shardType);
        int count = stats->getStat(statKey.c_str());
        
        if (count <= 99) {
            return shardType;
        }
        
        shardType = getNextShardType(shardType);
    }
    
    return SpecialRewardItem::FireShard;
}

int GJRewardItem::getRewardCount(SpecialRewardItem type) {
    if (!m_rewardObjects) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < m_rewardObjects->count(); ++i) {
        auto obj = m_rewardObjects->objectAtIndex(i);
        if (obj->m_rewardItem == type) {
            count += obj->m_count;
        }
    }
    return count;
}

GJRewardObject* GJRewardItem::getRewardObjectForType(SpecialRewardItem type) {
    if (!m_rewardObjects) {
        m_rewardObjects = cocos2d::CCArray::create();
        m_rewardObjects->retain();
    }
    
    for (int i = 0; i < m_rewardObjects->count(); ++i) {
        auto obj = m_rewardObjects->objectAtIndex(i);
        if (obj->m_rewardItem == type) {
            return obj;
        }
    }
    
    auto obj = GJRewardObject::create(type, 0);
    m_rewardObjects->addObject(obj);
    return obj;
}

bool GJRewardItem::isShardType(SpecialRewardItem type) {
    switch (type) {
        case SpecialRewardItem::FireShard:
        case SpecialRewardItem::IceShard:
        case SpecialRewardItem::PoisonShard:
        case SpecialRewardItem::ShadowShard:
        case SpecialRewardItem::LavaShard:
        case SpecialRewardItem::EarthShard:
        case SpecialRewardItem::BloodShard:
        case SpecialRewardItem::MetalShard:
        case SpecialRewardItem::LightShard:
        case SpecialRewardItem::SoulShard:
            return true;
        default:
            return false;
    }
}

std::string GJRewardItem::rewardItemToStat(SpecialRewardItem rewardItem) {
    switch (rewardItem) {
        case SpecialRewardItem::FireShard: return "18";
        case SpecialRewardItem::IceShard: return "19";
        case SpecialRewardItem::PoisonShard: return "17";
        case SpecialRewardItem::ShadowShard: return "16";
        case SpecialRewardItem::LavaShard: return "20";
        case SpecialRewardItem::BonusKey: return "21";
        case SpecialRewardItem::EarthShard: return "23";
        case SpecialRewardItem::BloodShard: return "24";
        case SpecialRewardItem::MetalShard: return "25";
        case SpecialRewardItem::LightShard: return "26";
        case SpecialRewardItem::SoulShard: return "27";
        case SpecialRewardItem::GoldKey: return "43";
        default: return "";
    }
}

GJRewardItem::~GJRewardItem() {
    if (m_rewardObjects) {
        m_rewardObjects->release();
    }
}