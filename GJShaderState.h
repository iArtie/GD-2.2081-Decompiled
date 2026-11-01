#pragma once

#include "cocos2d.h"
#include <unordered_map>

class GJValueTween;

class GJShaderState {
public:
    void reset();
    void stopTweenAction(int action);
    void timesyncShaderAction(int action);
    void timesyncShaderActions();
    void updateTweenAction(float value, int actionID);
    void updateTweenActions(float tweenValue);

private:
    float m_chromaticTargetX;
    float m_chromaticTargetY;
    float m_cGRGBOffset;
    float m_cGStrength;
    float m_cGLineThickness;
    float m_cGLineStrength;
    float m_cGSegmentHeight;
    float m_cGSpeed;
    float m_pixelateTargetX;
    float m_pixelateTargetY;
    float m_lensCircleSize;
    float m_lensCircleFade;
    float m_lensCircleStrength;
    cocos2d::CCPoint m_lensCircleScreenOffset;
    float m_radialBlurSize;
    cocos2d::CCPoint m_radialBlurScreenOffset;
    float m_motionBlurTargetX;
    float m_motionBlurTargetY;
    float m_bulgeValue;
    cocos2d::CCPoint m_bulgeScreenOffset;
    float m_bulgeRadius;
    float m_pinchTargetX;
    float m_pinchTargetY;
    cocos2d::CCPoint m_pinchScreenOffset;
    float m_pinchRadius;
    cocos2d::CCPoint m_pinchModifier;
    float m_grayscaleValue;
    float m_sepiaValue;
    float m_invertColorEditRGB;
    float m_invertColorR;
    float m_invertColorG;
    float m_invertColorB;
    float m_hueShiftDegrees;
    float m_colorChangeCR;
    float m_colorChangeCG;
    float m_colorChangeCB;
    float m_colorChangeBR;
    float m_colorChangeBG;
    float m_colorChangeBB;
    float m_splitTargetRows;
    float m_splitTargetCols;
    float m_blurIntensity;
    float m_radialBlurFade;
    float m_glitchStrength;
    float m_shockWaveSpeed;
    float m_shockWaveThickness;
    float m_shockWaveStrength;
    float m_shockWaveWidth;
    float m_shockWaveFadeIn;
    float m_shockWaveFadeOut;
    float m_shockWaveInner;
    cocos2d::CCPoint m_shockWaveScreenOffset;
    float m_shockWaveOuter;
    float m_shockWaveMaxSize;
    float m_shockWaveTimeOffset;
    float m_shockLineSpeed;
    float m_shockLineThickness;
    float m_shockLineStrength;
    float m_shockLineWidth;
    float m_shockLineFadeIn;
    float m_shockLineFadeOut;
    float m_shockLineScreenOffset;
    float m_shockLineMaxSize;
    float m_shockLineTimeOffset;
    std::unordered_map<int, GJValueTween> m_tweenActions;
    std::unordered_map<int, float> m_tweenTimes;
    bool m_cGEnable;
    bool m_pixelateSnapGrid;
    bool m_lensCircleTargetID;
    bool m_lensCircleTintChannel;
    bool m_motionBlurTargetIDX;
    bool m_motionBlurTargetIDY;
    float m_shockWaveStartTime;
    float m_shockLineStartTime;
    float m_textureScaleX;
    float m_textureScaleY;
    float m_motionBlurSpeedX;
    float m_motionBlurSpeedY;
    cocos2d::CCPoint m_radialBlurCenter;
    bool m_pinchTargetIDX;
    bool m_pinchTargetIDY;
    bool m_pinchTargetEnabledX;
    bool m_pinchTargetEnabledY;
    bool m_invertColorClampRGB;
    bool m_noPlayerParticles;
    int m_minBlendingLayer;
    int m_maxBlendingLayer;
    bool m_zLayerDirty;
    float m_prevTime;
    float m_time;
};