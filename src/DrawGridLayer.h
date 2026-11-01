#pragma once

#include "cocos2d.h"
#include <string>
#include <unordered_map>
#include <array>

class LevelEditorLayer;
class EffectGameObject;
class GameObject;
class AudioLineGuideGameObject;
class PlayerObject;

class DrawGridLayer : public cocos2d::CCLayer {
public:
    static DrawGridLayer* create(cocos2d::CCNode* parent, LevelEditorLayer* layer);
    bool init(cocos2d::CCNode* parent, LevelEditorLayer* layer);
    
    void draw() override;
    void update(float dt) override;
    
    void addAudioLineObject(AudioLineGuideGameObject* object);
    void addToEffects(EffectGameObject* object);
    void addToGuides(GameObject* object);
    void addToSpeedObjects(EffectGameObject* object);
    void removeAudioLineObject(AudioLineGuideGameObject* object);
    void removeFromEffects(EffectGameObject* object);
    void removeFromGuides(GameObject* object);
    void removeFromSpeedObjects(EffectGameObject* object);
    
    cocos2d::CCPoint getPortalMinMax(GameObject* object);
    cocos2d::CCPoint posForTime(float time);
    void loadTimeMarkers(std::string markers);
    void postUpdate();
    void sortSpeedObjects();
    void updateMusicGuideTime(float time);
    void updateTimeMarkers();
    
    virtual ~DrawGridLayer();

protected:
    LevelEditorLayer* m_editorLayer;
    cocos2d::CCNode* m_objectLayer;
    
    cocos2d::CCArray* m_effectGameObjects;
    cocos2d::CCArray* m_guideObjects;
    cocos2d::CCArray* m_speedObjects;
    cocos2d::CCArray* m_timeMarkers;
    std::string m_timeMarkerString;
    
    std::unordered_map<int, AudioLineGuideGameObject*> m_audioLineObjects;
    
    std::array<cocos2d::CCPoint, 400> m_pointArray1;
    std::array<cocos2d::CCPoint, 400> m_pointArray2;
    std::array<cocos2d::CCPoint, 400> m_pointArray3;
    
    float m_slowSpeed;
    float m_normalSpeed;
    float m_currentSpeed;
    float m_fastSpeed;
    float m_fasterSpeed;
    float m_fastestSpeed;
    float m_gridSize;
    float m_musicTime;
    float m_playbackX;
    float m_playbackY;
    float m_playbackTime;
    float m_oldPlaybackTime;
    
    bool m_sortEffects;
    bool m_updateSpeedObjects;
    bool m_updateTimeMarkers;
};