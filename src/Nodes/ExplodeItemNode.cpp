#include "Nodes/ExplodeItemNode.h"

ExplodeItemNode* ExplodeItemNode::create(cocos2d::CCRenderTexture* texture) {
    ExplodeItemNode* ret = new ExplodeItemNode();
    if (ret && ret->init(texture)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ExplodeItemNode::init(cocos2d::CCRenderTexture* texture) {
    if (!cocos2d::CCNode::init())
        return false;
    
    m_renderTexture = texture;
    m_floorHeight = 90.0f;
    m_noGravity = false;
    
    if (texture) {
        texture->retain();
    }
    
    m_sprites = cocos2d::CCArray::create();
    if (m_sprites) {
        m_sprites->retain();
    }
    
    return true;
}

void ExplodeItemNode::update(float dt) {
    float gravity = m_noGravity ? 0.0f : dt * 54.0f * 0.5f;
    float horizontalSpeed = dt * 54.0f;
    float floorY = m_floorHeight - getPosition().y;
    
    unsigned int count = m_sprites->count();
    for (unsigned int i = 0; i < count; i++) {
        cocos2d::CCObject* obj = m_sprites->objectAtIndex(i);
        
        // Get position from the object (assuming it's stored in obj[10])
        cocos2d::CCPoint pos = obj->getPosition();
        
        // Decrease lifetime
        float lifetime = *(float*)((char*)obj + 40); // obj[10].m_nZOrder
        lifetime -= dt;
        *(float*)((char*)obj + 40) = lifetime;
        
        if (lifetime <= 0.0f) {
            // Remove particle system if exists
            cocos2d::CCParticleSystem* particle = (cocos2d::CCParticleSystem*)*(void**)((char*)obj + 44); // obj[11].__vftable
            if (particle) {
                particle->stopSystem();
                particle->removeFromParentAndCleanup(true);
                *(void**)((char*)obj + 44) = nullptr;
            }
            m_sprites->removeObjectAtIndex(i);
            obj->release();
            i--;
            count--;
            continue;
        }
        
        // Apply physics
        float scale = *(float*)((char*)obj + 48); // obj[10].m_uUnknown
        float velocityY = *(float*)((char*)obj + 52); // obj[10].m_unknown2
        velocityY -= gravity;
        *(float*)((char*)obj + 52) = velocityY;
        
        float friction = (1.0f - horizontalSpeed) * 0.02f + 0.98f;
        scale *= friction;
        *(float*)((char*)obj + 48) = scale;
        
        // Update position
        pos.x += horizontalSpeed * scale;
        pos.y += horizontalSpeed * velocityY;
        
        // Bounce off floor
        float width = *(float*)((char*)obj + 28); // obj[7].m_unknown2
        float height = *(float*)((char*)obj + 32); // obj[7].m_nZOrder
        float halfSize = (width <= height) ? width * 0.5f : height * 0.5f;
        float floorLimit = floorY + halfSize;
        
        if (pos.y < floorLimit) {
            pos.y = floorLimit;
            float velY = *(float*)((char*)obj + 52);
            if (velY < 0.0f) {
                velY *= -0.8f;
                if (velY < 1.5f)
                    velY = 1.5f;
                *(float*)((char*)obj + 52) = velY;
            }
        }
        
        obj->setPosition(pos);
        
        // Update rotation
        float rotation = *(float*)((char*)obj + 56); // obj[10].m_unknown5
        rotation += rotation * horizontalSpeed;
        obj->setRotation(rotation);
        
        // Update opacity based on lifetime
        float maxLifetime = *(float*)((char*)obj + 60); // obj[10].m_uOrderOfArrival
        if (lifetime < maxLifetime) {
            float opacity = (lifetime / maxLifetime) * 255.0f;
            obj->setOpacity((unsigned char)opacity);
            
            // Update particle opacity if exists
            cocos2d::CCParticleSystem* particle = (cocos2d::CCParticleSystem*)*(void**)((char*)obj + 44);
            if (particle) {
                particle->setOpacity((unsigned char)opacity);
            }
        }
    }
    
    if (m_sprites->count() == 0) {
        unscheduleUpdate();
        removeMeAndCleanup();
    }
}

ExplodeItemNode::~ExplodeItemNode() {
    if (m_renderTexture) {
        m_renderTexture->release();
        m_renderTexture = nullptr;
    }
    
    if (m_sprites) {
        m_sprites->release();
        m_sprites = nullptr;
    }
}