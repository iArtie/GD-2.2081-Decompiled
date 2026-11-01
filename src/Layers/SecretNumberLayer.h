#pragma once

#include "Platform/cocos2d.h"

class SecretNumberLayer : public cocos2d::CCLayer {
public:
    static SecretNumberLayer* create();
    bool init() override;
    void playNumberEffect(int secretNumber);

    ~SecretNumberLayer();
};