#include "Utils/DS_Dictionary.h"
#include "Platform/cocos2d.h"
#include "Utils/FileOperation.h"
#include "ZipUtils.h"
#include "Editor/ObjectDecoder.h"
#include <cctype>

DS_Dictionary::DS_Dictionary()
{
    // Initialize XML document
    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    
    pugi::xml_node plist = doc.append_child("plist");
    plist.append_attribute("version") = "1.0";
    
    pugi::xml_node dict = plist.append_child("dict");
    dictTree.push_back(dict);
    
    compatible = false;
}

void DS_Dictionary::checkCompatibility()
{
    pugi::xml_node plist = doc.child("plist");
    pugi::xml_attribute ver = plist.attribute("gjver");
    compatible = ver.as_int(0) <= 1;
}

std::string DS_Dictionary::cleanStringWhiteSpace(const std::string* p0)
{
    std::string str = *p0;
    
    // Remove leading whitespace
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        return "";
    
    // Remove trailing whitespace
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    
    return str.substr(start, end - start + 1);
}

void DS_Dictionary::copyFile(const char* p0, const char* p1)
{
    // Implementation would copy file from p0 to p1 using platform-specific file operations
}

cocos2d::CCObject* DS_Dictionary::decodeObjectForKey(const char* p0, bool p1, int p2)
{
    if (!p1)
    {
        if (!stepIntoSubDictWithKey(p0))
            return nullptr;
    }
    
    if (p2 == -1)
        p2 = getIntegerForKey("kCEK");
    
    ObjectDecoder* decoder = ObjectDecoder::sharedDecoder();
    cocos2d::CCObject* obj = decoder->getDecodedObject(p2, this);
    
    stepOutOfSubDict();
    return obj;
}

std::vector<std::string>* DS_Dictionary::getAllKeys(std::vector<std::string>* retstr)
{
    retstr->clear();
    
    pugi::xml_node dict = dictTree.back();
    for (pugi::xml_node key = dict.first_child(); key; key = key.next_sibling())
    {
        retstr->push_back(key.child_value());
    }
    
    return retstr;
}

cocos2d::CCArray* DS_Dictionary::getArrayForKey(const char* p0, bool p1)
{
    if (!p1)
    {
        if (!stepIntoSubDictWithKey(p0))
            return cocos2d::CCArray::create();
    }
    
    cocos2d::CCArray* arr = cocos2d::CCArray::create();
    
    removeKey("_isArr");
    
    int numKeys = getNumKeys();
    for (int i = 0; i < numKeys; i++)
    {
        std::string key = getKey(i);
        cocos2d::CCObject* obj = getObjectForKey(key.c_str());
        if (obj)
            arr->addObject(obj);
    }
    
    stepOutOfSubDict();
    return arr;
}

bool DS_Dictionary::getBoolForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* valueTag = compatible ? "true" : "t";
            return strcmp(value.name(), valueTag) == 0;
        }
    }
    
    return false;
}

cocos2d::CCDictionary* DS_Dictionary::getDictForKey(const char* p0, bool p1)
{
    if (p0 && !p1 && !stepIntoSubDictWithKey(p0))
        return cocos2d::CCDictionary::create();
    
    cocos2d::CCDictionary* dict = cocos2d::CCDictionary::create();
    
    pugi::xml_node currentDict = dictTree.back();
    for (pugi::xml_node key = currentDict.first_child(); key; key = key.next_sibling())
    {
        pugi::xml_node value = key.next_sibling();
        std::string keyStr = key.child_value();
        
        const char* dictTag = compatible ? "dict" : "d";
        if (strcmp(value.name(), dictTag) == 0)
        {
            dictTree.push_back(value);
            
            bool isArr = getBoolForKey("_isArr");
            int encodeKey = getIntegerForKey("kCEK");
            cocos2d::CCObject* obj;
            
            if (encodeKey)
            {
                obj = decodeObjectForKey(keyStr.c_str(), true, encodeKey);
            }
            else if (isArr)
            {
                obj = getArrayForKey(keyStr.c_str(), true);
            }
            else
            {
                obj = getDictForKey(keyStr.c_str(), true);
            }
            
            if (obj)
                dict->setObject(obj, keyStr);
            
            dictTree.pop_back();
        }
        else
        {
            const char* trueTag = compatible ? "true" : "t";
            if (strcmp(value.name(), trueTag) == 0)
            {
                dict->setObject(cocos2d::CCString::create("1"), keyStr);
            }
            else
            {
                dict->setObject(cocos2d::CCString::create(value.child_value()), keyStr);
            }
        }
    }
    
    if (p0)
        stepOutOfSubDict();
    
    return dict;
}

