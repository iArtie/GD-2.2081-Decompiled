#pragma once

#include "cocos2d.h"
#include <vector>

struct Spline {
    std::vector<double> m_x;
    std::vector<double> m_y;
    std::vector<double> m_b;
    std::vector<double> m_c;
    std::vector<double> m_d;
};

struct KeyframeObject {
    Spline m_spline1;
    Spline m_spline2;
};

class GroupCommandObject2 {
public:
    GroupCommandObject2();
    void reset();
    void resetDelta(bool intermediate);
    void runFollowCommand(double xMod, double yMod, double duration);
    void step(float dt);
    void stepTransformCommand(float dt, bool intermediate, bool skipStep);
    void updateAction(int type, float value);
    void updateEffectAction(float value, int type);

private:
    cocos2d::CCPoint m_moveOffset;
    std::vector<KeyframeObject> m_keyframes;
    cocos2d::CCPoint m_splineRelated;
    std::vector<int> m_remapKeys;
    int m_groupCommandUniqueID;
    int m_easingType;
    int m_targetGroupID;
    int m_centerGroupID;
    bool m_finished;
    bool m_disabled;
    bool m_finishRelated;
    bool m_lockToPlayerX;
    bool m_lockToPlayerY;
    bool m_lockToCameraX;
    bool m_lockToCameraY;
    bool m_lockedInX;
    bool m_lockedInY;
    float m_easingRate;
    double m_duration;
    double m_deltaTime;
    double m_currentXOffset;
    double m_currentYOffset;
    double m_deltaX;
    double m_deltaY;
    double m_oldDeltaX;
    double m_oldDeltaY;
    double m_lockedCurrentXOffset;
    double m_lockedCurrentYOffset;
    double m_moveModX;
    bool m_lockObjectRotation;
    bool m_targetPlayer;
    int m_commandType;
    bool m_onlyMove;
    bool m_transformRelatedFalse;
    bool m_relativeRotation;
    double m_followYOffset;
    int m_triggerUniqueID;
    int m_controlID;
    int m_actionType1;
    int m_actionType2;
    double m_currentRotateOrTransformValue;
    double m_currentRotateOrTransformDelta;
    double m_someInterpValue1RelatedOne;
    double m_someInterpValue2RelatedOne;
    double m_rotationOffset;
    double m_keyframeRelated;
    double m_targetScaleX;
    double m_targetScaleY;
    double m_transformTriggerProperty450;
    double m_transformTriggerProperty451;
    double m_someInterpValue1RelatedZero;
    double m_someInterpValue2RelatedZero;
    double m_someInterpValue1Related;
    double m_someInterpValue2Related;
    double m_followYSpeed;
    double m_followYDelay;
    double m_followYMaxSpeed;
    double m_moveModY;
    double m_followXMod;
    double m_followYMod;
    double m_someInterpValue1;
    double m_someInterpValue2;
    double m_deltaX_3;
    double m_deltaY_3;
    double m_oldDeltaX_3;
    bool m_someInterpValue1RelatedFalse;
    double m_oldDeltaY_3;
    bool m_alreadyUpdated;
    double m_Delta_3_Related;
    double m_unkDoubleMaybeUnused;
    bool m_doUpdate;
    double m_actionValue1;
    double m_actionValue2;
    double m_deltaTimeInFloat;
    void* m_gameObject;
    bool m_someInterpValue2RelatedTrue;
    int m_unkInt204;
    double m_gameObjectRotation;
};