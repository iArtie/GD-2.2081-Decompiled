// GJBigSprite.cpp
#include "Sprites/GJBigSprite.h"
#include "Nodes/GJBigSpriteNode.h"

GJBigSprite* GJBigSprite::create() {
    GJBigSprite* instance = new GJBigSprite();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

GJBigSprite::GJBigSprite() : m_spriteNodes(nullptr), m_unloadingAll(false), 
                             m_loadSprites(false), m_scaleX(1.0f), m_scaleY(1.0f) {}

GJBigSprite::~GJBigSprite() {
    // Destructor implementation
}

bool GJBigSprite::init() {
    if (!cocos2d::CCNode::init()) {
        return false;
    }
    
    m_spriteNodes = cocos2d::CCArray::create();
    m_spriteNodes->retain();
    
    // Resize sprites loaded vector to 900 entries
    m_spritesLoaded.resize(900, false);
    
    // Get image size to calculate scale
    cocos2d::CCSprite* smallImage = cocos2d::CCSprite::create("mapimages/image_small.png");
    float imageWidth = smallImage->getContentSize().width;
    float imageHeight = smallImage->getContentSize().height;
    
    m_scaleX = 1.0f / (imageWidth / 120.0f / 120.0f);
    m_scaleY = 1.0f / (imageHeight / 50.0f / 50.0f);
    
    // Create 900 sprite nodes (30 columns x 30 rows)
    for (int index = 1; index <= 900; ++index) {
        int row = (index - 1) / 30;
        int col = (index - 1) % 30;
        
        GJBigSpriteNode* node = GJBigSpriteNode::create();
        
        float posX = col * 120.0f + 60.0f;
        float posY = -row * 50.0f - 25.0f;
        node->setPosition(cocos2d::CCPoint(posX, posY + 1500.0f));
        
        // Set sprite rect
        float rectX = imageWidth / 30.0f * col;
        float rectY = imageHeight / 30.0f * row;
        float rectWidth = imageWidth / 30.0f;
        float rectHeight = imageHeight / 30.0f;
        node->m_spriteRect = cocos2d::CCRect(rectX, rectY, rectWidth, rectHeight);
        
        // Set sprite name based on index
        char buffer[64];
        if (index <= 9) {
            snprintf(buffer, sizeof(buffer), "mapimages/image_part_00%d.png", index);
        } else if (index <= 99) {
            snprintf(buffer, sizeof(buffer), "mapimages/image_part_0%d.png", index);
        } else {
            snprintf(buffer, sizeof(buffer), "mapimages/image_part_%d.png", index);
        }
        node->m_spriteName = buffer;
        
        m_spriteNodes->addObject(node);
    }
    
    return true;
}

void GJBigSprite::finishedLoadingSpriteAsync(cocos2d::CCObject* obj) {
    int index = obj->getTag();
    GJBigSpriteNode* node = static_cast<GJBigSpriteNode*>(m_spriteNodes->objectAtIndex(index));
    
    if (node->m_isLoaded) {
        // Remove existing sprite if any
        if (node->m_sprite) {
            if (node->m_sprite->getReferenceCount() == -1) {
                node->m_sprite->release();
                node->m_sprite = nullptr;
            } else {
                return;
            }
        }
        
        // Create sprite from loaded texture
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(node->m_spriteName.c_str());
        sprite->setPosition(node->getPosition());
        sprite->setScale(1.002f);
        this->addChild(sprite);
        node->m_sprite = sprite;
    } else {
        // Unload sprite if not loaded
        unloadSprite(node->m_spriteName, index);
    }
}

void GJBigSprite::loadSpriteAsync(std::string filename, int index) {
    size_t bitIndex = index / 64;
    size_t bitOffset = index % 64;
    
    if (!m_spritesLoaded[bitIndex * 64 + bitOffset]) {
        m_spritesLoaded[bitIndex * 64 + bitOffset] = true;
        
        cocos2d::CCTextureCache::sharedTextureCache()->addImageAsync(
            filename.c_str(),
            this,
            cocos2d::SEL_MenuHandler(&GJBigSprite::finishedLoadingSpriteAsync),
            index,
            kCCTexture2DPixelFormat_RGB888
        );
    }
}

void GJBigSprite::unloadAll() {
    m_unloadingAll = true;
    
    for (int i = 0; i < m_spriteNodes->count(); ++i) {
        GJBigSpriteNode* node = static_cast<GJBigSpriteNode*>(m_spriteNodes->objectAtIndex(i));
        node->m_isLoaded = false;
        
        if (node->m_sprite) {
            node->m_sprite->release();
            node->m_sprite = nullptr;
        }
        
        unloadSprite(node->m_spriteName, i);
    }
}

void GJBigSprite::unloadSprite(std::string filename, int index) {
    cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(filename.c_str());
    
    size_t bitIndex = index / 64;
    size_t bitOffset = index % 64;
    m_spritesLoaded[bitIndex * 64 + bitOffset] = false;
}

void GJBigSprite::updateSpriteVisibility() {
    if (m_unloadingAll) {
        return;
    }
    
    // Save current pixel format
    cocos2d::CCTexture2DPixelFormat defaultFormat = cocos2d::CCTexture2D::defaultAlphaPixelFormat();
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGB888);
    
    float scale = this->getScale();
    float loadOffset = m_loadSprites ? 10.0f / scale : 0.0f;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint position = this->getPosition();
    
    // Calculate visible rect
    float minX = -position.x / scale - loadOffset;
    float minY = -position.y / scale - loadOffset;
    float maxX = (winSize.width - position.x) / scale + loadOffset;
    float maxY = (winSize.height - position.y) / scale + loadOffset;
    
    cocos2d::CCRect visibleRect(minX, minY, maxX - minX, maxY - minY);
    
    // Update each sprite node
    for (int i = 0; i < m_spriteNodes->count(); ++i) {
        GJBigSpriteNode* node = static_cast<GJBigSpriteNode*>(m_spriteNodes->objectAtIndex(i));
        cocos2d::CCPoint nodePos = node->getPosition();
        
        cocos2d::CCRect nodeRect(nodePos.x - 60.0f, nodePos.y - 25.0f, 120.0f, 50.0f);
        
        bool isVisible = cocos2d::CCRect::intersectsRect(visibleRect, nodeRect);
        
        if (!isVisible) {
            node->m_isLoaded = false;
            if (node->m_sprite) {
                node->m_sprite->release();
                node->m_sprite = nullptr;
            }
            unloadSprite(node->m_spriteName, i);
        } else {
            node->m_isLoaded = true;
            
            if (!node->m_sprite) {
                if (!m_loadSprites) {
                    // Create sprite immediately
                    cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(node->m_spriteName.c_str());
                    sprite->setPosition(node->getPosition());
                    sprite->setScale(1.002f);
                    this->addChild(sprite);
                    node->m_sprite = sprite;
                } else {
                    // Load asynchronously
                    loadSpriteAsync(node->m_spriteName, i);
                    
                    // Show placeholder small image
                    cocos2d::CCSprite* placeholder = cocos2d::CCSprite::create("mapimages/image_small.png");
                    placeholder->setScaleX(m_scaleX);
                    placeholder->setScaleY(m_scaleY);
                    placeholder->setPosition(node->getPosition());
                    placeholder->setTag(-1);
                    this->addChild(placeholder);
                    node->m_sprite = placeholder;
                }
            }
        }
    }
    
    // Restore pixel format
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(defaultFormat);
}