#pragma once

#include "Platform/cocos2d.h"
#include <string>
#include <vector>
#include <map>

class DS_Dictionary;
class GJSmartPrefab;
class SmartGameObject;

enum class SmartBlockType {
    Block = 48,
    NormalSlope1 = 49,
    NormalSlope2 = 50,
    NormalSlope3 = 51,
    NormalSlope4 = 52,
    WideSlope1 = 65,
    WideSlope2 = 66,
    WideSlope3 = 67,
    WideSlope4 = 68,
    WideSlope5 = 69,
    WideSlope6 = 70,
    WideSlope7 = 71,
    WideSlope8 = 72,
    WideSlope9 = 73,
    WideSlope10 = 74,
    WideSlope11 = 75,
    WideSlope12 = 76,
    WideSlope13 = 77,
    WideSlope14 = 78,
    WideSlope15 = 79,
    WideSlope16 = 80
};

enum class GJSmartDirection {
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4,
    UpLeft = 5,
    UpRight = 6,
    DownLeft = 7,
    DownRight = 8
};

struct SmartPrefabResult {
    GJSmartPrefab* m_smartPrefab;
    std::string m_binaryKey;
    std::string m_prefabKey;
    int m_prefabCount;
    bool m_unrequired;
    int m_rotation;
    bool m_flipX;
    bool m_flipY;
    bool m_ignoreCorners;
};

class GJSmartTemplate : public cocos2d::CCObject {
public:
    static void applyTransformationsForType(SmartBlockType type, cocos2d::CCSprite* sprite);
    bool canEncode();
    static GJSmartTemplate* create();
    static GJSmartTemplate* createWithCoder(DS_Dictionary* dict);
    void dataLoaded(DS_Dictionary* dict);
    void encodeWithCoder(DS_Dictionary* dict);
    static SmartBlockType flipBlockType(SmartBlockType type, bool flipX, bool flipY);
    static SmartBlockType flipBlockTypeX(SmartBlockType type);
    static SmartBlockType flipBlockTypeY(SmartBlockType type);
    static std::string flipKey(const std::string& key, bool flipX, bool flipY);
    static cocos2d::CCDictionary* generateRemapDict();
    std::string getNoCornerKey(const std::string& key);
    GJSmartPrefab* getPrefab(const std::string& key, bool flipX, bool flipY);
    GJSmartPrefab* getPrefabWithID(const std::string& key, int id);
    cocos2d::CCArray* getPrefabs(const std::string& key);
    GJSmartPrefab* getRandomPrefab(const std::string& key);
    static std::string getSimplifiedKey(const std::string& key);
    static SmartBlockType getSimplifiedType(SmartBlockType type, bool* changed);
    void getTemplateState(std::vector<SmartPrefabResult>* results);
    int getTotalChanceForPrefab(const std::string& key);
    static std::string getVerySimplifiedKey(const std::string& key);
    bool init() override;
    bool isUnrequired(const std::string& key);
    std::string logTemplateStatus(bool unused);
    static cocos2d::CCPoint offsetForDir(GJSmartDirection direction, int key);
    static cocos2d::CCPoint offsetForObject(SmartGameObject* object);
    static cocos2d::CCPoint offsetForType(SmartBlockType type);
    void removePrefab(const std::string& prefabKey, int prefabID);
    void resetScannedPrefabs();
    static SmartBlockType rotateBlockType(SmartBlockType type, int degrees);
    static SmartBlockType rotateBlockType90(SmartBlockType type);
    static std::string rotateKey(const std::string& key, int degrees);
    void savePrefab(const std::string& key, const std::string& data);
    void saveRemap(const std::string& key);
    static bool shouldDiscardObject(SmartBlockType type, GJSmartDirection direction);
    static int smartTypeToObjectKey(SmartBlockType type);
    virtual ~GJSmartTemplate();

    std::string m_nameIndex;
    std::string m_templateName;
    bool m_allowRotation;
    bool m_allowFlipX;
    bool m_allowFlipY;
    bool m_ignoreCorners;
    cocos2d::CCDictionary* m_prefabArrays;
    cocos2d::CCDictionary* m_remapDict1;
    cocos2d::CCDictionary* m_remapDict2;
    cocos2d::CCDictionary* m_remapDict3;
    cocos2d::CCDictionary* m_remapDict4;
    cocos2d::CCDictionary* m_remapDict5;
    cocos2d::CCDictionary* m_remapDict6;
    std::string m_unk0a8;
    std::map<std::string, SmartPrefabResult> m_prefabResults;
};