float DS_Dictionary::getFloatForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* realTag = compatible ? "real" : "r";
            if (strcmp(value.name(), realTag) == 0)
            {
                return std::strtof(value.child_value(), nullptr);
            }
        }
    }
    
    return 0.0f;
}

unsigned int DS_Dictionary::getIndexOfKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    unsigned int index = 0;
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
            return index;
        index++;
    }
    
    return -1;
}

unsigned int DS_Dictionary::getIndexOfKeyWithClosestAlphaNumericalMatch(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    unsigned int index = 0;
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(p0, key.child_value()) < 0)
            return index;
        index++;
    }
    
    return -1;
}

int DS_Dictionary::getIntegerForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* intTag = compatible ? "integer" : "i";
            if (strcmp(value.name(), intTag) == 0)
            {
                return std::strtol(value.child_value(), nullptr, 10);
            }
        }
    }
    
    return 0;
}

std::string DS_Dictionary::getKey(unsigned int p0)
{
    pugi::xml_node dict = dictTree.back();
    
    unsigned int index = 0;
    for (pugi::xml_node key = dict.first_child(); key; key = key.next_sibling())
    {
        if (index == p0)
            return key.child_value();
        index++;
    }
    
    return "";
}

unsigned int DS_Dictionary::getNumKeys()
{
    pugi::xml_node dict = dictTree.back();
    
    unsigned int count = 0;
    for (pugi::xml_node child = dict.first_child(); child; child = child.next_sibling())
        count++;
    
    return count;
}

cocos2d::CCObject* DS_Dictionary::getObjectForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* dictTag = compatible ? "dict" : "d";
            
            if (strcmp(value.name(), dictTag) == 0)
            {
                dictTree.push_back(value);
                
                bool isArr = getBoolForKey("_isArr");
                int encodeKey = getIntegerForKey("kCEK");
                cocos2d::CCObject* obj;
                
                if (encodeKey)
                    obj = decodeObjectForKey(p0, true, encodeKey);
                else if (isArr)
                    obj = getArrayForKey(p0, true);
                else
                    obj = getDictForKey(p0, true);
                
                dictTree.pop_back();
                return obj;
            }
            
            const char* trueTag = compatible ? "true" : "t";
            if (strcmp(value.name(), trueTag) == 0)
                return cocos2d::CCString::create("1");
            
            return cocos2d::CCString::create(value.child_value());
        }
    }
    
    return nullptr;
}

cocos2d::CCRect DS_Dictionary::getRectForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* strTag = compatible ? "string" : "s";
            if (strcmp(value.name(), strTag) == 0)
            {
                cocos2d::CCRect rect;
                if (rectFromString(&std::string(value.child_value()), &rect))
                    return rect;
            }
        }
    }
    
    return cocos2d::CCRect(0, 0, 0, 0);
}

std::string DS_Dictionary::getStringForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* strTag = compatible ? "string" : "s";
            if (strcmp(value.name(), strTag) == 0)
                return value.child_value();
        }
    }
    
    return "";
}

cocos2d::CCPoint DS_Dictionary::getVec2ForKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* strTag = compatible ? "string" : "s";
            if (strcmp(value.name(), strTag) == 0)
            {
                return cocos2d::CCPointFromString(value.child_value());
            }
        }
    }
    
    return cocos2d::CCPoint(0, 0);
}

