#include "CreateParticlePopup.h"

bool CreateParticlePopup::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint point;
    cocos2d::CCPoint point2;
    cocos2d::CCRect rect;

    cocos2d::CCTouch::getLocation(touch);
    cocos2d::CCNode::convertToNodeSpace(m_previewLayer, &point);
    cocos2d::CCRect::CCRect(&rect, 0.0, 0.0, 120.0, 180.0);
    if (cocos2d::CCRect::containsPoint(&rect, &point2))
    {
        m_touchID = touch->m_nId;
        m_touchDelta = cocos2d::CCPoint::operator-(m_particle->getPosition(m_particle), point2);
        ccTouchMoved(this, touch, event);
    }
    return FLAlertLayer::ccTouchBegan(this, touch, event);
}

void CreateParticlePopup::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    ccTouchEnded(this, touch, event);
}

void CreateParticlePopup::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_touchID == touch->m_nId)
        m_touchID = -1;
    FLAlertLayer::ccTouchEnded(this, touch, event);
}

void CreateParticlePopup::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (touch->m_nId == m_touchID)
    {
        cocos2d::CCPoint point;
        cocos2d::CCPoint x;
        cocos2d::CCRect rect;

        cocos2d::CCTouch::getLocation(touch);
        cocos2d::CCNode::convertToNodeSpace(m_previewLayer, &point);
        cocos2d::CCRect::CCRect(&rect, 0.0, 0.0, 120.0, 180.0);
        int emitterMode = m_particle->getEmitterMode(m_particle);

        if (cocos2d::CCRect::containsPoint(&rect, &point))
        {
            if (m_selectedMode == 0)
            {
                m_particle->setPosition(cocos2d::CCPoint::operator+(point, m_touchDelta));
            }
            else if (m_selectedMode == 1)
            {
                cocos2d::CCPoint diff = cocos2d::CCPoint::operator-(m_particle->getPosition(m_particle), point);
                if (emitterMode)
                {
                    float length = cocos2d::ccpLength(diff);
                    updateParticleValueForType(length, StartRad, nullptr);
                    updateInputNodeStringForType(StartRad);
                    updateSliderForType(StartRad);
                }
                else
                {
                    updateParticleValueForType(fabsf(diff.x), PosVarX, nullptr);
                    updateParticleValueForType(fabsf(diff.y), PosVarY, nullptr);
                    updateInputNodeStringForType(PosVarX);
                    updateSliderForType(PosVarX);
                    updateInputNodeStringForType(PosVarY);
                    updateSliderForType(PosVarY);
                }
            }
            else if (m_selectedMode == 2)
            {
                if (emitterMode)
                {
                    cocos2d::CCPoint diff = cocos2d::CCPoint::operator-(m_particle->getPosition(m_particle), point);
                    float length = cocos2d::ccpLength(diff);
                    updateParticleValueForType(length, EndRad, nullptr);
                    updateInputNodeStringForType(EndRad);
                    updateSliderForType(EndRad);
                }
            }
            else if (m_selectedMode == 3)
            {
                cocos2d::CCPoint diff = cocos2d::CCPoint::operator-(m_particle->getPosition(m_particle), point);
                float angle;
                if (emitterMode)
                {
                    angle = atan2f(diff.y, diff.x) * 57.296f;
                }
                else
                {
                    float length = cocos2d::ccpLength(diff);
                    updateParticleValueForType(length * 4.0f, Speed, nullptr);
                    angle = (atan2f(diff.y, diff.x) * 57.296f) + 180.0f;
                }
                if (angle > 180.0f)
                    angle -= 360.0f;
                else if (angle < -180.0f)
                    angle += 360.0f;
                updateParticleValueForType(-angle, Angle, nullptr);
                if (!emitterMode)
                {
                    updateInputNodeStringForType(Speed);
                    updateSliderForType(Speed);
                }
                updateInputNodeStringForType(Angle);
                updateSliderForType(Angle);
            }
        }
    }
    FLAlertLayer::ccTouchMoved(this, touch, event);
}

void CreateParticlePopup::centerAlignParticle(cocos2d::CCObject* sender)
{
    m_particle->setPosition(cocos2d::CCPoint(60.0f, 90.0f));
}

void CreateParticlePopup::colorSelectClosed(cocos2d::CCNode* popup)
{
    int tag = popup->getTag(popup);
    unsigned char r = LOWORD(popup[3].m_obAnchorPoint.y);
    unsigned char g = HIBYTE(LOWORD(popup[3].m_obAnchorPoint.y));
    unsigned char b = BYTE2(popup[3].m_obAnchorPoint.y);
    cocos2d::ccColor3B color = cocos2d::ccColor3B(r, g, b);

    if (tag == 0)
    {
        m_previewLayer->setColor(color);
        m_particleColorSprite->setColor(color);
        m_perfLogLabel->setColor(cocos2d::ccColor3B(~r, ~g, ~b));
    }
    else if (tag == 1)
    {
        updateParticleValueForType(r / 255.0f, StartR, nullptr);
        updateParticleValueForType(g / 255.0f, StartG, nullptr);
        updateParticleValueForType(b / 255.0f, StartB, nullptr);
        updateInputNodeStringForType(StartR);
        updateInputNodeStringForType(StartG);
        updateInputNodeStringForType(StartB);
        updateSliderForType(StartR);
        updateSliderForType(StartG);
        updateSliderForType(StartB);
        m_startColorSprite->setColor(color);
        m_particle->m_tUniformColor = *m_particle->getStartColor(m_particle);
        m_particle->m_tUniformDeltaColor = *m_particle->getEndColor(m_particle);
    }
    else if (tag == 2)
    {
        updateParticleValueForType(r / 255.0f, EndR, nullptr);
        updateParticleValueForType(g / 255.0f, EndG, nullptr);
        updateParticleValueForType(b / 255.0f, EndB, nullptr);
        updateInputNodeStringForType(EndR);
        updateInputNodeStringForType(EndG);
        updateInputNodeStringForType(EndB);
        updateSliderForType(EndR);
        updateSliderForType(EndG);
        updateSliderForType(EndB);
        m_endColorSprite->setColor(color);
    }
}

