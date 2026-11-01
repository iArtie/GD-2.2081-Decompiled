#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"
#include "ColorSelectDelegate.h"
#include "SliderDelegate.h"
#include <string>

class CreateParticlePopup : public FLAlertLayer, public TextInputDelegate, public ColorSelectDelegate, public SliderDelegate {
public:
    static CreateParticlePopup* create(std::string str);
    static CreateParticlePopup* create(ParticleGameObject* object, cocos2d::CCArray* objects);
    static CreateParticlePopup* create(ParticleGameObject* object, cocos2d::CCArray* objects, std::string str);
    
    bool init(ParticleGameObject* object, cocos2d::CCArray* objects, std::string str);
    
    virtual void update(float dt) override;
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void keyBackClicked() override;
    virtual void sliderBegan(Slider* slider) override;
    virtual void sliderEnded(Slider* slider) override;
    virtual void colorSelectClosed(cocos2d::CCNode* popup) override;
    virtual void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    virtual void textInputReturn(CCTextInputNode* node) override;
    virtual void textInputClosed(CCTextInputNode* node) override;
    virtual void textChanged(CCTextInputNode* node) override;
    
    void centerAlignParticle(cocos2d::CCObject* sender);
    void createParticleSlider(gjParticleValue value, int page, bool centerLabel, cocos2d::CCPoint position, cocos2d::CCArray* displayNodes);
    cocos2d::CCNode* getPage(int page);
    CCMenuItemSpriteExtra* getPageButton(int page);
    cocos2d::CCArray* getPageContainer(int page);
    cocos2d::CCDictionary* getPageInputNodes(int page);
    cocos2d::CCMenu* getPageMenu(int page);
    cocos2d::CCDictionary* getPageSliders(int page);
    float maxSliderValueForType(gjParticleValue type);
    float minSliderValueForType(gjParticleValue type);
    void onAnimateActiveOnly(cocos2d::CCObject* sender);
    void onAnimateOnTrigger(cocos2d::CCObject* sender);
    void onCalcEmission(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCopySettings(cocos2d::CCObject* sender);
    void onDuplicateColor(cocos2d::CCObject* sender);
    void onDurationForever(cocos2d::CCObject* sender);
    void onDynamicColor(cocos2d::CCObject* sender);
    void onDynamicRotation(cocos2d::CCObject* sender);
    void onEmitterMode(cocos2d::CCObject* sender);
    void onEndRGBVarSync(cocos2d::CCObject* sender);
    void onMaxEmission(cocos2d::CCObject* sender);
    void onMode(cocos2d::CCObject* sender);
    void onOrderSensitive(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void onPasteSettings(cocos2d::CCObject* sender);
    void onPosType(cocos2d::CCObject* sender);
    void onQuickStart(cocos2d::CCObject* sender);
    void onSelectColor(cocos2d::CCObject* sender);
    void onSelectParticleTexture(cocos2d::CCObject* sender);
    void onStartRGBVarSync(cocos2d::CCObject* sender);
    void onToggleBlending(cocos2d::CCObject* sender);
    void onTogglePerfLog(cocos2d::CCObject* sender);
    void onToggleStartRadiusEqualToEnd(cocos2d::CCObject* sender);
    void onToggleStartRotationIsDir(cocos2d::CCObject* sender);
    void onToggleStartSizeEqualToEnd(cocos2d::CCObject* sender);
    void onToggleStartSpinEqualToEnd(cocos2d::CCObject* sender);
    void onUniformColor(cocos2d::CCObject* sender);
    bool particleValueIsInt(gjParticleValue type);
    void sliderChanged(cocos2d::CCObject* sender);
    std::string titleForParticleValue(gjParticleValue type);
    void toggleGravityMode(bool gravityMode);
    void updateColorSprite(int tag);
    void updateInputNodeStringForType(gjParticleValue type);
    void updateParticleValueForType(float value, gjParticleValue type, cocos2d::CCParticleSystemQuad* particle);
    void updateSliderForType(gjParticleValue type);
    float valueForParticleValue(gjParticleValue type);
    void willClose();
    
    virtual ~CreateParticlePopup();

protected:
    ParticleGameObject* m_targetObject;
    cocos2d::CCArray* m_targetObjects;
    cocos2d::CCArray* m_particles;
    bool m_disableTextDelegate;
    cocos2d::CCParticleSystemQuad* m_particle;
    cocos2d::CCParticleSystemQuad* m_standaloneParticle;
    int m_touchID;
    cocos2d::CCPoint m_touchDelta;
    cocos2d::CCArray* m_inputDicts;
    cocos2d::CCArray* m_sliderDicts;
    cocos2d::CCArray* m_pageNodes;
    cocos2d::CCArray* m_pageButtons;
    cocos2d::CCArray* m_pageMenus;
    cocos2d::CCArray* m_pageObjectArrays;
    ParticlePreviewLayer* m_previewLayer;
    CCMenuItemSpriteExtra* m_mode1Button;
    CCMenuItemSpriteExtra* m_mode2Button;
    CCMenuItemSpriteExtra* m_mode3Button;
    float m_elapsed;
    int m_selectedMode;
    CCMenuItemToggler* m_freeToggler;
    CCMenuItemToggler* m_relativeToggler;
    CCMenuItemToggler* m_groupedToggler;
    CCMenuItemToggler* m_objColorToggler;
    CCMenuItemToggler* m_uniformColorToggler;
    CCMenuItemToggler* m_gravityToggler;
    CCMenuItemToggler* m_radiusToggler;
    cocos2d::CCSprite* m_selectSprite;
    cocos2d::CCSprite* m_particleColorSprite;
    cocos2d::CCSprite* m_startColorSprite;
    cocos2d::CCSprite* m_endColorSprite;
    cocos2d::CCArray* m_gravityObjects;
    cocos2d::CCArray* m_radiusObjects;
    bool m_perfLogEnabled;
    float m_perfLogElapsed;
    cocos2d::CCLabelBMFont* m_perfLogLabel;
    int m_particleIndex;
    int m_page;
    bool m_useObjectColor;
    bool m_animateOnTrigger;
    bool m_onlyAnimateActive;
    bool m_quickStart;
    float m_respawnResult;
};