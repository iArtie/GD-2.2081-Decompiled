#include "GroupCommandObject2.h"
#include "GameToolbox.h"

static int s_nextGroupCommandId = 0;

GroupCommandObject2::GroupCommandObject2() {
    reset();
}

void GroupCommandObject2::reset() {
    m_groupCommandUniqueID = s_nextGroupCommandId++;
    m_easingType = 0;
    m_targetGroupID = 0;
    m_centerGroupID = 0;
    m_finished = false;
    m_disabled = false;
    m_finishRelated = false;
    m_lockToPlayerX = false;
    m_lockToPlayerY = false;
    m_lockToCameraX = false;
    m_lockToCameraY = false;
    m_lockedInX = false;
    m_lockedInY = false;
    m_easingRate = 0.0f;
    m_duration = 0.0;
    m_deltaTime = 0.0;
    m_currentXOffset = 0.0;
    m_currentYOffset = 0.0;
    m_deltaX = 0.0;
    m_deltaY = 0.0;
    m_oldDeltaX = 0.0;
    m_oldDeltaY = 0.0;
    m_lockedCurrentXOffset = 0.0;
    m_lockedCurrentYOffset = 0.0;
    m_moveModX = 1.0;
    m_lockObjectRotation = false;
    m_targetPlayer = false;
    m_commandType = 0;
    m_onlyMove = false;
    m_transformRelatedFalse = false;
    m_relativeRotation = false;
    m_followYOffset = 0.0;
    m_triggerUniqueID = 0;
    m_controlID = 0;
    m_actionType1 = 0;
    m_actionType2 = 0;
    m_currentRotateOrTransformValue = 0.0;
    m_currentRotateOrTransformDelta = 0.0;
    m_someInterpValue1RelatedOne = 0.0;
    m_someInterpValue2RelatedOne = 0.0;
    m_rotationOffset = 0.0;
    m_keyframeRelated = 0.0;
    m_targetScaleX = 0.0;
    m_targetScaleY = 0.0;
    m_transformTriggerProperty450 = 0.0;
    m_transformTriggerProperty451 = 0.0;
    m_someInterpValue1RelatedZero = 0.0;
    m_someInterpValue2RelatedZero = 0.0;
    m_someInterpValue1Related = 0.0;
    m_someInterpValue2Related = 0.0;
    m_followYSpeed = 0.0;
    m_followYDelay = 0.0;
    m_followYMaxSpeed = 0.0;
    m_moveModY = 1.0;
    m_followXMod = 1.0;
    m_followYMod = 1.0;
    m_someInterpValue1 = 1.0;
    m_someInterpValue2 = 1.0;
    m_deltaX_3 = 0.0;
    m_deltaY_3 = 0.0;
    m_oldDeltaX_3 = 0.0;
    m_someInterpValue1RelatedFalse = false;
    m_oldDeltaY_3 = 0.0;
    m_alreadyUpdated = true;
    m_Delta_3_Related = 0.0;
    m_unkDoubleMaybeUnused = 0.0;
    m_doUpdate = false;
    m_actionValue1 = 0.0;
    m_actionValue2 = 0.0;
    m_deltaTimeInFloat = 0.0;
    m_gameObject = nullptr;
    m_someInterpValue2RelatedTrue = false;
    m_unkInt204 = 0;
    m_splineRelated = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameObjectRotation = 0.0;
    
    m_keyframes.clear();
    m_remapKeys.clear();
    m_moveOffset = cocos2d::CCPoint(0.0f, 0.0f);
}

void GroupCommandObject2::resetDelta(bool intermediate) {
    m_oldDeltaX = m_deltaX;
    m_oldDeltaY = m_deltaY;
    m_oldDeltaX_3 = m_deltaX_3;
    m_oldDeltaY_3 = m_deltaY_3;
    m_deltaX = 0.0;
    m_deltaY = 0.0;
    m_currentRotateOrTransformDelta = 0.0;
    
    if (!intermediate) {
        m_deltaX_3 = 0.0;
        m_deltaY_3 = 0.0;
        m_Delta_3_Related = 0.0;
    }
}