CreateParticlePopup* CreateParticlePopup::create(std::string str)
{
    return CreateParticlePopup::create(nullptr, nullptr, str);
}

CreateParticlePopup* CreateParticlePopup::create(ParticleGameObject* object, cocos2d::CCArray* objects)
{
    return CreateParticlePopup::create(object, objects, "");
}

CreateParticlePopup* CreateParticlePopup::create(ParticleGameObject* object, cocos2d::CCArray* objects, std::string str)
{
    CreateParticlePopup* pRet = new CreateParticlePopup();
    if (pRet && pRet->init(object, objects, str))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CreateParticlePopup::init(ParticleGameObject* object, cocos2d::CCArray* objects, std::string str)
{
    if (!FLAlertLayer::init(200))
        return false;

    m_targetObject = object;
    m_targetObjects = objects;
    if (objects) objects->retain();

    m_disableTextDelegate = false;
    m_touchID = -1;
    m_elapsed = 0.0f;
    m_selectedMode = 0;
    m_perfLogEnabled = false;
    m_perfLogElapsed = 0.0f;
    m_respawnResult = -1.0f;
    m_particleIndex = 0;
    m_page = 1;
    m_useObjectColor = false;
    m_animateOnTrigger = false;
    m_onlyAnimateActive = false;
    m_quickStart = false;

    // Create particle system from string
    m_particle = GameToolbox::particleFromString(str);
    if (!m_particle)
        return false;
    m_particle->retain();

    // Create preview layer
    m_previewLayer = ParticlePreviewLayer::create(m_particle);
    m_mainLayer->addChild(m_previewLayer);

    // Setup UI pages, sliders, input nodes, etc.
    // ... (rest of initialization code)

    return true;
}

cocos2d::CCNode* CreateParticlePopup::getPage(int page)
{
    return (cocos2d::CCNode*)cocos2d::CCArray::objectAtIndex(m_pageNodes, page - 1);
}

CCMenuItemSpriteExtra* CreateParticlePopup::getPageButton(int page)
{
    return (CCMenuItemSpriteExtra*)cocos2d::CCArray::objectAtIndex(m_pageButtons, page - 1);
}

cocos2d::CCArray* CreateParticlePopup::getPageContainer(int page)
{
    return (cocos2d::CCArray*)cocos2d::CCArray::objectAtIndex(m_pageObjectArrays, page - 1);
}

cocos2d::CCDictionary* CreateParticlePopup::getPageInputNodes(int page)
{
    return (cocos2d::CCDictionary*)cocos2d::CCArray::objectAtIndex(m_inputDicts, page);
}

cocos2d::CCMenu* CreateParticlePopup::getPageMenu(int page)
{
    return nullptr;
}

cocos2d::CCDictionary* CreateParticlePopup::getPageSliders(int page)
{
    return (cocos2d::CCDictionary*)cocos2d::CCArray::objectAtIndex(m_sliderDicts, page);
}

void CreateParticlePopup::keyBackClicked()
{
    onClose(nullptr);
}

float CreateParticlePopup::maxSliderValueForType(gjParticleValue type)
{
    unsigned int index = type - 1;
    if (index > 0x47)
        return 1.0f;
    return flt_F02EF0[index];
}

float CreateParticlePopup::minSliderValueForType(gjParticleValue type)
{
    unsigned int index = type - 1;
    if (index > 0x47)
        return 0.0f;
    return flt_F02DD0[index];
}

void CreateParticlePopup::onAnimateActiveOnly(cocos2d::CCObject* sender)
{
    m_onlyAnimateActive = !m_onlyAnimateActive;
    if (m_targetObject)
    {
        m_targetObject->m_animateOnlyWhenActive = m_onlyAnimateActive;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            cocos2d::CCObject* obj = m_targetObjects->objectAtIndex(i);
            obj[25].m_uAutoReleaseCount = m_onlyAnimateActive;
        }
    }
}

void CreateParticlePopup::onAnimateOnTrigger(cocos2d::CCObject* sender)
{
    m_animateOnTrigger = !m_animateOnTrigger;
    if (m_targetObject)
    {
        m_targetObject->m_animateOnTrigger = m_animateOnTrigger;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            cocos2d::CCObject* obj = m_targetObjects->objectAtIndex(i);
            BYTE1(obj[25].__vftable) = m_animateOnTrigger;
        }
    }
}

void CreateParticlePopup::onCalcEmission(cocos2d::CCObject* sender)
{
    float life = m_particle->getLife(m_particle);
    float lifeVar = life + m_particle->getLifeVar(m_particle);
    float emission = -1.0f;
    if (lifeVar > 0.0f)
        emission = (float)m_particle->getTotalParticles(m_particle) / lifeVar;
    updateParticleValueForType(emission, Emission, nullptr);
    updateSliderForType(Emission);
    updateInputNodeStringForType(Emission);
    m_particle->resetSystem();
}

