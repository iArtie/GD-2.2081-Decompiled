// GJGameLoadingLayer.cpp
#include "Layers/GJGameLoadingLayer.h"
#include "Models/GJGameLevel.h"
#include "Layers/PlayLayer.h"
#include "Layers/LevelEditorLayer.h"

GJGameLoadingLayer* GJGameLoadingLayer::create(GJGameLevel* level, bool editor) {
    GJGameLoadingLayer* instance = new GJGameLoadingLayer();
    if (instance && instance->init(level, editor)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void GJGameLoadingLayer::gameLayerDidUnload() {
    cocos2d::CCAction* action = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.01f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJGameLoadingLayer::loadLevel)),
        nullptr
    );
    runAction(action);
}

bool GJGameLoadingLayer::init(GJGameLevel* level, bool editor) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }
    
    if (level) {
        m_level = level;
        m_level->retain();
    }
    m_editor = editor;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create();
    label->setScale(0.7f);
    addChild(label, 1);
    label->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    label->setPosition(cocos2d::CCPoint(winSize.width - 20.0f, 20.0f));
    
    return true;
}

void GJGameLoadingLayer::loadLevel() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCScene* scene = nullptr;
    
    if (m_editor) {
        scene = LevelEditorLayer::scene(m_level, false);
    } else {
        scene = PlayLayer::scene(m_level, false, false);
    }
    
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.4f, scene);
    director->replaceScene(transition);
}

GJGameLoadingLayer* GJGameLoadingLayer::transitionToLoadingLayer(GJGameLevel* level, bool editor) {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    GJGameLoadingLayer* loadingLayer = GJGameLoadingLayer::create(level, editor);
    scene->addChild(loadingLayer);
    
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.1f, scene);
    director->replaceScene(transition);
    
    return loadingLayer;
}

GJGameLoadingLayer::~GJGameLoadingLayer() {
    if (m_level) {
        m_level->release();
    }
}