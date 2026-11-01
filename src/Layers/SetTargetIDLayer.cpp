#include "Layers/SetTargetIDLayer.h"
#include "Objects/EffectGameObject.h"

bool SetTargetIDLayer::init(EffectGameObject* object, cocos2d::CCArray* objects, int minID, int maxID) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 260.0f, 2)) {
        return false;
    }

    m_minimumID = minID;
    m_maximumID = maxID;

    return true;
}

void SetTargetIDLayer::valueDidChange(int tag, float value) {
    if (tag == 51) {
        float clampedValue = value;
        if (clampedValue < (float)m_minimumID) {
            clampedValue = (float)m_minimumID;
        } else if (clampedValue > (float)m_maximumID) {
            clampedValue = (float)m_maximumID;
        }

        if (clampedValue != value) {
            SetupTriggerPopup::updateValue(51, clampedValue);
        } else {
            SetupTriggerPopup::updateEditorLabel();
        }
    }
}

SetTargetIDLayer::~SetTargetIDLayer() {
}