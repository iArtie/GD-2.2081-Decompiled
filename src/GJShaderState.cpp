#include "GJShaderState.h"
#include "GJValueTween.h"

void GJShaderState::reset() {
    m_tweenActions.clear();
    m_tweenTimes.clear();
    
    m_cGEnable = false;
    m_pixelateSnapGrid = false;
    m_lensCircleTargetID = false;
    m_lensCircleTintChannel = false;
    m_motionBlurTargetIDX = false;
    m_motionBlurTargetIDY = false;
    m_shockWaveStartTime = 0.0f;
    m_shockLineStartTime = 0.0f;
    
    m_glitchStrength = 0.0f;
    m_chromaticTargetX = 0.0f;
    m_chromaticTargetY = 0.0f;
    m_blurIntensity = 0.0f;
    m_cGSpeed = 0.0f;
    m_cGSegmentHeight = 1.0f;
    m_cGStrength = 0.0f;
    m_cGLineThickness = 0.0f;
    m_cGLineStrength = 0.0f;
    m_cGRGBOffset = 0.0f;
    m_pixelateTargetX = 0.0f;
    m_pixelateTargetY = 0.0f;
    m_textureScaleX = 1.0f;
    m_textureScaleY = 1.0f;
    m_lensCircleSize = 1.0f;
    m_lensCircleFade = 0.0f;
    m_lensCircleStrength = 0.0f;
    m_radialBlurSize = 0.0f;
    m_motionBlurTargetX = 0.0f;
    m_motionBlurTargetY = 0.0f;
    m_motionBlurSpeedX = 0.0f;
    m_motionBlurSpeedY = 0.0f;
    m_radialBlurCenter = cocos2d::CCPoint(0.0f, 0.0f);
    m_pinchTargetIDX = false;
    m_pinchTargetIDY = false;
    m_pinchTargetEnabledX = false;
    m_pinchTargetEnabledY = false;
    m_bulgeValue = 0.0f;
    m_pinchTargetX = 0.0f;
    m_pinchTargetY = 0.0f;
    m_pinchScreenOffset = cocos2d::CCPoint(0.0f, 0.0f);
    m_invertColorClampRGB = false;
    m_noPlayerParticles = false;
    m_minBlendingLayer = 0;
    m_maxBlendingLayer = 0;
    m_zLayerDirty = false;
    m_grayscaleValue = 0.0f;
    m_sepiaValue = 0.0f;
    m_invertColorEditRGB = 0.0f;
    m_hueShiftDegrees = 0.0f;
    m_invertColorR = 1.0f;
    m_invertColorG = 1.0f;
    m_invertColorB = 1.0f;
    m_colorChangeCR = 1.0f;
    m_colorChangeCG = 1.0f;
    m_colorChangeCB = 1.0f;
    m_colorChangeBR = 0.0f;
    m_colorChangeBG = 0.0f;
    m_colorChangeBB = 0.0f;
    m_splitTargetRows = 0.0f;
    m_splitTargetCols = 0.0f;
    m_prevTime = -1.0f;
}

void GJShaderState::stopTweenAction(int action) {
    auto it = m_tweenActions.find(action);
    if (it != m_tweenActions.end()) {
        m_tweenActions.erase(it);
    }
}

void GJShaderState::timesyncShaderAction(int action) {
    auto it = m_tweenActions.find(action);
    if (it != m_tweenActions.end()) {
        auto timeIt = m_tweenTimes.find(action);
        if (timeIt != m_tweenTimes.end()) {
            float elapsed = m_time - timeIt->second;
            it->second.step(elapsed);
            updateTweenAction(it->second.m_currentValue, action);
        }
    }
}

void GJShaderState::timesyncShaderActions() {
    for (auto& pair : m_tweenTimes) {
        timesyncShaderAction(pair.first);
    }
}

