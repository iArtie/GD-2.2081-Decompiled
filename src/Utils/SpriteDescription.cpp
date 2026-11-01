#include "Utils/SpriteDescription.h"
#include "Utils/DS_Dictionary.h"

SpriteDescription* SpriteDescription::createDescription(DS_Dictionary* dict) {
    SpriteDescription* returnVal = new SpriteDescription();
    if (returnVal->initDescription(dict)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool SpriteDescription::initDescription(DS_Dictionary* dict) {
    cocos2d::CCString* str;

    str = (cocos2d::CCString*)dict->objectForKey("position");
    m_position = cocos2d::CCPointFromString(str->getCString());

    str = (cocos2d::CCString*)dict->objectForKey("flipped");
    m_flipped = cocos2d::CCPointFromString(str->getCString());

    str = (cocos2d::CCString*)dict->objectForKey("scale");
    m_scale = cocos2d::CCPointFromString(str->getCString());

    str = (cocos2d::CCString*)dict->objectForKey("rotation");
    m_rotation = str->floatValue();

    str = (cocos2d::CCString*)dict->objectForKey("zValue");
    m_zValue = str->intValue();

    str = (cocos2d::CCString*)dict->objectForKey("tag");
    m_tag = str->intValue();

    str = (cocos2d::CCString*)dict->objectForKey("usesCustomTag");
    m_usesCustomTag = str->boolValue() ? 1 : 0;

    if (m_usesCustomTag) {
        str = (cocos2d::CCString*)dict->objectForKey("texture");
        std::string textureName = str->getCString();
        m_texture = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(textureName.c_str());
        m_texture->retain();
        m_texture->m_strFrameName = textureName;
    }

    return true;
}

SpriteDescription::~SpriteDescription() {
}