#include "DrawGridLayer.h"
#include "LevelEditorLayer.h"
#include "LevelSettingsObject.h"
#include "EffectGameObject.h"
#include "GameObject.h"
#include "AudioLineGuideGameObject.h"
#include "PlayerObject.h"
#include "EditorUI.h"
#include "GameManager.h"
#include "LevelTools.h"
#include "FMODAudioEngine.h"
#include "GameToolbox.h"

DrawGridLayer* DrawGridLayer::create(cocos2d::CCNode* parent, LevelEditorLayer* layer)
{
    DrawGridLayer* pRet = new DrawGridLayer();
    if (pRet && pRet->init(parent, layer))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DrawGridLayer::init(cocos2d::CCNode* parent, LevelEditorLayer* layer)
{
    if (!cocos2d::CCLayer::init())
        return false;
    
    m_objectLayer = parent;
    m_editorLayer = layer;
    m_timeMarkers = nullptr;
    m_playbackTime = 0.0f;
    m_oldPlaybackTime = 0.0f;
    m_musicTime = -1.0f;
    m_playbackX = 0.0f;
    m_playbackY = 0.0f;
    m_gridSize = 30.0f;
    m_sortEffects = false;
    m_updateSpeedObjects = false;
    m_updateTimeMarkers = false;
    m_slowSpeed = 251.16f;
    m_normalSpeed = 311.58f;
    m_currentSpeed = 311.58f;
    m_fastSpeed = 387.42f;
    m_fasterSpeed = 468.0f;
    m_fastestSpeed = 576.0f;
    
    m_effectGameObjects = cocos2d::CCArray::create();
    m_effectGameObjects->retain();
    m_guideObjects = cocos2d::CCArray::create();
    m_guideObjects->retain();
    m_speedObjects = cocos2d::CCArray::create();
    m_speedObjects->retain();
    
    // Initialize point arrays
    for (int i = 0; i < 400; i++)
    {
        m_pointArray1[i] = cocos2d::CCPoint(0, 0);
        m_pointArray2[i] = cocos2d::CCPoint(0, 0);
        m_pointArray3[i] = cocos2d::CCPoint(0, 0);
    }
    
    return true;
}

void DrawGridLayer::addAudioLineObject(AudioLineGuideGameObject* object)
{
    int uniqueID = object->m_uniqueID;
    m_audioLineObjects[uniqueID] = object;
}

void DrawGridLayer::removeAudioLineObject(AudioLineGuideGameObject* object)
{
    int uniqueID = object->m_uniqueID;
    m_audioLineObjects.erase(uniqueID);
}

void DrawGridLayer::addToEffects(EffectGameObject* object)
{
    if (!m_effectGameObjects->containsObject(object))
    {
        m_effectGameObjects->addObject(object);
        m_sortEffects = true;
    }
}

void DrawGridLayer::removeFromEffects(EffectGameObject* object)
{
    m_effectGameObjects->removeObject(object);
    m_sortEffects = true;
}

void DrawGridLayer::addToGuides(GameObject* object)
{
    if (!m_guideObjects->containsObject(object))
        m_guideObjects->addObject(object);
}

void DrawGridLayer::removeFromGuides(GameObject* object)
{
    m_guideObjects->removeObject(object);
}

void DrawGridLayer::addToSpeedObjects(EffectGameObject* object)
{
    if (!m_speedObjects->containsObject(object))
    {
        m_speedObjects->addObject(object);
        m_updateSpeedObjects = true;
        object->updateSpeedModType();
    }
}

void DrawGridLayer::removeFromSpeedObjects(EffectGameObject* object)
{
    if (m_speedObjects->containsObject(object))
    {
        m_speedObjects->removeObject(object);
        m_updateSpeedObjects = true;
    }
}

void DrawGridLayer::sortSpeedObjects()
{
    LevelTools::sortSpeedObjects(m_speedObjects, m_editorLayer);
}

cocos2d::CCPoint DrawGridLayer::getPortalMinMax(GameObject* object)
{
    float height;
    
    if (object->getType() == BallPortal)
        height = 240.0f;
    else if (object->getType() == SpiderPortal)
        height = 270.0f;
    else
        height = 300.0f;
    
    float minY = floorf((object->getPosition().y - (height * 0.5f)) / 30.0f) * 30.0f;
    if (minY <= 90.0f)
        minY = 90.0f;
    
    return cocos2d::CCPoint(minY, minY + height);
}

cocos2d::CCPoint DrawGridLayer::posForTime(float time)
{
    return LevelTools::posForTime(
        time,
        m_speedObjects,
        m_editorLayer->m_levelSettings->m_startSpeed,
        m_editorLayer->m_levelSettings->m_platformerMode,
        &m_editorLayer->m_gameState.m_rotateChannel
    );
}

void DrawGridLayer::loadTimeMarkers(std::string markers)
{
    m_timeMarkerString = markers;
    
    if (m_timeMarkers)
    {
        m_timeMarkers->release();
        m_timeMarkers = nullptr;
    }
    
    if (markers.empty())
        return;
    
    // Set current speed based on start speed
    switch (m_editorLayer->m_levelSettings->m_startSpeed)
    {
        case Slow: m_currentSpeed = m_slowSpeed; break;
        case Fast: m_currentSpeed = m_fastSpeed; break;
        case Faster: m_currentSpeed = m_fasterSpeed; break;
        case Fastest: m_currentSpeed = m_fastestSpeed; break;
        default: m_currentSpeed = m_normalSpeed; break;
    }
    
    // Parse markers string (format: "time~multiplier,time~multiplier,...")
    std::vector<std::string> pairs = GameToolbox::splitString(markers, ',');
    
    m_timeMarkers = cocos2d::CCArray::createWithCapacity(pairs.size() * 2);
    m_timeMarkers->retain();
    
    float songOffset = m_editorLayer->m_levelSettings->m_songOffset;
    
    for (const std::string& pair : pairs)
    {
        std::vector<std::string> parts = GameToolbox::splitString(pair, '~');
        if (parts.size() < 2)
            continue;
        
        float time = atof(parts[0].c_str());
        float multiplier = atof(parts[1].c_str());
        
        if (multiplier == 0.0f)
            multiplier = 0.8f;
        else if (multiplier < 0.8f)
            continue;
        
        cocos2d::CCPoint pos = posForTime(time - songOffset);
        m_timeMarkers->addObject(cocos2d::CCString::createWithFormat("%.02f", pos.x));
        m_timeMarkers->addObject(cocos2d::CCString::createWithFormat("%.02f", multiplier));
    }
}

void DrawGridLayer::postUpdate()
{
    m_oldPlaybackTime = m_playbackTime;
}

void DrawGridLayer::updateMusicGuideTime(float time)
{
    m_musicTime = time;
    m_playbackTime = time;
    m_oldPlaybackTime = time;
}

void DrawGridLayer::updateTimeMarkers()
{
    m_updateTimeMarkers = true;
    m_updateSpeedObjects = false;
    sortSpeedObjects();
    loadTimeMarkers(m_timeMarkerString);
}

void DrawGridLayer::update(float dt)
{
    EditorUI* ui = m_editorLayer->m_editorUI;
    m_oldPlaybackTime = m_playbackTime;
    
    if (ui && ui->m_playbackActive)
    {
        FMODAudioEngine* audio = FMODAudioEngine::sharedEngine();
        bool isPlaying = audio->isMusicPlaying();
        
        if (m_musicTime == -1.0f)
            m_musicTime = ui->m_playbackStartWarp;
        
        LevelTools::posForTimeInternal(
            m_playbackTime,
            m_speedObjects,
            m_editorLayer->m_levelSettings->m_startSpeed,
            m_editorLayer->m_levelSettings->m_platformerMode,
            false,
            true,
            &m_editorLayer->m_gameState.m_rotateChannel,
            nullptr
        );
        
        float delta = dt * LevelTools::getLastTimewarp();
        float musicTime;
        
        if (isPlaying)
        {
            musicTime = audio->getMusicTime();
        }
        else
        {
            musicTime = delta + m_musicTime;
        }
        
        if (m_musicTime != -1.0f && fabsf(musicTime - m_musicTime) >= 1000.0f)
        {
            m_playbackTime = musicTime;
        }
        else
        {
            m_playbackTime = delta + m_playbackTime;
        }
        
        m_musicTime = musicTime;
    }
}

void DrawGridLayer::draw()
{
    if (!m_editorLayer || !m_editorLayer->m_editorUI)
        return;
    
    PlaybackMode playbackMode = m_editorLayer->m_playbackMode;
    int showGrid = 0;
    
    if (m_editorLayer->m_showGrid)
    {
        showGrid = 1;
        if (playbackMode == Playing)
            showGrid = !m_editorLayer->m_hideGridOnPlay;
    }
    
    bool gameVariable;
    int drawEffects = 0;
    
    if (playbackMode == Playing || !m_editorLayer->m_drawEffectLines)
    {
        gameVariable = GameManager::sharedState()->getGameVariable("0121");
        drawEffects = 0;
    }
    else
    {
        gameVariable = GameManager::sharedState()->getGameVariable("0121");
        if (m_sortEffects)
        {
            m_sortEffects = false;
            if (m_effectGameObjects && m_effectGameObjects->count() > 0)
            {
                drawEffects = 1;
                qsort(m_effectGameObjects->data->arr, m_effectGameObjects->data->num, 8, (int(*)(const void*, const void*))xCompReal);
            }
            else
            {
                drawEffects = 1;
            }
        }
        else
        {
            drawEffects = 1;
        }
    }
    
    cocos2d::ccGLBlendFunc(1, 771);
    
    float maxY = m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 30090.0f : 2490.0f;
    
    cocos2d::ccDrawColor4B(0, 0, 0, 150);
    glLineWidth(1.0f);
    
    unsigned int showGround = m_editorLayer->m_showGround;
    cocos2d::CCPoint layerPos = m_objectLayer->getPosition();
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Calculate visible area
    cocos2d::CCPoint zeroPos = m_objectLayer->convertToNodeSpace(cocos2d::CCPoint(0, 0));
    float x = zeroPos.x;
    float y = zeroPos.y;
    float scale = m_objectLayer->getScale();
    
    float left = (winSize.width / scale) + x;
    float right = x;
    float top = (winSize.height / scale) + y;
    float bottom = y;
    
    // Draw grid lines
    int startX = (int)floorf(right / m_gridSize);
    int startY = (int)floorf(bottom / m_gridSize);
    int pointCount = 0;
    
    for (int i = startX; i * m_gridSize <= left; i++)
    {
        float posX = i * m_gridSize;
        if (posX >= -3000.0f && posX <= 240000.0f)
        {
            if (posX == 240000.0f || posX == 0.0f)
            {
                cocos2d::ccDrawColor4B(255, 255, 255, 150);
                float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
                float y2 = (top >= maxY) ? maxY : top;
                cocos2d::ccDrawLine(cocos2d::CCPoint(posX, y1), cocos2d::CCPoint(posX, y2));
            }
            else if (pointCount <= 397 && showGrid)
            {
                float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
                float y2 = (top >= maxY) ? maxY : top;
                m_pointArray1[pointCount] = cocos2d::CCPoint(posX, y1);
                m_pointArray1[pointCount + 1] = cocos2d::CCPoint(posX, y2);
                pointCount += 2;
            }
        }
    }
    
    // Draw horizontal grid lines
    for (int i = startY; i * m_gridSize <= top; i++)
    {
        float posY = i * m_gridSize;
        if (posY < -3000.0f || posY > maxY)
            continue;
        
        if (posY != maxY && posY != 90.0f)
        {
            if (pointCount <= 397 && showGrid)
            {
                float x1 = (right <= -3000.0f) ? -3000.0f : right;
                float x2 = (left >= 240000.0f) ? 240000.0f : left;
                m_pointArray1[pointCount] = cocos2d::CCPoint(x1, posY);
                m_pointArray1[pointCount + 1] = cocos2d::CCPoint(x2, posY);
                pointCount += 2;
            }
        }
        else if (posY != 90.0f || showGround)
        {
            cocos2d::ccDrawColor4B(255, 255, 255, 150);
            float x1 = (right <= -3000.0f) ? -3000.0f : right;
            float x2 = (left >= 240000.0f) ? 240000.0f : left;
            cocos2d::ccDrawLine(cocos2d::CCPoint(x1, posY), cocos2d::CCPoint(x2, posY));
        }
    }
    
    if (pointCount > 0 && showGrid)
    {
        cocos2d::ccDrawColor4B(0, 0, 0, 150);
        glLineWidth(1.0f);
        cocos2d::ccDrawLines(&m_pointArray1[0], pointCount);
    }
    
    // Draw time markers
    if (GameManager::sharedState()->m_showSongMarkers && m_timeMarkers)
    {
        int count = m_timeMarkers->count();
        int p1 = 0, p2 = 0, p3 = 0;
        
        for (int i = 0; i < count; i += 2)
        {
            cocos2d::CCString* str = (cocos2d::CCString*)m_timeMarkers->objectAtIndex(i);
            float posX = str->floatValue();
            
            if (posX < -3000.0f)
                continue;
            
            if (posX > left || posX < right)
                continue;
            
            cocos2d::CCString* str2 = (cocos2d::CCString*)m_timeMarkers->objectAtIndex(i + 1);
            float multiplier = str2->floatValue();
            
            float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
            float y2 = (top >= maxY) ? maxY : top;
            
            if (multiplier == 0.9f)
            {
                if (p2 <= 397)
                {
                    m_pointArray2[p2] = cocos2d::CCPoint(posX, y1);
                    m_pointArray2[p2 + 1] = cocos2d::CCPoint(posX, y2);
                    p2 += 2;
                }
            }
            else if (multiplier == 1.0f)
            {
                if (p3 <= 397)
                {
                    m_pointArray3[p3] = cocos2d::CCPoint(posX, y1);
                    m_pointArray3[p3 + 1] = cocos2d::CCPoint(posX, y2);
                    p3 += 2;
                }
            }
            else
            {
                if (p1 <= 397)
                {
                    m_pointArray1[p1] = cocos2d::CCPoint(posX, y1);
                    m_pointArray1[p1 + 1] = cocos2d::CCPoint(posX, y2);
                    p1 += 2;
                }
            }
        }
        
        if (p1 > 0)
        {
            cocos2d::ccDrawColor4B(255, 127, 0, 255);
            glLineWidth(1.0f);
            cocos2d::ccDrawLines(&m_pointArray1[0], p1);
        }
        if (p2 > 0)
        {
            cocos2d::ccDrawColor4B(255, 255, 0, 255);
            glLineWidth(1.0f);
            cocos2d::ccDrawLines(&m_pointArray2[0], p2);
        }
        if (p3 > 0)
        {
            cocos2d::ccDrawColor4B(255, 255, 0, 255);
            glLineWidth(1.0f);
            cocos2d::ccDrawLines(&m_pointArray3[0], p3);
        }
    }
    
    // Draw audio line objects (speed markers)
    if (m_audioLineObjects.size() > 0)
    {
        int p1 = 0, p2 = 0;
        
        for (auto& pair : m_audioLineObjects)
        {
            AudioLineGuideGameObject* obj = pair.second;
            if (obj->m_isDisabled)
                continue;
            
            int speed = obj->m_speed;
            if (speed > 0 && (!gameVariable || !obj->m_isHide || obj->m_isSelected))
            {
                float speedVal;
                switch (speed)
                {
                    case 1: speedVal = m_slowSpeed; break;
                    case 2: speedVal = m_fastSpeed; break;
                    case 3: speedVal = m_fasterSpeed; break;
                    case 4: speedVal = m_fastestSpeed; break;
                    default: speedVal = m_normalSpeed; break;
                }
                
                int multiplier = obj->m_speedMultiplier;
                if (multiplier > 0)
                    speed *= multiplier;
                
                float spacing = (60.0f / speed) * speedVal;
                float startPos = obj->getPosition().x;
                float endPos = startPos + (obj->m_duration * speedVal);
                
                if (left < endPos)
                    endPos = left;
                
                int numLines = 0;
                if (right > startPos)
                {
                    numLines = (int)((right - startPos) / spacing);
                    startPos += numLines * spacing;
                }
                
                while (endPos > startPos)
                {
                    float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
                    float y2 = (top >= maxY) ? maxY : top;
                    
                    if (multiplier > 0 && numLines % multiplier)
                    {
                        if (p2 <= 397)
                        {
                            m_pointArray2[p2] = cocos2d::CCPoint(startPos, y1);
                            m_pointArray2[p2 + 1] = cocos2d::CCPoint(startPos, y2);
                            p2 += 2;
                        }
                    }
                    else
                    {
                        if (p1 <= 397)
                        {
                            m_pointArray1[p1] = cocos2d::CCPoint(startPos, y1);
                            m_pointArray1[p1 + 1] = cocos2d::CCPoint(startPos, y2);
                            p1 += 2;
                        }
                    }
                    
                    startPos += spacing;
                    numLines++;
                }
            }
        }
        
        if (p1 > 0)
        {
            cocos2d::ccDrawColor4B(255, 255, 0, 255);
            glLineWidth(1.0f);
            cocos2d::ccDrawLines(&m_pointArray1[0], p1);
        }
        if (p2 > 0)
        {
            cocos2d::ccDrawColor4B(255, 127, 0, 255);
            glLineWidth(1.0f);
            cocos2d::ccDrawLines(&m_pointArray2[0], p2);
        }
    }
    
    glLineWidth(1.0f);
    
    // Draw effect objects
    if (playbackMode != Playing && drawEffects)
    {
        cocos2d::ccDrawColor4B(0, 255, 255, 150);
        
        if (m_effectGameObjects && m_effectGameObjects->count() > 0)
        {
            int p1 = 0;
            float lastX = -1.0f;
            
            for (int i = 0; i < m_effectGameObjects->count(); i++)
            {
                EffectGameObject* obj = (EffectGameObject*)m_effectGameObjects->objectAtIndex(i);
                float posX = obj->getPosition().x;
                
                if (posX >= right && posX <= left)
                {
                    if (!GameObject::getGroupDisabled(obj) && (!gameVariable || !obj->m_isHide) && !obj->m_isDisabled && lastX < posX)
                    {
                        lastX = posX;
                        
                        if (p1 <= 397)
                        {
                            float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
                            float y2 = (top >= maxY) ? maxY : top;
                            m_pointArray1[p1] = cocos2d::CCPoint(posX, y1);
                            m_pointArray1[p1 + 1] = cocos2d::CCPoint(posX, y2);
                            p1 += 2;
                        }
                    }
                }
                else if (posX > left)
                {
                    break;
                }
            }
            
            if (p1 > 0)
            {
                cocos2d::ccDrawColor4B(0, 255, 255, 150);
                cocos2d::ccDrawLines(&m_pointArray1[0], p1);
            }
        }
        
        // Draw guide objects (portals)
        int guideCount = m_guideObjects->count();
        cocos2d::ccDrawColor4B(0, 255, 255, 255);
        
        for (int i = 0; i < guideCount; i++)
        {
            GameObject* obj = (GameObject*)m_guideObjects->objectAtIndex(i);
            if (!gameVariable || !obj->m_isHide || obj->m_isSelected)
            {
                cocos2d::CCPoint minMax = getPortalMinMax(obj);
                
                if (minMax.x >= -3000.0f && minMax.x <= maxY)
                {
                    float x1 = (right <= -3000.0f) ? -3000.0f : right;
                    float x2 = (left >= 240000.0f) ? 240000.0f : left;
                    cocos2d::ccDrawLine(cocos2d::CCPoint(x1, minMax.x), cocos2d::CCPoint(x2, minMax.x));
                }
                
                if (minMax.y >= -3000.0f && minMax.y <= maxY)
                {
                    float x1 = (right <= -3000.0f) ? -3000.0f : right;
                    float x2 = (left >= 240000.0f) ? 240000.0f : left;
                    cocos2d::ccDrawLine(cocos2d::CCPoint(x1, minMax.y), cocos2d::CCPoint(x2, minMax.y));
                }
            }
        }
    }
    
    // Draw playback position
    bool drawPlayback = false;
    
    if (playbackMode == Playing)
    {
        // Handle playing mode playback position
        m_playbackX = 0.0f;
        m_playbackY = 0.0f;
        
        if (m_editorLayer->m_gameState.m_isDualMode || PlayerObject::isFlying(m_editorLayer->m_player1) ||
            m_editorLayer->m_player1->m_isBall || m_editorLayer->m_player1->m_isSpider)
        {
            // Draw portal bounds
            float minY = GJBaseGameLayer::getMinPortalY(m_editorLayer);
            float maxY2 = GJBaseGameLayer::getMaxPortalY(m_editorLayer);
            cocos2d::ccDrawColor4B(255, 175, 50, 255);
            
            if (minY != 90.0f)
            {
                float x1 = (right <= -3000.0f) ? -3000.0f : right;
                float x2 = (left >= 240000.0f) ? 240000.0f : left;
                cocos2d::ccDrawLine(cocos2d::CCPoint(x1, minY), cocos2d::CCPoint(x2, minY));
            }
            
            float x1 = (right <= -3000.0f) ? -3000.0f : right;
            float x2 = (left >= 240000.0f) ? 240000.0f : left;
            cocos2d::ccDrawLine(cocos2d::CCPoint(x1, maxY2), cocos2d::CCPoint(x2, maxY2));
        }
    }
    else if (playbackMode == None && !m_editorLayer->m_editorUI->m_playbackActive)
    {
        cocos2d::ccDrawColor4B(50, 255, 50, 100);
        glLineWidth(1.0f);
        m_musicTime = -1.0f;
        
        if (left >= m_playbackX && m_playbackX > 0.0f && right <= m_playbackX)
        {
            drawPlayback = true;
        }
    }
    else if (m_editorLayer->m_editorUI->m_playbackActive)
    {
        cocos2d::ccDrawColor4B(50, 255, 50, 255);
        glLineWidth(1.0f);
        
        cocos2d::CCPoint pos;
        LevelTools::posForTimeInternal(
            m_playbackTime,
            m_speedObjects,
            m_editorLayer->m_levelSettings->m_startSpeed,
            m_editorLayer->m_levelSettings->m_platformerMode,
            true,
            true,
            &m_editorLayer->m_gameState.m_rotateChannel,
            nullptr
        );
        
        m_playbackX = pos.x;
        m_playbackY = 0.0f;
        
        if (pos.x > 0.0f)
        {
            m_playbackX = pos.x;
            m_playbackY = 0.0f;
            drawPlayback = (right <= m_playbackX && left >= m_playbackX);
        }
        else if (pos.y > 0.0f)
        {
            m_playbackX = 0.0f;
            m_playbackY = pos.y;
            // Draw horizontal line
        }
        else
        {
            drawPlayback = (left >= m_playbackX && right <= m_playbackX && m_playbackX > 0.0f);
        }
    }
    
    if (drawPlayback)
    {
        float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
        float y2 = (top >= maxY) ? maxY : top;
        cocos2d::ccDrawLine(cocos2d::CCPoint(m_playbackX, y1), cocos2d::CCPoint(m_playbackX, y2));
    }
    else if (m_playbackY > 0.0f && bottom <= m_playbackY && top >= m_playbackY)
    {
        float x1 = (right <= -3000.0f) ? -3000.0f : right;
        float x2 = (left >= 240000.0f) ? 240000.0f : left;
        cocos2d::ccDrawLine(cocos2d::CCPoint(x1, m_playbackY), cocos2d::CCPoint(x2, m_playbackY));
    }
    
    // Draw preview position
    cocos2d::CCPoint previewPos = m_editorLayer->m_previewPosition;
    if ((previewPos.x != 0.0f || previewPos.y != 0.0f) && previewPos.x > 0.0f && right <= previewPos.x && left >= previewPos.x)
    {
        cocos2d::ccDrawColor4B(255, 150, 0, 255);
        glLineWidth(1.0f);
        float y1 = (bottom <= -3000.0f) ? -3000.0f : bottom;
        float y2 = (top >= maxY) ? maxY : top;
        cocos2d::ccDrawLine(cocos2d::CCPoint(previewPos.x, y1), cocos2d::CCPoint(previewPos.x, y2));
    }
    
    // Draw duration lines
    if (playbackMode != Playing && m_editorLayer->m_showDurationLines)
    {
        cocos2d::CCArray* durationObjects = m_editorLayer->m_durationObjects;
        if (durationObjects && durationObjects->count() > 0)
        {
            int p1 = 0;
            
            for (int i = 0; i < durationObjects->count(); i++)
            {
                cocos2d::CCObject* obj = durationObjects->objectAtIndex(i);
                if (gameVariable && obj[22].m_nZOrder && !obj[19].m_unknown5)
                    continue;
                
                float duration = 0.0f;
                // Get duration based on object type
                // ... (complex duration calculation from decompiled code)
                
                if (duration > 0.0f)
                {
                    cocos2d::CCPoint pos = obj->getPosition();
                    if (left >= pos.x && pos.y <= maxY)
                    {
                        cocos2d::CCPoint endPos = pos;
                        // Calculate end position
                        // ... (complex position calculation from decompiled code)
                        
                        if (right <= endPos.x && bottom <= endPos.y && p1 <= 397)
                        {
                            m_pointArray1[p1] = cocos2d::CCPoint(
                                (right >= pos.x) ? right : pos.x,
                                (bottom >= pos.y) ? bottom : pos.y
                            );
                            m_pointArray1[p1 + 1] = cocos2d::CCPoint(
                                (left <= endPos.x) ? left : endPos.x,
                                (top <= endPos.y) ? top : endPos.y
                            );
                            p1 += 2;
                        }
                    }
                }
            }
            
            if (p1 > 0)
            {
                glLineWidth(1.0f);
                cocos2d::ccDrawColor4B(100, 100, 100, 75);
                cocos2d::ccDrawLines(&m_pointArray1[0], p1);
            }
        }
    }
    
    glLineWidth(1.0f);
    m_updateTimeMarkers = false;
}

DrawGridLayer::~DrawGridLayer()
{
    CC_SAFE_RELEASE(m_effectGameObjects);
    CC_SAFE_RELEASE(m_guideObjects);
    CC_SAFE_RELEASE(m_speedObjects);
    CC_SAFE_RELEASE(m_timeMarkers);
}