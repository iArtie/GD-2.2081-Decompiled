#include "GJSmartTemplate.h"
#include "GJSmartPrefab.h"
#include "SmartGameObject.h"
#include "ObjectToolbox.h"
#include "GameToolbox.h"
#include "DS_Dictionary.h"

GJSmartTemplate* GJSmartTemplate::create() {
    auto templateObj = new GJSmartTemplate();
    if (templateObj && templateObj->init()) {
        templateObj->autorelease();
        return templateObj;
    }
    delete templateObj;
    return nullptr;
}

GJSmartTemplate* GJSmartTemplate::createWithCoder(DS_Dictionary* dict) {
    auto templateObj = GJSmartTemplate::create();
    templateObj->dataLoaded(dict);
    return templateObj;
}

bool GJSmartTemplate::init() {
    m_prefabArrays = cocos2d::CCDictionary::create();
    m_prefabArrays->retain();
    
    m_remapDict1 = cocos2d::CCDictionary::create();
    m_remapDict1->retain();
    
    m_remapDict2 = cocos2d::CCDictionary::create();
    m_remapDict2->retain();
    
    m_remapDict3 = cocos2d::CCDictionary::create();
    m_remapDict3->retain();
    
    m_remapDict4 = cocos2d::CCDictionary::create();
    m_remapDict4->retain();
    
    m_remapDict5 = cocos2d::CCDictionary::create();
    m_remapDict5->retain();
    
    m_remapDict6 = cocos2d::CCDictionary::create();
    m_remapDict6->retain();
    
    m_allowRotation = false;
    m_allowFlipX = false;
    m_allowFlipY = false;
    m_ignoreCorners = false;
    
    return true;
}

bool GJSmartTemplate::canEncode() {
    return true;
}

void GJSmartTemplate::dataLoaded(DS_Dictionary* dict) {
    m_nameIndex = dict->getStringForKey("1");
    m_templateName = dict->getStringForKey("2");
    m_allowRotation = dict->getIntegerForKey("4") != 0;
    m_allowFlipX = dict->getIntegerForKey("5") != 0;
    m_allowFlipY = dict->getIntegerForKey("6") != 0;
    m_ignoreCorners = dict->getIntegerForKey("7") != 0;
    
    auto dictForKey = dict->getDictForKey("3");
    if (dictForKey != m_prefabArrays) {
        if (dictForKey) {
            dictForKey->retain();
        }
        if (m_prefabArrays) {
            m_prefabArrays->release();
        }
        m_prefabArrays = dictForKey;
    }
    
    auto keys = m_prefabArrays->allKeys();
    for (int i = 0; i < keys->count(); ++i) {
        auto key = static_cast<cocos2d::CCString*>(keys->objectAtIndex(i));
        saveRemap(key->getCString());
    }
}

void GJSmartTemplate::encodeWithCoder(DS_Dictionary* dict) {
    dict->setIntegerForKey("kCEK", 10);
    dict->setStringForKey("1", m_nameIndex);
    dict->setStringForKey("2", m_templateName);
    dict->setIntegerForKey("4", m_allowRotation ? 1 : 0);
    dict->setIntegerForKey("5", m_allowFlipX ? 1 : 0);
    dict->setIntegerForKey("6", m_allowFlipY ? 1 : 0);
    dict->setIntegerForKey("7", m_ignoreCorners ? 1 : 0);
    dict->setDictForKey("3", m_prefabArrays);
}

void GJSmartTemplate::applyTransformationsForType(SmartBlockType type, cocos2d::CCSprite* sprite) {
    switch (type) {
        case SmartBlockType::NormalSlope2:
        case SmartBlockType::WideSlope11:
        case SmartBlockType::WideSlope12:
            sprite->setRotation(90.0f);
            break;
        case SmartBlockType::NormalSlope3:
            sprite->setRotation(270.0f);
            break;
        case SmartBlockType::NormalSlope4:
        case SmartBlockType::WideSlope7:
        case SmartBlockType::WideSlope8:
            sprite->setRotation(180.0f);
            break;
        case SmartBlockType::WideSlope3:
        case SmartBlockType::WideSlope4:
            sprite->setFlipX(true);
            break;
        case SmartBlockType::WideSlope5:
        case SmartBlockType::WideSlope6:
            sprite->setFlipY(true);
            break;
        case SmartBlockType::WideSlope9:
        case SmartBlockType::WideSlope10:
            sprite->setFlipX(true);
            sprite->setRotation(-90.0f);
            break;
        case SmartBlockType::WideSlope13:
        case SmartBlockType::WideSlope14:
            sprite->setRotation(-90.0f);
            break;
        case SmartBlockType::WideSlope15:
        case SmartBlockType::WideSlope16:
            sprite->setFlipY(true);
            sprite->setRotation(-90.0f);
            break;
        default:
            break;
    }
}

SmartBlockType GJSmartTemplate::flipBlockType(SmartBlockType type, bool flipX, bool flipY) {
    if (flipX) {
        type = flipBlockTypeX(type);
        if (flipY) {
            type = flipBlockTypeY(type);
        }
    } else if (flipY) {
        type = flipBlockTypeY(type);
    }
    return type;
}

