#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include <string>

class CreateMenuItem : public CCMenuItemSpriteExtra {
public:
    static CreateMenuItem* create(const std::string& spriteFileName, 
                                   cocos2d::CCObject* target, 
                                   cocos2d::SEL_MenuHandler selector);
    bool init(const std::string& spriteFileName, 
              cocos2d::CCObject* target, 
              cocos2d::SEL_MenuHandler selector);
    
    virtual ~CreateMenuItem();

    const std::string& getSpriteFileName() const { return m_spriteFileName; }
    void setSpriteFileName(const std::string& filename) { m_spriteFileName = filename; }

protected:
    std::string m_spriteFileName;
};