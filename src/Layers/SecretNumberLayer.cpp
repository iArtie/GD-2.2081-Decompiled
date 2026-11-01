#include "Layers/SecretNumberLayer.h"

SecretNumberLayer* SecretNumberLayer::create() {
    SecretNumberLayer* layer = new SecretNumberLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void SecretNumberLayer::playNumberEffect(int secretNumber) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    std::string numberStr = cocos2d::CCString::createWithFormat("%i", secretNumber)->getCString();

    int currentNumber = (int)((float)rand() * 4.6566e-10f * 100.0f);
    int digitIndex = 0;
    int digitCount = 0;
    int usedSingle = 0;
    int usedDouble = 0;

    for (int i = 0; i < 6; i++) {
        std::string labelStr = cocos2d::CCString::createWithFormat("%i", currentNumber)->getCString();
        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(labelStr.c_str(), "bigFont.fnt");
        addChild(label);
        label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 94.0f));
        label->setVisible(false);
        label->setOpacity(0);
        label->setScale(3.0f);

        cocos2d::CCShow* show = cocos2d::CCShow::create();
        cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create((float)i * 0.55f);
        cocos2d::CCFadeIn* fadeIn = cocos2d::CCFadeIn::create(0.1f);
        cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.2f, 1.0f);
        cocos2d::CCSpawn* spawn = cocos2d::CCSpawn::createWithTwoActions(fadeIn, scaleTo);
        cocos2d::CCDelayTime* delay2 = cocos2d::CCDelayTime::create(0.2f);
        float fadeOutDuration = (i == 5) ? 0.5f : 0.25f;
        cocos2d::CCFadeOut* fadeOut = cocos2d::CCFadeOut::create(fadeOutDuration);
        cocos2d::CCCallFunc* removeFunc = cocos2d::CCCallFunc::create(label, callfunc_selector(cocos2d::CCNode::removeFromParent));

        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            delay, show, spawn, delay2, fadeOut, removeFunc, nullptr);

        if (i == 5) {
            cocos2d::CCCallFunc* doneFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(cocos2d::CCNode::removeFromParent));
            seq = cocos2d::CCSequence::create(seq, doneFunc, nullptr);
            label->setColor(ccc3(150, 255, 255));
        }

        cocos2d::CCNode::runAction(label, seq);

        if (i == 5) {
            break;
        }

        int digitLength;
        if (usedSingle <= 2 && (usedDouble > 1 || (float)rand() * 4.6566e-10f > 0.5f)) {
            digitLength = 1;
            usedSingle++;
        } else {
            digitLength = 2;
            usedDouble++;
        }

        std::string digitSubstr = numberStr.substr(digitIndex, digitLength);
        digitIndex += digitLength;
        currentNumber += atoi(digitSubstr.c_str());
    }
}

SecretNumberLayer::~SecretNumberLayer() {
}