SmartBlockType GJSmartTemplate::flipBlockTypeX(SmartBlockType type) {
    switch (type) {
        case SmartBlockType::NormalSlope1: return SmartBlockType::NormalSlope2;
        case SmartBlockType::NormalSlope2: return SmartBlockType::NormalSlope1;
        case SmartBlockType::NormalSlope3: return SmartBlockType::NormalSlope4;
        case SmartBlockType::NormalSlope4: return SmartBlockType::NormalSlope3;
        case SmartBlockType::WideSlope1: return SmartBlockType::WideSlope3;
        case SmartBlockType::WideSlope2: return SmartBlockType::WideSlope4;
        case SmartBlockType::WideSlope3: return SmartBlockType::WideSlope1;
        case SmartBlockType::WideSlope4: return SmartBlockType::WideSlope2;
        case SmartBlockType::WideSlope5: return SmartBlockType::WideSlope7;
        case SmartBlockType::WideSlope6: return SmartBlockType::WideSlope8;
        case SmartBlockType::WideSlope7: return SmartBlockType::WideSlope5;
        case SmartBlockType::WideSlope8: return SmartBlockType::WideSlope6;
        case SmartBlockType::WideSlope9: return SmartBlockType::WideSlope11;
        case SmartBlockType::WideSlope10: return SmartBlockType::WideSlope12;
        case SmartBlockType::WideSlope11: return SmartBlockType::WideSlope9;
        case SmartBlockType::WideSlope12: return SmartBlockType::WideSlope10;
        case SmartBlockType::WideSlope13: return SmartBlockType::WideSlope15;
        case SmartBlockType::WideSlope14: return SmartBlockType::WideSlope16;
        case SmartBlockType::WideSlope15: return SmartBlockType::WideSlope13;
        case SmartBlockType::WideSlope16: return SmartBlockType::WideSlope14;
        default: return type;
    }
}

SmartBlockType GJSmartTemplate::flipBlockTypeY(SmartBlockType type) {
    switch (type) {
        case SmartBlockType::NormalSlope1: return SmartBlockType::NormalSlope3;
        case SmartBlockType::NormalSlope2: return SmartBlockType::NormalSlope4;
        case SmartBlockType::NormalSlope3: return SmartBlockType::NormalSlope1;
        case SmartBlockType::NormalSlope4: return SmartBlockType::NormalSlope2;
        case SmartBlockType::WideSlope1: return SmartBlockType::WideSlope5;
        case SmartBlockType::WideSlope2: return SmartBlockType::WideSlope6;
        case SmartBlockType::WideSlope3: return SmartBlockType::WideSlope7;
        case SmartBlockType::WideSlope4: return SmartBlockType::WideSlope8;
        case SmartBlockType::WideSlope5: return SmartBlockType::WideSlope1;
        case SmartBlockType::WideSlope6: return SmartBlockType::WideSlope2;
        case SmartBlockType::WideSlope7: return SmartBlockType::WideSlope3;
        case SmartBlockType::WideSlope8: return SmartBlockType::WideSlope4;
        case SmartBlockType::WideSlope9: return SmartBlockType::WideSlope13;
        case SmartBlockType::WideSlope10: return SmartBlockType::WideSlope14;
        case SmartBlockType::WideSlope11: return SmartBlockType::WideSlope15;
        case SmartBlockType::WideSlope12: return SmartBlockType::WideSlope16;
        case SmartBlockType::WideSlope13: return SmartBlockType::WideSlope9;
        case SmartBlockType::WideSlope14: return SmartBlockType::WideSlope10;
        case SmartBlockType::WideSlope15: return SmartBlockType::WideSlope11;
        case SmartBlockType::WideSlope16: return SmartBlockType::WideSlope12;
        default: return type;
    }
}

std::string GJSmartTemplate::flipKey(const std::string& key, bool flipX, bool flipY) {
    if (!flipX && !flipY) {
        return key;
    }
    
    if (key.length() < 9) {
        return "";
    }
    
    char center = key[0];
    char right = key[1];
    char up = key[2];
    char down = key[3];
    char upRight = key[4];
    char downRight = key[5];
    char downLeft = key[6];
    char upLeft = key[7];
    char center2 = key[8];
    
    char newUpLeft, newUpRight, newDownLeft, newDownRight, newRight, newUp, newDown, newCenter;
    
    if (!flipX) {
        if (flipY) {
            // Flip Y only - swap up/down
            newUpLeft = downLeft;
            newUpRight = downRight;
            newDownLeft = upLeft;
            newDownRight = upRight;
            newRight = right;
            newUp = down;
            newDown = up;
            newCenter = center2;
        } else {
            // No flip
            newUpLeft = 0;
            newUpRight = 0;
            newDownLeft = 0;
            newDownRight = 0;
            newRight = 0;
            newUp = 0;
            newDown = 0;
            newCenter = 0;
        }
    } else if (flipY) {
        // Flip X and Y - swap both
        newUpLeft = downRight;
        newUpRight = downLeft;
        newDownLeft = upRight;
        newDownRight = upLeft;
        newRight = right;
        newUp = down;
        newDown = up;
        newCenter = center2;
    } else {
        // Flip X only - swap left/right
        newUpLeft = upRight;
        newUpRight = upLeft;
        newDownLeft = downRight;
        newDownRight = downLeft;
        newRight = right;
        newUp = up;
        newDown = down;
        newCenter = center2;
    }
    
    char flippedCenter = GJSmartTemplate::flipBlockType(center, flipX, flipY);
    char flippedRight = GJSmartTemplate::flipBlockType(right, flipX, flipY);
    char flippedUp = GJSmartTemplate::flipBlockType(up, flipX, flipY);
    char flippedDown = GJSmartTemplate::flipBlockType(down, flipX, flipY);
    char flippedUpRight = GJSmartTemplate::flipBlockType(upRight, flipX, flipY);
    char flippedDownRight = GJSmartTemplate::flipBlockType(downRight, flipX, flipY);
    char flippedDownLeft = GJSmartTemplate::flipBlockType(downLeft, flipX, flipY);
    char flippedUpLeft = GJSmartTemplate::flipBlockType(upLeft, flipX, flipY);
    char flippedCenter2 = GJSmartTemplate::flipBlockType(center2, flipX, flipY);
    
    std::string result;
    result.push_back(flippedCenter);
    result.push_back(flippedRight);
    result.push_back(flippedUp);
    result.push_back(flippedDown);
    result.push_back(flippedUpRight);
    result.push_back(flippedDownRight);
    result.push_back(flippedDownLeft);
    result.push_back(flippedUpLeft);
    result.push_back(flippedCenter2);
    
    return result;
}


