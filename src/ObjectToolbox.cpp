#include "ObjectToolbox.h"

ObjectToolbox* ObjectToolbox::sharedState()
{
    static ObjectToolbox* instance = nullptr;
    if (!instance)
    {
        instance = new ObjectToolbox();
        instance->init();
    }
    return instance;
}

bool ObjectToolbox::init()
{
    return cocos2d::CCNode::init();
}

cocos2d::CCArray* ObjectToolbox::allKeys()
{
    return nullptr;
}

float ObjectToolbox::gridNodeSizeForKey(int key)
{
    if (key <= 1911)
    {
        if (key >= 1910)
            return 15.0f;
        if (key <= 874)
        {
            if (key >= 873)
                return 15.0f;
            if (key <= 459)
            {
                if (key < 458)
                {
                    if (key > 235)
                    {
                        if (key > 278)
                        {
                            if ((unsigned int)(key - 392) > 1)
                                return 30.0f;
                        }
                        else if (key < 277 && (unsigned int)(key - 245) > 1)
                        {
                            return 30.0f;
                        }
                    }
                    else if (key < 234 && key != 204)
                    {
                        if (key <= 204)
                        {
                            if ((unsigned int)(key - 195) > 1)
                                return 30.0f;
                        }
                        else if (key != 206 && key != 220)
                        {
                            return 30.0f;
                        }
                    }
                }
                return 15.0f;
            }
            if (key <= 736)
            {
                if (key < 735)
                {
                    if (key > 664)
                    {
                        if ((unsigned int)(key - 723) > 1)
                            return 30.0f;
                    }
                    else if (key < 661 && (unsigned int)(key - 653) > 4)
                    {
                        return 30.0f;
                    }
                }
                return 15.0f;
            }
            if (key <= 857)
            {
                if (key < 853 && key != 850)
                    return 30.0f;
                return 15.0f;
            }
            if (key == 859 || (key >= 859 && (unsigned int)(key - 861) <= 2))
                return 15.0f;
        }
        else if (key > 1111)
        {
            if (key <= 1246)
            {
                if (key < 1241)
                {
                    if (key > 1158)
                    {
                        if (key != 1208)
                            return 30.0f;
                    }
                    else if (key < 1154 && (unsigned int)(key - 1138) > 1)
                    {
                        return 30.0f;
                    }
                }
                return 15.0f;
            }
            if (key == 1592)
                return 7.5f;
            if (key > 1592)
            {
                if (key != 1838 && key != 1892 && key != 1836)
                    return 30.0f;
                return 15.0f;
            }
            if (key >= 1271)
            {
                if (key <= 1272)
                    return 15.0f;
                if (key == 1292)
                    return 15.0f;
            }
        }
        else
        {
            if (key >= 1110)
                return 15.0f;
            if (key > 1010)
            {
                if (key > 1081)
                {
                    if ((unsigned int)(key - 1095) > 3)
                        return 30.0f;
                }
                else if (key < 1078 && (unsigned int)(key - 1053) > 1)
                {
                    return 30.0f;
                }
                return 15.0f;
            }
            if (key >= 1009)
                return 15.0f;
            if (key <= 894)
            {
                if (key < 893 && key != 891)
                    return 30.0f;
                return 15.0f;
            }
            if (key == 916)
                return 15.0f;
            if (key == 917)
                return 7.5f;
        }
        return 30.0f;
    }

    if (key == 3093)
        return 15.0f;

    if (key <= 3093)
    {
        if (key == 3036)
            return 15.0f;
        if (key <= 3036)
        {
            if (key <= 2296)
            {
                if (key >= 2281)
                    return 15.0f;
                if (key != 1964)
                {
                    if (key > 1964)
                    {
                        if ((unsigned int)(key - 2252) > 3)
                            return 30.0f;
                        return 15.0f;
                    }
                    if ((unsigned int)(key - 1922) <= 6)
                        return 10.0f;
                    return 30.0f;
                }
                return 10.0f;
            }
            if (key <= 2863)
            {
                if (key < 2838 && (unsigned int)(key - 2346) > 1)
                    return 30.0f;
                return 15.0f;
            }
            if (key < 2952)
                return 30.0f;
            if (key <= 2960)
                return 15.0f;
            if (key != 3035)
                return 30.0f;
        }
        else
        {
            if (key == 3045)
                return 10.0f;
            if (key > 3045)
            {
                if (key > 3085)
                {
                    if ((unsigned int)(key - 3090) > 1)
                        return 30.0f;
                    return 15.0f;
                }
                if (key >= 3054 || key == 3048)
                    return 15.0f;
                if (key == 3049)
                    return 10.0f;
                if (key != 3047)
                    return 30.0f;
            }
            else
            {
                if (key == 3040)
                    return 15.0f;
                if (key <= 3040)
                {
                    if (key != 3037)
                    {
                        if (key == 3039)
                            return 20.0f;
                        return 30.0f;
                    }
                    return 10.0f;
                }
                if (key != 3043)
                {
                    if (key > 3043)
                        return 15.0f;
                    if (key == 3041)
                        return 10.0f;
                    return 30.0f;
                }
            }
        }
        return 20.0f;
    }

    if (key > 4059)
    {
        if (key <= 4194)
        {
            if (key < 4192)
            {
                if (key > 4121)
                {
                    if ((unsigned int)(key - 4123) > 0x10)
                        return 30.0f;
                }
                else if (key < 4108 && (unsigned int)(key - 4077) > 3)
                {
                    return 30.0f;
                }
            }
            return 15.0f;
        }
        if (key <= 4290)
        {
            if (key < 4286 && (unsigned int)(key - 4202) > 2)
                return 30.0f;
            return 15.0f;
        }
        if ((unsigned int)(key - 4342) <= 3)
            return 7.5f;
        return 30.0f;
    }

    if (key >= 4046)
        return 15.0f;

    if (key <= 3566)
    {
        if (key < 3558)
        {
            if (key == 3096)
                return 6.0f;
            if (key <= 3096)
            {
                if (key != 3094)
                    return 7.5f;
                return 10.0f;
            }
            if (key == 3097)
                return 5.0f;
            if ((unsigned int)(key - 3553) > 1)
                return 30.0f;
        }
        return 15.0f;
    }

    if (key <= 3788)
    {
        if (key < 3702 && (unsigned int)(key - 3584) > 0xD)
            return 30.0f;
        return 15.0f;
    }

    if ((unsigned int)(key - 3797) <= 2)
        return 15.0f;
    return 30.0f;
}

