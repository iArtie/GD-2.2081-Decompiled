#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>

class GameObject;
class FLAlertLayer;
class CCMenuItemToggler;

class GameToolbox
{
public:
    static void addBackButton(cocos2d::CCLayer* parent, cocos2d::CCMenuItem* menuItem);
    static void addRThumbScrollButton(cocos2d::CCLayer* parent);
    static void alignItemsVertically(cocos2d::CCArray* items, float gap, cocos2d::CCPoint position);
    static void alignItemsHorizontally(cocos2d::CCArray* items, float gap, cocos2d::CCPoint position);
    static float bounceTime(float time);
    static void contentScaleClipRect(cocos2d::CCRect* rect);
    static std::string createHashString(const std::string& str, int length);
    static CCMenuItemToggler* createToggleButton(const std::string& label, cocos2d::SEL_MenuHandler callback, int tag, bool initialValue, cocos2d::CCMenu* menu, cocos2d::CCPoint position, cocos2d::CCObject* target, cocos2d::CCLayer* layer);
    static CCMenuItemToggler* createToggleButton(const std::string& label, cocos2d::SEL_MenuHandler callback, int tag, bool initialValue, cocos2d::CCMenu* menu, cocos2d::CCPoint position, cocos2d::CCObject* target, cocos2d::CCLayer* layer, float scale, float labelScale, float maxWidth, cocos2d::CCPoint labelOffset, const char* font, bool useCheckbox, bool unknown, cocos2d::CCArray* container);
    static bool doWeHaveInternet();
    static std::string easeToText(int easingType);
    static uint64_t fast_rand();
    static float fast_rand_0_1();
    static float fast_rand_minus1_1();
    static void fast_srand(uint64_t seed);
    static std::string gen_random(int length);
    static float getEasedValue(float value, int easingType, float easingRate);
    static int getInvertedEasing(int easingType);
    static cocos2d::CCPoint getRelativeOffset(GameObject* object, cocos2d::CCPoint offset);
    static std::string getResponse(cocos2d::extension::CCHttpResponse* response);
    static std::string getTimeString(int seconds, bool noSeconds);
    static uint64_t getfast_srand();
    static std::string intToShortString(int value);
    static std::string intToString(int value);
    static bool isIOS();
    static bool isRateEasing(int easingType);
    static void mergeDictsSaveLargestInt(cocos2d::CCDictionary* toDict, cocos2d::CCDictionary* fromDict);
    static void mergeDictsSkipConflict(cocos2d::CCDictionary* toDict, cocos2d::CCDictionary* fromDict);
    static std::string msToTimeString(int milliseconds, int formattingMode);
    static void openRateURL(const std::string& str1, const std::string& str2);
    static void particleStringToStruct(const std::string* str, cocos2d::ParticleStruct* particleStruct);
    static std::string pointsToString(int points);
    static void postClipVisit();
    static void preVisitWithClipRect(cocos2d::CCRect rect);
    static std::string saveParticleToString(cocos2d::CCParticleSystemQuad* system);
    static bool saveStringToFile(const std::string& path, const std::string& content);
    static std::string stringFromHSV(cocos2d::ccHSVValue value, const char* separator);
    static cocos2d::CCDictionary* stringSetupToDict(const std::string& str, const char* separator);
    static cocos2d::ccColor3B strongColor(cocos2d::ccColor3B color);
    static std::string timestampToHumanReadable(time_t timestamp);
    static cocos2d::ccColor3B transformColor(const cocos2d::ccColor3B& color, cocos2d::ccHSVValue hsv);

private:
    static uint64_t s_randSeed;
};