SmartBlockType GJSmartTemplate::rotateBlockType(SmartBlockType type, int degrees) {
    if (type == SmartBlockType::Block) {
        return type;
    }
    
    while (degrees > 0) {
        degrees -= 90;
        type = rotateBlockType90(type);
    }
    return type;
}

SmartBlockType GJSmartTemplate::rotateBlockType90(SmartBlockType type) {
    switch (type) {
        case SmartBlockType::NormalSlope1: return SmartBlockType::NormalSlope2;
        case SmartBlockType::NormalSlope2: return SmartBlockType::NormalSlope4;
        case SmartBlockType::NormalSlope3: return SmartBlockType::NormalSlope1;
        case SmartBlockType::NormalSlope4: return SmartBlockType::NormalSlope3;
        case SmartBlockType::WideSlope1: return SmartBlockType::WideSlope11;
        case SmartBlockType::WideSlope2: return SmartBlockType::WideSlope12;
        case SmartBlockType::WideSlope3: return SmartBlockType::WideSlope15;
        case SmartBlockType::WideSlope4: return SmartBlockType::WideSlope16;
        case SmartBlockType::WideSlope5: return SmartBlockType::WideSlope9;
        case SmartBlockType::WideSlope6: return SmartBlockType::WideSlope10;
        case SmartBlockType::WideSlope7: return SmartBlockType::WideSlope13;
        case SmartBlockType::WideSlope8: return SmartBlockType::WideSlope14;
        case SmartBlockType::WideSlope9: return SmartBlockType::WideSlope3;
        case SmartBlockType::WideSlope10: return SmartBlockType::WideSlope4;
        case SmartBlockType::WideSlope11: return SmartBlockType::WideSlope7;
        case SmartBlockType::WideSlope12: return SmartBlockType::WideSlope8;
        case SmartBlockType::WideSlope13: return SmartBlockType::WideSlope1;
        case SmartBlockType::WideSlope14: return SmartBlockType::WideSlope2;
        case SmartBlockType::WideSlope15: return SmartBlockType::WideSlope5;
        case SmartBlockType::WideSlope16: return SmartBlockType::WideSlope6;
        default: return type;
    }
}

std::string GJSmartTemplate::rotateKey(const std::string& key, int degrees) {
    if (degrees == 0) {
        return key;
    }
    
    if (key.length() < 9) {
        return "";
    }
    
    char center = key[0];
    char right = key[1];
    char up = key[2];
    char down = key[3];
    char upRight = key[4];
    char downRight = key[5];
    char downLeft = key[6];
    char upLeft = key[7];
    char center2 = key[8];
    
    char newRight, newUp, newDown, newUpRight, newDownRight, newDownLeft, newUpLeft, newCenter;
    
    if (degrees == 90) {
        newUpLeft = downRight;
        newUpRight = key[8];
        newDownLeft = upRight;
        newDownRight = upLeft;
        newRight = down;
        newUp = right;
        newDown = up;
        newCenter = center2;
    } else if (degrees == 180) {
        newUpLeft = upRight;
        newUpRight = downRight;
        newDownLeft = upLeft;
        newDownRight = key[8];
        newRight = down;
        newUp = up;
        newDown = down;
        newCenter = center2;
    } else if (degrees == 270) {
        newUpLeft = upLeft;
        newUpRight = upRight;
        newDownLeft = key[8];
        newDownRight = downRight;
        newRight = up;
        newUp = down;
        newDown = right;
        newCenter = center2;
    }
    
    char rotatedCenter = GJSmartTemplate::rotateBlockType(center, degrees);
    char rotatedCenter2 = GJSmartTemplate::rotateBlockType(center2, degrees);
    char rotatedRight = GJSmartTemplate::rotateBlockType(right, degrees);
    char rotatedUp = GJSmartTemplate::rotateBlockType(up, degrees);
    char rotatedDown = GJSmartTemplate::rotateBlockType(down, degrees);
    char rotatedUpRight = GJSmartTemplate::rotateBlockType(upRight, degrees);
    char rotatedDownRight = GJSmartTemplate::rotateBlockType(downRight, degrees);
    char rotatedDownLeft = GJSmartTemplate::rotateBlockType(downLeft, degrees);
    char rotatedUpLeft = GJSmartTemplate::rotateBlockType(upLeft, degrees);
    char rotatedNewCenter = GJSmartTemplate::rotateBlockType(center2, degrees);
    
    std::string result;
    result.push_back(rotatedCenter);
    result.push_back(rotatedRight);
    result.push_back(rotatedUp);
    result.push_back(rotatedDown);
    result.push_back(rotatedUpRight);
    result.push_back(rotatedDownRight);
    result.push_back(rotatedDownLeft);
    result.push_back(rotatedUpLeft);
    result.push_back(rotatedCenter2);
    
    return result;
}