void CreateParticlePopup::onClose(cocos2d::CCObject* sender)
{
    willClose();

    if (m_targetObject)
    {
        std::string particleStr = GameToolbox::saveParticleToString(m_particle);
        m_targetObject->setParticleString(particleStr);
        m_targetObject->m_hasUniformObjectColor = m_particle->m_bUseUniformColorMode;
        m_targetObject->m_popupPage = m_page;
        m_targetObject->updateParticle();
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            ParticleGameObject* obj = (ParticleGameObject*)m_targetObjects->objectAtIndex(i);
            cocos2d::CCParticleSystemQuad* particle = (cocos2d::CCParticleSystemQuad*)m_particles->objectAtIndex(i);
            std::string str = GameToolbox::saveParticleToString(particle);
            obj->setParticleString(str);
            obj->m_hasUniformObjectColor = particle->m_bUseUniformColorMode;
            obj->updateParticle();
        }
    }

    removeFromParentAndCleanup(this, 1);
}

void CreateParticlePopup::onCopySettings(cocos2d::CCObject* sender)
{
    if (m_targetObject)
    {
        std::string particleStr = GameToolbox::saveParticleToString(m_particle);
        m_targetObject->setParticleString(particleStr);
        m_targetObject->m_hasUniformObjectColor = m_particle->m_bUseUniformColorMode;

        if (GameManager::sharedState()->m_levelEditorLayer)
        {
            LevelEditorLayer::copyParticleState(GameManager::sharedState()->m_levelEditorLayer, m_targetObject);
        }
    }
}

void CreateParticlePopup::onDuplicateColor(cocos2d::CCObject* sender)
{
    const cocos2d::ccColor4F* startColor = m_particle->getStartColor(m_particle);
    updateParticleValueForType(startColor->r, EndR, nullptr);
    updateParticleValueForType(startColor->g, EndG, nullptr);
    updateParticleValueForType(startColor->b, EndB, nullptr);
    updateInputNodeStringForType(EndR);
    updateInputNodeStringForType(EndG);
    updateInputNodeStringForType(EndB);
    updateSliderForType(EndR);
    updateSliderForType(EndG);
    updateSliderForType(EndB);

    cocos2d::ccColor3B color = cocos2d::ccColor3B(
        (unsigned char)(startColor->r * 255.0f),
        (unsigned char)(startColor->g * 255.0f),
        (unsigned char)(startColor->b * 255.0f)
    );
    m_endColorSprite->setColor(color);
}

void CreateParticlePopup::onDurationForever(cocos2d::CCObject* sender)
{
    updateParticleValueForType(-1.0f, Duration, nullptr);
    updateSliderForType(Duration);
    updateInputNodeStringForType(Duration);
    m_particle->resetSystem();
}

void CreateParticlePopup::onDynamicColor(cocos2d::CCObject* sender)
{
    m_useObjectColor = !m_useObjectColor;

    if (m_useObjectColor && m_particle->m_bUseUniformColorMode)
        m_particle->m_bUseUniformColorMode = 0;

    if (m_targetObject)
    {
        m_targetObject->m_particleUseObjectColor = m_useObjectColor;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            cocos2d::CCObject* obj = m_targetObjects->objectAtIndex(i);
            cocos2d::CCObject* particleObj = m_particles->objectAtIndex(i);
            BYTE1(obj[14].m_uID) = m_useObjectColor;
            if (m_useObjectColor && LOBYTE(particleObj[12].m_unknown5))
                LOBYTE(particleObj[12].m_unknown5) = 0;
        }
    }

    m_uniformColorToggler->toggle(m_particle->m_bUseUniformColorMode);
    m_objColorToggler->toggle(m_useObjectColor);
}

void CreateParticlePopup::onDynamicRotation(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bDynamicRotationIsDir, DynamicRotation, nullptr);
}

void CreateParticlePopup::onEmitterMode(cocos2d::CCObject* sender)
{
    int mode = sender->getTag(sender);
    updateParticleValueForType((float)mode, EmitterMode, nullptr);
    m_gravityToggler->toggle(mode == 0);
    m_radiusToggler->toggle(mode == 1);
    toggleGravityMode(m_particle->getEmitterMode(m_particle) == 0);
}

void CreateParticlePopup::onEndRGBVarSync(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bEndRGBVarSync, EndRGBVarSync, nullptr);
}

void CreateParticlePopup::onMaxEmission(cocos2d::CCObject* sender)
{
    updateParticleValueForType(-1.0f, Emission, nullptr);
    updateSliderForType(Emission);
    updateInputNodeStringForType(Emission);
    m_particle->resetSystem();
}

void CreateParticlePopup::onMode(cocos2d::CCObject* sender)
{
    int mode = sender->getTag(sender);
    if (m_selectedMode == mode)
        mode = 0;

    m_selectedMode = mode;
    m_previewLayer->m_particleMode = mode;

    ButtonSprite* btn1 = (ButtonSprite*)m_mode1Button->getNormalImage();
    ButtonSprite::updateBGImage(btn1, m_selectedMode == 1 ? "GJ_button_01.png" : "GJ_button_04.png");

    ButtonSprite* btn2 = (ButtonSprite*)m_mode2Button->getNormalImage();
    ButtonSprite::updateBGImage(btn2, m_selectedMode == 2 ? "GJ_button_01.png" : "GJ_button_04.png");

    ButtonSprite* btn3 = (ButtonSprite*)m_mode3Button->getNormalImage();
    ButtonSprite::updateBGImage(btn3, m_selectedMode == 3 ? "GJ_button_01.png" : "GJ_button_04.png");
}

