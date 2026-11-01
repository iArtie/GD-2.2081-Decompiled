#include "Utils/GameToolbox.h"
#include "Layers/FLAlertLayer.h"
#include "Utils/PlatformToolbox.h"

uint64_t GameToolbox::s_randSeed = 0;

void GameToolbox::addBackButton(cocos2d::CCLayer* parent, cocos2d::CCMenuItem* menuItem)
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png");
    parent->addChild(sprite);
    
    cocos2d::CCPoint pos = menuItem->getParent()->convertToWorldSpace(menuItem->getPosition());
    pos = parent->convertToNodeSpace(pos);
    sprite->setPosition(pos + cocos2d::CCPoint(28.0f, 10.0f));
    sprite->setScale(0.7f);
}

void GameToolbox::addRThumbScrollButton(cocos2d::CCLayer* parent)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    cocos2d::CCSprite* thumb = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_RThumb_001.png");
    parent->addChild(thumb);
    thumb->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f, winSize.height * 0.5f + 80.0f));
    
    cocos2d::CCSprite* upBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
    parent->addChild(upBtn);
    upBtn->setPosition(thumb->getPosition() + cocos2d::CCPoint(0.0f, 22.0f));
    upBtn->setScale(0.7f);
    
    cocos2d::CCSprite* downBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    parent->addChild(downBtn);
    downBtn->setPosition(thumb->getPosition() + cocos2d::CCPoint(0.0f, -22.0f));
    downBtn->setScale(0.7f);
}

void GameToolbox::alignItemsVertically(cocos2d::CCArray* items, float gap, cocos2d::CCPoint position)
{
    if (!items || items->count() == 0)
    {
        return;
    }
    
    float totalHeight = -gap;
    for (unsigned int i = 0; i < items->count(); i++)
    {
        cocos2d::CCNode* item = (cocos2d::CCNode*)items->objectAtIndex(i);
        if (item)
        {
            totalHeight += gap + item->getContentSize().height * item->getScaleY();
        }
    }
    
    float yOffset = -totalHeight * 0.5f;
    for (unsigned int i = 0; i < items->count(); i++)
    {
        cocos2d::CCNode* item = (cocos2d::CCNode*)items->objectAtIndex(i);
        if (item)
        {
            cocos2d::CCSize size = item->getContentSize();
            item->setPosition(position + cocos2d::CCPoint(0.0f, yOffset + size.height * item->getScaleY() * 0.5f));
            yOffset += gap + size.height * item->getScaleY();
        }
    }
}

void GameToolbox::alignItemsHorizontally(cocos2d::CCArray* items, float gap, cocos2d::CCPoint position)
{
    if (!items || items->count() == 0)
    {
        return;
    }
    
    float totalWidth = -gap;
    for (unsigned int i = 0; i < items->count(); i++)
    {
        cocos2d::CCNode* item = (cocos2d::CCNode*)items->objectAtIndex(i);
        if (item)
        {
            totalWidth += gap + item->getContentSize().width * item->getScaleX();
        }
    }
    
    float xOffset = -totalWidth * 0.5f;
    for (unsigned int i = 0; i < items->count(); i++)
    {
        cocos2d::CCNode* item = (cocos2d::CCNode*)items->objectAtIndex(i);
        if (item)
        {
            cocos2d::CCSize size = item->getContentSize();
            item->setPosition(position + cocos2d::CCPoint(xOffset + size.width * item->getScaleX() * 0.5f, 0.0f));
            xOffset += gap + size.width * item->getScaleX();
        }
    }
}

float GameToolbox::bounceTime(float time)
{
    if (time < 0.363636364f)
    {
        return time * 7.5625f * time;
    }
    if (time < 0.727272727f)
    {
        float t = time - 0.54545f;
        return t * 7.5625f * t + 0.75f;
    }
    if (time < 0.909090909f)
    {
        float t = time - 0.81818f;
        return t * 7.5625f * t + 0.9375f;
    }
    float t = time - 0.95455f;
    return t * 7.5625f * t + 0.98438f;
}

void GameToolbox::contentScaleClipRect(cocos2d::CCRect* rect)
{
}

std::string GameToolbox::createHashString(const std::string& str, int length)
{
    if (length <= 0)
    {
        return "";
    }
    
    std::string result;
    int strLen = (int)str.length();
    int step = strLen / length;
    
    for (int i = 0; i < length; i++)
    {
        int idx = i * step;
        if (idx < strLen)
        {
            result += str[idx];
        }
    }
    
    return result;
}