cocos2d::CCPoint GJSmartTemplate::offsetForType(SmartBlockType type) {
    switch (type) {
        case SmartBlockType::WideSlope1:
        case SmartBlockType::WideSlope4:
        case SmartBlockType::WideSlope5:
        case SmartBlockType::WideSlope8:
            return cocos2d::CCPoint(15.0f, 0.0f);
        case SmartBlockType::WideSlope2:
        case SmartBlockType::WideSlope3:
        case SmartBlockType::WideSlope6:
        case SmartBlockType::WideSlope7:
            return cocos2d::CCPoint(-15.0f, 0.0f);
        case SmartBlockType::WideSlope9:
        case SmartBlockType::WideSlope11:
        case SmartBlockType::WideSlope14:
        case SmartBlockType::WideSlope16:
            return cocos2d::CCPoint(0.0f, -15.0f);
        case SmartBlockType::WideSlope10:
        case SmartBlockType::WideSlope12:
        case SmartBlockType::WideSlope13:
        case SmartBlockType::WideSlope15:
            return cocos2d::CCPoint(0.0f, 15.0f);
        default:
            return cocos2d::CCPoint(0.0f, 0.0f);
    }
}

cocos2d::CCPoint GJSmartTemplate::offsetForDir(GJSmartDirection direction, int key) {
    auto toolbox = ObjectToolbox::sharedState();
    float gridSize = toolbox->gridNodeSizeForKey(key);
    float offset = (gridSize != 15.0f) ? 30.0f : 15.0f;
    
    switch (direction) {
        case GJSmartDirection::Up: return cocos2d::CCPoint(0.0f, offset);
        case GJSmartDirection::Down: return cocos2d::CCPoint(0.0f, -offset);
        case GJSmartDirection::Left: return cocos2d::CCPoint(-offset, 0.0f);
        case GJSmartDirection::Right: return cocos2d::CCPoint(offset, 0.0f);
        case GJSmartDirection::UpLeft: return cocos2d::CCPoint(-offset, offset);
        case GJSmartDirection::UpRight: return cocos2d::CCPoint(offset, offset);
        case GJSmartDirection::DownLeft: return cocos2d::CCPoint(-offset, -offset);
        case GJSmartDirection::DownRight: return cocos2d::CCPoint(offset, -offset);
        default: return cocos2d::CCPoint(0.0f, 0.0f);
    }
}

cocos2d::CCPoint GJSmartTemplate::offsetForObject(SmartGameObject* object) {
    if (object) {
        SmartBlockType type = smartObjectToType(object);
        return offsetForType(type);
    }
    return cocos2d::CCPoint(0.0f, 0.0f);
}

