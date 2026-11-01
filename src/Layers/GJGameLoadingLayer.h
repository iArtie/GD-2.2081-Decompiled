// GJGameLoadingLayer.h
#pragma once

#include "Platform/cocos2d.h"

class GJGameLevel;
class PlayLayer;
class LevelEditorLayer;

class GJGameLoadingLayer : public cocos2d::CCLayer {
public:
    static GJGameLoadingLayer* create(GJGameLevel* level, bool editor);
    void gameLayerDidUnload();
    bool init(GJGameLevel* level, bool editor);
    void loadLevel();
    static GJGameLoadingLayer* transitionToLoadingLayer(GJGameLevel* level, bool editor);
    virtual ~GJGameLoadingLayer();

private:
    GJGameLevel* m_level;
    bool m_editor;
};  