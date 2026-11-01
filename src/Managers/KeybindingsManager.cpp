#include "Managers/KeybindingsManager.h"
#include "Utils/DS_Dictionary.h"

KeybindingsManager* KeybindingsManager::s_sharedInstance = nullptr;

bool KeybindingsManager::init() {
    return true;
}

KeybindingsManager* KeybindingsManager::sharedState() {
    if (!s_sharedInstance) {
        s_sharedInstance = new KeybindingsManager();
        s_sharedInstance->init();
    }
    return s_sharedInstance;
}

void KeybindingsManager::firstSetup() {
    auto dict1 = cocos2d::CCDictionary::create();
    if (dict1 != m_keyToCommandDict) {
        if (dict1) dict1->retain();
        if (m_keyToCommandDict) m_keyToCommandDict->release();
        m_keyToCommandDict = dict1;
    }
    
    auto dict2 = cocos2d::CCDictionary::create();
    if (dict2 != m_commandToKeyDict) {
        if (dict2) dict2->retain();
        if (m_commandToKeyDict) m_commandToKeyDict->release();
        m_commandToKeyDict = dict2;
    }
}

void KeybindingsManager::dataLoaded(DS_Dictionary* dict) {
    auto dict1 = dict->getDictForKey("KBM_001");
    if (dict1 != m_keyToCommandDict) {
        if (dict1) dict1->retain();
        if (m_keyToCommandDict) m_keyToCommandDict->release();
        m_keyToCommandDict = dict1;
    }
    
    auto dict2 = dict->getDictForKey("KBM_002");
    if (dict2 != m_commandToKeyDict) {
        if (dict2) dict2->retain();
        if (m_commandToKeyDict) m_commandToKeyDict->release();
        m_commandToKeyDict = dict2;
    }
}

void KeybindingsManager::encodeDataTo(DS_Dictionary* dict) {
    dict->setDictForKey("KBM_001", m_keyToCommandDict);
    dict->setDictForKey("KBM_002", m_commandToKeyDict);
}

bool KeybindingsManager::commandForKeyMods(cocos2d::enumKeyCodes key, GJKeyGroup group) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getKeyboardDispatcher();
    return commandForKey(key, group, dispatcher->m_bControlPressed, dispatcher->m_bAltPressed, dispatcher->m_bShiftPressed);
}

bool KeybindingsManager::commandForKeyNoMods(cocos2d::enumKeyCodes key, GJKeyGroup group) {
    return commandForKey(key, group, false, false, false);
}

cocos2d::CCDictionary* KeybindingsManager::commandToKeyForGroup(GJKeyGroup group) {
    auto key = cocos2d::CCString::createWithFormat("%i", static_cast<int>(group))->getCString();
    auto dict = static_cast<cocos2d::CCDictionary*>(m_commandToKeyDict->objectForKey(key));
    
    if (!dict) {
        dict = cocos2d::CCDictionary::create();
        m_commandToKeyDict->setObject(dict, key);
    }
    
    return dict;
}

cocos2d::CCDictionary* KeybindingsManager::keyToCommandForGroup(GJKeyGroup group) {
    auto key = cocos2d::CCString::createWithFormat("%i", static_cast<int>(group))->getCString();
    auto dict = static_cast<cocos2d::CCDictionary*>(m_keyToCommandDict->objectForKey(key));
    
    if (!dict) {
        dict = cocos2d::CCDictionary::create();
        m_keyToCommandDict->setObject(dict, key);
    }
    
    return dict;
}

GJKeyGroup KeybindingsManager::groupForCommand(GJKeyCommand command) {
    if (command == GJKeyCommand::None) {
        return GJKeyGroup::Gameplay;
    }
    
    if (static_cast<int>(command) <= 4) {
        return GJKeyGroup::Gameplay;
    }
    
    return GJKeyGroup::Editor;
}

cocos2d::enumKeyCodes KeybindingsManager::keyForCommand(GJKeyCommand command) {
    return cocos2d::enumKeyCodes::KEY_None;
}

KeybindingsManager::~KeybindingsManager() {
    if (m_keyToCommandDict) {
        m_keyToCommandDict->release();
    }
    if (m_commandToKeyDict) {
        m_commandToKeyDict->release();
    }
}