std::string GJSmartTemplate::getSimplifiedKey(const std::string& key) {
    if (key.length() < 9) {
        return "";
    }
    
    int left = key[0];
    int right = key[1];
    int up = key[2];
    int down = key[3];
    int upRight = key[4];
    int downRight = key[5];
    int downLeft = key[6];
    int upLeft = key[7];
    int center = key[8];
    
    bool changed = false;
    
    // Simplify right neighbor
    switch (right) {
        case '4': case '5': case 'A': case 'B': case 'C': case 'D':
        case 'I': case 'J': case 'K': case 'L':
            right = '0';
            changed = true;
            break;
        default:
            break;
    }
    
    // Simplify upRight neighbor
    switch (upRight) {
        case '3': case '4': case '5': case '6': case '7': case '8':
        case 'D': case 'E': case 'F': case 'G': case 'H':
            if (upRight != down) {
                upRight = '0';
                changed = true;
            }
            break;
        case 'A': case 'B': case 'C':
            if (upRight != down) {
                upRight = '0';
                changed = true;
            }
            break;
        default:
            break;
    }
    
    // Simplify downRight neighbor
    switch (downRight) {
        case '3': case '4': case '5': case '6': case '7': case '8':
        case 'D': case 'E': case 'F': case 'G': case 'H':
            if (downRight != upRight) {
                downRight = '0';
                changed = true;
            }
            break;
        case 'A': case 'B': case 'C':
            if (downRight != upRight) {
                downRight = '0';
                changed = true;
            }
            break;
        default:
            break;
    }
    
    // Simplify up neighbor
    switch (up) {
        case '2': case '3': case '6': case '7': case '8': case '9':
        case 'E': case 'F': case 'G': case 'H':
            up = '0';
            changed = true;
            break;
        default:
            break;
    }
    
    // Simplify upLeft neighbor
    switch (upLeft) {
        case '2': case '3': case '6': case '7': case '8': case '9':
            upLeft = '0';
            changed = true;
            break;
        case 'E': case 'F': case 'G': case 'H':
            if (upLeft != down) {
                upLeft = '0';
                changed = true;
            }
            break;
        default:
            break;
    }
    
    // Simplify downLeft neighbor
    switch (downLeft) {
        case '2': case '3': case '6': case '7': case '8': case '9':
            downLeft = '0';
            changed = true;
            break;
        case 'E': case 'F': case 'G': case 'H':
            if (downLeft == upRight) {
                downLeft = upRight;
            } else {
                downLeft = '0';
                changed = true;
            }
            break;
        default:
            break;
    }
    
    // Simplify down neighbor
    switch (down) {
        case '3': case '5': case '8': case 'C': case 'G': case 'K':
            down = '0';
            changed = true;
            break;
        default:
            break;
    }
    
    // Simplify upRight neighbor
    switch (upRight) {
        case '2': case '4': case '6': case 'A': case 'E': case 'I':
            upRight = '0';
            changed = true;
            break;
        default:
            break;
    }
    
    // Simplify center based on pattern
    switch (center) {
        case '2': case '6': case 'E':
            upRight = '0';
            right = '0';
            down = '0';
            changed = true;
            break;
        case '3': case '8': case 'G':
            downRight = '0';
            upRight = '0';
            right = '0';
            changed = true;
            break;
        case '4': case 'A': case 'I':
            upLeft = '0';
            down = '0';
            up = '0';
            changed = true;
            break;
        case '5': case 'C': case 'K':
            downLeft = '0';
            upRight = '0';
            up = '0';
            changed = true;
            break;
        default:
            break;
    }
    
    // Additional simplification logic
    int simplifiedUpRight = upRight;
    int simplifiedDownRight = downRight;
    int simplifiedUpLeft = upLeft;
    int simplifiedDownLeft = downLeft;
    
    int simpleCenter = GJSmartTemplate::getSimplifiedType(center, &changed);
    int simpleDownRight = GJSmartTemplate::getSimplifiedType(downRight, &changed);
    int simpleUpLeft = GJSmartTemplate::getSimplifiedType(upLeft, &changed);
    int simpleDownLeft = GJSmartTemplate::getSimplifiedType(downLeft, &changed);
    
    if (simpleCenter != '0' && ((down & 0xFFFFFFFD) == '0' || (right & 0xFFFFFFFD) == '0')) {
        simpleCenter = '0';
        changed = true;
    }
    
    if (simpleDownRight != '0' && (right == '0' || upRight == '0' || right == '3' || upRight == '3')) {
        simpleDownRight = '0';
        changed = true;
    }
    
    if (simpleUpLeft != '0' && ((down & 0xFFFFFFFB) == '0' || (up & 0xFFFFFFFB) == '0')) {
        simpleUpLeft = '0';
        changed = true;
    }
    
    if (simpleDownLeft == '0') {
        if (simpleDownRight != '0' || simpleCenter != '0') {
            goto build_key;
        }
        if (simpleUpLeft != '0') {
            goto finalize;
        }
    } else {
        if (up != '0' && upRight != '0' && up != '5' && upRight != '5') {
            goto build_key;
        }
        changed = true;
        if (simpleDownRight != '0' || simpleCenter != '0') {
            simpleDownLeft = '0';
            goto build_key;
        }
        if (simpleUpLeft != '0') {
            goto finalize;
        }
    }
    
    if (down == '0' && right != '0' && upRight == '0') {
        changed = true;
        if (up == '0' || down != '0') {
            simpleDownLeft = '0';
            right = '1';
            upRight = '0';
            goto build_key;
        }
        right = '1';
        changed = true;
        up = '1';
    } else if (down == '0' && up != '0' && upRight == '0') {
        changed = true;
        up = '1';
    } else if (right == '0' && down != '0' && up == '0') {
        changed = true;
        if (upRight == '0' || (down = '1', right != '0')) {
            simpleDownLeft = '0';
            down = '1';
            up = '0';
            goto build_key;
        }
        simpleDownLeft = '0';
        down = '1';
        changed = true;
        up = '0';
        right = '0';
        goto build_key;
    } else if (upRight != '0' && right == '0') {
        if (up != '0') {
            simpleDownLeft = '0';
            right = '0';
            if (!changed) {
                goto build_key;
            }
        } else {
            simpleDownLeft = '0';
            upRight = '1';
            changed = true;
            up = '0';
            right = '0';
            goto build_key;
        }
    }
    
finalize:
    simpleDownLeft = '0';
    
build_key:
    if (!changed) {
        return "";
    }
    
    std::string result;
    result.push_back(center);
    result.push_back(right);
    result.push_back(up);
    result.push_back(down);
    result.push_back(upRight);
    result.push_back(downRight);
    result.push_back(downLeft);
    result.push_back(upLeft);
    result.push_back(simpleUpLeft);
    result.push_back(simpleDownLeft);
    result.push_back(simpleCenter);
    result.push_back(simpleDownRight);
    
    return result;
}


