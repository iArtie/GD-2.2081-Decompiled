#pragma once

#include "Platform/cocos2d.h"
#include <string>

class TextInputDelegate;
class TextArea;

class CCTextInputNode : public cocos2d::CCLayer, public cocos2d::CCIMEDelegate
{
public:
    static CCTextInputNode* create(float width, float height, const char* placeholder, const char* fontPath);
    
    bool init(float width, float height, const char* placeholder, const char* fontPath, int unknown1, const char* unknown2);
    
    void addTextArea(TextArea* textArea);
    std::string getString();
    void setString(const std::string& text);
    void setAllowedChars(const std::string& filter);
    void setMaxLabelWidth(float v);
    void setMaxLabelScale(float v);
    void setLabelPlaceholderScale(float v);
    void setLabelNormalColor(cocos2d::ccColor3B color);
    void setLabelPlaceholderColor(cocos2d::ccColor3B color);
    void setMaxLabelLength(int length);
    void setUsePasswordChar(bool use);
    void setTouchEnabled(bool enabled);
    void onClickTrackNode(bool selected);
    void forceOffset();
    
    void updateLabel(const std::string& text);
    void updateDefaultFontValues(const std::string& font);
    void refreshLabel();
    void updateBlinkLabel();
    void updateBlinkLabelToChar(int index);
    void textChanged();
    
    bool onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF* tField);
    bool onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF* tField);
    void keyboardWillShow(cocos2d::CCIMEKeyboardNotificationInfo* info);
    void keyboardWillHide(cocos2d::CCIMEKeyboardNotificationInfo* info);
    
    virtual void registerWithTouchDispatcher();
    virtual void visit();
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    
    virtual ~CCTextInputNode();

public:
    TextInputDelegate* m_delegate;
    bool m_forceOffset;
    bool m_selected;
    bool m_unknown2;
    bool m_isChatFont;
    bool m_usePasswordChar;
    bool m_filterSwearWords;
    bool m_numberInput;
    float m_fontValue1;
    float m_fontValue2;
    float m_maxLabelWidth;
    float m_maxLabelScale;
    float m_placeholderScale;
    float m_kerningAmount;
    int m_maxLabelLength;
    cocos2d::CCTextFieldTTF* m_textField;
    cocos2d::CCLabelBMFont* m_textLabel;
    cocos2d::CCLabelBMFont* m_cursor;
    TextArea* m_textArea;
    std::string m_allowedChars;
    std::string m_caption;
    cocos2d::ccColor3B m_textColor;
    cocos2d::ccColor3B m_placeholderColor;
};