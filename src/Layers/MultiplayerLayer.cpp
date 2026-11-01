#include "Layers/MultiplayerLayer.h"
#include "Layers/CreatorLayer.h"
#include "Layers/MPLobbyLayer.h"

MultiplayerLayer* MultiplayerLayer::create()
{
    MultiplayerLayer* ret = new MultiplayerLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MultiplayerLayer::keyBackClicked()
{
    onBack(nullptr);
}

void MultiplayerLayer::onBack(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = CreatorLayer::scene();
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MultiplayerLayer::onBtn1(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = MPLobbyLayer::scene(0);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MultiplayerLayer::onBtn2(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = MPLobbyLayer::scene(-1);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

cocos2d::CCScene* MultiplayerLayer::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    MultiplayerLayer* layer = MultiplayerLayer::create();
    scene->addChild(layer);
    return scene;
}

MultiplayerLayer::~MultiplayerLayer()
{
}