SmartBlockType GJSmartTemplate::getSimplifiedType(SmartBlockType type, bool* changed) {
    // Convert complex slope types to simple block
    switch (type) {
        case SmartBlockType::NormalSlope1:
        case SmartBlockType::NormalSlope2:
        case SmartBlockType::NormalSlope3:
        case SmartBlockType::NormalSlope4:
        case SmartBlockType::WideSlope1:
        case SmartBlockType::WideSlope2:
        case SmartBlockType::WideSlope3:
        case SmartBlockType::WideSlope4:
        case SmartBlockType::WideSlope5:
        case SmartBlockType::WideSlope6:
        case SmartBlockType::WideSlope7:
        case SmartBlockType::WideSlope8:
        case SmartBlockType::WideSlope9:
        case SmartBlockType::WideSlope10:
        case SmartBlockType::WideSlope11:
        case SmartBlockType::WideSlope12:
        case SmartBlockType::WideSlope13:
        case SmartBlockType::WideSlope14:
        case SmartBlockType::WideSlope15:
        case SmartBlockType::WideSlope16:
            *changed = true;
            return SmartBlockType::Block;
        default:
            return type;
    }
}

std::string GJSmartTemplate::getVerySimplifiedKey(const std::string& key) {
    // Very simplified key - all non-block neighbors become 1
    std::string result = key;
    bool changed = false;
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] != '0' && result[i] != '1') {
            result[i] = '1';
            changed = true;
        }
    }
    if (!changed) {
        return "";
    }
    return result;
}

std::string GJSmartTemplate::getNoCornerKey(const std::string& key) {
    if (key.empty()) {
        return "";
    }
    // Remove corner information from key
    std::string result = key;
    if (result.length() >= 4) {
        result.replace(result.length() - 4, 4, "0000");
    }
    return result;
}

bool GJSmartTemplate::isUnrequired(const std::string& key) {
    // Check if this key represents an unrequired pattern
    if (key.length() < 9) {
        return true;
    }
    
    bool left = key[0] != '0';
    bool right = key[1] != '0';
    bool up = key[2] != '0';
    bool down = key[3] != '0';
    bool upLeft = key[4] != '0';
    bool upRight = key[5] != '0';
    bool downLeft = key[6] != '0';
    bool downRight = key[7] != '0';
    bool center = key[8] != '0';
    
    // Complex unrequired logic
    return false;
}

GJSmartPrefab* GJSmartTemplate::getPrefab(
    SmartGameObject* obj1,
    SmartGameObject* obj2,
    SmartGameObject* obj3,
    SmartGameObject* obj4,
    SmartGameObject* obj5,
    SmartGameObject* obj6,
    SmartGameObject* obj7,
    SmartGameObject* obj8,
    SmartGameObject* obj9) {
    
    std::string key;
    keyFromNeighbors(obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9, key);
    
    return getPrefab(key, false, false);
}

GJSmartPrefab* GJSmartTemplate::getPrefabWithID(const std::string& key, int id) {
    if (id <= 0) {
        return nullptr;
    }
    
    auto array = getPrefabs(key);
    if (!array) {
        return nullptr;
    }
    
    for (int i = 0; i < array->count(); ++i) {
        auto prefab = static_cast<GJSmartPrefab*>(array->objectAtIndex(i));
        if (prefab->m_prefabID == id) {
            return prefab;
        }
    }
    return nullptr;
}

cocos2d::CCArray* GJSmartTemplate::getPrefabs(const std::string& key) {
    return static_cast<cocos2d::CCArray*>(m_prefabArrays->objectForKey(key));
}

GJSmartPrefab* GJSmartTemplate::getRandomPrefab(const std::string& key) {
    auto prefabs = getPrefabs(key);
    if (!prefabs || prefabs->count() == 0) {
        return nullptr;
    }
    
    if (prefabs->count() == 1) {
        return static_cast<GJSmartPrefab*>(prefabs->objectAtIndex(0));
    }
    
    int totalChance = getTotalChanceForPrefab(key);
    float random = rand() * 4.6566e-10f * totalChance;
    int cumulative = 0;
    
    for (int i = 0; i < prefabs->count(); ++i) {
        auto prefab = static_cast<GJSmartPrefab*>(prefabs->objectAtIndex(i));
        cumulative += prefab->m_prefabChance;
        if (random <= cumulative || i == prefabs->count() - 1) {
            return prefab;
        }
    }
    return nullptr;
}

int GJSmartTemplate::getTotalChanceForPrefab(const std::string& key) {
    auto prefabs = getPrefabs(key);
    if (!prefabs || prefabs->count() == 0) {
        return 0;
    }
    
    int total = 0;
    for (int i = 0; i < prefabs->count(); ++i) {
        auto prefab = static_cast<GJSmartPrefab*>(prefabs->objectAtIndex(i));
        total += prefab->m_prefabChance;
    }
    return total;
}