const char* ObjectToolbox::intKeyToFrame(int key)
{
    auto it = m_allKeys.find(key);
    if (it != m_allKeys.end())
        return it->second.c_str();
    return "";
}

const char* ObjectToolbox::perspectiveBlockFrame(int key)
{
    if (key <= 613)
    {
        if (key >= 605)
            return "persp_lblock04";
        if (key <= 559)
        {
            if (key >= 551)
                return "persp_blockf";
            if (key > 532)
            {
                if (key <= 541)
                    return "persp_blockd";
                return "persp_blocke";
            }
            if (key < 524)
            {
                if (key >= 515)
                    return "persp_blockb";
                return "";
            }
            return "persp_blockc";
        }
        if (key > 586)
        {
            if (key <= 595)
                return "persp_lblock02";
            return "persp_lblock03";
        }
        if (key < 578)
        {
            if (key <= 568)
                return "persp_blockg";
            return "persp_blockh";
        }
        return "persp_lblock01";
    }

    if (key <= 988)
    {
        if (key >= 980)
            return "persp_block007";
        if (key <= 640)
        {
            if (key >= 632)
                return "persp_block002";
            if (key <= 622)
                return "persp_lblock05";
            return "persp_block001";
        }
        if (key != 902)
        {
            if (key >= 902 && (unsigned int)(key - 943) <= 8)
                return "persp_block005";
            return "";
        }
        return "persp_lblock01";
    }

    if (key > 1071)
    {
        if (key < 1529)
            return "";
        if (key > 1540)
        {
            if ((unsigned int)(key - 1552) <= 8)
                return "persp_block011";
            return "";
        }
        return "persp_block013";
    }
    else
    {
        if (key < 1063)
        {
            if ((unsigned int)(key - 1024) <= 8)
                return "persp_block005b";
            return "";
        }
        return "persp_block009";
    }
}

ObjectToolbox::~ObjectToolbox()
{
}