cocos2d::CCMenuItemToggler* GameToolbox::createToggleButton(
    const std::string& label,
    cocos2d::SEL_MenuHandler callback,
    int tag,
    bool initialValue,
    cocos2d::CCMenu* menu,
    cocos2d::CCPoint position,
    cocos2d::CCObject* target,
    cocos2d::CCLayer* layer)
{
    cocos2d::CCPoint labelOffset(8.0f, 0.0f);
    return createToggleButton(
        label,
        callback,
        tag,
        initialValue,
        menu,
        position,
        target,
        layer,
        0.7f,
        0.5f,
        80.0f,
        labelOffset,
        "bigFont.fnt",
        false,
        false,
        nullptr
    );
}

cocos2d::CCMenuItemToggler* GameToolbox::createToggleButton(
    const std::string& label,
    cocos2d::SEL_MenuHandler callback,
    int tag,
    bool initialValue,
    cocos2d::CCMenu* menu,
    cocos2d::CCPoint position,
    cocos2d::CCObject* target,
    cocos2d::CCLayer* layer,
    float scale,
    float labelScale,
    float maxWidth,
    cocos2d::CCPoint labelOffset,
    const char* font,
    bool useCheckbox,
    bool unknown,
    cocos2d::CCArray* container)
{
    cocos2d::CCSprite* checkOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    cocos2d::CCSprite* checkOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    CCMenuItemToggler* toggler = CCMenuItemToggler::create(checkOff, checkOn, target, callback);
    toggler->setTag(tag);
    toggler->setScale(scale * labelScale);
    toggler->toggle(initialValue);

    if (menu) {
        menu->addChild(toggler);
        if (position.x != 0.0f || position.y != 0.0f) {
            toggler->setPosition(menu->convertToNodeSpace(position));
        }
    }

    if (layer && !label.empty()) {
        cocos2d::CCLabelBMFont* labelNode = cocos2d::CCLabelBMFont::create(label.c_str(), font);
        labelNode->setScale(scale);
        labelNode->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        layer->addChild(labelNode);
        labelNode->setPosition(position + labelOffset);
        labelNode->limitLabelWidth(maxWidth, labelScale, 0.0f);
    }

    if (container) {
        container->addObject(toggler);
    }

    return toggler;
}

bool GameToolbox::doWeHaveInternet()
{
    if (!PlatformToolbox::isNetworkAvailable())
    {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "No Internet Connection.");
        alert->show();
        return false;
    }
    return true;
}

std::string GameToolbox::easeToText(int easingType)
{
    switch (easingType)
    {
        case 0: return "None";
        case 1: return "Ease In Out";
        case 2: return "Ease In";
        case 3: return "Ease Out";
        case 4: return "Elastic In Out";
        case 5: return "Elastic In";
        case 6: return "Elastic Out";
        case 7: return "Bounce In Out";
        case 8: return "Bounce In";
        case 9: return "Bounce Out";
        case 10: return "Exponential In Out";
        case 11: return "Exponential In";
        case 12: return "Exponential Out";
        case 13: return "Sine In Out";
        case 14: return "Sine In";
        case 15: return "Sine Out";
        case 16: return "Back In Out";
        case 17: return "Back In";
        case 18: return "Back Out";
        default: return "None";
    }
}

uint64_t GameToolbox::fast_rand()
{
    s_randSeed = 214013 * s_randSeed + 2531011;
    return ((s_randSeed >> 16) & 0x7FFF);
}

float GameToolbox::fast_rand_0_1()
{
    return (float)fast_rand() / 32767.0f;
}

float GameToolbox::fast_rand_minus1_1()
{
    return fast_rand_0_1() * 2.0f - 1.0f;
}

void GameToolbox::fast_srand(uint64_t seed)
{
    s_randSeed = seed;
}

std::string GameToolbox::gen_random(int length)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; i++)
    {
        result += charset[rand() % (sizeof(charset) - 1)];
    }
    
    return result;
}

float GameToolbox::getEasedValue(float value, int easingType, float easingRate)
{
    if (easingType == 0)
    {
        return value;
    }
    
    if (easingRate <= 0.0f)
    {
        easingRate = 2.0f;
    }
    
    switch (easingType)
    {
        case 1: // Ease In Out
        {
            float t = value * 2.0f;
            if (t < 1.0f)
            {
                return powf(2.0f, t * 10.0f) * 0.5f;
            }
            return (2.0f - powf(2.0f, (t - 1.0f) * -10.0f)) * 0.5f;
        }
        case 2: // Ease In
            return powf(value, easingRate);
        case 3: // Ease Out
            return powf(value, 1.0f / easingRate);
        default:
            return value;
    }
}

