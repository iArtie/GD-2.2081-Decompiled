// GJEffectManager.cpp

GJEffectManager* GJEffectManager::create() {
    GJEffectManager* instance = new GJEffectManager();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}


cocos2d::ccColor3B GJEffectManager::activeColorForIndex(int index) {
    ColorActionSprite* colorSprite = nullptr;
    
    if (index == 1010) {
        return cocos2d::ccc3(0, 0, 0);
    }
    
    if (index != 1011 && index != 0) {
        colorSprite = getColorSprite(index);
    }
    
    if (colorSprite) {
        return colorSprite->m_copyColor;
    }
    
    return cocos2d::ccc3(255, 255, 255);
}

float GJEffectManager::activeOpacityForIndex(int index) {
    ColorActionSprite* colorSprite = getColorSprite(index);
    if (colorSprite) {
        return colorSprite->m_opacity / 255.0f;
    }
    return 1.0f;
}

void GJEffectManager::addAllInheritedColorActions(cocos2d::CCArray* actions) {
    if (!m_unkObject148) {
        return;
    }
    
    cocos2d::CCDictElement* element = m_unkObject148->m_pElements;
    while (element) {
        cocos2d::CCArray* actionArray = static_cast<cocos2d::CCArray*>(element->m_pObject);
        if (actionArray && actionArray->count() > 0) {
            if (!actions) {
                actions = cocos2d::CCArray::create();
            }
            actions->addObject(actionArray);
        }
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJEffectManager::addCountToItem(int itemId, int value) {
    int id = 9999;
    if (itemId <= 9998) {
        if (itemId < 0) {
            id = 0;
        } else {
            id = itemId;
        }
    }
    int currentCount = m_itemCountMap[id];
    updateCountForItem(id, value + currentCount);
}

void GJEffectManager::calculateBaseActiveColors() {
    if (!m_unkDict150) {
        return;
    }
    
    cocos2d::CCDictElement* element = m_unkDict150->m_pElements;
    while (element) {
        ColorActionSprite* sprite = static_cast<ColorActionSprite*>(element->m_pObject);
        if (sprite) {
            if (sprite->m_hsv && sprite->m_hsv->m_useHSV) {
                // Transform color with HSV
                cocos2d::ccColor3B transformed = GameToolbox::transformColor(
                    sprite->m_color, 
                    *sprite->m_hsv
                );
                sprite->m_color = transformed;
                sprite->m_copyColor = transformed;
            } else {
                // Use original color
                sprite->m_color = sprite->m_originalColor;
                sprite->m_copyColor = sprite->m_originalColor;
            }
        }
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJEffectManager::calculateInheritedColor(int id, ColorAction* action) {
    if (!action) {
        return;
    }
    
    if (action->m_copyColorCalculated) {
        return;
    }
    
    int copyID = action->m_copyID;
    if (copyID == 0) {
        action->m_copyColorCalculated = true;
        return;
    }
    
    ColorActionSprite* sourceSprite = getColorSprite(copyID);
    if (!sourceSprite) {
        action->m_copyColorCalculated = true;
        return;
    }
    
    cocos2d::ccColor3B color = sourceSprite->m_copyColor;
    float opacity = sourceSprite->m_opacity;
    
    // Apply easing if duration > 0
    if (action->m_duration > 0.0f) {
        float elapsed = action->m_deltaTime + action->m_stepInterval;
        if (elapsed < action->m_duration) {
            float progress = elapsed / action->m_duration;
            if (progress < 0.0f) progress = 0.0f;
            if (progress > 1.0f) progress = 1.0f;
            
            color = GameToolbox::interpolateColor(action->m_fromColor, color, progress);
            opacity = (progress * (opacity / 255.0f) + (1.0f - progress) * action->m_fromOpacity) * 255.0f;
        }
    }
    
    // Apply HSV if present
    if (action->m_hsv.h != 0.0f || action->m_hsv.s != 0.0f || action->m_hsv.v != 0.0f) {
        cocos2d::ccHSVValue hsv = action->m_hsv;
        if (action->m_legacyHSV && action->m_duration > 0.0f) {
            float progress = (action->m_deltaTime - action->m_actionDelay) / action->m_duration;
            if (progress < 0.0f) progress = 0.0f;
            if (progress > 1.0f) progress = 1.0f;
            hsv = GameToolbox::getMultipliedHSV(&action->m_hsv, progress);
        }
        color = GameToolbox::transformColor(color, hsv);
    }
    
    // Apply pulse effects
    for (auto& pulse : m_pulseEffectVector) {
        if (pulse.m_targetGroupID == action->m_colorID && pulse.m_colorIndex == 0) {
            color = colorForPulseEffect(color, &pulse);
        }
    }
    
    // Update color sprite
    ColorActionSprite* targetSprite = action->m_colorSprite;
    targetSprite->m_copyColor = color;
    targetSprite->m_color = color;
    
    if (action->m_copyOpacity) {
        targetSprite->m_opacity = opacity * action->m_currentOpacity;
    } else {
        targetSprite->m_opacity = action->m_currentOpacity * 255.0f;
    }
    
    action->m_copyColorCalculated = true;
}

bool GJEffectManager::checkCollision(const int* blockAID, const int* blockBID) {
    int a = *blockAID;
    int b = *blockBID;
    
    if (a > 10001) {
        return false;
    }
    
    if (a < b) {
        int temp = a;
        a = b;
        b = temp;
    }
    
    int key = getCollisionKey(a, b);
    auto it = m_unkMap288.find(key);
    return it != m_unkMap288.end();
}

void GJEffectManager::colorActionChanged(ColorAction* action) {
    if (action->m_copyID <= 0) {
        m_colorActionDict->removeObjectForKey(action->m_colorID);
    } else {
        m_colorActionDict->setObject(action, action->m_colorID);
    }
}

bool GJEffectManager::colorExists(int col) {
    int id = 1101;
    if (col <= 1100) {
        if (col < 0) {
            id = 0;
        } else {
            id = col;
        }
    }
    return m_colorActionVector[id] != nullptr;
}

cocos2d::ccColor3B GJEffectManager::colorForIndex(int index) {
    ColorAction* action = getColorAction(index);
    if (action) {
        return action->m_fromColor;
    }
    return cocos2d::ccc3(255, 255, 255);
}

void GJEffectManager::controlActionsForControlID(int id, GJActionCommand command) {
    // Control GroupCommandObject2 actions
    for (auto& action : m_unkVector560) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                action.m_finished = true;
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control opacity effects
    for (auto& pair : m_opacityEffectMap) {
        if (pair.second.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                pair.second.m_finished = true;
            } else if (command == GJActionCommand::Pause) {
                pair.second.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                pair.second.m_disabled = false;
            }
        }
    }
    
    // Control count trigger actions
    for (auto& pair : m_countTriggerActions) {
        for (auto& action : pair.second) {
            if (action.m_controlID == id) {
                if (command == GJActionCommand::Stop) {
                    // Remove action
                } else if (command == GJActionCommand::Pause) {
                    action.m_disabled = true;
                } else if (command == GJActionCommand::Resume) {
                    action.m_disabled = false;
                }
            }
        }
    }
    
    // Control touch toggle actions
    for (auto& action : m_unkVector1e0) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                // Remove action
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control collision trigger actions
    for (auto& action : m_unkVector230) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                // Remove action
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control toggle trigger actions
    for (auto& action : m_unkVector248) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                // Remove action
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control spawn trigger actions
    for (auto& action : m_spawnTriggerActions) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                // Remove action
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control pulse effects
    for (auto& action : m_pulseEffectVector) {
        if (action.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                action.m_finished = true;
            } else if (command == GJActionCommand::Pause) {
                action.m_disabled = true;
            } else if (command == GJActionCommand::Resume) {
                action.m_disabled = false;
            }
        }
    }
    
    // Control timer items
    for (auto& pair : m_timerItemMap) {
        if (pair.second.m_controlID == id) {
            if (command == GJActionCommand::Stop) {
                // Remove timer
            } else if (command == GJActionCommand::Pause) {
                pair.second.m_paused = true;
            } else if (command == GJActionCommand::Resume) {
                pair.second.m_paused = false;
            }
        }
    }
}

int GJEffectManager::countForItem(int id) {
    int itemId = 9999;
    if (id <= 9998) {
        if (id < 0) {
            itemId = 0;
        } else {
            itemId = id;
        }
    }
    
    auto it = m_itemCountMap.find(itemId);
    if (it != m_itemCountMap.end()) {
        return it->second;
    }
    return 0;
}


bool GJEffectManager::init() {
    if (!cocos2d::CCNode::init()) {
        return false;
    }
    
    m_unkObject148 = cocos2d::CCDictionary::create();
    m_unkObject148->retain();
    
    m_unkDict150 = cocos2d::CCDictionary::create();
    m_unkDict150->retain();
    
    m_unkArray430 = cocos2d::CCArray::create();
    m_unkArray430->retain();
    
    m_unkDict268 = cocos2d::CCDictionary::create();
    m_unkDict268->retain();
    
    m_colorActionDict = cocos2d::CCDictionary::create();
    m_colorActionDict->retain();
    
    // Initialize color action sprite vector (1102 entries)
    m_colorActionSpriteVector.resize(1102, nullptr);
    
    // Initialize color action vector (1102 entries)
    m_colorActionVector.resize(1102, nullptr);
    
    // Initialize group toggle vector (10000 entries)
    m_unkVector438.resize(10000, true);
    
    // Initialize inheritance node vector (10000 entries)
    m_unkVector270.resize(10000, nullptr);
    
    // Initialize pulse group vector (10000 entries)
    m_unkVector2f0.resize(10000, false);
    
    return true;
}

cocos2d::CCArray* GJEffectManager::getAllColorActions() {
    cocos2d::CCArray* array = cocos2d::CCArray::create();
    
    if (m_unkObject148) {
        cocos2d::CCDictElement* element = m_unkObject148->m_pElements;
        while (element) {
            array->addObject(static_cast<cocos2d::CCObject*>(element->m_pObject));
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
    
    return array;
}

cocos2d::CCArray* GJEffectManager::getAllColorSprites() {
    cocos2d::CCArray* array = cocos2d::CCArray::create();
    
    if (m_unkDict150) {
        cocos2d::CCDictElement* element = m_unkDict150->m_pElements;
        while (element) {
            array->addObject(static_cast<cocos2d::CCObject*>(element->m_pObject));
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
    
    return array;
}

ColorAction* GJEffectManager::getColorAction(int index) {
    int id = 0;
    if (index > 1100) {
        id = 1101;
        if (m_colorActionVector[id]) {
            return m_colorActionVector[id];
        }
    } else {
        if (index < 0) {
            id = 0;
        } else {
            id = index;
        }
        if (m_colorActionVector[id]) {
            return m_colorActionVector[id];
        }
    }
    
    ColorAction* action = ColorAction::create();
    setColorAction(action, id);
    return action;
}

ColorActionSprite* GJEffectManager::getColorSprite(int index) {
    int id = 0;
    size_t idx = 0;
    
    if (index > 1100) {
        idx = 1101;
        if (m_colorActionSpriteVector[idx]) {
            return m_colorActionSpriteVector[idx];
        }
    } else {
        if (index < 0) {
            id = 0;
        } else {
            id = index;
        }
        idx = id;
        if (m_colorActionSpriteVector[idx]) {
            return m_colorActionSpriteVector[idx];
        }
    }
    
    ColorActionSprite* sprite = ColorActionSprite::create();
    sprite->m_colorID = id;
    addChild(sprite);
    m_colorActionSpriteVector[idx] = sprite;
    m_unkDict150->setObject(sprite, id);
    return sprite;
}

cocos2d::ccColor3B GJEffectManager::getMixedColor(cocos2d::ccColor3B color1, cocos2d::ccColor3B color2, float ratio) {
    float r = color2.r * (1.0f - ratio) + color1.r * ratio;
    float g = color2.g * (1.0f - ratio) + color1.g * ratio;
    float b = color2.b * (1.0f - ratio) + color1.b * ratio;
    
    if (r < 0.0f) r = 0.0f;
    if (r > 255.0f) r = 255.0f;
    if (g < 0.0f) g = 0.0f;
    if (g > 255.0f) g = 255.0f;
    if (b < 0.0f) b = 0.0f;
    if (b > 255.0f) b = 255.0f;
    
    return cocos2d::ccc3((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

CCMoveCNode* GJEffectManager::getMoveCommandNode(GroupCommandObject2* command) {
    int targetGroupID = command->m_targetGroupID;
    CCMoveCNode* node = nullptr;
    
    auto it = m_unkMap618.find(targetGroupID);
    if (it != m_unkMap618.end()) {
        node = it->second;
    }
    
    if (!node) {
        node = getMoveCommandObject();
        node->setTag(command->m_targetGroupID);
        m_unkMap618[command->m_targetGroupID] = node;
        m_unkVector6c0.push_back(node);
        node->m_eObjType = 0;
        node->m_groupObject = command;
    }
    
    node->m_groupObjects.push_back(command);
    return node;
}

CCMoveCNode* GJEffectManager::getMoveCommandObject() {
    if (!m_unkVector548.empty()) {
        CCMoveCNode* node = m_unkVector548.back();
        m_unkVector548.pop_back();
        return node;
    }
    
    CCMoveCNode* node = CCMoveCNode::create();
    node->retain();
    return node;
}

OpacityEffectAction* GJEffectManager::getOpacityActionForGroup(int id) {
    auto it = m_opacityEffectMap.find(id);
    if (it != m_opacityEffectMap.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string GJEffectManager::getPersistentStateString() {
    std::string result;
    bool first = true;
    
    // Build persistent item count string
    for (auto& pair : m_persistentItemCountMap) {
        if (pair.second > 0) {
            if (!first) {
                result += "|";
            }
            result += std::to_string(pair.first) + "|" + std::to_string(pair.second);
            first = false;
        }
    }
    
    result += "@";
    
    // Build persistent timer set string
    first = true;
    for (int id : m_persistentTimerItemSet) {
        if (!first) {
            result += "|";
        }
        result += std::to_string(id);
        first = false;
    }
    
    return result;
}

std::string GJEffectManager::getSaveString() {
    std::string result;
    
    if (m_unkObject148) {
        cocos2d::CCDictElement* element = m_unkObject148->m_pElements;
        bool first = true;
        
        while (element) {
            ColorAction* action = static_cast<ColorAction*>(element->m_pObject);
            if (action->m_colorID > 999) {
                std::string actionStr = action->getSaveString();
                if (!actionStr.empty()) {
                    if (!first) {
                        result += "|";
                    }
                    result += actionStr;
                    first = false;
                }
            }
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
    
    return result;
}

GroupCommandObject2* GJEffectManager::getTempGroupCommand() {
    if (!m_unkVector530.empty()) {
        GroupCommandObject2* cmd = m_unkVector530.back();
        m_unkVector530.pop_back();
        return cmd;
    }
    
    return new GroupCommandObject2();
}

void GJEffectManager::handleObjectCollision(bool triggerOnExit, int blockAID, int blockBID) {
    for (auto& action : m_unkVector230) {
        if (action.m_disabled) continue;
        if (action.m_triggerOnExit == triggerOnExit) continue;
        if (action.m_blockAID != blockAID) continue;
        if (action.m_blockBID != blockBID) continue;
        
        if (m_triggerEffectDelegate) {
            m_triggerEffectDelegate->toggleGroupTriggered(
                action.m_targetGroupID,
                action.m_activateGroup,
                action.m_remapKeys,
                action.m_triggerUniqueID,
                action.m_controlID
            );
        } else {
            toggleGroup(action.m_targetGroupID, action.m_activateGroup);
        }
    }
}

bool GJEffectManager::hasActiveDualTouch() {
    for (auto& action : m_unkVector1e0) {
        if (action.m_dualMode && !action.m_disabled) {
            return true;
        }
    }
    return false;
}

bool GJEffectManager::hasBeenTriggered(int objectUniqueID, int playerUniqueID) {
    auto it = m_unkMap498.find({objectUniqueID, playerUniqueID});
    return it != m_unkMap498.end();
}

bool GJEffectManager::hasPulseEffectForGroupID(int id) {
    auto it = m_pulseEffectMap.find(id);
    if (it != m_pulseEffectMap.end()) {
        return !it->second.empty();
    }
    return false;
}

bool GJEffectManager::isGroupEnabled(int id) {
    if (id > 9998) {
        return (m_unkVector438[9999] == true);
    }
    
    if (id < 0) {
        id = 0;
    }
    
    return m_unkVector438[id];
}

void GJEffectManager::itemWasUsed(int id) {
    // Empty implementation
}

void GJEffectManager::loadFromState(EffectManagerState* state) {
    reset();
    
    // Load unordered set
    m_unkMap4c8 = state->m_unorderedSet_int2;
    
    // Load color action states
    for (auto& caState : state->m_unkVecCAState) {
        ColorAction* action = ColorAction::create();
        action->loadFromState(&caState);
        setColorAction(action, action->m_colorID);
        colorActionChanged(action);
    }
    
    // Load pulse effect vector
    m_pulseEffectVector = state->m_unkVecPulseEffectAction;
    
    // Load pulse effect map
    m_pulseEffectMap = state->m_unorderedMapInt_vectorPulseEffectAction;
    
    // Update pulse group vector
    for (auto& pair : m_pulseEffectMap) {
        if (!pair.second.empty()) {
            m_unkVector2f0[pair.first] = true;
        }
    }
    
    // Load opacity effect map
    m_opacityEffectMap = state->m_unorderedMapInt_OpacityEffectAction;
    
    // Load unordered set 1
    m_unkMap460 = state->m_unorderedSet_int1;
    
    // Update group toggle vector
    for (int id : m_unkMap460) {
        m_unkVector438[id] = false;
    }
    
    // Load map
    m_unkMap498 = state->m_mapInt_Int;
    
    // Load spawn trigger actions
    m_spawnTriggerActions = state->m_vectorSpawnTriggerAction;
    
    // Load group command objects
    m_unkVector560 = state->m_vectorGroupCommandObject2;
    
    // Load move position map
    m_unkMap578 = state->m_unorderedMapInt_pair_double_double;
    
    // Load touch toggle actions
    m_unkVector1e0 = state->m_vectorTouchToggleAction;
    
    // Load count trigger actions
    m_countTriggerActions = state->m_unorderedMapInt_vectorCountTriggerAction;
    
    // Load toggle trigger actions
    m_unkVector248 = state->m_vectorToggleTriggerAction;
    
    // Load collision trigger actions
    m_unkVector230 = state->m_vectorCollisionTriggerAction;
    
    // Load collision map
    m_unkMap288 = state->m_unorderedMapInt_bool;
    
    // Load item count map
    m_itemCountMap = state->m_itemCountMap;
    
    // Transfer persistent items
    transferPersistentItems();
    
    // Remove non-persistent timers
    for (auto it = m_timerItemMap.begin(); it != m_timerItemMap.end();) {
        if (m_persistentTimerItemSet.find(it->first) == m_persistentTimerItemSet.end()) {
            it = m_timerItemMap.erase(it);
        } else {
            ++it;
        }
    }
    
    // Load timer item map from state
    for (auto& pair : state->m_timerItemMap) {
        if (m_persistentTimerItemSet.find(pair.first) != m_persistentTimerItemSet.end()) {
            m_timerItemMap[pair.first] = pair.second;
        }
    }
    
    // Load timer trigger actions
    m_unkMap3f8 = state->m_unorderedMapInt_vectorTimerTriggerAction;
    
    // Update effects
    updateEffects(0.0f);
    calculateBaseActiveColors();
    processInheritedColors();
}

void GJEffectManager::loadPersistentStateString(std::string state) {
    // Clear persistent item count map
    m_persistentItemCountMap.clear();
    
    // Clear persistent timer set
    m_persistentTimerItemSet.clear();
    
    if (state.empty()) {
        return;
    }
    
    // Split state string by '@'
    size_t atPos = state.find('@');
    if (atPos == std::string::npos) {
        return;
    }
    
    std::string itemPart = state.substr(0, atPos);
    std::string timerPart = state.substr(atPos + 1);
    
    // Parse item count part (format: id|count|id|count...)
    if (!itemPart.empty()) {
        size_t pos = 0;
        while (pos < itemPart.length()) {
            size_t sep1 = itemPart.find('|', pos);
            if (sep1 == std::string::npos) break;
            
            int id = std::stoi(itemPart.substr(pos, sep1 - pos));
            pos = sep1 + 1;
            
            size_t sep2 = itemPart.find('|', pos);
            if (sep2 == std::string::npos) {
                int count = std::stoi(itemPart.substr(pos));
                m_persistentItemCountMap[id] = count;
                if (count > 0) {
                    m_itemCountMap[id] = count;
                }
                break;
            }
            
            int count = std::stoi(itemPart.substr(pos, sep2 - pos));
            m_persistentItemCountMap[id] = count;
            if (count > 0) {
                m_itemCountMap[id] = count;
            }
            pos = sep2 + 1;
        }
    }
    
    // Parse timer part (format: id|id|id...)
    if (!timerPart.empty()) {
        size_t pos = 0;
        while (pos < timerPart.length()) {
            size_t sep = timerPart.find('|', pos);
            if (sep == std::string::npos) {
                int id = std::stoi(timerPart.substr(pos));
                m_persistentTimerItemSet.insert(id);
                break;
            }
            int id = std::stoi(timerPart.substr(pos, sep - pos));
            m_persistentTimerItemSet.insert(id);
            pos = sep + 1;
        }
    }
}

void GJEffectManager::objectsCollided(int blockAID, int blockBID) {
    if (blockAID > 10001 || blockBID > 10001) {
        return;
    }
    
    int a = blockAID;
    int b = blockBID;
    
    if (blockAID < blockBID) {
        a = blockBID;
        b = blockAID;
    }
    
    int key = getCollisionKey(a, b);
    
    auto it = m_unkMap288.find(key);
    if (it != m_unkMap288.end()) {
        it->second = true;
    } else {
        handleObjectCollision(true, b, a);
        m_unkMap288[key] = true;
    }
}

float GJEffectManager::opacityForIndex(int index) {
    ColorAction* action = getColorAction(index);
    if (action) {
        return action->m_fromOpacity;
    }
    return 1.0f;
}

float GJEffectManager::opacityModForGroup(int id) {
    if (id > 9998) {
        id = 9999;
    } else if (id < 0) {
        id = 0;
    }
    
    auto it = m_opacityEffectMap.find(id);
    if (it != m_opacityEffectMap.end()) {
        return it->second.m_currentValue;
    }
    return 1.0f;
}

void GJEffectManager::pauseTimer(int id) {
    auto it = m_timerItemMap.find(id);
    if (it != m_timerItemMap.end()) {
        it->second.m_paused = true;
    }
}

void GJEffectManager::playerButton(bool down, bool player1) {
    for (auto& action : m_unkVector1e0) {
        if (action.m_disabled) continue;
        if (action.m_dualMode && !player1) continue;
        
        if (action.m_touchTriggerControl == TouchTriggerControl::Player1 && !player1) continue;
        if (action.m_touchTriggerControl == TouchTriggerControl::Player2 && player1) continue;
        
        if (!down && !action.m_holdMode) continue;
        
        bool activate = !isGroupEnabled(action.m_targetGroupID);
        
        if (action.m_holdMode) {
            if (action.m_touchTriggerType == TouchTriggerType::Toggle) {
                // Toggle behavior
            } else if (action.m_touchTriggerType == TouchTriggerType::ToggleOn) {
                activate = down;
            } else {
                activate = !down;
            }
        } else {
            if (action.m_touchTriggerType == TouchTriggerType::Toggle) {
                activate = !activate;
            } else if (action.m_touchTriggerType == TouchTriggerType::ToggleOn) {
                // On only
            }
        }
        
        if (m_triggerEffectDelegate) {
            m_triggerEffectDelegate->toggleGroupTriggered(
                action.m_targetGroupID,
                activate,
                action.m_remapKeys,
                action.m_triggerUniqueID,
                action.m_controlID
            );
        } else {
            toggleGroup(action.m_targetGroupID, activate);
        }
    }
}

void GJEffectManager::playerDied() {
    if (!m_triggerEffectDelegate) {
        return;
    }
    
    for (auto& action : m_unkVector248) {
        if (action.m_disabled) continue;
        
        m_triggerEffectDelegate->toggleGroupTriggered(
            action.m_targetGroupID,
            action.m_activateGroup,
            action.m_remapKeys,
            action.m_triggerUniqueID,
            action.m_controlID
        );
    }
}

void GJEffectManager::postCollisionCheck() {
    auto it = m_unkMap288.begin();
    while (it != m_unkMap288.end()) {
        if (!it->second) {
            int key = it->first;
            int blockAID, blockBID;
            splitCollisionKey(key, &blockAID, &blockBID);
            
            handleObjectCollision(false, blockAID, blockBID);
            it = m_unkMap288.erase(it);
        } else {
            ++it;
        }
    }
}

void GJEffectManager::postMoveActions() {
    for (auto it = m_unkVector560.begin(); it != m_unkVector560.end();) {
        if (it->m_finished) {
            if ((it->m_commandType & 0xFFFFFFFD) == 0) {
                double dx = it->m_deltaX;
                double dy = it->m_deltaY;
                
                if (it->m_lockToPlayerX) {
                    dx = it->m_lockedCurrentXOffset;
                }
                if (it->m_lockToPlayerY) {
                    dy = it->m_lockedCurrentYOffset;
                }
                
                m_unkMap578[it->m_targetGroupID].first += dx;
                m_unkMap578[it->m_targetGroupID].second += dy;
            }
            it = m_unkVector560.erase(it);
        } else {
            ++it;
        }
    }
}

void GJEffectManager::preCollisionCheck() {
    for (auto& pair : m_unkMap288) {
        pair.second = false;
    }
}

void GJEffectManager::prepareMoveActions(float dt, bool intermediate) {
    // Clear rotation command maps
    if (m_unkMap5c8._M_h._M_before_begin._M_nxt) {
        std::__detail::_Hash_node_base* node = m_unkMap5c8._M_h._M_before_begin._M_nxt;
        do {
            std::__detail::_Hash_node_base* next = node->_M_nxt;
            if (node[2]._M_nxt) {
                operator delete(node[2]._M_nxt);
            }
            operator delete(node);
            node = next;
        } while (node);
    }
    
    memset(m_unkMap5c8._M_h._M_buckets, 0, 8 * m_unkMap5c8._M_h._M_bucket_count);
    m_unkMap5c8._M_h._M_element_count = 0;
    m_unkMap5c8._M_h._M_before_begin._M_nxt = nullptr;
    m_unkVector600.m_finish = m_unkVector600.m_start;
    
    resetTempGroupCommands(false);
    
    // Reset move command nodes
    if (m_unkMap618._M_h._M_before_begin._M_nxt) {
        std::__detail::_Hash_node_base* node = m_unkMap618._M_h._M_before_begin._M_nxt;
        do {
            CCMoveCNode::reset(static_cast<CCMoveCNode*>(node[2]._M_nxt));
            if (m_unkVector548.m_finish == m_unkVector548.m_reserveEnd) {
                std::vector<CCMoveCNode*>::_M_emplace_back_aux<CCMoveCNode* const&>(
                    reinterpret_cast<__int64>(&m_unkVector548),
                    &node[2]._M_nxt
                );
            } else {
                if (m_unkVector548.m_finish) {
                    *m_unkVector548.m_finish = static_cast<cocos2d::CCObject*>(node[2]._M_nxt);
                    m_unkVector548.m_finish = m_unkVector548.m_finish;
                }
                m_unkVector548.m_finish = m_unkVector548.m_finish + 1;
            }
            node = node->_M_nxt;
        } while (node);
        
        std::__detail::_Hash_node_base* node2 = m_unkMap618._M_h._M_before_begin._M_nxt;
        if (node2) {
            do {
                std::__detail::_Hash_node_base* next = node2->_M_nxt;
                operator delete(node2);
                node2 = next;
            } while (node2);
        }
    }
    
    memset(m_unkMap618._M_h._M_buckets, 0, 8 * m_unkMap618._M_h._M_bucket_count);
    m_unkMap618._M_h._M_before_begin._M_nxt = nullptr;
    m_unkMap618._M_h._M_element_count = 0;
    
    // Reset scale command nodes
    if (m_unkMap650._M_h._M_before_begin._M_nxt) {
        std::__detail::_Hash_node_base* node = m_unkMap650._M_h._M_before_begin._M_nxt;
        do {
            CCMoveCNode::reset(static_cast<CCMoveCNode*>(node[2]._M_nxt));
            if (m_unkVector548.m_finish == m_unkVector548.m_reserveEnd) {
                std::vector<CCMoveCNode*>::_M_emplace_back_aux<CCMoveCNode* const&>(
                    reinterpret_cast<__int64>(&m_unkVector548),
                    &node[2]._M_nxt
                );
            } else {
                if (m_unkVector548.m_finish) {
                    *m_unkVector548.m_finish = static_cast<cocos2d::CCObject*>(node[2]._M_nxt);
                    m_unkVector548.m_finish = m_unkVector548.m_finish;
                }
                m_unkVector548.m_finish = m_unkVector548.m_finish + 1;
            }
            node = node->_M_nxt;
        } while (node);
        
        std::__detail::_Hash_node_base* node2 = m_unkMap650._M_h._M_before_begin._M_nxt;
        if (node2) {
            do {
                std::__detail::_Hash_node_base* next = node2->_M_nxt;
                operator delete(node2);
                node2 = next;
            } while (node2);
        }
    }
    
    memset(m_unkMap650._M_h._M_buckets, 0, 8 * m_unkMap650._M_h._M_bucket_count);
    m_unkMap650._M_h._M_before_begin._M_nxt = nullptr;
    m_unkMap650._M_h._M_element_count = 0;
    
    // Reset follow command nodes
    if (m_unkMap688._M_h._M_before_begin._M_nxt) {
        std::__detail::_Hash_node_base* node = m_unkMap688._M_h._M_before_begin._M_nxt;
        do {
            CCMoveCNode::reset(static_cast<CCMoveCNode*>(node[2]._M_nxt));
            if (m_unkVector548.m_finish == m_unkVector548.m_reserveEnd) {
                std::vector<CCMoveCNode*>::_M_emplace_back_aux<CCMoveCNode* const&>(
                    reinterpret_cast<__int64>(&m_unkVector548),
                    &node[2]._M_nxt
                );
            } else {
                if (m_unkVector548.m_finish) {
                    *m_unkVector548.m_finish = static_cast<cocos2d::CCObject*>(node[2]._M_nxt);
                    m_unkVector548.m_finish = m_unkVector548.m_finish;
                }
                m_unkVector548.m_finish = m_unkVector548.m_finish + 1;
            }
            node = node->_M_nxt;
        } while (node);
        
        std::__detail::_Hash_node_base* node2 = m_unkMap688._M_h._M_before_begin._M_nxt;
        if (node2) {
            do {
                std::__detail::_Hash_node_base* next = node2->_M_nxt;
                operator delete(node2);
                node2 = next;
            } while (node2);
        }
    }
    
    memset(m_unkMap688._M_h._M_buckets, 0, 8 * m_unkMap688._M_h._M_bucket_count);
    m_unkMap688._M_h._M_before_begin._M_nxt = nullptr;
    m_unkMap688._M_h._M_element_count = 0;
    
    // Clear command vectors
    m_unkVector6c0.m_finish = m_unkVector6c0.m_start;
    m_unkVector6d8.m_finish = m_unkVector6d8.m_start;
    m_unkVector6f0.m_finish = m_unkVector6f0.m_start;
    m_unkVector5b0.m_finish = m_unkVector5b0.m_start;
    
    // Clear rotation map
    if (m_unkMap770.m_header.m_parent) {
        std::_rb_tree_base* parent = m_unkMap770.m_header.m_parent;
        do {
            std::_Rb_tree<std::pair<int,int>,std::pair<std::pair<int,int> const,std::vector<GroupCommandObject2*>>,std::_Select1st<std::pair<std::pair<int,int> const,std::vector<GroupCommandObject2*>>>,std::less<std::pair<int,int>>,std::allocator<std::pair<std::pair<int,int> const,std::vector<GroupCommandObject2*>>>>::_M_erase(
                reinterpret_cast<__int64>(&m_unkMap770),
                &parent->m_right->m_isblack
            );
            std::_rb_tree_base* left = parent->m_left;
            if (parent[1].m_parent) {
                operator delete(parent[1].m_parent);
            }
            operator delete(parent);
            parent = left;
        } while (parent);
    }
    
    m_unkMap770.m_header.m_left = &m_unkMap770.m_header;
    m_unkMap770.m_header.m_parent = nullptr;
    m_unkMap770.m_header.m_right = &m_unkMap770.m_header;
    m_unkMap770.m_nodecount = 0;
    
    // Process each move command
    GroupCommandObject2* start = m_unkVector560.m_start;
    GroupCommandObject2* finish = m_unkVector560.m_finish;
    
    if (0xFC0FC0FC0FC0FC1LL * (((char*)finish - (char*)start) >> 3) == 0) {
        return;
    }
    
    size_t index = 0;
    size_t count = 0;
    
    while (count < 0xFC0FC0FC0FC0FC1LL * (((char*)finish - (char*)start) >> 3)) {
        CCMoveCNode* moveNode = nullptr;
        GroupCommandObject2* command = &start[index];
        
        if (command->m_disabled) {
            ++count;
            ++index;
            continue;
        }
        
        switch (command->m_commandType) {
            case 0: { // Move command
                moveNode = getMoveCommandNode(command);
                
                if (command->m_finishRelated) {
                    double deltaX3 = command->m_deltaX_3;
                    double deltaY3 = command->m_deltaY_3;
                    moveNode->m_unk078 = 1;
                    moveNode->m_unk090 += deltaX3;
                    moveNode->m_unk098 += deltaY3;
                } else {
                    command->step(dt);
                    
                    double deltaX = command->m_deltaX;
                    double deltaY = command->m_deltaY;
                    double deltaX3 = command->m_deltaX_3;
                    double deltaY3 = command->m_deltaY_3;
                    
                    if (!intermediate) {
                        if (command->m_lockToPlayerX) {
                            deltaX = m_unk780 * command->m_moveModX;
                            deltaX3 += deltaX;
                        }
                        if (command->m_lockToCameraX) {
                            deltaX = m_unk788 * command->m_moveModX;
                            deltaX3 += deltaX;
                        }
                        if (command->m_lockToPlayerY) {
                            deltaY = m_unk784 * command->m_moveModY;
                            deltaY3 += deltaY;
                        }
                        if (command->m_lockToCameraY) {
                            deltaY = m_unk78C * command->m_moveModY;
                            deltaY3 += deltaY;
                        }
                        
                        moveNode->m_unk038 += deltaX;
                        moveNode->m_unk040 += deltaY;
                        moveNode->m_unk090 += deltaX3;
                        moveNode->m_unk098 += deltaY3;
                        
                        command->m_lockedCurrentXOffset += deltaX;
                        command->m_lockedCurrentYOffset += deltaY;
                    } else {
                        if (command->m_lockToPlayerX) {
                            deltaX = m_unk780 * command->m_moveModX;
                            command->m_deltaX_3 += deltaX;
                        }
                        if (command->m_lockToCameraX) {
                            deltaX = m_unk788 * command->m_moveModX;
                            command->m_deltaX_3 += deltaX;
                        }
                        if (command->m_lockToPlayerY) {
                            deltaY = m_unk784 * command->m_moveModY;
                            command->m_deltaY_3 += deltaY;
                        }
                        if (command->m_lockToCameraY) {
                            deltaY = m_unk78C * command->m_moveModY;
                            command->m_deltaY_3 += deltaY;
                        }
                        
                        deltaX3 = 0.0;
                        deltaY3 = 0.0;
                        
                        moveNode->m_unk038 += deltaX;
                        moveNode->m_unk040 += deltaY;
                        moveNode->m_unk090 += deltaX3;
                        moveNode->m_unk098 += deltaY3;
                        
                        command->m_lockedCurrentXOffset += deltaX;
                        command->m_lockedCurrentYOffset += deltaY;
                    }
                }
                break;
            }
            case 1: { // Rotate command
                if (m_unk798) {
                    std::vector<GroupCommandObject2*>& vec = m_unkMap5c8[command->m_targetGroupID];
                }
                command->m_someInterpValue1RelatedFalse = 0;
                
                std::vector<GroupCommandObject2*>& vec = m_unkMap5c8[command->m_targetGroupID];
                vec.push_back(command);
                
                registerRotationCommand(command, m_unk798);
                command->stepTransformCommand(dt, intermediate, 0);
                break;
            }
            case 2: { // Scale command
                int centerGroupID = command->m_centerGroupID;
                int key = centerGroupID + 1000 * command->m_targetGroupID;
                
                auto it = m_unkMap650.find(key);
                if (it == m_unkMap650.end() || !it->second) {
                    CCMoveCNode* node = getMoveCommandObject();
                    node->setTag(command->m_targetGroupID);
                    m_unkMap650[key] = node;
                    m_unkVector6d8.push_back(node);
                    node->m_eObjType = 0;
                    node->m_groupObject = command;
                    moveNode = node;
                } else {
                    moveNode = it->second;
                }
                
                moveNode->m_unk080 = command->m_followXMod;
                moveNode->m_unk074 = centerGroupID;
                moveNode->m_unk088 = command->m_followYMod;
                
                if (command->m_finishRelated) {
                    moveNode->m_unk078 = 1;
                } else {
                    command->step(dt);
                }
                break;
            }
            case 3: { // Follow command
                auto it = m_unkMap688.find(command->m_targetGroupID);
                if (it == m_unkMap688.end() || !it->second) {
                    CCMoveCNode* node = getMoveCommandObject();
                    node->setTag(command->m_targetGroupID);
                    m_unkMap688[command->m_targetGroupID] = node;
                    m_unkVector6f0.push_back(node);
                    node->m_eObjType = 0;
                    moveNode = node;
                } else {
                    moveNode = it->second;
                }
                
                moveNode->m_unk0a8 = command->m_followYSpeed;
                moveNode->m_unk0b0 = command->m_followYDelay;
                moveNode->m_unk0b8 = command->m_followYOffset;
                moveNode->m_unk074 = command->m_centerGroupID;
                moveNode->m_unk0c0 = command->m_followYMaxSpeed;
                
                if (command->m_finishRelated) {
                    moveNode->m_unk078 = 1;
                } else {
                    command->step(dt);
                }
                break;
            }
            case 4: { // Transform command
                m_unkVector600.push_back(command);
                command->stepTransformCommand(dt, intermediate, 0);
                command->resetDelta(false);
                
                if (!command->m_finishRelated) {
                    if (command->m_finished) {
                        command->m_finishRelated = 1;
                    }
                } else {
                    command->m_doUpdate = 1;
                }
                break;
            }
            case 5: { // Keyframe animation command
    bool hasGameObject = false;
    float* gameObject = static_cast<float*>(command->m_gameObject);
    if (gameObject) {
        hasGameObject = true;
        if (gameObject[169] == 0.0f && gameObject[173] == 0.0f) {
            hasGameObject = gameObject[174] != 0.0f;
        }
    }
    
    command->m_deltaTime += dt;
    
    cocos2d::CCPoint point = cocos2d::CCPoint(0.0f, 0.0f);
    KeyframeObject* keyframes = command->m_keyframes.m_start;
    double deltaTime = command->m_deltaTime;
    int keyframeCount = -1227133513 * (((char*)command->m_keyframes.m_finish - (char*)keyframes) >> 6) - 1;
    
    double scaleX = 1.0;
    double scaleY = 1.0;
    double rotation = 0.0;
    double unknown = 1.0;
    float* pValues = nullptr;
    char* pData = nullptr;
    char* pDataStart = nullptr;
    double m_unk1a8 = 0.0;
    bool hasPosition = false;
    
    if (keyframeCount > 0) {
        double accumulatedTime = 0.0;
        int keyframeIndex = 0;
        int iteration = 0;
        bool foundKeyframe = false;
        
        while (!foundKeyframe && iteration < keyframeCount) {
            KeyframeObject* currentKeyframe = &keyframes[keyframeIndex];
            double keyframeDuration = currentKeyframe->m_unk178;
            
            if (deltaTime >= keyframeDuration && keyframeDuration > 0.0) {
                double keyframeStart = currentKeyframe->m_unk000;
                if (deltaTime >= keyframeStart) {
                    if (iteration != keyframeCount - 1) {
                        ++iteration;
                        ++keyframeIndex;
                        deltaTime -= currentKeyframe->m_unk000;
                        accumulatedTime += currentKeyframe->m_unk000;
                        continue;
                    }
                    foundKeyframe = true;
                } else {
                    foundKeyframe = true;
                }
            } else if (keyframeDuration > 0.0) {
                float progress = deltaTime / keyframeDuration;
                float easedValue = GameToolbox::getEasedValue(progress, currentKeyframe->m_unk008, currentKeyframe->m_unk00c);
                keyframes = command->m_keyframes.m_start;
                deltaTime = easedValue * keyframeDuration;
                double keyframeStart = currentKeyframe->m_unk000;
                if (deltaTime >= keyframeStart) {
                    if (iteration != keyframeCount - 1) {
                        ++iteration;
                        ++keyframeIndex;
                        deltaTime -= currentKeyframe->m_unk000;
                        accumulatedTime += currentKeyframe->m_unk000;
                        continue;
                    }
                    foundKeyframe = true;
                } else {
                    foundKeyframe = true;
                }
            } else {
                foundKeyframe = true;
            }
        }
        
        accumulatedTime += deltaTime;
        
        int keyframeIndex2 = 0;
        int iteration2 = 0;
        double progress = 0.0;
        bool processedAll = false;
        
        while (!processedAll && iteration2 < keyframeCount) {
            KeyframeObject* currentKeyframe = &keyframes[keyframeIndex2];
            KeyframeObject* nextKeyframe = (keyframeIndex2 + 1 < keyframeCount) ? &keyframes[keyframeIndex2 + 1] : nullptr;
            
            if (iteration2 == 0) {
                m_unk1a8 = currentKeyframe->m_unk1a8;
            }
            
            double keyframeEnd = currentKeyframe->m_unk170;
            int keyframeEasing = 0;
            
            if (accumulatedTime < keyframeEnd && keyframeEnd > 0.0) {
                double splineTime = accumulatedTime;
                if (!(currentKeyframe->m_unk000 <= deltaTime || currentKeyframe->m_unk008 <= 0 || currentKeyframe->m_unk010 || currentKeyframe->m_unk178 > 0.0)) {
                    float splineProgress = deltaTime / currentKeyframe->m_unk000;
                    double easedValue = GameToolbox::getEasedValue(splineProgress, currentKeyframe->m_unk008, currentKeyframe->m_unk00c);
                    splineTime = accumulatedTime - deltaTime + currentKeyframe->m_unk000 * easedValue;
                    keyframeEasing = (easedValue > 0.95 && currentKeyframe->m_unk01c) ? 1 : 0;
                }
                
                double splineX = tk::spline::operator()(&currentKeyframe->m_spline1, splineTime);
                double splineY = tk::spline::operator()(&currentKeyframe->m_spline2, splineTime);
                point.x += splineX;
                point.y += splineY;
                hasPosition = true;
            }
            
            if (iteration2 == keyframeCount - 1 && command->m_deltaTime < command->m_duration) {
                // Last keyframe, continue processing
            }
            
            if (currentKeyframe->m_unk000 > 0.0) {
                progress = accumulatedTime / currentKeyframe->m_unk000;
            } else {
                progress = 0.0;
            }
            
            if (progress > 1.0) {
                progress = 1.0;
            }
            
            if (currentKeyframe->m_unk008 > 0 && !currentKeyframe->m_unk010 && currentKeyframe->m_unk178 <= 0.0) {
                progress = GameToolbox::getEasedValue(progress, currentKeyframe->m_unk008, currentKeyframe->m_unk00c);
            }
            
            if (progress > 0.95 && currentKeyframe->m_unk01c) {
                // Trigger on progress
            }
            
            if (!currentKeyframe->m_unk01d) {
                int triggerValue = currentKeyframe->m_unk014;
                currentKeyframe->m_unk01d = 1;
                if (triggerValue > 0) {
                    // Store trigger data for later processing
                }
            }
            
            if (!hasPosition && nextKeyframe) {
                point.x += nextKeyframe->m_unk188.x * progress;
                point.y += nextKeyframe->m_unk188.y * progress;
            }
            
            if (nextKeyframe) {
                unknown += nextKeyframe->m_unk198 * progress;
                scaleX += nextKeyframe->m_unk1a0 * progress;
                rotation += nextKeyframe->m_unk1a8 * progress;
            }
            
            if (currentKeyframe->m_unk000 <= accumulatedTime) {
                ++iteration2;
                if (iteration2 < keyframeCount) {
                    accumulatedTime -= currentKeyframe->m_unk000;
                    keyframeIndex2 = iteration2;
                    keyframes = command->m_keyframes.m_start;
                    continue;
                }
            }
            processedAll = true;
        }
    }
    
    // Process keyframe result
    point.x -= command->m_splineRelated.x;
    point.y -= command->m_splineRelated.y;
    
    bool shouldCreateRotationCommand = false;
    bool shouldCreateTransformCommand = false;
    int transformType = 0;
    
    if (point.x == 0.0f && point.y == 0.0f && !command->m_finishRelated) {
        if (rotation - command->m_keyframeRelated != 0.0) {
            shouldCreateRotationCommand = true;
        }
    } else {
        CCMoveCNode* moveNode = getMoveCommandNode(command);
        if (command->m_finishRelated) {
            moveNode->m_unk078 = 1;
        }
        if (hasGameObject) {
            // Add move calculation
        }
        moveNode->m_unk038 += point.x;
        moveNode->m_unk040 += point.y;
        moveNode->m_unk090 += point.x;
        moveNode->m_unk098 += point.y;
        
        if (rotation - command->m_keyframeRelated != 0.0 || command->m_finishRelated) {
            shouldCreateRotationCommand = true;
        }
    }
    
    if (shouldCreateRotationCommand) {
        GroupCommandObject2* tempCommand = getTempGroupCommand();
        tempCommand->m_finishRelated = command->m_finishRelated;
        tempCommand->m_targetGroupID = command->m_targetGroupID;
        tempCommand->m_centerGroupID = command->m_centerGroupID;
        tempCommand->m_someInterpValue1RelatedFalse = 0;
        tempCommand->m_someInterpValue1RelatedOne = rotation - command->m_keyframeRelated;
        tempCommand->m_someInterpValue2RelatedOne = rotation - command->m_keyframeRelated;
        
        if (m_unk798) {
            m_unkMap5c8[command->m_targetGroupID].push_back(tempCommand);
        }
        m_unkMap5c8[command->m_targetGroupID].push_back(tempCommand);
        registerRotationCommand(tempCommand, m_unk798);
        
        if (command->m_finishRelated) {
            shouldCreateTransformCommand = true;
            transformType = 1;
        }
    }
    
    // Check if scale values are valid
    bool invalidScale = false;
    if (fabs(unknown) < 0.01 || fabs(scaleX) < 0.01) {
        invalidScale = true;
    }
    
    if (!invalidScale) {
        if (command->m_someInterpValue1 != unknown || command->m_someInterpValue2 != scaleX) {
            shouldCreateTransformCommand = true;
            if (transformType == 0) {
                transformType = 2;
            }
        }
    }
    
    if (shouldCreateTransformCommand && command->m_finishRelated) {
        GroupCommandObject2* transformCommand = getTempGroupCommand();
        transformCommand->m_finishRelated = command->m_finishRelated;
        m_unkVector600.push_back(transformCommand);
        transformCommand->m_targetGroupID = command->m_targetGroupID;
        transformCommand->m_centerGroupID = command->m_centerGroupID;
        transformCommand->m_targetScaleX = unknown;
        transformCommand->m_relativeRotation = 1;
        transformCommand->m_targetScaleY = scaleX;
        transformCommand->m_someInterpValue1RelatedFalse = 0;
        transformCommand->m_someInterpValue1RelatedZero = 0.0;
        transformCommand->m_someInterpValue1RelatedOne = 1.0;
        transformCommand->m_someInterpValue1Related = command->m_someInterpValue1;
        transformCommand->m_someInterpValue2RelatedTrue = 1;
        transformCommand->m_someInterpValue2RelatedZero = 0.0;
        transformCommand->m_someInterpValue2RelatedOne = 1.0;
        transformCommand->m_someInterpValue2Related = command->m_someInterpValue2;
        transformCommand->m_keyframeRelated = m_unk1a8;
    }
    
    if (!invalidScale) {
        command->m_someInterpValue1 = unknown;
        command->m_someInterpValue2 = scaleX;
    }
    
    command->m_splineRelated = point;
    command->m_keyframeRelated = rotation;
    
    if (command->m_deltaTime >= command->m_duration) {
        command->m_finished = 1;
    }
    
        // Process spawn triggers from keyframes
    if (pValues != pData) {
        std::vector<int> remapKeys(command->m_remapKeys.m_start, command->m_remapKeys.m_finish);
        
        float* currentValues = pData;
        while (currentValues != pValues) {
            float delay = currentValues[1];
            int targetGroupID = static_cast<int>(currentValues[2]);
            int uniqueID = static_cast<int>(currentValues[0]);
            
            if (delay > 0.0f) {
                // Spawn with delay
                if (m_triggerEffectDelegate) {
                    m_triggerEffectDelegate->spawnGroup(
                        targetGroupID,
                        0,
                        &remapKeys,
                        uniqueID,
                        command->m_controlID,
                        0.0
                    );
                } else {
                    spawnGroup(targetGroupID, delay, 0, &remapKeys, uniqueID, command->m_controlID);
                }
            } else {
                // Immediate spawn
                if (!m_triggerEffectDelegate) {
                    spawnGroup(targetGroupID, 0.0f, 0, &remapKeys, uniqueID, command->m_controlID);
                }
            }
            currentValues += 3;
        }
    }
    
    // Clean up spawn trigger data
    if (pData) {
        operator delete(pData);
    }
    break;
}
            default:
                break;
        }
        
        command->resetDelta(false);
        
        if (command->m_finishRelated) {
            finish = m_unkVector560.m_finish;
            start = m_unkVector560.m_start;
            command->m_doUpdate = 1;
        } else {
            start = m_unkVector560.m_start;
            finish = m_unkVector560.m_finish;
            if (command->m_finished) {
                command->m_finishRelated = 1;
            }
        }
        
        ++count;
        ++index;
    }
}

void GJEffectManager::processColors() {
    calculateBaseActiveColors();
    processPulseActions();
    processInheritedColors();
    processCopyColorPulseActions();
}

void GJEffectManager::processCopyColorPulseActions() {
    if (0x8E38E38E38E38E39LL * (((char*)m_pulseEffectVector.m_finish - (char*)m_pulseEffectVector.m_start) >> 3) == 0) {
        return;
    }
    
    std::vector<ColorActionSprite*> spritesToUpdate;
    
    size_t index = 0;
    size_t count = 0;
    size_t total = 0x8E38E38E38E38E39LL * (((char*)m_pulseEffectVector.m_finish - (char*)m_pulseEffectVector.m_start) >> 3);
    
    while (count < total) {
        PulseEffectAction* pulseAction = &m_pulseEffectVector.m_start[index];
        
        if (pulseAction->m_colorIndex) {
            ColorActionSprite* colorSprite = getColorSprite(pulseAction->m_targetGroupID);
            cocos2d::ccColor3B activeColor = activeColorForIndex(pulseAction->m_targetGroupID);
            cocos2d::ccColor3B pulseColor = colorForPulseEffect(&activeColor, pulseAction);
            
            colorSprite->m_pulseColor = pulseColor;
            spritesToUpdate.push_back(colorSprite);
        }
        
        ++count;
        ++index;
    }
    
    // Apply pulse colors to sprites
    for (ColorActionSprite* sprite : spritesToUpdate) {
        sprite->m_copyColor = sprite->m_pulseColor;
    }
}

void GJEffectManager::processInheritedColors() {
    if (m_colorActionDict->count() == 0) {
        return;
    }
    
    if (!m_unkDict268) {
        return;
    }
    
    // Mark all inheritance nodes as unprocessed
    cocos2d::CCDictElement* element = m_unkDict268->m_pElements;
    while (element) {
        InheritanceNode* node = static_cast<InheritanceNode*>(element->m_pObject);
        node->m_processed = false;
        node->m_hasInheritance = true;
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
    
    // Process each color action
    cocos2d::CCDictElement* colorElement = m_colorActionDict->m_pElements;
    while (colorElement) {
        ColorAction* action = static_cast<ColorAction*>(colorElement->m_pObject);
        
        if (action->m_copyID <= 9998) {
            int colorID = action->m_colorID;
            int copyID = (action->m_copyID < 0) ? 0 : action->m_copyID;
            
            InheritanceNode* node = m_unkVector270[copyID];
            if (!node) {
                node = InheritanceNode::create(copyID, 0);
                m_unkDict268->setObject(node, copyID);
                m_unkVector270[copyID] = node;
            }
            
            action->m_inheritanceNode = node;
            node->m_colorAction = action;
            
            if (!node->m_inheritanceNode) {
                node->m_inheritanceNode = m_unkVector270[colorID];
                if (!node->m_inheritanceNode) {
                    node->m_inheritanceNode = InheritanceNode::create(colorID, 0);
                    m_unkDict268->setObject(node->m_inheritanceNode, colorID);
                    m_unkVector270[colorID] = node->m_inheritanceNode;
                }
            }
        }
        
        colorElement = static_cast<cocos2d::CCDictElement*>(colorElement->hh.next);
    }
    
    // Traverse inheritance chains
    cocos2d::CCDictElement* inheritElement = m_unkDict268->m_pElements;
    while (inheritElement) {
        InheritanceNode* node = static_cast<InheritanceNode*>(inheritElement->m_pObject);
        if (!node->m_processed && node->m_hasInheritance) {
            traverseInheritanceChain(node);
        }
        inheritElement = static_cast<cocos2d::CCDictElement*>(inheritElement->hh.next);
    }
}

void GJEffectManager::processMoveCalculations() {
    if (m_unkVector708.m_finish == m_unkVector708.m_start) {
        return;
    }
    
    DynamicMoveCalculation* current = m_unkVector708.m_start;
    while (current != m_unkVector708.m_finish) {
        float angle = -current->m_gameObject->m_rotationXOffset * 0.017453292f;
        float cosAngle = cosf(angle);
        float sinAngle = sinf(angle);
        
        float x = current->m_offset.x;
        float y = current->m_offset.y;
        
        float newX = (x * cosAngle - y * sinAngle) * (current->m_gameObject->m_scaleXOffset + 1.0f);
        float newY = (y * cosAngle + x * sinAngle) * (current->m_gameObject->m_scaleYOffset + 1.0f);
        
        cocos2d::CCPoint newOffset = cocos2d::CCPoint(newX, newY);
        cocos2d::CCPoint delta = newOffset - current->m_offset;
        
        CCMoveCNode* moveNode = current->m_moveNode;
        moveNode->m_unk0d1 = 0;
        moveNode->m_unk038 += delta.x;
        moveNode->m_unk040 += delta.y;
        moveNode->m_unk090 += delta.x;
        moveNode->m_unk098 += delta.y;
        
        ++current;
    }
    
    m_unkVector708.m_finish = m_unkVector708.m_start;
}

void GJEffectManager::processPulseActions() {
    size_t total = 0x8E38E38E38E38E39LL * (((char*)m_pulseEffectVector.m_finish - (char*)m_pulseEffectVector.m_start) >> 3);
    if (total == 0) {
        return;
    }
    
    size_t index = 0;
    size_t count = 0;
    
    while (count < total) {
        PulseEffectAction* pulseAction = &m_pulseEffectVector.m_start[index];
        ColorAction* colorAction = getColorAction(pulseAction->m_targetGroupID);
        ColorActionSprite* colorSprite = colorAction->m_colorSprite;
        
        if (!pulseAction->m_colorIndex && !colorAction->m_copyID) {
            cocos2d::ccColor3B pulseColor = colorForPulseEffect(&colorSprite->m_copyColor, pulseAction);
            colorSprite->m_copyColor = pulseColor;
        }
        
        ++count;
        ++index;
    }
}

void GJEffectManager::registerRotationCommand(GroupCommandObject2* object, bool clear) {
    int targetGroupID = object->m_targetGroupID;
    int centerGroupID = object->m_centerGroupID;
    
    if (clear) {
        // Remove existing rotation commands for this pair
        auto it = m_unkMap770.find({targetGroupID, centerGroupID});
        if (it != m_unkMap770.end()) {
            // Clear the vector
            it->second.clear();
        }
    }
    
    // Add to rotation map
    m_unkMap770[{targetGroupID, centerGroupID}].push_back(object);
    m_unkVector5b0.push_back(object);
}

void GJEffectManager::removeAllPulseActions() {
    m_pulseEffectVector.clear();
    m_pulseEffectMap.clear();
    
    // Reset pulse group vector
    for (size_t i = 0; i < m_unkVector2f0.size(); ++i) {
        m_unkVector2f0[i] = false;
    }
}

void GJEffectManager::removeColorAction(int id) {
    int index = (id > 1100) ? 1101 : ((id < 0) ? 0 : id);
    
    m_unkObject148->removeObjectForKey(index);
    m_colorActionVector[index] = nullptr;
    m_colorActionDict->removeObjectForKey(index);
}

void GJEffectManager::removePersistentFromAllItems() {
    m_persistentItemCountMap.clear();
}

void GJEffectManager::removePersistentFromAllTimers() {
    m_persistentTimerItemSet.clear();
}

void GJEffectManager::removeTriggeredID(int objectUniqueID, int playerUniqueID) {
    auto it = m_unkMap498.find({objectUniqueID, playerUniqueID});
    if (it != m_unkMap498.end()) {
        m_unkMap498.erase(it);
    }
}

void GJEffectManager::reset() {
    // Clear collision map
    m_unkMap288.clear();
    
    // Clear color action dictionary
    m_colorActionDict->removeAllObjects();
    
    // Clear inheritance dictionary
    m_unkDict268->removeAllObjects();
    
    // Clear color action object dictionary
    m_unkObject148->removeAllObjects();
    
    // Reset color action vectors
    for (size_t i = 0; i < m_colorActionVector.size(); ++i) {
        m_colorActionVector[i] = nullptr;
    }
    
    // Reset inheritance nodes
    for (size_t i = 0; i < m_unkVector270.size(); ++i) {
        m_unkVector270[i] = nullptr;
    }
    
    // Reset color sprites
    if (m_unkDict150) {
        cocos2d::CCDictElement* element = m_unkDict150->m_pElements;
        while (element) {
            ColorActionSprite* sprite = static_cast<ColorActionSprite*>(element->m_pObject);
            sprite->m_color = cocos2d::ccc3(255, 255, 255);
            sprite->m_copyColor = cocos2d::ccc3(255, 255, 255);
            sprite->m_opacity = 1.0f;
            sprite->m_hsv = nullptr;
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
    
    resetEffects();
    resetMoveActions();
}

void GJEffectManager::resetEffects() {
    removeAllPulseActions();
    m_opacityEffectMap.clear();
    
    // Clear touch toggle actions
    for (auto& action : m_unkVector1e0) {
        if (action.m_remapKeys.m_start) {
            delete[] action.m_remapKeys.m_start;
        }
    }
    m_unkVector1e0.clear();
    
    // Clear toggle trigger actions
    for (auto& action : m_unkVector248) {
        if (action.m_remapKeys.m_start) {
            delete[] action.m_remapKeys.m_start;
        }
    }
    m_unkVector248.clear();
    
    // Clear collision trigger actions
    for (auto& action : m_unkVector230) {
        if (action.m_remapKeys.m_start) {
            delete[] action.m_remapKeys.m_start;
        }
    }
    m_unkVector230.clear();
    
    resetToggledGroups();
    m_itemCountMap.clear();
    transferPersistentItems();
    resetTriggeredIDs();
    
    // Clear spawn trigger actions
    for (auto& action : m_spawnTriggerActions) {
        if (action.m_remapKeys.m_start) {
            delete[] action.m_remapKeys.m_start;
        }
    }
    m_spawnTriggerActions.clear();
    
    m_unkMap4c8.clear();
    m_countTriggerActions.clear();
    m_unkMap3f8.clear();
    
    // Clear non-persistent timers
    for (auto it = m_timerItemMap.begin(); it != m_timerItemMap.end();) {
        if (m_persistentTimerItemSet.find(it->first) == m_persistentTimerItemSet.end()) {
            it = m_timerItemMap.erase(it);
        } else {
            ++it;
        }
    }
    
    m_unk780 = 0.0f;
    m_unk784 = 0.0f;
    m_unk788 = 0.0f;
    m_unk78C = 0.0f;
    m_unk790 = 0.0f;
    m_unk794 = 0.0f;
}

void GJEffectManager::resetMoveActions() {
    m_unkVector560.clear();
    m_unkMap578.clear();
    m_unkMap618.clear();
    m_unkVector6c0.clear();
}

void GJEffectManager::resetTempGroupCommands(bool noRemove) {
    GroupCommandObject2** finish = m_unkVector518.m_finish;
    GroupCommandObject2** start = m_unkVector518.m_start;
    
    if (finish == start) {
        m_unkVector518.m_finish = finish;
        return;
    }
    
    while (start != finish) {
        GroupCommandObject2* command = *start;
        GroupCommandObject2* commandPtr = *start;
        
        if (!noRemove && (unsigned __int64)(m_unkVector530.m_finish - m_unkVector530.m_start) <= 0x3E7) {
            // Recycle the command
            command->reset();
            if (m_unkVector530.m_finish == m_unkVector530.m_reserveEnd) {
                std::vector<GroupCommandObject2*>::_M_emplace_back_aux<GroupCommandObject2* const&>(
                    reinterpret_cast<__int64>(&m_unkVector530),
                    &commandPtr
                );
            } else {
                if (m_unkVector530.m_finish) {
                    *m_unkVector530.m_finish = commandPtr;
                    m_unkVector530.m_finish = m_unkVector530.m_finish;
                }
                m_unkVector530.m_finish = m_unkVector530.m_finish + 1;
            }
            ++start;
            continue;
        }
        
        if (command) {
            // Clean up remap keys
            if (command->m_remapKeys.m_start) {
                operator delete(command->m_remapKeys.m_start);
            }
            
            // Clean up keyframes
            KeyframeObject* keyframes = command->m_keyframes.m_start;
            KeyframeObject* keyframesEnd = command->m_keyframes.m_finish;
            
            if (keyframes != keyframesEnd) {
                do {
                    // Clean up spline2 data
                    if (keyframes->m_spline2.m_d.m_start) {
                        operator delete(keyframes->m_spline2.m_d.m_start);
                    }
                    if (keyframes->m_spline2.m_c.m_start) {
                        operator delete(keyframes->m_spline2.m_c.m_start);
                    }
                    if (keyframes->m_spline2.m_b.m_start) {
                        operator delete(keyframes->m_spline2.m_b.m_start);
                    }
                    if (keyframes->m_spline2.m_y.m_start) {
                        operator delete(keyframes->m_spline2.m_y.m_start);
                    }
                    if (keyframes->m_spline2.m_x.m_start) {
                        operator delete(keyframes->m_spline2.m_x.m_start);
                    }
                    
                    // Clean up spline1 data
                    if (keyframes->m_spline1.m_d.m_start) {
                        operator delete(keyframes->m_spline1.m_d.m_start);
                    }
                    if (keyframes->m_spline1.m_c.m_start) {
                        operator delete(keyframes->m_spline1.m_c.m_start);
                    }
                    if (keyframes->m_spline1.m_b.m_start) {
                        operator delete(keyframes->m_spline1.m_b.m_start);
                    }
                    if (keyframes->m_spline1.m_y.m_start) {
                        operator delete(keyframes->m_spline1.m_y.m_start);
                    }
                    if (keyframes->m_spline1.m_x.m_start) {
                        operator delete(keyframes->m_spline1.m_x.m_start);
                    }
                    
                    ++keyframes;
                } while (keyframesEnd != keyframes);
                
                keyframesEnd = command->m_keyframes.m_start;
            }
            
            if (keyframesEnd) {
                operator delete(keyframesEnd);
            }
            
            ++start;
            operator delete(command);
            
            if (finish == start) {
                finish = m_unkVector518.m_start;
                break;
            }
        } else {
            ++start;
            if (finish == start) {
                finish = m_unkVector518.m_start;
                break;
            }
        }
    }
    
    m_unkVector518.m_finish = finish;
}


void GJEffectManager::resetToggledGroups() {
    // Reset all groups to enabled
    for (size_t i = 0; i < m_unkVector438.size(); ++i) {
        m_unkVector438[i] = true;
    }
    m_unkMap460.clear();
}

void GJEffectManager::resetTriggeredIDs() {
    m_unkMap498.clear();
}

void GJEffectManager::resetUsedItemState() {
    m_unk3f0.clear();
    m_unk430.clear();
}

void GJEffectManager::resumeTimer(int id) {
    auto it = m_timerItemMap.find(id);
    if (it != m_timerItemMap.end()) {
        it->second.m_paused = false;
    }
}

void GJEffectManager::saveCompletedMove(int groupId, double dx, double dy) {
    m_unkMap578[groupId].first += dx;
    m_unkMap578[groupId].second += dy;
}

void GJEffectManager::resetToggledGroups() {
    // Reset all groups to enabled
    for (size_t i = 0; i < m_unkVector438.size(); ++i) {
        m_unkVector438[i] = true;
    }
    m_unkMap460.clear();
}

void GJEffectManager::resetTriggeredIDs() {
    m_unkMap498.clear();
}

void GJEffectManager::resetUsedItemState() {
    m_unk3f0.clear();
    m_unk430.clear();
}

void GJEffectManager::resumeTimer(int id) {
    auto it = m_timerItemMap.find(id);
    if (it != m_timerItemMap.end()) {
        it->second.m_paused = false;
    }
}

void GJEffectManager::saveCompletedMove(int groupId, double dx, double dy) {
    m_unkMap578[groupId].first += dx;
    m_unkMap578[groupId].second += dy;
}

void GJEffectManager::saveToState(EffectManagerState* state) {
    // Save unordered set 2
    state->m_unorderedSet_int2 = m_unkMap4c8;
    
    // Save color action states
    int colorActionCount = m_unkObject148->count();
    state->m_unkVecCAState.resize(colorActionCount);
    
    if (m_unkObject148) {
        cocos2d::CCDictElement* element = m_unkObject148->m_pElements;
        size_t index = 0;
        while (element) {
            ColorAction* action = static_cast<ColorAction*>(element->m_pObject);
            ColorAction::saveToState(action, &state->m_unkVecCAState[index++]);
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
    
    // Save pulse effect vector
    state->m_unkVecPulseEffectAction = m_pulseEffectVector;
    
    // Save pulse effect map
    state->m_unorderedMapInt_vectorPulseEffectAction = m_pulseEffectMap;
    
    // Save opacity effect map
    state->m_unorderedMapInt_OpacityEffectAction = m_opacityEffectMap;
    
    // Save unordered set 1
    state->m_unorderedSet_int1 = m_unkMap460;
    
    // Save map
    state->m_mapInt_Int = m_unkMap498;
    
    // Save spawn trigger actions
    state->m_vectorSpawnTriggerAction = m_spawnTriggerActions;
    
    // Save group command objects
    state->m_vectorGroupCommandObject2 = m_unkVector560;
    
    // Save move position map
    state->m_unorderedMapInt_pair_double_double = m_unkMap578;
    
    // Save touch toggle actions
    state->m_vectorTouchToggleAction = m_unkVector1e0;
    
    // Save count trigger actions
    state->m_unorderedMapInt_vectorCountTriggerAction = m_countTriggerActions;
    
    // Save toggle trigger actions
    state->m_vectorToggleTriggerAction = m_unkVector248;
    
    // Save collision trigger actions
    state->m_vectorCollisionTriggerAction = m_unkVector230;
    
    // Save collision map
    state->m_unorderedMapInt_bool = m_unkMap288;
    
    // Save item count map
    state->m_itemCountMap = m_itemCountMap;
    
    // Save timer item map
    state->m_timerItemMap = m_timerItemMap;
    
    // Save timer trigger actions
    state->m_unorderedMapInt_vectorTimerTriggerAction = m_unkMap3f8;
}

void GJEffectManager::setColorAction(ColorAction* action, int id) {
    int index = (id > 1100) ? 1101 : ((id < 0) ? 0 : id);
    
    action->m_colorID = index;
    m_unkObject148->setObject(action, index);
    m_colorActionVector[index] = action;
    
    ColorActionSprite* sprite = getColorSprite(index);
    action->m_colorSprite = sprite;
    sprite->m_colorAction = action;
}

void GJEffectManager::setFollowing(int targetID, int targetPosID, bool following) {
    int key = targetPosID + 2000 * targetID;
    
    if (following) {
        m_unkMap4c8.insert(key);
    } else {
        m_unkMap4c8.erase(key);
    }
}

void GJEffectManager::setupFromString(std::string str) {
    // Parse the string and create color actions
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t prevPos = 0;
    
    while ((pos = str.find('|', prevPos)) != std::string::npos) {
        tokens.push_back(str.substr(prevPos, pos - prevPos));
        prevPos = pos + 1;
    }
    if (prevPos < str.length()) {
        tokens.push_back(str.substr(prevPos));
    }
    
    for (const std::string& token : tokens) {
        ColorAction* action = ColorAction::create();
        action->setupFromString(token);
        
        if (action->m_colorID <= 1101) {
            setColorAction(action, action->m_colorID);
        }
    }
}

bool GJEffectManager::shouldBlend(int id) {
    int index = (id > 1100) ? 1101 : ((id < 0) ? 0 : id);
    ColorAction* action = m_colorActionVector[index];
    return action && action->m_blending;
}

void GJEffectManager::storeTriggeredID(int objectUniqueID, int playerUniqueID) {
    m_unkMap498.insert({objectUniqueID, playerUniqueID});
}

double GJEffectManager::timeForItem(int id) {
    int itemId = (id > 9998) ? 9999 : ((id < 0) ? 0 : id);
    auto it = m_timerItemMap.find(itemId);
    if (it != m_timerItemMap.end()) {
        return it->second.m_time;
    }
    return 0.0;
}

bool GJEffectManager::timerExists(int id) {
    return m_timerItemMap.find(id) != m_timerItemMap.end();
}

void GJEffectManager::timerWasUsed(int id) {
    // Empty implementation
}

void GJEffectManager::toggleGroup(int id, bool activate) {
    int groupId = (id > 9998) ? 9999 : ((id < 0) ? 0 : id);
    
    if (activate) {
        m_unkVector438[groupId] = true;
        m_unkMap460.erase(groupId);
    } else {
        m_unkVector438[groupId] = false;
        m_unkMap460.insert(groupId);
    }
}

void GJEffectManager::toggleItemPersistent(int id, bool persistent) {
    if (persistent) {
        int value = 0;
        auto it = m_itemCountMap.find(id);
        if (it != m_itemCountMap.end()) {
            value = it->second;
        }
        m_persistentItemCountMap[id] = value;
    } else {
        m_persistentItemCountMap.erase(id);
    }
}

void GJEffectManager::toggleTimerPersistent(int id, bool persistent) {
    if (persistent) {
        m_persistentTimerItemSet.insert(id);
    } else {
        m_persistentTimerItemSet.erase(id);
    }
}

void GJEffectManager::transferPersistentItems() {
    for (auto& pair : m_persistentItemCountMap) {
        int id = pair.first;
        int count = pair.second;
        if (count > 0) {
            m_itemCountMap[id] = count;
        } else {
            m_itemCountMap.erase(id);
        }
    }
}

void GJEffectManager::traverseInheritanceChain(InheritanceNode* node) {
    if (!node) {
        return;
    }
    
    // Collect all nodes in the chain
    std::vector<InheritanceNode*> chain;
    InheritanceNode* current = node;
    while (current) {
        chain.push_back(current);
        current = current->m_inheritanceNode;
    }
    
    // Add to array for processing
    for (InheritanceNode* n : chain) {
        m_unkArray430->addObject(n);
    }
    
    // Process inheritance chain in reverse order
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        InheritanceNode* n = *it;
        if (n->m_colorAction) {
            calculateInheritedColor(n->m_colorID, n->m_colorAction);
        }
    }
    
    m_unkArray430->removeAllObjects();
}

CCMoveCNode* GJEffectManager::tryGetMoveCommandNode(int id) {
    auto it = m_unkMap618.find(id);
    if (it != m_unkMap618.end()) {
        return it->second;
    }
    return nullptr;
}

void GJEffectManager::updateActiveOpacityEffects() {
    for (auto it = m_opacityEffectMap.begin(); it != m_opacityEffectMap.end();) {
        if (it->second.m_disabled || it->second.m_currentValue < 1.0f) {
            it = m_opacityEffectMap.erase(it);
        } else {
            ++it;
        }
    }
}

void GJEffectManager::updateColorAction(ColorAction* action) {
    if (!action->m_copyID) {
        ColorActionSprite* sprite = action->m_colorSprite;
        sprite->m_color = action->m_color;
        sprite->m_opacity = action->m_currentOpacity * 255.0f;
    }
}

void GJEffectManager::updateColorEffects(float dt) {
    if (!m_unkObject148) {
        return;
    }
    
    cocos2d::CCDictElement* element = m_unkObject148->m_pElements;
    while (element) {
        ColorAction* action = static_cast<ColorAction*>(element->m_pObject);
        action->step(dt);
        
        if (!action->m_copyID) {
            ColorActionSprite* sprite = action->m_colorSprite;
            if (sprite) {
                sprite->m_color = action->m_color;
                sprite->m_opacity = action->m_currentOpacity * 255.0f;
            }
        }
        
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJEffectManager::updateCountForItem(int id, int count) {
    int itemId = (id > 9998) ? 9999 : ((id < 0) ? 0 : id);
    int oldCount = countForItem(itemId);
    
    if (count == 0) {
        m_itemCountMap.erase(itemId);
    } else {
        m_itemCountMap[itemId] = count;
    }
    
    auto it = m_persistentItemCountMap.find(itemId);
    if (it != m_persistentItemCountMap.end()) {
        m_persistentItemCountMap[itemId] = count;
    }
    
    // Check count triggers
    auto triggerIt = m_countTriggerActions.find(itemId);
    if (triggerIt != m_countTriggerActions.end()) {
        auto& actions = triggerIt->second;
        
        // Sort actions by value
        std::sort(actions.begin(), actions.end(), 
            [](const CountTriggerAction& a, const CountTriggerAction& b) {
                return a.m_targetValue < b.m_targetValue;
            });
        
        for (auto& action : actions) {
            if (action.m_disabled) continue;
            
            if ((count >= action.m_targetValue && action.m_oldCount < action.m_targetValue) ||
                (count <= action.m_targetValue && action.m_oldCount > action.m_targetValue)) {
                
                if (m_triggerEffectDelegate) {
                    m_triggerEffectDelegate->toggleGroupTriggered(
                        action.m_targetGroupID,
                        action.m_activateGroup,
                        action.m_remapKeys,
                        action.m_triggerUniqueID,
                        action.m_controlID
                    );
                } else {
                    toggleGroup(action.m_targetGroupID, action.m_activateGroup);
                }
            }
        }
    }
}

void GJEffectManager::updateEffects(float dt) {
    updateColorEffects(dt);
    updatePulseEffects(dt);
    updateOpacityEffects(dt);
}

void GJEffectManager::updateOpacityAction(OpacityEffectAction* action) {
    if (action->m_finished && action->m_currentValue >= 1.0f) {
        m_opacityEffectMap.erase(action->m_targetGroupID);
    }
}

void GJEffectManager::updateOpacityEffects(float dt) {
    auto it = m_opacityEffectMap.begin();
    while (it != m_opacityEffectMap.end()) {
        OpacityEffectAction& action = it->second;
        action.step(dt);
        
        if (action.m_finished && action.m_currentValue >= 1.0f) {
            it = m_opacityEffectMap.erase(it);
        } else {
            ++it;
        }
    }
}

void GJEffectManager::updatePulseEffects(float dt) {
    // Update pulse effect vector
    size_t index = 0;
    while (index < m_pulseEffectVector.size()) {
        PulseEffectAction& action = m_pulseEffectVector[index];
        action.step(dt);
        
        if (action.isFinished()) {
            m_pulseEffectVector.erase(m_pulseEffectVector.begin() + index);
        } else {
            ++index;
        }
    }
    
    // Update pulse effect map
    for (auto& pair : m_pulseEffectMap) {
        int groupID = pair.first;
        auto& actions = pair.second;
        
        size_t mapIndex = 0;
        while (mapIndex < actions.size()) {
            PulseEffectAction& action = actions[mapIndex];
            action.step(dt);
            
            if (action.isFinished()) {
                actions.erase(actions.begin() + mapIndex);
            } else {
                ++mapIndex;
            }
        }
        
        // Update pulse group vector
        if (actions.empty()) {
            m_unkVector2f0[groupID] = false;
        } else {
            m_unkVector2f0[groupID] = true;
        }
    }
}

void GJEffectManager::updateSpawnTriggers(float dt) {
    size_t index = 0;
    while (index < m_spawnTriggerActions.size()) {
        SpawnTriggerAction& action = m_spawnTriggerActions[index];
        action.step(dt);
        
        if (action.isFinished() && m_triggerEffectDelegate) {
            if (action.m_gameObject) {
                m_triggerEffectDelegate->spawnObject(
                    action.m_gameObject,
                    action.m_remapKeys,
                    action.m_deltaTime - action.m_duration
                );
            } else {
                m_triggerEffectDelegate->spawnGroup(
                    action.m_targetGroupID,
                    action.m_spawnOrdered,
                    action.m_remapKeys,
                    action.m_triggerUniqueID,
                    action.m_controlID,
                    action.m_deltaTime - action.m_duration
                );
            }
            
            if (m_triggerEffectDelegate->checkSpawnAbuse()) {
                // Clean up and exit
                break;
            }
            
            m_spawnTriggerActions.erase(m_spawnTriggerActions.begin() + index);
        } else {
            ++index;
        }
    }
    
    // Compact spawn triggers
    size_t writeIndex = 0;
    for (size_t readIndex = 0; readIndex < m_spawnTriggerActions.size(); ++readIndex) {
        if (!m_spawnTriggerActions[readIndex].m_finished) {
            if (writeIndex != readIndex) {
                m_spawnTriggerActions[writeIndex] = m_spawnTriggerActions[readIndex];
            }
            ++writeIndex;
        }
    }
    m_spawnTriggerActions.resize(writeIndex);
}

void GJEffectManager::updateTimer(int id, double time) {
    int itemId = (id > 9998) ? 9999 : ((id < 0) ? 0 : id);
    
    double clampedTime = time;
    if (time > 9999999.0) {
        clampedTime = 9999999.0;
    } else if (time < -9999999.0) {
        clampedTime = -9999999.0;
    }
    
    auto it = m_timerItemMap.find(itemId);
    if (it != m_timerItemMap.end()) {
        it->second.m_time = clampedTime;
    } else {
        // Create new timer
        TimerItem item;
        item.m_time = clampedTime;
        item.m_paused = false;
        item.m_active = true;
        m_timerItemMap[itemId] = item;
    }
}

void GJEffectManager::updateTimers(float dt, float timeWarp) {
    std::vector<int> timerIds;
    timerIds.reserve(m_timerItemMap.size());
    
    // Collect timer IDs
    for (auto& pair : m_timerItemMap) {
        timerIds.push_back(pair.first);
    }
    
    for (int id : timerIds) {
        auto it = m_timerItemMap.find(id);
        if (it == m_timerItemMap.end()) continue;
        
        TimerItem& item = it->second;
        if (!item.m_active || item.m_paused) continue;
        
        double oldTime = item.m_time;
        item.m_time += dt * item.m_speed;
        
        if (item.m_hasLimit) {
            if ((oldTime < item.m_limit && item.m_time >= item.m_limit) ||
                (oldTime > item.m_limit && item.m_time <= item.m_limit)) {
                
                item.m_active = false;
                item.m_time = item.m_limit;
                
                if (item.m_targetGroupID > 0) {
                    if (m_triggerEffectDelegate) {
                        m_triggerEffectDelegate->spawnGroup(
                            item.m_targetGroupID,
                            0,
                            &item.m_remapKeys,
                            item.m_triggerUniqueID,
                            item.m_controlID,
                            0.0
                        );
                    }
                }
            }
        }
        
        // Check timer triggers
        auto triggerIt = m_unkMap3f8.find(id);
        if (triggerIt != m_unkMap3f8.end()) {
            auto& triggers = triggerIt->second;
            for (auto& trigger : triggers) {
                if (trigger.m_disabled) continue;
                if (trigger.m_startTime < item.m_time && trigger.m_startTime >= oldTime) {
                    if (m_triggerEffectDelegate) {
                        m_triggerEffectDelegate->spawnGroup(
                            trigger.m_targetGroupID,
                            0,
                            &item.m_remapKeys,
                            trigger.m_triggerUniqueID,
                            trigger.m_controlID,
                            0.0
                        );
                    }
                }
            }
        }
    }
}

bool GJEffectManager::wasFollowing(int targetID, int targetPosID) {
    int key = targetPosID + 2000 * targetID;
    return m_unkMap4c8.find(key) != m_unkMap4c8.end();
}

bool GJEffectManager::wouldCreateLoop(InheritanceNode* node, int id) {
    InheritanceNode* current = node;
    while (current) {
        if (current->m_colorID == id) {
            return true;
        }
        current = current->m_inheritanceNode;
    }
    return false;
}

GJEffectManager::~GJEffectManager() {
    // Clean up dictionaries
    if (m_unkObject148) {
        m_unkObject148->release();
    }
    if (m_unkDict150) {
        m_unkDict150->release();
    }
    if (m_unkArray430) {
        m_unkArray430->release();
    }
    if (m_unkDict268) {
        m_unkDict268->release();
    }
    if (m_colorActionDict) {
        m_colorActionDict->release();
    }
}