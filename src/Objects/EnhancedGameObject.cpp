#include "Objects/EnhancedGameObject.h"
#include "Objects/PlayerObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Managers/GameManager.h"
#include "Platform/cocos2d.h"
#include <cstdlib>

void EnhancedGameObject::activatedByPlayer(PlayerObject* player) {
    m_activated = true;
    if (!canMultiActivate(player->m_isPlatformer)) {
        if (player->m_uniqueID == 1)
            m_activatedByPlayer1 = true;
        else
            m_activatedByPlayer2 = true;
    }
}

void EnhancedGameObject::animationTriggered() {
    if (m_animateOnTrigger)
        triggerAnimation();
}

bool EnhancedGameObject::canAllowMultiActivate() {
    if (m_objectID == 660) return true;
    if (m_objectID > 660) {
        if (m_objectID == 1751) return true;
        if (m_objectID <= 1751) {
            if (m_objectID > 1333) {
                if (m_objectID == 1594 || m_objectID == 1704)
                    return true;
            } else if (m_objectID < 1330 && m_objectID != 747 && m_objectID != 1022 && m_objectID != 745) {
                return false;
            }
        } else {
            if (m_objectID <= 3005)
                return m_objectID >= 3004 || m_objectID == 2902 || m_objectID == 2926 || m_objectID == 1933;
            if (m_objectID == 3027 || m_objectID == 3643)
                return true;
            return false;
        }
        return true;
    }
    if (m_objectID == 84) return true;
    if (m_objectID > 84) {
        if (m_objectID <= 141)
            return m_objectID >= 140 || m_objectID == 101 || m_objectID == 111 || m_objectID == 99;
        return (m_objectID - 286) <= 1;
    } else {
        if (m_objectID <= 36)
            return m_objectID >= 35 || (m_objectID - 10) <= 3;
        if (m_objectID < 45) return false;
        return m_objectID <= 47 || m_objectID == 67;
    }
}

bool EnhancedGameObject::canMultiActivate(bool multiActivate) {
    if (multiActivate)
        return !m_isNoMultiActivate;
    return false;
}