int GameToolbox::getInvertedEasing(int easingType)
{
    switch (easingType)
    {
        case 2: return 3;
        case 3: return 2;
        case 5: return 6;
        case 6: return 5;
        case 8: return 9;
        case 9: return 8;
        case 11: return 12;
        case 12: return 11;
        case 14: return 15;
        case 15: return 14;
        case 17: return 18;
        case 18: return 17;
        default: return easingType;
    }
}

cocos2d::CCPoint GameToolbox::getRelativeOffset(GameObject* object, cocos2d::CCPoint offset)
{
    return offset;
}

std::string GameToolbox::getResponse(cocos2d::extension::CCHttpResponse* response)
{
    if (!response || !response->_succeed)
    {
        return "-1";
    }
    
    std::string result((char*)response->_responseData.m_start, response->_responseData.m_finish - response->_responseData.m_start);
    if (result.empty())
    {
        return "-1";
    }
    
    return result;
}

std::string GameToolbox::getTimeString(int seconds, bool noSeconds)
{
    int days = seconds / 86400;
    seconds %= 86400;
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;
    
    if (days + hours + minutes + seconds == 0)
    {
        return cocos2d::CCString::createWithFormat("%isec", seconds)->getCString();
    }
    if (days + hours == 0)
    {
        if (noSeconds)
        {
            return cocos2d::CCString::createWithFormat("%imin", minutes)->getCString();
        }
        return cocos2d::CCString::createWithFormat("%imin %02dsec", minutes, seconds)->getCString();
    }
    if (days == 0)
    {
        return cocos2d::CCString::createWithFormat("%ih %imin", hours, minutes)->getCString();
    }
    return cocos2d::CCString::createWithFormat("%idays %ih", days, hours)->getCString();
}

uint64_t GameToolbox::getfast_srand()
{
    return s_randSeed;
}

std::string GameToolbox::intToShortString(int value)
{
    if (value < 0)
    {
        return "-" + intToShortString(-value);
    }
    if (value <= 999)
    {
        return intToString(value);
    }
    if (value <= 9999)
    {
        return cocos2d::CCString::createWithFormat("%.1fK", value / 1000.0f)->getCString();
    }
    if (value <= 999999)
    {
        return cocos2d::CCString::createWithFormat("%.0fK", value / 1000.0f)->getCString();
    }
    return cocos2d::CCString::createWithFormat("%.1fM", value / 1000000.0f)->getCString();
}

std::string GameToolbox::intToString(int value)
{
    return cocos2d::CCString::createWithFormat("%i", value)->getCString();
}

bool GameToolbox::isIOS()
{
    return false;
}

bool GameToolbox::isRateEasing(int easingType)
{
    return easingType >= 1 && easingType <= 6;
}

void GameToolbox::mergeDictsSaveLargestInt(cocos2d::CCDictionary* toDict, cocos2d::CCDictionary* fromDict)
{
    if (!toDict || !fromDict)
    {
        return;
    }
    
    cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)fromDict->m_pElements;
    while (element)
    {
        std::string key = element->getKey();
        cocos2d::CCString* fromValue = (cocos2d::CCString*)fromDict->objectForKey(key);
        cocos2d::CCString* toValue = (cocos2d::CCString*)toDict->objectForKey(key);
        
        if (fromValue)
        {
            if (!toValue || fromValue->intValue() > toValue->intValue())
            {
                toDict->setObject(fromValue, key);
            }
        }
        
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
}

