#include "Managers/ObjectManager.h"
#include "Managers/SpriteAnimationManager.h"

ObjectManager* ObjectManager::instance()
{
    static ObjectManager* instance = nullptr;
    if (!instance)
    {
        instance = new ObjectManager();
        instance->init();
    }
    return instance;
}

bool ObjectManager::init()
{
    m_objectDefinitions = cocos2d::CCDictionary::createWithContentsOfFile("objectDefinitions.plist");
    m_objectDefinitions->retain();

    loadCopiedAnimations();
    loadCopiedSets();

    m_loadedAnimations = cocos2d::CCDictionary::create();
    m_loadedAnimations->retain();

    return true;
}

void ObjectManager::loadCopiedAnimations()
{
    unsigned int i = 0;
    cocos2d::CCArray* keys = m_objectDefinitions->allKeys();
    while (i < keys->count())
    {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        std::string keyStr = key->getCString();

        cocos2d::CCDictionary* objectDef = (cocos2d::CCDictionary*)m_objectDefinitions->objectForKey(keyStr);
        cocos2d::CCDictionary* animations = (cocos2d::CCDictionary*)objectDef->objectForKey("animations");

        if (animations)
        {
            unsigned int j = 0;
            cocos2d::CCArray* animKeys = animations->allKeys();
            while (j < animKeys->count())
            {
                cocos2d::CCString* animKey = (cocos2d::CCString*)animKeys->objectAtIndex(j);
                std::string animKeyStr = animKey->getCString();

                cocos2d::CCDictionary* animDef = (cocos2d::CCDictionary*)animations->objectForKey(animKeyStr);
                cocos2d::CCString* copy = (cocos2d::CCString*)animDef->objectForKey("copy");

                if (copy)
                {
                    cocos2d::CCDictionary* globalCopy = getGlobalAnimCopy(copy->getCString());
                    animations->setObject(globalCopy, animKeyStr);
                }
                j++;
            }
        }
        i++;
    }
}

void ObjectManager::loadCopiedSets()
{
    unsigned int i = 0;
    cocos2d::CCArray* keys = m_objectDefinitions->allKeys();
    while (i < keys->count())
    {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        std::string keyStr = key->getCString();

        cocos2d::CCDictionary* objectDef = (cocos2d::CCDictionary*)m_objectDefinitions->objectForKey(keyStr);
        cocos2d::CCString* copy = (cocos2d::CCString*)objectDef->objectForKey("copy");
        cocos2d::CCString* replaceTag = (cocos2d::CCString*)objectDef->objectForKey("replaceTag");

        if (replaceTag)
        {
            if (copy)
            {
                std::string copyStr = copy->getCString();
                objectDef = (cocos2d::CCDictionary*)m_objectDefinitions->objectForKey(copyStr);
            }

            cocos2d::CCString* xyz = cocos2d::CCString::create("xyz");
            cocos2d::CCDictionary* replaced = ObjectManager::replaceAllOccurencesOfString(xyz, replaceTag, objectDef);
            m_objectDefinitions->setObject(replaced, keyStr);
        }
        i++;
    }
}

bool ObjectManager::animLoaded(const char* anim)
{
    std::string key = anim;
    return m_loadedAnimations->objectForKey(key) != nullptr;
}

cocos2d::CCDictionary* ObjectManager::getDefinition(const char* definitionKey)
{
    std::string key = definitionKey;
    return (cocos2d::CCDictionary*)m_objectDefinitions->objectForKey(key);
}

cocos2d::CCDictionary* ObjectManager::getGlobalAnimCopy(const char* anim)
{
    std::string globalKey = "GlobalAnimations";
    cocos2d::CCDictionary* globalAnims = (cocos2d::CCDictionary*)m_objectDefinitions->objectForKey(globalKey);

    std::string key = anim;
    return (cocos2d::CCDictionary*)globalAnims->objectForKey(key);
}

void ObjectManager::purgeObjectManager()
{
    if (qword_1241930)
    {
        cocos2d::CCObject::release((cocos2d::CCObject*)qword_1241930);
        qword_1241930 = 0;
    }
}

void ObjectManager::setLoaded(const char* objectName)
{
    cocos2d::CCNode* node = cocos2d::CCNode::create();
    std::string key = objectName;
    m_loadedAnimations->setObject(node, key);
}

void ObjectManager::setup()
{
    unsigned int i = 0;
    cocos2d::CCArray* keys = m_objectDefinitions->allKeys();
    if (keys->count())
    {
        do
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            std::string keyStr = key->getCString();

            bool isGlobal = false;
            if (keyStr == "GlobalAnimations")
                isGlobal = true;

            bool shouldCreate = !isGlobal && keyStr.find("xyz") == std::string::npos;

            if (shouldCreate)
            {
                SpriteAnimationManager::createAnimations(keyStr);
            }
            i++;
        }
        while (i < keys->count());
    }
}

ObjectManager::~ObjectManager()
{
}