bool DS_Dictionary::loadRootSubDictFromCompressedFile(const char* p0)
{
    dictTree.clear();
    dictTree.push_back(pugi::xml_node());
    
    std::string path = FileOperation::getFilePath();
    path += p0;
    
    cocos2d::CCFileUtils* utils = cocos2d::CCFileUtils::sharedFileUtils();
    unsigned long size;
    unsigned char* data = utils->getFileData(path.c_str(), "r", &size);
    
    if (!data)
        return false;
    
    unsigned char* decompressed = cocos2d::ZipUtils::decompressString2(data, true, size);
    delete[] data;
    
    if (!decompressed)
        return false;
    
    pugi::xml_parse_result result = doc.load_buffer(decompressed, strlen((char*)decompressed));
    delete[] decompressed;
    
    if (!result)
        return false;
    
    pugi::xml_node plist = doc.child("plist");
    dictTree.back() = plist.child("dict");
    
    checkCompatibility();
    return true;
}

bool DS_Dictionary::loadRootSubDictFromFile(const char* p0)
{
    dictTree.clear();
    dictTree.push_back(pugi::xml_node());
    
    cocos2d::CCFileUtils* utils = cocos2d::CCFileUtils::sharedFileUtils();
    unsigned long size;
    unsigned char* data = utils->getFileData(p0, "r", &size);
    
    if (!data)
        return false;
    
    pugi::xml_parse_result result = doc.load_buffer(data, size);
    delete[] data;
    
    if (!result)
        return false;
    
    pugi::xml_node plist = doc.child("plist");
    dictTree.back() = plist.child("dict");
    
    checkCompatibility();
    return true;
}

bool DS_Dictionary::loadRootSubDictFromString(const std::string* p0)
{
    if (p0->empty())
        return false;
    
    dictTree.clear();
    dictTree.push_back(pugi::xml_node());
    
    pugi::xml_parse_result result = doc.load_string(p0->c_str());
    
    if (!result)
        return false;
    
    pugi::xml_node plist = doc.child("plist");
    dictTree.back() = plist.child("dict");
    
    checkCompatibility();
    return true;
}

bool DS_Dictionary::rectFromString(const std::string* p0, cocos2d::CCRect* p1)
{
    std::string str = cleanStringWhiteSpace(p0);
    
    if (str.empty())
        return false;
    
    // Parse format: {{x,y},{width,height}} or {x,y,width,height}
    size_t openBrace = str.find('{');
    if (openBrace == std::string::npos)
        return false;
    
    size_t closeBrace = str.find('}', openBrace + 1);
    if (closeBrace == std::string::npos)
        return false;
    
    std::string firstPart = str.substr(openBrace + 1, closeBrace - openBrace - 1);
    
    // Remove any extra braces
    size_t comma = firstPart.find(',');
    if (comma == std::string::npos)
        return false;
    
    float x = std::strtof(firstPart.substr(0, comma).c_str(), nullptr);
    float y = std::strtof(firstPart.substr(comma + 1).c_str(), nullptr);
    
    size_t secondOpen = str.find('{', closeBrace + 1);
    if (secondOpen == std::string::npos)
        return false;
    
    size_t secondClose = str.find('}', secondOpen + 1);
    if (secondClose == std::string::npos)
        return false;
    
    std::string secondPart = str.substr(secondOpen + 1, secondClose - secondOpen - 1);
    
    comma = secondPart.find(',');
    if (comma == std::string::npos)
        return false;
    
    float w = std::strtof(secondPart.substr(0, comma).c_str(), nullptr);
    float h = std::strtof(secondPart.substr(comma + 1).c_str(), nullptr);
    
    p1->origin = cocos2d::CCPoint(x, y);
    p1->size = cocos2d::CCSize(w, h);
    
    return true;
}

void DS_Dictionary::removeAllKeys()
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    while (dict.child(keyTag))
    {
        pugi::xml_node key = dict.child(keyTag);
        pugi::xml_node value = key.next_sibling();
        dict.remove_child(value);
        dict.remove_child(key);
    }
}

