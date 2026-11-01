#pragma once

#include "cocos2d.h"
#include <string>

class MultilineBitmapFont {
public:
    void moveSpecialDescriptors(int index, int size);
    std::string readColorInfo(std::string text);
    void setOpacity(unsigned __int8 opacity);
    ~MultilineBitmapFont();

private:
    cocos2d::CCArray *m_specialDescriptors;
    void* size; // tipo no inferido (nunca se copio a una local con nombre)
    void* m_characters; // tipo no inferido (nunca se copio a una local con nombre)
};