void GroupCommandObject2::runFollowCommand(double xMod, double yMod, double duration) {
    m_commandType = 2;
    m_followXMod = xMod;
    m_followYMod = yMod;
    m_duration = duration;
    
    if (xMod == 0.0 && yMod == 0.0) {
        m_finished = true;
        m_finishRelated = true;
    }
}

void GroupCommandObject2::step(float dt) {
    if (m_disabled || m_finished) {
        return;
    }
    
    m_deltaTime += dt;
    m_deltaTimeInFloat += dt;
    
    if (!m_alreadyUpdated) {
        if (m_deltaTime >= m_duration && m_duration != -1.0) {
            m_finished = true;
        }
        return;
    }
    
    m_alreadyUpdated = false;
    
    if (m_actionType1) {
        updateAction(m_actionType1, m_actionValue1);
        if (m_actionType2) {
            updateAction(m_actionType2, m_actionValue2);
        }
    }
    
    if (m_deltaTime >= m_duration && m_duration != -1.0) {
        m_finished = true;
    }
}

void GroupCommandObject2::stepTransformCommand(float dt, bool intermediate, bool skipStep) {
    if (m_finishRelated) {
        m_someInterpValue1RelatedZero = m_someInterpValue1RelatedOne;
        m_someInterpValue2RelatedZero = m_someInterpValue2RelatedOne;
        m_someInterpValue2RelatedOne = m_someInterpValue1RelatedOne;
        return;
    }
    
    if (skipStep) {
        double newValue = m_someInterpValue1RelatedOne + m_currentRotateOrTransformDelta;
        m_someInterpValue1RelatedZero = m_someInterpValue1RelatedOne;
        m_someInterpValue1RelatedOne = newValue;
        m_someInterpValue2RelatedZero = m_someInterpValue2RelatedOne;
        if (!intermediate) {
            m_someInterpValue2RelatedOne = newValue;
        }
        return;
    }
    
    step(dt);
    
    double newValue = m_someInterpValue1RelatedOne + m_currentRotateOrTransformDelta;
    m_someInterpValue1RelatedZero = m_someInterpValue1RelatedOne;
    m_someInterpValue1RelatedOne = newValue;
    m_someInterpValue2RelatedZero = m_someInterpValue2RelatedOne;
    
    if (!intermediate) {
        m_someInterpValue2RelatedOne = newValue;
    }
}

void GroupCommandObject2::updateAction(int type, float value) {
    float progress;
    
    if (m_duration < 1.1929e-7f) {
        if (m_deltaTimeInFloat * 8388608.0f >= 1.0f) {
            progress = 1.0f;
        } else {
            progress = m_deltaTimeInFloat / 1.1929e-7f;
        }
    } else {
        if (m_deltaTimeInFloat / m_duration >= 1.0f) {
            progress = 1.0f;
        } else {
            progress = m_deltaTimeInFloat / m_duration;
        }
    }
    
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    
    float easedValue = GameToolbox::getEasedValue(progress, m_easingType, m_easingRate);
    float result = value - (1.0f - easedValue) * value;
    
    updateEffectAction(result, type);
}

void GroupCommandObject2::updateEffectAction(float value, int type) {
    switch (type) {
        case 1: {
            if (!m_lockedInX) {
                double delta = value - m_currentXOffset;
                m_currentXOffset = value;
                m_deltaX += delta;
                m_deltaX_3 += delta;
            }
            break;
        }
        case 2: {
            if (!m_lockedInY) {
                double delta = value - m_currentYOffset;
                m_currentYOffset = value;
                m_deltaY += delta;
                m_deltaY_3 += delta;
            }
            break;
        }
        case 3:
        case 4: {
            double delta = value - m_currentRotateOrTransformValue;
            m_currentRotateOrTransformValue = value;
            m_currentRotateOrTransformDelta += delta;
            break;
        }
        default:
            break;
    }
}