void CreateParticlePopup::onOrderSensitive(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bOrderSensitive, OrderSensitive, nullptr);
}

void CreateParticlePopup::onPage(cocos2d::CCObject* sender)
{
    if (sender)
        m_page = sender->getTag(sender);

    for (int i = 1; i <= 4; i++)
    {
        bool isActive = (m_page == i);

        cocos2d::CCNode* page = getPage(i);
        page->setVisible(isActive);

        CCMenuItemSpriteExtra* btn = getPageButton(i);
        ButtonSprite* sprite = (ButtonSprite*)btn->getNormalImage();
        ButtonSprite::updateBGImage(sprite, isActive ? "GJ_button_01.png" : "GJ_button_04.png");

        cocos2d::CCDictionary* inputs = getPageInputNodes(i);
        if (inputs)
        {
            cocos2d::CCDictElement* el = inputs->m_pElements;
            while (el)
            {
                el->m_pObject->setVisible(isActive);
                if (!isActive)
                    el->m_pObject->setEnabled(false);
                el = (cocos2d::CCDictElement*)el->hh.next;
            }
        }

        cocos2d::CCDictionary* sliders = getPageSliders(i);
        if (sliders)
        {
            cocos2d::CCDictElement* el = sliders->m_pElements;
            while (el)
            {
                el->m_pObject->setVisible(isActive);
                el = (cocos2d::CCDictElement*)el->hh.next;
            }
        }

        cocos2d::CCArray* container = getPageContainer(i);
        for (unsigned int j = 0; j < container->count(); j++)
        {
            cocos2d::CCObject* obj = container->objectAtIndex(j);
            obj->setVisible(isActive);
        }
    }

    if (m_page == 1)
        toggleGravityMode(m_particle->getEmitterMode(m_particle) == 0);
}

void CreateParticlePopup::onPasteSettings(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->m_levelEditorLayer)
    {
        LevelEditorLayer::pasteParticleState(
            GameManager::sharedState()->m_levelEditorLayer,
            m_targetObject,
            m_targetObjects
        );
        willClose();
        removeFromParentAndCleanup(this, 1);
    }
}

void CreateParticlePopup::onPosType(cocos2d::CCObject* sender)
{
    unsigned int type = sender->getTag(sender);
    updateParticleValueForType((float)type, PositionType, nullptr);

    if (type == 0)
    {
        m_relativeToggler->toggle(0);
        m_groupedToggler->toggle(0);
    }
    else
    {
        m_freeToggler->toggle(0);
        if (type == 1)
            m_groupedToggler->toggle(0);
        else if (type == 2)
            m_relativeToggler->toggle(0);
    }
}

void CreateParticlePopup::onQuickStart(cocos2d::CCObject* sender)
{
    m_quickStart = !m_quickStart;

    if (m_targetObject)
    {
        m_targetObject->m_shouldQuickStart = m_quickStart;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            cocos2d::CCObject* obj = m_targetObjects->objectAtIndex(i);
            LOBYTE(obj[30].m_uOrderOfArrival) = m_quickStart;
        }
    }
}

void CreateParticlePopup::onSelectColor(cocos2d::CCObject* sender)
{
    int tag = sender->getTag(sender);
    cocos2d::ccColor3B color;

    if (tag == 0)
    {
        color = m_previewLayer->getColor(m_previewLayer);
    }
    else if (tag == 1)
    {
        const cocos2d::ccColor4F* startColor = m_particle->getStartColor(m_particle);
        color = cocos2d::ccColor3B(
            (unsigned char)(startColor->r * 255.0f),
            (unsigned char)(startColor->g * 255.0f),
            (unsigned char)(startColor->b * 255.0f)
        );
    }
    else if (tag == 2)
    {
        const cocos2d::ccColor4F* endColor = m_particle->getEndColor(m_particle);
        color = cocos2d::ccColor3B(
            (unsigned char)(endColor->r * 255.0f),
            (unsigned char)(endColor->g * 255.0f),
            (unsigned char)(endColor->b * 255.0f)
        );
    }
    else
        return;

    ColorSelectPopup* popup = ColorSelectPopup::create(color);
    popup->setTag(tag);
    popup->m_delegate = this;
    popup->show();
}

void CreateParticlePopup::onSelectParticleTexture(cocos2d::CCObject* sender)
{
    int index = sender->getTag(sender);
    if (index < 0) index = 0;
    if (index > 212) index = 212;
    m_particleIndex = index;

    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
        cocos2d::CCString::createWithFormat("particle_%02d_001.png", m_particleIndex)->getCString()
    );
    m_particle->setDisplayFrame(frame);
    updateParticleValueForType((float)m_particleIndex, ParticleIdx, nullptr);

    cocos2d::CCPoint pos = cocos2d::CCNode::convertToNodeSpace(
        m_selectSprite->getParent(),
        cocos2d::CCNode::convertToWorldSpace(sender->getParent(), sender->getPosition())
    );
    m_selectSprite->setPosition(pos);
}

void CreateParticlePopup::onStartRGBVarSync(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bStartRGBVarSync, StartRGBVarSync, nullptr);
}

void CreateParticlePopup::onToggleBlending(cocos2d::CCObject* sender)
{
    bool isAdditive = m_particle->isBlendAdditive(m_particle);
    updateParticleValueForType((float)!isAdditive, Blending, nullptr);
}

