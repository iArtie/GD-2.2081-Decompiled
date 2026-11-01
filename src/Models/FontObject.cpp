#include "Models/FontObject.h"
#include <cstring>
#include <cstdlib>

FontObject* FontObject::createWithConfigFile(const char* font, float scale) {
    FontObject* ret = new FontObject();
    if (ret && ret->initWithConfigFile(font, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

int FontObject::getFontWidth(int character) {
    if (character >= 0 && character < (int)m_fontWidths.size()) {
        return m_fontWidths[character];
    }
    return 0;
}

bool FontObject::initWithConfigFile(const char* font, float scale) {
    parseConfigFile(font, scale);
    return true;
}

void FontObject::parseConfigFile(const char* font, float scale) {
    m_fontWidths.resize(300, 0);
    
    cocos2d::CCString* content = cocos2d::CCString::createWithContentsOfFile(font);
    if (!content) return;
    
    std::string str = content->getCString();
    cocos2d::CCArray* lines = cocos2d::CCArray::create();
    
    // Parse the config file line by line
    char* token = strtok((char*)str.c_str(), "\n");
    while (token) {
        std::string line = token;
        cocos2d::CCString* lineStr = cocos2d::CCString::create(line.c_str());
        lines->addObject(lineStr);
        token = strtok(nullptr, "\n");
    }
    
    for (unsigned int i = 0; i < lines->count(); i++) {
        cocos2d::CCString* lineStr = (cocos2d::CCString*)lines->objectAtIndex(i);
        if (!lineStr) continue;
        
        std::string line = lineStr->getCString();
        
        // Check if line starts with "chars"
        if (line.find("chars") == 0) {
            // Parse character data
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string data = line.substr(pos + 1);
                
                // Parse key=value pairs
                cocos2d::CCArray* pairs = cocos2d::CCArray::create();
                char* pairToken = strtok((char*)data.c_str(), "=");
                while (pairToken) {
                    cocos2d::CCString* pairStr = cocos2d::CCString::create(pairToken);
                    pairs->addObject(pairStr);
                    pairToken = strtok(nullptr, "=");
                }
                
                if (pairs->count() >= 4) {
                    cocos2d::CCString* charStr = (cocos2d::CCString*)pairs->objectAtIndex(0);
                    std::string charData = charStr->getCString();
                    
                    // Find the character ID (first number before space)
                    size_t spacePos = charData.find(' ');
                    if (spacePos != std::string::npos) {
                        std::string idStr = charData.substr(0, spacePos);
                        int charId = atoi(idStr.c_str());
                        
                        if (charId >= 0 && charId < 300) {
                            cocos2d::CCString* xStr = (cocos2d::CCString*)pairs->objectAtIndex(3);
                            cocos2d::CCString* widthStr = (cocos2d::CCString*)pairs->objectAtIndex(7);
                            int x = xStr->intValue();
                            int width = widthStr->intValue();
                            m_fontWidths[charId] = x + width;
                        }
                    }
                }
            }
        }
    }
    
    lines->release();
}

FontObject::~FontObject() {
    // Base class destructor handles cleanup
}