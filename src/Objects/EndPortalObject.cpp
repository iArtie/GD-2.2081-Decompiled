#include "Objects/EndPortalObject.h"
#include "Managers/GameManager.h"
#include "Layers/PlayLayer.h"
#include "Layers/UILayer.h"
#include "Objects/PlayerObject.h"

EndPortalObject* EndPortalObject::create() {
    EndPortalObject* ret = new EndPortalObject();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

cocos2d::CCPoint EndPortalObject::getSpawnPos() {
    cocos2d::CCPoint pos;
    float x = m_startPosition.x;
    float offset = 300.0f / GameManager::sharedState()->m_playLayer->m_gameState.m_cameraZoom;
    pos.x = x - (offset * (m_flippedX ? -1.0f : 1.0f));
    pos.y = m_startPosition.y;
    return pos;
}

bool EndPortalObject::init() {
    if (!GameObject::init("block001_01_001.png")) {
        return false;
    }
    
    setTextureRect(&stru_1242490);
    
    m_isInvisible = false;
    m_unk3F8 = true;
    m_objectType = 0; // Decoration
    
    m_particle = createAndAddParticle(7, "endEffectPortal.plist", 4, kCCPositionTypeGrouped);
    if (m_particle) {
        float scale = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraZoom;
        m_particle->setStartRadius(scale * 0.5f);
        m_particleOffset = cocos2d::CCPoint(-15.0f, 0.0f);
        
        PlayerObject* player = GameManager::sharedState()->m_playLayer->m_player1;
        cocos2d::ccColor4F startColor = {
            player->m_playerColor1.r / 255.0f,
            player->m_playerColor1.g / 255.0f,
            player->m_playerColor1.b / 255.0f,
            1.0f
        };
        m_particle->setStartColor(&startColor);
        
        cocos2d::ccColor4F endColor = {
            player->m_playerColor1.r / 255.0f,
            player->m_playerColor1.g / 255.0f,
            player->m_playerColor1.b / 255.0f,
            1.0f
        };
        m_particle->setEndColor(&endColor);
    }
    
    m_gradientBar = cocos2d::CCSprite::createWithSpriteFrameName("gradientBar.png");
    if (m_gradientBar) {
        GameManager::sharedState()->m_playLayer->m_gameBlendingLayerT1->addChild(m_gradientBar);
        m_gradientBar->setBlendFunc((cocos2d::ccBlendFunc){GL_SRC_ALPHA, GL_ONE});
        float scale = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraZoom;
        m_gradientBar->setScaleY((scale * 1.5f) / m_gradientBar->getTextureRect().size.height);
        
        int count = (int)llroundf((scale * 1.5f) / 30.0f);
        for (int i = 0; i < count; i++) {
            cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("square_02_001.png");
            if (sprite) {
                addChild(sprite);
                sprite->setRotation(-90.0f);
                float y = -((i * 30.0f) - ((count >> 1) * 30.0f));
                sprite->setPosition(cocos2d::CCPoint(0.0f, y));
            }
        }
    }
    
    m_unk370 = true;
    return true;
}

void EndPortalObject::setPosition(const cocos2d::CCPoint& position) {
    GameObject::setPosition(position);
    
    if (m_gradientBar) {
        float scale = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraZoom;
        float dir = m_flippedX ? -1.0f : 1.0f;
        cocos2d::CCPoint offset = cocos2d::CCPoint((-14.0f - (15.0f / scale)) * dir, 0.0f);
        m_gradientBar->setPosition(getPosition() + offset);
    }
    
    if (m_particle) {
        float dir = m_flippedX ? -1.0f : 1.0f;
        cocos2d::CCPoint pos = getPosition() + (m_particleOffset * dir);
        m_particle->setPosition(pos);
    }
}

void EndPortalObject::setVisible(bool visible) {
    bool wasVisible = isVisible();
    GameObject::setVisible(visible);
    if (!wasVisible && visible) {
        updateEndPos(true);
    }
}

void EndPortalObject::triggerObject(GJBaseGameLayer* layer) {
    if (!GameManager::sharedState()->m_playLayer->m_player1->m_isDead) {
        UILayer::disableMenu(GameManager::sharedState()->m_playLayer->m_uiLayer);
        cocos2d::CCPoint spawnPos = getSpawnPos();
        GameManager::sharedState()->m_playLayer->playEndAnimationToPos(spawnPos);
    }
}

void EndPortalObject::updateEndPos(bool updateParticle) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float y = 225.0f;
    float scale = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraZoom;
    float targetScale = GameManager::sharedState()->m_playLayer->m_gameState.m_targetCameraZoom;
    float height = winSize.height;
    
    if (m_startPosHeightRelated) {
        cocos2d::CCPoint startPos = getStartPos(winSize.width, winSize.height);
        if (startPos.y > 225.0f) {
            startPos = getStartPos(startPos.y);
            y = startPos.y;
        }
    } else {
        PlayerObject* player = GameManager::sharedState()->m_playLayer->m_player1;
        if (player->m_isShip) {
            float camY = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraPosition2.y;
            float halfHeight = (height * 0.5f) / scale;
            y = camY + halfHeight;
        } else {
            float camY = GameManager::sharedState()->m_playLayer->m_gameState.m_cameraPosition2.y;
            float halfHeight = (height * 0.5f) / scale;
            if (camY + halfHeight > 225.0f) {
                y = camY + halfHeight;
            }
        }
    }
    
    cocos2d::CCPoint pos = getPosition();
    pos.y = y;
    setStartPos(pos);
    
    if (m_particle) {
        if (updateParticle) {
            m_particle->update(1.0f);
        }
        float dir = m_flippedX ? -1.0f : 1.0f;
        m_particle->setScale(dir / targetScale);
    }
    
    float dir = m_flippedX ? -1.0f : 1.0f;
    setScaleX(dir / targetScale);
    setScaleY(1.0f / targetScale);
    
    if (m_gradientBar) {
        m_gradientBar->setScaleY(((height / targetScale) * 1.5f) / m_gradientBar->getTextureRect().size.height);
        m_gradientBar->setScaleX(m_flippedX ? -1.0f : 1.0f);
    }
    
    setPosition(getPosition());
}

EndPortalObject::~EndPortalObject() {
    // Destructor implementation
}