void GJSmartTemplate::savePrefab(const std::string& key, const std::string& data) {
    auto array = getPrefabs(key);
    if (!array) {
        array = cocos2d::CCArray::create();
        m_prefabArrays->setObject(array, key);
    }
    
    // Check if prefab already exists
    for (int i = 0; i < array->count(); ++i) {
        auto prefab = static_cast<GJSmartPrefab*>(array->objectAtIndex(i));
        if (prefab->m_prefabData == data) {
            return;
        }
    }
    
    auto newPrefab = GJSmartPrefab::create();
    newPrefab->m_prefabData = data;
    newPrefab->m_prefabKey = key;
    array->addObject(newPrefab);
    saveRemap(key);
}

void GJSmartTemplate::saveRemap(const std::string& key) {
    // Save remapping dictionaries for this key
    std::string simplified = getSimplifiedKey(key);
    std::string noCorner = getNoCornerKey(key);
    std::string verySimplified = getVerySimplifiedKey(key);
    
    // Store in appropriate dictionaries
    if (!simplified.empty() && simplified != key) {
        m_remapDict1->setObject(cocos2d::CCString::create(key), simplified);
    }
    if (!noCorner.empty() && noCorner != key) {
        m_remapDict2->setObject(cocos2d::CCString::create(key), noCorner);
    }
    if (!verySimplified.empty() && verySimplified != key) {
        m_remapDict3->setObject(cocos2d::CCString::create(key), verySimplified);
    }
}

void GJSmartTemplate::removePrefab(const std::string& prefabKey, int prefabID) {
    if (prefabID <= 0) {
        return;
    }
    
    auto array = getPrefabs(prefabKey);
    if (!array) {
        return;
    }
    
    for (int i = 0; i < array->count(); ++i) {
        auto prefab = static_cast<GJSmartPrefab*>(array->objectAtIndex(i));
        if (prefab->m_prefabID == prefabID) {
            array->removeObjectAtIndex(i);
            return;
        }
    }
}

void GJSmartTemplate::resetScannedPrefabs() {
    m_prefabResults.clear();
}

void GJSmartTemplate::getTemplateState(std::vector<SmartPrefabResult>* results) {
    auto binaryDict = cocos2d::CCDictionary::create();
    auto node = cocos2d::CCNode::create();
    
    int binaryValues[8];
    generateAllBinaryStrings(8, binaryValues, 0, binaryDict, node);
    
    auto keys = binaryDict->allKeys();
    
    for (unsigned int idx = 0; idx < keys->count(); ++idx) {
        auto key = static_cast<cocos2d::CCString*>(keys->objectAtIndex(idx));
        
        std::string binaryKey;
        std::string prefabKey;
        prefabKeyFromBinary(binaryKey, prefabKey);
        
        auto prefabs = getPrefabs(prefabKey);
        bool unrequired = isUnrequired(prefabKey);
        
        GJSmartPrefab* smartPrefab = nullptr;
        std::string binaryKeyStr;
        std::string prefabKeyStr;
        int prefabCount = 0;
        bool isUnrequiredFlag = false;
        int rotation = 0;
        bool flipX = false;
        bool flipY = false;
        bool ignoreCorners = false;
        
        if (prefabs) {
            prefabCount = prefabs->count();
        }
        
        isUnrequiredFlag = unrequired;
        
        if (prefabCount <= 0 && !isUnrequiredFlag) {
            scanForPrefab(prefabKey, &smartPrefab, &binaryKeyStr, &prefabKeyStr);
            if (smartPrefab) {
                prefabCount = 1;
            }
        }
        
        SmartPrefabResult result;
        result.m_smartPrefab = smartPrefab;
        result.m_binaryKey = binaryKeyStr;
        result.m_prefabKey = prefabKeyStr;
        result.m_prefabCount = prefabCount;
        result.m_unrequired = isUnrequiredFlag;
        result.m_rotation = rotation;
        result.m_flipX = flipX;
        result.m_flipY = flipY;
        result.m_ignoreCorners = ignoreCorners;
        
        results->push_back(result);
    }
}

std::string GJSmartTemplate::logTemplateStatus(bool unused) {
    int defined = 0;
    int total = 0;
    int variations = 0;
    int prefabCount = 0;
    int requiredMissing = 0;
    
    auto binaryDict = cocos2d::CCDictionary::create();
    auto node = cocos2d::CCNode::create();
    
    int binaryValues[8];
    generateAllBinaryStrings(8, binaryValues, 0, binaryDict, node);
    
    auto keys = binaryDict->allKeys();
    
    for (unsigned int idx = 0; idx < keys->count(); ++idx) {
        auto key = static_cast<cocos2d::CCString*>(keys->objectAtIndex(idx));
        
        std::string binaryKey;
        std::string prefabKey;
        prefabKeyFromBinary(binaryKey, prefabKey);
        
        bool unrequired = isUnrequired(prefabKey);
        auto prefabs = getPrefabs(prefabKey);
        bool hasPrefab = prefabs && prefabs->count() > 0;
        
        if (prefabs) {
            ++total;
            if (unrequired) {
                if (hasPrefab) {
                    ++defined;
                    prefabCount += prefabs->count();
                }
            } else {
                ++defined;
                if (hasPrefab) {
                    prefabCount += prefabs->count();
                }
            }
        } else {
            ++total;
            if (unrequired) {
                if (hasPrefab) {
                    ++defined;
                    prefabCount += prefabs->count();
                }
            } else {
                ++defined;
                ++requiredMissing;
            }
        }
        
        if (hasPrefab || unrequired) {
            ++variations;
        }
    }
    
    return cocos2d::CCString::createWithFormat(
        "Defined: %i / %i(%i), Variations : %i, Required Missing : %i",
        defined,
        total,
        variations,
        prefabCount,
        requiredMissing
    )->getCString();
}