EnhancedGameObject* EnhancedGameObject::create(const char* frame) {
    EnhancedGameObject* ret = new EnhancedGameObject();
    if (ret && ret->init(frame)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void EnhancedGameObject::createRotateAction(float angle, int clockwiseDirection) {
    if (angle != 0.0f)
        m_rotationAngle = angle;
    if (m_rotationSpeed != 0.0f) {
        m_rotationDelta = 1.0f * m_rotationSpeed;
        return;
    }
    if (clockwiseDirection) {
        m_rotationDelta = (float)clockwiseDirection * angle;
    } else {
        int randVal = rand();
        m_rotationSpeed = angle;
        if (((float)randVal * 4.6566e-10f) > 0.5f) {
            m_rotationDelta = 1.0f * m_rotationSpeed;
        } else {
            m_rotationDelta = -1.0f * angle;
        }
    }
}

void EnhancedGameObject::customSetup() {
    // Large switch statement for different object types
    if (m_objectID <= 1528) {
        if (m_objectID < 1521) {
            if (m_objectID > 680) {
                if (m_objectID > 1052) {
                    if (m_objectID <= 1061) {
                        if (m_objectID >= 1058) {
                            GameObject::setDefaultMainColorMode(1007);
                            m_objectType = 0; // Decoration
                            m_defaultZOrder = 9;
                            m_defaultZLayer = 1; // B1
                            if (!m_disableRotation) {
                                int randVal = rand();
                                float angle = (((float)randVal * 4.6566e-10f) * 90.0f) + 270.0f;
                                createRotateAction(angle, 0);
                            }
                        } else if (m_objectID > 1054) {
                            GameObject::setDefaultMainColorMode(1007);
                            m_usesAudioScale = true;
                            m_customAudioScale = true;
                            m_objectType = 0;
                            m_defaultZOrder = 9;
                            m_defaultZLayer = 1;
                            m_minAudioScale = 0.8f;
                            m_maxAudioScale = 1.2f;
                            if (!m_disableRotation) {
                                int randVal = rand();
                                float angle = (((float)randVal * 4.6566e-10f) * 90.0f) + 270.0f;
                                createRotateAction(angle, 0);
                            }
                        }
                    } else if (m_objectID == 1516) {
                        GameObject::setDefaultMainColorMode(1);
                        m_objectType = 0;
                        m_defaultZOrder = 9;
                        m_hasCustomAnimation = true;
                        m_defaultZLayer = 1;
                        m_colorZLayerRelated = true;
                    } else if (m_objectID > 1516) {
                        if ((m_objectID - 1518) <= 1) {
                            m_objectType = 0;
                            GameObject::setDefaultMainColorMode(1011);
                            m_hasCustomAnimation = true;
                            int randVal = rand();
                            m_animationRandomizedStart = 1;
                            m_randomFrameTime = (((float)randVal * 4.6566e-10f) * 0.4f) + 0.8f;
                            if (((float)rand() * 4.6566e-10f) <= 0.7f) {
                                *(float*)&m_animationRandomizedStartValue = ((float)rand() * 4.6566e-10f) * 0.5f;
                            } else {
                                m_unk548 = true;
                                m_animationStart = -1.0f;
                            }
                        }
                    } else if (m_objectID == 1330) {
                        m_objectType = 6; // DropRing
                        m_usesAudioScale = true;
                        m_defaultZOrder = 12;
                        m_defaultZLayer = 1;
                        m_spriteWidthScale = 1.2f;
                        m_spriteHeightScale = 1.2f;
                        GameObject::setDefaultMainColorMode(0);
                        if (!m_editorEnabled) {
                            cocos2d::CCParticleSystemQuad* particle = createAndAddParticle(m_objectType, "ringEffect.plist", 4, kCCPositionTypeGrouped);
                            if (particle) {
                                particle->setPositionType(kCCPositionTypeGrouped);
                                cocos2d::ccColor4F startColor = {1.0f, 1.0f, 1.0f, 1.0f};
                                particle->setStartColor(&startColor);
                                cocos2d::ccColor4F endColor = {0.0f, 0.0f, 0.0f, 1.0f};
                                particle->setEndColor(&endColor);
                            }
                        }
                    }
                } else if (m_objectID >= 1050) {
                    GameObject::setDefaultMainColorMode(1007);
                    m_objectType = 0;
                    m_defaultZOrder = 9;
                    m_defaultZLayer = 1;
                    m_hasCustomAnimation = true;
                } else if (m_objectID <= 924) {
                    if (m_objectID < 923) {
                        if (m_objectID < 740) {
                            // Default case
                        } else if (m_objectID <= 742) {
                            m_objectType = 1; // Hazard
                            m_defaultZOrder = 1;
                            m_isInvisibleBlock = true;
                            if (!m_disableRotation) {
                                float angle = (m_objectID == 740) ? 300.0f : 360.0f;
                                createRotateAction(angle, 0);
                            }
                            m_hasCustomRotation = true;
                        } else if ((m_objectID - 920) <= 1) {
                            m_objectType = 0;
                            GameObject::setDefaultMainColorMode(1011);
                            m_hasCustomAnimation = true;
                            int randVal = rand();
                            m_animationRandomizedStart = 1;
                            m_randomFrameTime = (((float)randVal * 4.6566e-10f) * 0.4f) + 0.8f;
                            if (((float)rand() * 4.6566e-10f) <= 0.7f) {
                                *(float*)&m_animationRandomizedStartValue = ((float)rand() * 4.6566e-10f) * 0.5f;
                            } else {
                                m_unk548 = true;
                                m_animationStart = -1.0f;
                            }
                        }
                    } else {
                        // m_objectID == 923 case
                        m_objectType = 0;
                        GameObject::setDefaultMainColorMode(1011);
                        m_hasCustomAnimation = true;
                        int randVal = rand();
                        m_animationRandomizedStart = 1;
                        m_randomFrameTime = (((float)randVal * 4.6566e-10f) * 0.4f) + 0.8f;
                        if (((float)rand() * 4.6566e-10f) <= 0.7f) {
                            *(float*)&m_animationRandomizedStartValue = ((float)rand() * 4.6566e-10f) * 0.5f;
                        } else {
                            m_unk548 = true;
                            m_animationStart = -1.0f;
                        }
                    }
                } else if (m_objectID <= 1021) {
                    if (m_objectID >= 1019) {
                        GameObject::setDefaultMainColorMode(1005);
                        m_objectType = 0;
                        float size = m_height + m_height;
                        m_defaultZOrder = 9;
                        m_defaultZLayer = 1;
                        m_updateCustomContentSize = true;
                        m_width = size;
                        m_height = size;
                        if (!m_disableRotation) {
                            int randVal = rand();
                            float angle = (((float)randVal * 4.6566e-10f) * 135.0f) + 45.0f;
                            createRotateAction(angle, 0);
                        }
                    } else if ((m_objectID - 997) <= 3) {
                        GameObject::setDefaultMainColorMode(1005);
                        m_objectType = 0;
                        m_defaultZOrder = 9;
                        m_defaultZLayer = 1;
                        float scale = ((m_objectID - 154) <= 2) ? 1.5f : 1.0f;
                        if (!m_disableRotation) {
                            int randVal = rand();
                            float angle = ((((float)randVal * 4.6566e-10f) * 90.0f) + 90.0f) * scale;
                            createRotateAction(angle, 0);
                        }
                    }
                } else if (m_objectID == 1022) {
                    m_objectType = 5; // GreenRing
                    m_usesAudioScale = true;
                    m_defaultZOrder = 12;
                    m_defaultZLayer = 1;
                    m_spriteWidthScale = 1.2f;
                    m_spriteHeightScale = 1.2f;
                    GameObject::setDefaultMainColorMode(0);
                    if (!m_editorEnabled) {
                        cocos2d::CCParticleSystemQuad* particle = createAndAddParticle(m_objectType, "ringEffect.plist", 4, kCCPositionTypeGrouped);
                        if (particle) {
                            particle->setPositionType(kCCPositionTypeGrouped);
                            cocos2d::ccColor4F startColor = {0.29412f, 1.0f, 0.29412f, 1.0f};
                            particle->setStartColor(&startColor);
                            cocos2d::ccColor4F endColor = {0.0f, 1.0f, 0.0f, 1.0f};
                            particle->setEndColor(&endColor);
                        }
                    }
                }
            } else {
                // Objects 678-680
                m_defaultZOrder = 1;
                m_objectType = 1;
                m_hasCustomRotation = true;
                // Additional logic...
            }
        } else {
            // Objects 1521-1528
            m_objectType = 0;
            m_defaultZOrder = 9;
            m_defaultZLayer = 1;
            // Various cases...
        }
    } else {
        // Objects above 1528
        // Additional logic...
    }
    
    GameObject::customSetup();
}

void EnhancedGameObject::deactivateObject(bool deactivate) {
    GameObject::deactivateObject(deactivate);
    if (m_isActivated && !m_isActivated && m_animateOnTrigger && m_hasCustomAnimation) {
        updateSyncedAnimation(0xFFFFFFFFLL, 0.0f);
        waitForAnimationTrigger();
    }
}

bool EnhancedGameObject::getHasRotateAction() {
    return m_hasCustomRotation;
}

bool EnhancedGameObject::getHasSyncedAnimation() {
    return m_hasCustomAnimation;
}

std::string EnhancedGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = GameObject::getSaveString(layer);
    char buffer[500];
    fmt::BasicWriter<char> writer(buffer, sizeof(buffer));
    writer << saveString.c_str();
    
    if (m_isMultiActivate) {
        writer << ',' << 'c' << ',' << m_isMultiActivate;
    }
    
    if (m_isNoMultiActivate) {
        writer << ',' << (char)0x1BC << ',' << m_isNoMultiActivate;
    }
    
    if (m_hasCustomRotation) {
        if (m_disableRotation) {
            writer << ',' << 'b' << ',' << 1;
        } else if (m_rotationSpeed != 0.0f) {
            writer << ',' << 'a' << ',' << m_rotationSpeed;
        }
    }
    
    if (m_hasCustomAnimation) {
        if (m_animationRandomizedStart) {
            writer << ',' << 'j' << ',' << m_animationRandomizedStart;
        }
        if (m_animateOnTrigger) {
            writer << ',' << '{' << ',' << m_animateOnTrigger;
        }
        if (m_animateOnlyWhenActive) {
            writer << ',' << (char)0xD6 << ',' << m_animateOnlyWhenActive;
        }
        if (m_animationSpeed != 0.0f) {
            writer << ',' << 'k' << ',' << m_animationSpeed;
        }
        if (m_animationShouldUseSpeed) {
            writer << ',' << 'z' << ',' << m_animationShouldUseSpeed;
        }
        if (m_disableDelayedLoop) {
            writer << ',' << '~' << ',' << m_disableDelayedLoop;
        }
        if (m_singleFrame) {
            writer << ',' << (char)0x1CE << ',' << m_singleFrame;
        }
        if (m_animationOffset) {
            writer << ',' << (char)0x250 << ',' << m_animationOffset;
        }
    }
    
    writer << '\0';
    return std::string(buffer);
}

bool EnhancedGameObject::hasBeenActivated() {
    return m_activatedByPlayer1 || m_activatedByPlayer2;
}

bool EnhancedGameObject::hasBeenActivatedByPlayer(PlayerObject* player) {
    if (!canMultiActivate(player->m_isPlatformer)) {
        if (player->m_uniqueID == 1)
            return m_activatedByPlayer1;
        else
            return m_activatedByPlayer2;
    }
    return false;
}

bool EnhancedGameObject::init(const char* frame) {
    if (!GameObject::init(frame))
        return false;
    
    m_bUnkBool2 = false;
    m_classType = 1; // Enhanced
    return true;
}

void EnhancedGameObject::powerOffObject() {
    m_state = 0;
    if (m_isRingPoweredOn)
        m_isRingPoweredOn = false;
}

void EnhancedGameObject::powerOnObject(int state) {
    m_state = state;
    m_poweredOn = true;
    m_isRingPoweredOn = true;
}

void EnhancedGameObject::previewAnimateOnTrigger() {
    m_animateOnTrigger = false;
    m_isDisabled2 = false;
    updateSyncedAnimation(0xFFFFFFFFLL, 0.0f);
    m_animateOnTrigger = true;
}

void EnhancedGameObject::refreshRotateAction() {
    createRotateAction(m_rotationAngle, 0);
}

void EnhancedGameObject::resetObject() {
    GameObject::resetObject();
    m_activatedByPlayer1 = false;
    m_activatedByPlayer2 = false;
    m_activated = false;
    m_poweredOn = false;
    m_state = 0;
    m_unk4C0 = -1;
    m_unk4C4 = -1;
    m_unk4C8 = -1;
    m_unk4CC = -1;
    m_unk544 = 0.0f;
    if (m_unk548)
        m_animationStart = -1.0f;
    m_unkAnimationInt = 0;
    m_animationRandomizedStartValue = 0;
    m_rotationAnimationSpeed = 0.0f;
    if (m_animateOnTrigger)
        waitForAnimationTrigger();
}

void EnhancedGameObject::resetSyncedAnimation() {
    m_unk544 = 0.0f;
    m_unkAnimationInt = 0;
    m_animationStart = m_unk548 ? -1.0f : 0.0f;
    updateSyncedAnimation(0xFFFFFFFFLL, -*(float*)&m_animationRandomizedStartValue);
}

void EnhancedGameObject::restoreObject() {
    GameObject::restoreObject();
    m_activatedByPlayer1 = false;
    m_activatedByPlayer2 = false;
}

void EnhancedGameObject::saveActiveColors() {
    GameObject::saveActiveColors();
    if (!m_animationRandomizedStart) {
        m_animationRandomizedStartValue = 0;
        m_animationStart = 0.0f;
        if (m_hasCustomAnimation)
            setupAnimationVariables();
    } else {
        if (*(float*)&m_animationRandomizedStartValue <= 0.0f && !m_unk548)
            *(float*)&m_animationRandomizedStartValue = (float)rand() * 4.6566e-10f;
        if (m_hasCustomAnimation)
            setupAnimationVariables();
    }
}

void EnhancedGameObject::setupAnimationVariables() {
    if (m_objectID != 2065) {
        m_frames = GameManager::sharedState()->framesForAnimation(m_objectID);
        m_frameTime = GameManager::sharedState()->frameTimeForAnimation(m_objectID);
    }
    
    if (m_animationShouldUseSpeed && m_animationSpeed != 0.0f)
        m_randomFrameTime = fabsf(m_animationSpeed);
    
    m_frameTime = m_frameTime / m_randomFrameTime;
    
    bool usesFreeze = GameObject::usesFreezeAnimation();
    if (usesFreeze && !m_disableDelayedLoop) {
        m_isDisabled2 = false;
        usesFreeze = false;
    }
    
    m_visible = usesFreeze;
    m_shouldNotHideAnimFreeze = GameObject::shouldNotHideAnimFreeze();
    m_usesSpecialAnimation = GameObject::usesSpecialAnimation();
}

void EnhancedGameObject::stateSensitiveOff(GJBaseGameLayer* layer) {
    // Empty implementation
}

void EnhancedGameObject::triggerActivated(float xPosition) {
    m_activatedByPlayer1 = true;
    m_activatedByPlayer2 = true;
}

void EnhancedGameObject::triggerAnimation() {
    if (!m_animateOnlyWhenActive || m_isActivated) {
        m_isDisabled2 = false;
        m_animationTriggered = true;
        m_maybeAnimationVariableXInt = -1;
        updateSyncedAnimation(0xFFFFFFFFLL, 0.0f);
        m_animationRandomizedStartValue = 0;
    }
}

void EnhancedGameObject::updateAnimateOnTrigger(bool animate) {
    if (m_hasCustomAnimation && m_animateOnTrigger) {
        if (animate) {
            m_isDisabled2 = false;
            setOpacity(0xFFFFFFFF);
            m_animateOnTrigger = false;
            m_maybeAnimationVariableXInt = -1;
            int defaultFrame = GameManager::sharedState()->defaultFrameForAnimation(m_objectID);
            updateSyncedAnimation(defaultFrame, 0.0f);
            m_animateOnTrigger = true;
        } else {
            waitForAnimationTrigger();
        }
    }
}

void EnhancedGameObject::updateRotateAction(float dt) {
    if (!m_disableRotation) {
        m_rotationAnimationSpeed += dt;
        float angle = m_rotationAnimationSpeed * m_rotationDelta;
        
        // Update rotation on all children
        for (int i = 0; i < 3; i++) {
            cocos2d::CCNode* node = nullptr;
            if (i == 0) {
                node = this;
                if (m_colorSpriteLocked || !m_colorSprite)
                    continue;
                node = m_colorSprite;
            } else if (i == 1) {
                node = m_glowSprite;
            } else {
                node = this;
            }
            
            if (node) {
                cocos2d::CCArray* children = node->getChildren();
                if (children) {
                    for (int j = 0; j < children->count(); j++) {
                        cocos2d::CCNode* child = (cocos2d::CCNode*)children->objectAtIndex(j);
                        if (child) {
                            float currentAngle = child->getRotation();
                            child->setRotation(currentAngle + angle);
                        }
                    }
                }
            }
        }
    }
}

void EnhancedGameObject::updateState(int state) {
    if (m_state < state) {
        m_poweredOn = false;
        powerOffObject();
    } else if (!m_poweredOn) {
        powerOffObject();
    }
}

void EnhancedGameObject::updateUserCoin() {
    if (!m_editorEnabled) {
        bool hasCoin = GJBaseGameLayer::hasUniqueCoin(GameManager::sharedState()->m_playLayer, (EffectGameObject*)this);
        m_hasUniqueCoin = hasCoin;
        if (!hasCoin) {
            cocos2d::CCParticleSystemQuad* particle = createAndAddParticle(m_objectType, "coinEffect.plist", 0, kCCPositionTypeGrouped);
            if (particle && m_objectID != 142) {
                cocos2d::ccColor4F startColor = {1.0f, 1.0f, 1.0f, particle->getStartColor()->a};
                particle->setStartColor(&startColor);
                cocos2d::ccColor4F endColor = {0.5f, 0.5f, 0.5f, particle->getEndColor()->a};
                particle->setEndColor(&endColor);
            }
            m_particleOffset = cocos2d::CCPoint(0.0f, -19.0f);
        }
    }
}

void EnhancedGameObject::waitForAnimationTrigger() {
    m_animationTriggered = false;
    m_isDisabled2 = true;
    m_unk544 = 0.0f;
    m_unk540 = 0.0f;
    m_animationStart = 0.0f;
    m_animationRandomizedStartValue = 0;
    setOpacity(0);
}

EnhancedGameObject::~EnhancedGameObject() {
    // Base class destructor handles cleanup
}