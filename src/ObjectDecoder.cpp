#include "ObjectDecoder.h"
#include "ObjectDecoderDelegate.h"
#include "DS_Dictionary.h"

ObjectDecoder* ObjectDecoder::sharedDecoder()
{
    static ObjectDecoder* instance = nullptr;
    if (!instance)
    {
        instance = new ObjectDecoder();
        instance->m_delegate = nullptr;
    }
    return instance;
}

cocos2d::CCObject* ObjectDecoder::getDecodedObject(int p0, DS_Dictionary* p1)
{
    if (m_delegate)
        return m_delegate->getDecodedObject(p0, p1);
    return nullptr;
}

ObjectDecoder::~ObjectDecoder()
{
}