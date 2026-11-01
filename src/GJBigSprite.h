// GJBigSprite.h
#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>

class GJBigSpriteNode;

class GJBigSprite : public cocos2d::CCNode {
public:
    static GJBigSprite* create();
    
    void finishedLoadingSpriteAsync(cocos2d::CCObject* obj);
    bool init();
    void loadSpriteAsync(std::string filename, int index);
    void unloadAll();
    void unloadSprite(std::string filename, int index);
    void updateSpriteVisibility();
    
    virtual ~GJBigSprite();

private:
    std::vector<bool> m_spritesLoaded;
    cocos2d::CCArray* m_spriteNodes;
    bool m_unloadingAll;
    bool m_loadSprites;
    float m_scaleX;
    float m_scaleY;
};