void DS_Dictionary::removeKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            dict.remove_child(value);
            dict.remove_child(key);
            return;
        }
    }
}

bool DS_Dictionary::saveRootSubDictToCompressedFile(const char* p0)
{
    std::string xml = saveRootSubDictToString();
    unsigned char* compressed = cocos2d::ZipUtils::compressString(xml, true);
    
    if (!compressed)
        return false;
    
    std::string path = FileOperation::getFilePath();
    path += p0;
    
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
        return false;
    
    fwrite(compressed, 1, xml.size(), file);
    fclose(file);
    
    return true;
}

bool DS_Dictionary::saveRootSubDictToFile(const char* p0)
{
    std::string xml = saveRootSubDictToString();
    
    std::string path = FileOperation::getFilePath();
    path += p0;
    
    FILE* file = fopen(path.c_str(), "w");
    if (!file)
        return false;
    
    fwrite(xml.c_str(), 1, xml.size(), file);
    fclose(file);
    
    return true;
}

std::string DS_Dictionary::saveRootSubDictToString()
{
    pugi::xml_node plist = doc.child("plist");
    plist.append_attribute("gjver") = "2.0";
    
    std::ostringstream ss;
    doc.save(ss, "  ");
    return ss.str();
}

void DS_Dictionary::setArrayForKey(const char* p0, cocos2d::CCArray* p1)
{
    if (!p1)
        return;
    
    setSubDictForKey(p0);
    setBoolForKey("_isArr", true);
    
    for (unsigned int i = 0; i < p1->count(); i++)
    {
        cocos2d::CCObject* obj = p1->objectAtIndex(i);
        std::string key = "k_" + std::to_string(i);
        setObjectForKey(key.c_str(), obj);
    }
    
    stepOutOfSubDict();
}

void DS_Dictionary::setBoolForKey(const char* p0, bool p1)
{
    setBoolForKey(p0, p1, false);
}

void DS_Dictionary::setBoolMapForKey(const char* p0, std::map<std::string, bool>* p1)
{
    setSubDictForKey(p0);
    
    if (stepIntoSubDictWithKey(p0))
    {
        for (auto& pair : *p1)
        {
            setBoolForKey(pair.first.c_str(), pair.second);
        }
        stepOutOfSubDict();
    }
}

void DS_Dictionary::setDictForKey(const char* p0, cocos2d::CCDictionary* p1)
{
    if (!p1)
        return;
    
    setSubDictForKey(p0);
    
    if (stepIntoSubDictWithKey(p0))
    {
        cocos2d::CCArray* keys = p1->allKeys();
        for (unsigned int i = 0; i < keys->count(); i++)
        {
            cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
            cocos2d::CCObject* obj = p1->objectForKey(key->getCString());
            setObjectForKey(key->getCString(), obj);
        }
        stepOutOfSubDict();
    }
}

void DS_Dictionary::setFloatForKey(const char* p0, float p1)
{
    setFloatForKey(p0, p1, false);
}

void DS_Dictionary::setIntegerForKey(const char* p0, int p1)
{
    setIntegerForKey(p0, p1, false);
}

void DS_Dictionary::setObjectForKey(const char* p0, cocos2d::CCObject* p1)
{
    if (!p1)
        return;
    
    // CCString
    if (dynamic_cast<cocos2d::CCString*>(p1))
    {
        setStringForKey(p0, &std::string(((cocos2d::CCString*)p1)->getCString()));
        return;
    }
    
    // CCDictionary
    if (dynamic_cast<cocos2d::CCDictionary*>(p1))
    {
        setDictForKey(p0, (cocos2d::CCDictionary*)p1);
        return;
    }
    
    // CCArray
    if (dynamic_cast<cocos2d::CCArray*>(p1))
    {
        setArrayForKey(p0, (cocos2d::CCArray*)p1);
        return;
    }
    
    // Custom object with encodeWithCoder
    if (p1->canEncode())
    {
        setSubDictForKey(p0);
        p1->encodeWithCoder(this);
        stepOutOfSubDict();
    }
}