cocos2d::CCDictionary* GJSmartTemplate::generateRemapDict() {
    auto result = cocos2d::CCDictionary::create();
    auto binaryDict = cocos2d::CCDictionary::create();
    auto node = cocos2d::CCNode::create();
    
    int binaryValues[8];
    generateAllBinaryStrings(8, binaryValues, 0, binaryDict, node);
    
    auto keys = binaryDict->allKeys();
    
    for (unsigned int idx = 0; idx < keys->count(); ++idx) {
        auto key = static_cast<cocos2d::CCString*>(keys->objectAtIndex(idx));
        std::string binaryKey = key->getCString();
        
        if (binaryKey.length() < 8) continue;
        
        bool left = binaryKey[0] != '0';
        bool right = binaryKey[1] != '0';
        bool up = binaryKey[2] != '0';
        bool down = binaryKey[3] != '0';
        bool upRight = binaryKey[4] != '0';
        bool downRight = binaryKey[5] != '0';
        bool downLeft = binaryKey[6] != '0';
        bool upLeft = binaryKey[7] != '0';
        
        bool shouldRemap = false;
        bool cornerFlag = false;
        
        if (upRight) {
            shouldRemap = !up >= left;
            if (!up >= left) {
                upRight = false;
            }
        }
        
        bool upLeftCorner = false;
        if (downRight != 0) {
            if (!down >= left) {
                shouldRemap = true;
            }
            upLeftCorner = !down < left;
        }
        
        if (downLeft && right <= !up) {
            downLeft = false;
            if (upLeft != 0) {
                shouldRemap = true;
                downLeft = false;
                if (right <= !down) {
                    upLeft = false;
                }
            } else {
                upLeft = false;
            }
        } else if (upLeft != 0) {
            if (right <= !down) {
                upLeft = false;
            }
        }
        
        std::string prefabKey;
        std::string binaryKeyStr;
        
        keyFromNeighborsOld(left, right, up, down, upRight, upLeftCorner, downLeft, upLeft, &prefabKey, &binaryKeyStr);
        
        if (!prefabKey.empty()) {
            result->setObject(cocos2d::CCString::create(prefabKey), binaryKeyStr);
        }
    }
    
    return result;
}

bool GJSmartTemplate::shouldDiscardObject(SmartBlockType type, GJSmartDirection direction) {
    switch (type) {
        case SmartBlockType::WideSlope2:
        case SmartBlockType::WideSlope6:
            return (direction == GJSmartDirection::UpRight || 
                    direction == GJSmartDirection::DownRight ||
                    direction == GJSmartDirection::Up ||
                    direction == GJSmartDirection::Down);
        case SmartBlockType::WideSlope4:
        case SmartBlockType::WideSlope8:
            return (direction == GJSmartDirection::UpLeft ||
                    direction == GJSmartDirection::DownLeft ||
                    direction == GJSmartDirection::Up ||
                    direction == GJSmartDirection::Down);
        case SmartBlockType::WideSlope10:
        case SmartBlockType::WideSlope12:
            return (direction == GJSmartDirection::Left ||
                    direction == GJSmartDirection::Right);
        case SmartBlockType::WideSlope14:
        case SmartBlockType::WideSlope16:
            return true;
        default:
            return false;
    }
}

int GJSmartTemplate::smartTypeToObjectKey(SmartBlockType type) {
    switch (type) {
        case SmartBlockType::NormalSlope1:
        case SmartBlockType::NormalSlope2:
        case SmartBlockType::NormalSlope3:
        case SmartBlockType::NormalSlope4:
            return 2896;
        case SmartBlockType::WideSlope1:
        case SmartBlockType::WideSlope2:
        case SmartBlockType::WideSlope3:
        case SmartBlockType::WideSlope4:
        case SmartBlockType::WideSlope5:
        case SmartBlockType::WideSlope6:
        case SmartBlockType::WideSlope7:
        case SmartBlockType::WideSlope8:
        case SmartBlockType::WideSlope9:
        case SmartBlockType::WideSlope10:
        case SmartBlockType::WideSlope11:
        case SmartBlockType::WideSlope12:
        case SmartBlockType::WideSlope13:
        case SmartBlockType::WideSlope14:
        case SmartBlockType::WideSlope15:
        case SmartBlockType::WideSlope16:
            return 2897;
        default:
            return 2895;
    }
}

GJSmartTemplate::~GJSmartTemplate() {
    if (m_prefabArrays) {
        m_prefabArrays->release();
    }
    if (m_remapDict1) {
        m_remapDict1->release();
    }
    if (m_remapDict2) {
        m_remapDict2->release();
    }
    if (m_remapDict3) {
        m_remapDict3->release();
    }
    if (m_remapDict4) {
        m_remapDict4->release();
    }
    if (m_remapDict5) {
        m_remapDict5->release();
    }
    if (m_remapDict6) {
        m_remapDict6->release();
    }
}