void CreateParticlePopup::onTogglePerfLog(cocos2d::CCObject* sender)
{
    GameManager::toggleGameVariable("0178");
    m_perfLogEnabled = GameManager::getGameVariable("0178");
    m_perfLogElapsed = 1.0f;
    m_perfLogLabel->setVisible(m_perfLogEnabled);
}

void CreateParticlePopup::onToggleStartRadiusEqualToEnd(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bStartRadiusEqualToEnd, StartRadiusEqualToEnd, nullptr);
}

void CreateParticlePopup::onToggleStartRotationIsDir(cocos2d::CCObject* sender)
{
    bool isDir = m_particle->getRotationIsDir(m_particle);
    updateParticleValueForType((float)!isDir, StartRotationIsDir, nullptr);
}

void CreateParticlePopup::onToggleStartSizeEqualToEnd(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bStartSizeEqualToEnd, StartSizeEqualToEnd, nullptr);
}

void CreateParticlePopup::onToggleStartSpinEqualToEnd(cocos2d::CCObject* sender)
{
    updateParticleValueForType((float)!m_particle->m_bStartSpinEqualToEnd, StartSpinEqualToEnd, nullptr);
}

void CreateParticlePopup::onUniformColor(cocos2d::CCObject* sender)
{
    bool uniform = !m_particle->m_bUseUniformColorMode;
    updateParticleValueForType((float)uniform, UniformColor, nullptr);

    if (m_particle->m_bUseUniformColorMode)
    {
        if (m_targetObject)
        {
            m_targetObject->m_particleUseObjectColor = 0;
        }
        else if (m_targetObjects)
        {
            for (unsigned int i = 0; i < m_targetObjects->count(); i++)
            {
                cocos2d::CCObject* obj = m_targetObjects->objectAtIndex(i);
                BYTE1(obj[14].m_uID) = 0;
            }
        }
        m_useObjectColor = 0;
    }

    m_uniformColorToggler->toggle(m_particle->m_bUseUniformColorMode);
    m_objColorToggler->toggle(m_useObjectColor);
}

bool CreateParticlePopup::particleValueIsInt(gjParticleValue type)
{
    if ((unsigned int)type > EndSpinVar)
    {
        if ((unsigned int)(type - 50) > 5)
            return false;
    }
    else if ((unsigned int)type < Emission && type != MaxParticles)
    {
        return false;
    }
    return true;
}

void CreateParticlePopup::sliderBegan(Slider* slider)
{
    int tag = slider->getTag(slider);
    switch (tag)
    {
        case 6:
        case 7:
        case 8:
        case 9:
            m_previewLayer->m_drawMode = 3;
            break;
        case 10:
        case 11:
        case 50:
        case 52:
            m_previewLayer->m_drawMode = 1;
            break;
        case 12:
        case 13:
            m_previewLayer->m_drawMode = 2;
            break;
        default:
            break;
    }
}

void CreateParticlePopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();
    gjParticleValue type = (gjParticleValue)sender->getTag(sender);

    float minVal = minSliderValueForType(type);
    float maxVal = maxSliderValueForType(type);
    float result = minVal + (value * (maxVal - minVal));

    if (particleValueIsInt(type))
        result = roundf(result);
    else
        result = roundf(result * 100.0f) / 100.0f;

    updateParticleValueForType(result, type, nullptr);
    updateSliderForType(type);
    updateInputNodeStringForType(type);
}

void CreateParticlePopup::sliderEnded(Slider* slider)
{
    m_previewLayer->m_drawMode = 0;
}

void CreateParticlePopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;

    const char* str = node->getString();
    float value = atof(str);

    if (node->getTag(node) == 5 && value < 1.0f)
        value = 1.0f;

    gjParticleValue type = (gjParticleValue)node->getTag(node);
    updateParticleValueForType(value, type, nullptr);
    updateSliderForType(type);
}

void CreateParticlePopup::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void CreateParticlePopup::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, 0));
    m_mainLayer->runAction(move);
}

void CreateParticlePopup::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0, yOffset));
    m_mainLayer->runAction(move);
}

std::string CreateParticlePopup::titleForParticleValue(gjParticleValue type)
{
    std::string result;

    switch (type)
    {
        case MaxParticles: result = "Max\nParticles"; break;
        case Duration: result = "Duration"; break;
        case Lifetime: result = "Lifetime"; break;
        case LifetimeVar:
        case AngleVar:
        case SpeedVar:
        case AccelRadVar:
        case AccelTanVar:
        case StartSizeVar:
        case EndSizeVar:
        case StartSpinVar:
        case EndSpinVar:
        case StartRVar:
        case StartGVar:
        case StartBVar:
        case StartAVar:
        case EndRVar:
        case EndGVar:
        case EndBVar:
        case EndAVar:
        case FadeInVar:
        case FadeOutVar:
        case FrictionPVar:
        case RespawnVar:
        case StartRadVar:
        case EndRadVar:
        case RotSecVar:
        case FrictionSVar:
        case FrictionRVar:
            result = "+-"; break;
        case Emission: result = "Emission"; break;
        case Angle: result = "Angle"; break;
        case Speed: result = "Speed"; break;
        case PosVarX: result = "PosVar X"; break;
        case PosVarY:
        case GravityY: result = "Y"; break;
        case GravityX: result = "Gravity X"; break;
        case AccelRad: result = "AccelRad"; break;
        case AccelTan: result = "AccelTan"; break;
        case StartSize: result = "StartSize"; break;
        case EndSize: result = "EndSize"; break;
        case StartSpin: result = "StartSpin"; break;
        case EndSpin: result = "EndSpin"; break;
        case StartR: result = "Start_R"; break;
        case StartG: result = "Start_G"; break;
        case StartB: result = "Start_B"; break;
        case StartA: result = "Start_A"; break;
        case EndR: result = "End_R"; break;
        case EndG: result = "End_G"; break;
        case EndB: result = "End_B"; break;
        case EndA: result = "End_A"; break;
        case FadeIn: result = "Fade in"; break;
        case FadeOut: result = "Fade out"; break;
        case FrictionP: result = "FrictionP"; break;
        case Respawn: result = "Respawn"; break;
        case StartRad: result = "StartRad"; break;
        case EndRad: result = "EndRad"; break;
        case RotSec: result = "RotSec"; break;
        case FrictionS: result = "FrictionS"; break;
        case FrictionR: result = "FrictionR"; break;
        default: result = "None"; break;
    }

    return result;
}