void DS_Dictionary::setRectArrayForKey(const char* p0, const std::vector<cocos2d::CCRect>* p1)
{
    setRectArrayForKey(p0, p1, false);
}

void DS_Dictionary::setRectForKey(const char* p0, const cocos2d::CCRect* p1)
{
    setRectForKey(p0, p1, false);
}

void DS_Dictionary::setStringArrayForKey(const char* p0, const std::vector<std::string>* p1)
{
    setStringArrayForKey(p0, p1, false);
}

void DS_Dictionary::setStringForKey(const char* p0, const std::string* p1)
{
    setStringForKey(p0, p1, false);
}

void DS_Dictionary::setSubDictForKey(const char* p0)
{
    setSubDictForKey(p0, false, false);
}

void DS_Dictionary::setVec2ArrayForKey(const char* p0, const std::vector<cocos2d::CCPoint>* p1)
{
    setVec2ArrayForKey(p0, p1, false);
}

void DS_Dictionary::setVec2ForKey(const char* p0, const cocos2d::CCPoint* p1)
{
    setVec2ForKey(p0, p1, false);
}

void DS_Dictionary::split(const std::string* p0, const char* p1, std::vector<std::string>* p2)
{
    size_t start = 0;
    size_t end = p0->find(p1);
    
    while (end != std::string::npos)
    {
        p2->push_back(p0->substr(start, end - start));
        start = end + strlen(p1);
        end = p0->find(p1, start);
    }
    
    p2->push_back(p0->substr(start));
}

bool DS_Dictionary::splitWithForm(const std::string* p0, std::vector<std::string>* p1)
{
    if (p0->empty())
        return false;
    
    size_t open = p0->find('{');
    size_t close = p0->find('}', open + 1);
    
    if (open == std::string::npos || close == std::string::npos || open > close)
        return false;
    
    std::string content = p0->substr(open + 1, close - open - 1);
    split(&content, ",", p1);
    
    return p1->size() == 2 && !p1->at(0).empty() && !p1->at(1).empty();
}

void DS_Dictionary::stepBackToRootSubDict()
{
    if (dictTree.size() > 1)
    {
        dictTree.clear();
        dictTree.push_back(doc.child("plist").child("dict"));
    }
}

bool DS_Dictionary::stepIntoSubDictWithKey(const char* p0)
{
    pugi::xml_node dict = dictTree.back();
    const char* keyTag = compatible ? "key" : "k";
    
    for (pugi::xml_node key = dict.child(keyTag); key; key = key.next_sibling(keyTag))
    {
        if (strcmp(key.child_value(), p0) == 0)
        {
            pugi::xml_node value = key.next_sibling();
            const char* dictTag = compatible ? "dict" : "d";
            if (strcmp(value.name(), dictTag) == 0)
            {
                dictTree.push_back(value);
                return true;
            }
        }
    }
    
    return false;
}

void DS_Dictionary::stepOutOfSubDict()
{
    if (dictTree.size() > 1)
        dictTree.pop_back();
}

bool DS_Dictionary::vec2FromString(const std::string* p0, cocos2d::CCPoint* p1)
{
    std::string str = cleanStringWhiteSpace(p0);
    
    if (str.empty())
        return false;
    
    // Parse format: {x,y}
    size_t open = str.find('{');
    size_t close = str.find('}', open + 1);
    
    if (open == std::string::npos || close == std::string::npos)
        return false;
    
    std::string content = str.substr(open + 1, close - open - 1);
    size_t comma = content.find(',');
    
    if (comma == std::string::npos)
        return false;
    
    float x = std::strtof(content.substr(0, comma).c_str(), nullptr);
    float y = std::strtof(content.substr(comma + 1).c_str(), nullptr);
    
    *p1 = cocos2d::CCPoint(x, y);
    return true;
}