void GameToolbox::mergeDictsSkipConflict(cocos2d::CCDictionary* toDict, cocos2d::CCDictionary* fromDict)
{
    if (!toDict || !fromDict)
    {
        return;
    }
    
    cocos2d::CCDictElement* element = (cocos2d::CCDictElement*)fromDict->m_pElements;
    while (element)
    {
        std::string key = element->getKey();
        cocos2d::CCObject* fromValue = fromDict->objectForKey(key);
        
        if (fromValue && !toDict->objectForKey(key))
        {
            toDict->setObject(fromValue, key);
        }
        
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
}

std::string GameToolbox::msToTimeString(int milliseconds, int formattingMode)
{
    int hours = milliseconds / 1000 / 3600;
    int minutes = milliseconds / 1000 / 60 % 60;
    int seconds = milliseconds / 1000 % 60;
    int ms = milliseconds % 1000;
    
    if (hours > 0 || formattingMode == 2)
    {
        return cocos2d::CCString::createWithFormat("%i:%02d:%02d.%03d", hours, minutes, seconds, ms)->getCString();
    }
    if (minutes > 0)
    {
        return cocos2d::CCString::createWithFormat("%i:%02d.%03d", minutes, seconds, ms)->getCString();
    }
    return cocos2d::CCString::createWithFormat("%i.%03d", seconds, ms)->getCString();
}

void GameToolbox::openRateURL(const std::string& str1, const std::string& str2)
{
    std::string url = "https://example.com/rate?app=" + str1 + "&id=" + str2;
    cocos2d::CCApplication::sharedApplication()->openURL(url.c_str());
}

void GameToolbox::particleStringToStruct(const std::string* str, cocos2d::ParticleStruct* particleStruct)
{
}

std::string GameToolbox::pointsToString(int points)
{
    return intToShortString(points);
}

void GameToolbox::postClipVisit()
{
    glDisable(GL_SCISSOR_TEST);
}

void GameToolbox::preVisitWithClipRect(cocos2d::CCRect rect)
{
    glEnable(GL_SCISSOR_TEST);
    cocos2d::CCEGLView* view = cocos2d::CCEGLView::sharedOpenGLView();
    view->setScissorInPoints(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

std::string GameToolbox::saveParticleToString(cocos2d::CCParticleSystemQuad* system)
{
    std::string result;
    if (!system)
    {
        return result;
    }
    
    result = cocos2d::CCString::createWithFormat("%i", system->getTotalParticles())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%f", system->getDuration())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%f", system->getLife())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%f", system->getLifeVar())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getEmissionRate())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getAngle())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getAngleVar())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getSpeed())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getSpeedVar())->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getPosVar()->x)->getCString();
    result += ",";
    result += cocos2d::CCString::createWithFormat("%i", (int)system->getPosVar()->y)->getCString();
    
    return result;
}

bool GameToolbox::saveStringToFile(const std::string& path, const std::string& content)
{
    std::string fullPath = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + path;
    FILE* file = fopen(fullPath.c_str(), "w");
    if (!file)
    {
        return false;
    }
    
    size_t written = fwrite(content.c_str(), 1, content.length(), file);
    fclose(file);
    return written == content.length();
}

std::string GameToolbox::stringFromHSV(cocos2d::ccHSVValue value, const char* separator)
{
    std::string result = cocos2d::CCString::createWithFormat("%f", value.h)->getCString();
    if (separator)
    {
        result += separator;
    }
    result += cocos2d::CCString::createWithFormat("%f", value.s)->getCString();
    if (separator)
    {
        result += separator;
    }
    result += cocos2d::CCString::createWithFormat("%f", value.v)->getCString();
    if (separator)
    {
        result += separator;
    }
    result += cocos2d::CCString::createWithFormat("%f", (float)value.unk)->getCString();
    return result;
}

cocos2d::CCDictionary* GameToolbox::stringSetupToDict(const std::string& str, const char* separator)
{
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    
    std::string s = str;
    size_t pos = 0;
    std::vector<std::string> tokens;
    
    while ((pos = s.find(separator)) != std::string::npos)
    {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + strlen(separator));
    }
    tokens.push_back(s);
    
    for (unsigned int i = 0; i + 1 < tokens.size(); i += 2)
    {
        dict->setObject(cocos2d::CCString::create(tokens[i + 1]), tokens[i]);
    }
    
    return dict;
}

cocos2d::ccColor3B GameToolbox::strongColor(cocos2d::ccColor3B color)
{
    if (color.r == 255 || color.g == 255 || color.b == 255)
    {
        return color;
    }
    
    int maxVal = std::max(color.r, std::max(color.g, color.b));
    float scale = 255.0f / maxVal;
    if (scale > 1.5f)
    {
        scale = 1.5f;
    }
    
    return cocos2d::ccc3(
        (unsigned char)(color.r * scale),
        (unsigned char)(color.g * scale),
        (unsigned char)(color.b * scale)
    );
}

std::string GameToolbox::timestampToHumanReadable(time_t timestamp)
{
    time_t now = time(nullptr);
    double diff = difftime(now, timestamp);
    
    if (diff < 60)
    {
        return "Less than 1 minute ago";
    }
    if (diff < 3600)
    {
        int minutes = (int)(diff / 60);
        return minutes <= 1 ? "1 minute ago" : intToString(minutes) + " minutes ago";
    }
    if (diff < 86400)
    {
        int hours = (int)(diff / 3600);
        return hours <= 1 ? "1 hour ago" : intToString(hours) + " hours ago";
    }
    if (diff < 2592000)
    {
        int days = (int)(diff / 86400);
        return days <= 1 ? "1 day ago" : intToString(days) + " days ago";
    }
    if (diff < 31536000)
    {
        int months = (int)(diff / 2592000);
        return months <= 1 ? "1 month ago" : intToString(months) + " months ago";
    }
    int years = (int)(diff / 31536000);
    return years <= 1 ? "1 year ago" : intToString(years) + " years ago";
}

cocos2d::ccColor3B GameToolbox::transformColor(const cocos2d::ccColor3B& color, cocos2d::ccHSVValue hsv)
{
    return color;
}