void CreateParticlePopup::toggleGravityMode(bool gravityMode)
{
    cocos2d::CCDictionary* inputs = getPageInputNodes(0);
    cocos2d::CCDictionary* sliders = getPageSliders(0);

    // Toggle gravity-specific controls
    const char* gravityKeys[] = {"6", "7", "8", "9", "10", "11"};
    for (int i = 0; i < 6; i++)
    {
        cocos2d::CCObject* input = inputs->objectForKey(gravityKeys[i]);
        input->setVisible(gravityMode);
        input->setEnabled(gravityMode);

        cocos2d::CCObject* slider = sliders->objectForKey(gravityKeys[i]);
        slider->setVisible(gravityMode);
        slider->setEnabled(gravityMode);
    }

    // Toggle radius-specific controls
    const char* radiusKeys[] = {"50", "51", "52", "53"};
    bool radiusMode = !gravityMode;
    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCObject* input = inputs->objectForKey(radiusKeys[i]);
        input->setVisible(radiusMode);
        input->setEnabled(radiusMode);

        cocos2d::CCObject* slider = sliders->objectForKey(radiusKeys[i]);
        slider->setVisible(radiusMode);
        slider->setEnabled(radiusMode);
    }

    for (unsigned int i = 0; i < m_gravityObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_gravityObjects->objectAtIndex(i);
        obj->setVisible(gravityMode);
    }

    for (unsigned int i = 0; i < m_radiusObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_radiusObjects->objectAtIndex(i);
        obj->setVisible(radiusMode);
    }

    m_previewLayer->m_gravityMode = (m_particle->getEmitterMode(m_particle) == 0);
}

void CreateParticlePopup::update(float dt)
{
    if (m_perfLogEnabled)
    {
        m_perfLogElapsed += dt;
        if (m_perfLogElapsed >= 0.1f)
        {
            m_perfLogElapsed = 0.0f;
            unsigned int count = m_particle->getParticleCount();
            unsigned int total = m_particle->getTotalParticles();
            int cost = m_particle->getParticleDrawCost();
            m_perfLogLabel->setString(
                cocos2d::CCString::createWithFormat("%i/%i\n%i", count, total, cost)->getCString()
            );
        }
    }

    float duration = m_particle->getDuration();
    if (duration >= 0.0f)
    {
        m_elapsed += dt;

        if (m_respawnResult < 0.0f)
        {
            unsigned int seed = 214013 * qword_1243D50 + 2531011;
            qword_1243D50 = seed;
            float respawn = m_particle->m_fRespawn;
            float respawnVar = m_particle->m_fRespawnVar;
            float random = ((float)((seed >> 16) & 0x7FFF) / 32767.0f) * 2.0f - 1.0f;
            float result = respawn + (random * respawnVar);

            if (result <= 0.0f)
            {
                m_respawnResult = 0.0f;
            }
            else
            {
                seed = 214013 * seed + 2531011;
                qword_1243D50 = seed;
                random = ((float)((seed >> 16) & 0x7FFF) / 32767.0f) * 2.0f - 1.0f;
                m_respawnResult = respawn + (respawnVar * random);
            }
        }

        float life = m_particle->getLife(m_particle);
        float lifeVar = m_particle->getLifeVar(m_particle);
        float totalTime = duration + life + lifeVar + m_respawnResult;

        if (m_elapsed > totalTime)
        {
            m_respawnResult = -1.0f;
            m_elapsed = 0.0f;
            m_particle->resetSystem();
        }
    }
}

void CreateParticlePopup::updateColorSprite(int tag)
{
    const cocos2d::ccColor4F* color;

    if (tag == 1)
        color = m_particle->getStartColor(m_particle);
    else if (tag == 2)
        color = m_particle->getEndColor(m_particle);
    else
        return;

    cocos2d::ccColor3B c = cocos2d::ccColor3B(
        (unsigned char)(color->r * 255.0f),
        (unsigned char)(color->g * 255.0f),
        (unsigned char)(color->b * 255.0f)
    );
    m_startColorSprite->setColor(c);
}

void CreateParticlePopup::updateInputNodeStringForType(gjParticleValue type)
{
    cocos2d::CCDictionary* inputs = getPageInputNodes(0);
    CCTextInputNode* node = (CCTextInputNode*)inputs->objectForKey((const std::string*)(unsigned int)type);

    if (node)
    {
        float value = valueForParticleValue(type);
        std::string str;

        if (particleValueIsInt(type))
            str = cocos2d::CCString::createWithFormat("%i", (int)value)->getCString();
        else
            str = cocos2d::CCString::createWithFormat("%.02f", value)->getCString();

        m_disableTextDelegate = true;
        node->setString(str);
        m_disableTextDelegate = false;
    }
}