void GJShaderState::updateTweenAction(float value, int actionID) {
    switch (actionID) {
        case 1: m_chromaticTargetX = value; break;
        case 2: m_chromaticTargetY = value; break;
        case 3: m_cGRGBOffset = value; break;
        case 4: m_cGStrength = value; break;
        case 5: m_cGLineThickness = value; break;
        case 6: m_cGLineStrength = value; break;
        case 7: m_cGSegmentHeight = value; break;
        case 8: m_cGSpeed = value; break;
        case 9: m_pixelateTargetX = value; break;
        case 10: m_pixelateTargetY = value; break;
        case 11: m_lensCircleSize = value; break;
        case 12: m_lensCircleFade = value; break;
        case 13: m_lensCircleStrength = value; break;
        case 14: m_lensCircleScreenOffset.x = value; break;
        case 15: m_lensCircleScreenOffset.y = value; break;
        case 16: m_radialBlurSize = value; break;
        case 17: m_radialBlurScreenOffset.x = value; break;
        case 18: m_radialBlurScreenOffset.y = value; break;
        case 19: m_motionBlurTargetX = value; break;
        case 20: m_motionBlurTargetY = value; break;
        case 21: m_bulgeValue = value; break;
        case 22: m_bulgeScreenOffset.x = value; break;
        case 23: m_bulgeScreenOffset.y = value; break;
        case 24: m_bulgeRadius = value; break;
        case 25: m_pinchTargetX = value; break;
        case 26: m_pinchTargetY = value; break;
        case 27: m_pinchScreenOffset.x = value; break;
        case 28: m_pinchScreenOffset.y = value; break;
        case 29: m_pinchRadius = value; break;
        case 30: m_pinchModifier.x = value; break;
        case 31: m_pinchModifier.y = value; break;
        case 32: m_grayscaleValue = value; break;
        case 33: m_sepiaValue = value; break;
        case 34: m_invertColorEditRGB = value; break;
        case 35: m_invertColorR = value; break;
        case 36: m_invertColorG = value; break;
        case 37: m_invertColorB = value; break;
        case 38: m_hueShiftDegrees = value; break;
        case 39: m_colorChangeCR = value; break;
        case 40: m_colorChangeCG = value; break;
        case 41: m_colorChangeCB = value; break;
        case 42: m_colorChangeBR = value; break;
        case 43: m_colorChangeBG = value; break;
        case 44: m_colorChangeBB = value; break;
        case 45: m_splitTargetRows = value; break;
        case 46: m_splitTargetCols = value; break;
        case 47: m_blurIntensity = value; break;
        case 48: m_radialBlurFade = value; break;
        case 49: m_glitchStrength = value; break;
        case 50: m_shockWaveSpeed = value; break;
        case 51: m_shockWaveThickness = value; break;
        case 52: m_shockWaveStrength = value; break;
        case 53: m_shockWaveWidth = value; break;
        case 54: m_shockWaveFadeIn = value; break;
        case 55: m_shockWaveFadeOut = value; break;
        case 56: m_shockWaveInner = value; break;
        case 57: m_shockWaveScreenOffset.x = value; break;
        case 58: m_shockWaveScreenOffset.y = value; break;
        case 59: m_shockWaveOuter = value; break;
        case 60: m_shockWaveMaxSize = value; break;
        case 61: m_shockWaveTimeOffset = value; break;
        case 62: m_shockLineSpeed = value; break;
        case 63: m_shockLineThickness = value; break;
        case 64: m_shockLineStrength = value; break;
        case 65: m_shockLineWidth = value; break;
        case 66: m_shockLineFadeIn = value; break;
        case 67: m_shockLineFadeOut = value; break;
        case 68: m_shockLineScreenOffset = value; break;
        case 69: m_shockLineMaxSize = value; break;
        case 70: m_shockLineTimeOffset = value; break;
        default: break;
    }
}

void GJShaderState::updateTweenActions(float tweenValue) {
    for (auto it = m_tweenActions.begin(); it != m_tweenActions.end(); ) {
        it->second.step(tweenValue);
        updateTweenAction(it->second.m_currentValue, it->first);
        
        if (it->second.m_finished) {
            it = m_tweenActions.erase(it);
        } else {
            ++it;
        }
    }
}