void CreateParticlePopup::updateSliderForType(gjParticleValue type)
{
    cocos2d::CCDictionary* sliders = getPageSliders(0);
    Slider* slider = (Slider*)sliders->objectForKey((const std::string*)(unsigned int)type);

    if (slider)
    {
        float value = valueForParticleValue(type);
        float minVal = minSliderValueForType(type);
        float maxVal = maxSliderValueForType(type);
        float sliderValue = (value - minVal) / (maxVal - minVal);

        if (sliderValue > 1.0f)
            sliderValue = 1.0f;
        else if (sliderValue < 0.0f)
            sliderValue = 0.0f;

        slider->setValue(sliderValue);
    }
}

void CreateParticlePopup::updateParticleValueForType(float value, gjParticleValue type, cocos2d::CCParticleSystemQuad* particle)
{
    if (!particle)
        particle = m_particle;

    switch (type)
    {
        case MaxParticles: particle->setTotalParticles((int)value); break;
        case Duration: particle->setDuration(value); break;
        case Lifetime: particle->setLife(value); break;
        case LifetimeVar: particle->setLifeVar(value); break;
        case Emission: particle->setEmissionRate(value); break;
        case Angle: particle->setAngle(-value); break;
        case AngleVar: particle->setAngleVar(value); break;
        case Speed: particle->setSpeed(value); break;
        case SpeedVar: particle->setSpeedVar(value); break;
        case PosVarX: particle->getPosVar()->x = value; break;
        case PosVarY: particle->getPosVar()->y = value; break;
        case GravityX: particle->getGravity()->x = value; break;
        case GravityY: particle->getGravity()->y = value; break;
        case AccelRad: particle->setRadialAccel(value); break;
        case AccelRadVar: particle->setRadialAccelVar(value); break;
        case AccelTan: particle->setTangentialAccel(value); break;
        case AccelTanVar: particle->setTangentialAccelVar(value); break;
        case StartSize: particle->setStartSize(value); break;
        case StartSizeVar: particle->setStartSizeVar(value); break;
        case EndSize: particle->setEndSize(value); break;
        case EndSizeVar: particle->setEndSizeVar(value); break;
        case StartSpin: particle->setStartSpin(value); break;
        case StartSpinVar: particle->setStartSpinVar(value); break;
        case EndSpin: particle->setEndSpin(value); break;
        case EndSpinVar: particle->setEndSpinVar(value); break;
        case StartR: particle->getStartColor()->r = value; break;
        case StartRVar: particle->getStartColorVar()->r = value; break;
        case StartG: particle->getStartColor()->g = value; break;
        case StartGVar: particle->getStartColorVar()->g = value; break;
        case StartB: particle->getStartColor()->b = value; break;
        case StartBVar: particle->getStartColorVar()->b = value; break;
        case StartA: particle->getStartColor()->a = value; break;
        case StartAVar: particle->getStartColorVar()->a = value; break;
        case EndR: particle->getEndColor()->r = value; break;
        case EndRVar: particle->getEndColorVar()->r = value; break;
        case EndG: particle->getEndColor()->g = value; break;
        case EndGVar: particle->getEndColorVar()->g = value; break;
        case EndB: particle->getEndColor()->b = value; break;
        case EndBVar: particle->getEndColorVar()->b = value; break;
        case EndA: particle->getEndColor()->a = value; break;
        case EndAVar: particle->getEndColorVar()->a = value; break;
        case FadeIn: particle->m_fFadeInTime = value; break;
        case FadeInVar: particle->m_fFadeInTimeVar = value; break;
        case FadeOut: particle->m_fFadeOutTime = value; break;
        case FadeOutVar: particle->m_fFadeOutTimeVar = value; break;
        case FrictionP: particle->m_fFrictionPos = value; break;
        case FrictionPVar: particle->m_fFrictionPosVar = value; break;
        case Respawn: particle->m_fRespawn = value; break;
        case RespawnVar: particle->m_fRespawnVar = value; break;
        case StartRad: particle->setStartRadius(value); break;
        case StartRadVar: particle->setStartRadiusVar(value); break;
        case EndRad: particle->setEndRadius(value); break;
        case EndRadVar: particle->setEndRadiusVar(value); break;
        case RotSec: particle->setRotatePerSecond(-value); break;
        case RotSecVar: particle->setRotatePerSecondVar(value); break;
        case FrictionS: particle->m_fFrictionSize = value; break;
        case FrictionSVar: particle->m_fFrictionSizeVar = value; break;
        case FrictionR: particle->m_fFrictionRot = value; break;
        case FrictionRVar: particle->m_fFrictionRotVar = value; break;
        case StartRGBVarSync: particle->m_bStartRGBVarSync = (value != 0.0f); break;
        case EndRGBVarSync: particle->m_bEndRGBVarSync = (value != 0.0f); break;
        case StartRadiusEqualToEnd: particle->m_bStartRadiusEqualToEnd = (value != 0.0f); break;
        case StartSizeEqualToEnd: particle->m_bStartSizeEqualToEnd = (value != 0.0f); break;
        case StartSpinEqualToEnd: particle->m_bStartSpinEqualToEnd = (value != 0.0f); break;
        case StartRotationIsDir: particle->setRotationIsDir(value != 0.0f); break;
        case DynamicRotation: particle->m_bDynamicRotationIsDir = (value != 0.0f); break;
        case UniformColor: particle->m_bUseUniformColorMode = (value != 0.0f); break;
        case Blending: particle->setBlendAdditive(value != 0.0f); break;
        case OrderSensitive: particle->m_bOrderSensitive = (value != 0.0f); break;
        case PositionType: particle->setPositionType((cocos2d::tCCPositionType)(int)value); break;
        case EmitterMode: particle->setEmitterMode((cocos2d::tCCParticleEmitterMode)(int)value); break;
        case ParticleIdx: particle->setTextureIndex((int)value); break;
        default: break;
    }
}

float CreateParticlePopup::valueForParticleValue(gjParticleValue type)
{
    float value = 0.0f;

    switch (type)
    {
        case MaxParticles: value = (float)m_particle->getTotalParticles(); break;
        case Duration: value = m_particle->getDuration(); break;
        case Lifetime: value = m_particle->getLife(); break;
        case LifetimeVar: value = m_particle->getLifeVar(); break;
        case Emission: value = m_particle->getEmissionRate(); break;
        case Angle: value = -m_particle->getAngle(); break;
        case AngleVar: value = m_particle->getAngleVar(); break;
        case Speed: value = m_particle->getSpeed(); break;
        case SpeedVar: value = m_particle->getSpeedVar(); break;
        case PosVarX: value = m_particle->getPosVar()->x; break;
        case PosVarY: value = m_particle->getPosVar()->y; break;
        case GravityX: value = m_particle->getGravity()->x; break;
        case GravityY: value = m_particle->getGravity()->y; break;
        case AccelRad: value = m_particle->getRadialAccel(); break;
        case AccelRadVar: value = m_particle->getRadialAccelVar(); break;
        case AccelTan: value = m_particle->getTangentialAccel(); break;
        case AccelTanVar: value = m_particle->getTangentialAccelVar(); break;
        case StartSize: value = m_particle->getStartSize(); break;
        case StartSizeVar: value = m_particle->getStartSizeVar(); break;
        case EndSize: value = m_particle->getEndSize(); break;
        case EndSizeVar: value = m_particle->getEndSizeVar(); break;
        case StartSpin: value = m_particle->getStartSpin(); break;
        case StartSpinVar: value = m_particle->getStartSpinVar(); break;
        case EndSpin: value = m_particle->getEndSpin(); break;
        case EndSpinVar: value = m_particle->getEndSpinVar(); break;
        case StartR: value = m_particle->getStartColor()->r; break;
        case StartRVar: value = m_particle->getStartColorVar()->r; break;
        case StartG: value = m_particle->getStartColor()->g; break;
        case StartGVar: value = m_particle->getStartColorVar()->g; break;
        case StartB: value = m_particle->getStartColor()->b; break;
        case StartBVar: value = m_particle->getStartColorVar()->b; break;
        case StartA: value = m_particle->getStartColor()->a; break;
        case StartAVar: value = m_particle->getStartColorVar()->a; break;
        case EndR: value = m_particle->getEndColor()->r; break;
        case EndRVar: value = m_particle->getEndColorVar()->r; break;
        case EndG: value = m_particle->getEndColor()->g; break;
        case EndGVar: value = m_particle->getEndColorVar()->g; break;
        case EndB: value = m_particle->getEndColor()->b; break;
        case EndBVar: value = m_particle->getEndColorVar()->b; break;
        case EndA: value = m_particle->getEndColor()->a; break;
        case EndAVar: value = m_particle->getEndColorVar()->a; break;
        case FadeIn: value = m_particle->m_fFadeInTime; break;
        case FadeInVar: value = m_particle->m_fFadeInTimeVar; break;
        case FadeOut: value = m_particle->m_fFadeOutTime; break;
        case FadeOutVar: value = m_particle->m_fFadeOutTimeVar; break;
        case FrictionP: value = m_particle->m_fFrictionPos; break;
        case FrictionPVar: value = m_particle->m_fFrictionPosVar; break;
        case Respawn: value = m_particle->m_fRespawn; break;
        case RespawnVar: value = m_particle->m_fRespawnVar; break;
        case StartRad: value = m_particle->getStartRadius(); break;
        case StartRadVar: value = m_particle->getStartRadiusVar(); break;
        case EndRad: value = m_particle->getEndRadius(); break;
        case EndRadVar: value = m_particle->getEndRadiusVar(); break;
        case RotSec: value = -m_particle->getRotatePerSecond(); break;
        case RotSecVar: value = m_particle->getRotatePerSecondVar(); break;
        case FrictionS: value = m_particle->m_fFrictionSize; break;
        case FrictionSVar: value = m_particle->m_fFrictionSizeVar; break;
        case FrictionR: value = m_particle->m_fFrictionRot; break;
        case FrictionRVar: value = m_particle->m_fFrictionRotVar; break;
        default: value = 0.0f; break;
    }

    return value;
}

void CreateParticlePopup::willClose()
{
    cocos2d::CCDictionary* inputs = getPageInputNodes(0);
    if (inputs)
    {
        cocos2d::CCDictElement* el = inputs->m_pElements;
        while (el)
        {
            CCTextInputNode* node = (CCTextInputNode*)el->m_pObject;
            if (node)
            {
                node->setDelegate(nullptr);
                node->setEnabled(false);
            }
            el = (cocos2d::CCDictElement*)el->hh.next;
        }
    }
    setKeypadEnabled(false);
}

CreateParticlePopup::~CreateParticlePopup()
{
    CC_SAFE_RELEASE(m_targetObjects);
    CC_SAFE_RELEASE(m_particles);
    CC_SAFE_RELEASE(m_particle);
}