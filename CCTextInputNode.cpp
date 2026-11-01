#include "CCTextInputNode.h"
#include "TextArea.h"
#include "TextInputDelegate.h"
#include "PlatformToolbox.h"

CCTextInputNode* CCTextInputNode::create(float width, float height, const char* placeholder, const char* fontPath)
{
    CCTextInputNode* node = new CCTextInputNode();
    if (node && node->init(width, height, placeholder, fontPath, 0, nullptr))
    {
        node->autorelease();
        return node;
    }
    
    delete node;
    return nullptr;
}

bool CCTextInputNode::init(float width, float height, const char* placeholder, const char* fontPath, int unknown1, const char* unknown2)
{
    if (!cocos2d::CCLayer::init())
    {
        return false;
    }
    
    m_textField = cocos2d::CCTextFieldTTF::create(placeholder, fontPath, height);
    m_textField->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_textField->setContentSize(cocos2d::CCSize(width, height));
    m_textField->setDelegate(this);
    addChild(m_textField);
    
    m_textLabel = nullptr;
    m_textArea = nullptr;
    m_cursor = nullptr;
    m_delegate = nullptr;
    m_forceOffset = false;
    m_selected = false;
    m_unknown2 = false;
    m_isChatFont = false;
    m_usePasswordChar = false;
    m_filterSwearWords = false;
    m_numberInput = false;
    m_fontValue1 = 1.5f;
    m_fontValue2 = 20.0f;
    m_maxLabelWidth = width;
    m_maxLabelScale = 1.0f;
    m_placeholderScale = 0.0f;
    m_kerningAmount = 0;
    m_maxLabelLength = 0;
    m_allowedChars = "";
    m_caption = placeholder ? placeholder : "";
    m_textColor = cocos2d::ccc3(255, 255, 255);
    m_placeholderColor = cocos2d::ccc3(128, 128, 128);
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    return true;
}

void CCTextInputNode::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->addPrioTargetedDelegate(this, -500, false);
}

bool CCTextInputNode::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint touchPos = touch->getLocation();
    touchPos = convertToNodeSpace(touchPos);
    
    cocos2d::CCRect rect = getBoundingBox();
    if (!rect.containsPoint(touchPos))
    {
        return false;
    }
    
    if (m_delegate && !m_delegate->allowTextInput(this))
    {
        return false;
    }
    
    onClickTrackNode(true);
    updateCursorPosition(touchPos);
    
    return true;
}

void CCTextInputNode::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void CCTextInputNode::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void CCTextInputNode::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void CCTextInputNode::onClickTrackNode(bool selected)
{
    if (selected)
    {
        m_textField->attachWithIME();
    }
    else
    {
        m_textField->detachWithIME();
    }
}

bool CCTextInputNode::onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF* tField)
{
    if (m_cursor)
    {
        m_cursor->setVisible(true);
    }
    
    m_selected = true;
    PlatformToolbox::setKeyboardState(true);
    
    if (m_numberInput)
    {
        const char* str = m_textField->getString();
        if (atof(str) == 0.0f)
        {
            setString("");
        }
    }
    
    std::string text = m_textField->getString();
    if (text.empty())
    {
        if (m_textLabel)
        {
            m_textLabel->setString(" ");
        }
        else if (m_textArea)
        {
            m_textArea->setString(" ");
        }
        updateBlinkLabel();
    }
    
    if (m_delegate)
    {
        m_delegate->textInputOpened(this);
    }
    
    if (m_forceOffset && m_delegate)
    {
        cocos2d::CCRect rect = getBoundingBox();
        float offset = rect.origin.y - 4.0f;
        m_delegate->textInputShouldOffset(this, -offset);
    }
    
    return false;
}

bool CCTextInputNode::onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF* tField)
{
    if (m_cursor)
    {
        m_cursor->setVisible(false);
    }
    
    if (m_filterSwearWords)
    {
        std::string text = m_textField->getString();
        // Filtrar palabras ofensivas
        // Implementación simplificada
    }
    
    std::string text = m_textField->getString();
    updateLabel(text);
    
    m_selected = false;
    PlatformToolbox::setKeyboardState(false);
    
    if (m_delegate)
    {
        m_delegate->textInputClosed(this);
        if (m_forceOffset)
        {
            m_delegate->textInputReturn(this);
        }
    }
    
    return false;
}

void CCTextInputNode::keyboardWillShow(cocos2d::CCIMEKeyboardNotificationInfo* info)
{
    if (m_forceOffset || !m_textField || !m_selected)
    {
        return;
    }
    
    cocos2d::CCRect rect = getBoundingBox();
    rect.origin.y -= 4.0f;
    
    if (rect.intersectsRect(info->end))
    {
        m_unknown2 = true;
        float offset = info->end.getMaxY() - rect.getMinY();
        if (m_delegate)
        {
            m_delegate->textInputShouldOffset(this, offset);
        }
    }
}

void CCTextInputNode::keyboardWillHide(cocos2d::CCIMEKeyboardNotificationInfo* info)
{
    if (m_forceOffset)
    {
        if (m_selected && m_textField)
        {
            if (m_delegate)
            {
                m_delegate->textInputReturn(this);
            }
            m_selected = false;
        }
    }
    else if (m_textField && m_unknown2)
    {
        m_unknown2 = false;
        if (m_delegate)
        {
            m_delegate->textInputReturn(this);
        }
    }
}

void CCTextInputNode::addTextArea(TextArea* textArea)
{
    if (m_textArea)
    {
        return;
    }
    
    m_textArea = textArea;
    addChild(textArea);
    textArea->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    m_textField->setVisible(false);
    
    std::string text = m_textField->getString();
    updateLabel(text);
    updateDefaultFontValues(m_textArea->m_fontFile);
}

void CCTextInputNode::setString(const std::string& text)
{
    m_textField->setString(text.c_str());
    updateLabel(text);
    
    if (m_delegate)
    {
        m_delegate->textChanged(this);
    }
}

std::string CCTextInputNode::getString()
{
    return m_textField->getString() ? m_textField->getString() : "";
}

void CCTextInputNode::setAllowedChars(const std::string& filter)
{
    m_allowedChars = filter;
}

void CCTextInputNode::setMaxLabelWidth(float v)
{
    m_maxLabelWidth = v;
    refreshLabel();
}

void CCTextInputNode::setMaxLabelScale(float v)
{
    m_maxLabelScale = v;
    refreshLabel();
}

void CCTextInputNode::setLabelPlaceholderScale(float v)
{
    m_placeholderScale = v;
    refreshLabel();
}

void CCTextInputNode::setLabelNormalColor(cocos2d::ccColor3B color)
{
    m_textColor = color;
    refreshLabel();
}

void CCTextInputNode::setLabelPlaceholderColor(cocos2d::ccColor3B color)
{
    m_placeholderColor = color;
    refreshLabel();
}

void CCTextInputNode::setMaxLabelLength(int length)
{
    m_maxLabelLength = length;
}

void CCTextInputNode::setUsePasswordChar(bool use)
{
    m_usePasswordChar = use;
}

void CCTextInputNode::forceOffset()
{
    m_forceOffset = true;
}

void CCTextInputNode::updateLabel(const std::string& text)
{
    bool isEmpty = text.empty();
    
    if (isEmpty)
    {
        // Mostrar placeholder
        if (m_textLabel)
        {
            m_textLabel->setString(m_caption.c_str());
        }
        else if (m_textArea)
        {
            m_textArea->setString(m_caption);
        }
    }
    else
    {
        std::string displayText = text;
        if (m_usePasswordChar)
        {
            displayText = std::string(text.length(), '*');
        }
        
        if (m_textLabel)
        {
            m_textLabel->setString(displayText.c_str());
        }
        else if (m_textArea)
        {
            m_textArea->setString(displayText);
        }
    }
    
    refreshLabel();
    updateBlinkLabel();
}

void CCTextInputNode::refreshLabel()
{
    if (!m_textLabel && !m_textArea)
    {
        return;
    }
    
    std::string text = m_textField->getString();
    bool isEmpty = text.empty();
    
    if (m_textLabel)
    {
        if (isEmpty)
        {
            m_textLabel->setScale(m_placeholderScale > 0.0f ? m_placeholderScale : m_maxLabelScale);
            m_textLabel->setColor(m_placeholderColor);
        }
        else
        {
            float scale = m_maxLabelWidth / m_textLabel->getContentSize().width;
            if (scale > m_maxLabelScale)
            {
                scale = m_maxLabelScale;
            }
            m_textLabel->setScale(scale);
            m_textLabel->setColor(m_textColor);
        }
    }
    else if (m_textArea)
    {
        cocos2d::ccColor3B color = isEmpty ? m_placeholderColor : m_textColor;
        m_textArea->colorAllLabels(color);
    }
}

void CCTextInputNode::updateBlinkLabel()
{
    updateBlinkLabelToChar(m_textField->m_uCursorPos);
}

void CCTextInputNode::updateBlinkLabelToChar(int index)
{
    if (!m_cursor)
    {
        return;
    }
    
    if (m_textLabel)
    {
        float xPos = 0.0f;
        cocos2d::CCArray* children = m_textLabel->getChildren();
        
        if (index >= 0 && (unsigned int)index < children->count())
        {
            cocos2d::CCSprite* sprite = (cocos2d::CCSprite*)children->objectAtIndex(index);
            xPos = m_textLabel->getLetterPosXLeft(sprite, m_fontValue2, m_isChatFont);
            xPos += m_fontValue1 * m_textLabel->getScaleX();
        }
        else
        {
            xPos = m_textLabel->getContentSize().width * m_textLabel->getScale();
        }
        
        float yPos = 0.0f;
        std::string text = m_textField->getString();
        if (text.empty())
        {
            xPos = 2.0f;
        }
        
        cocos2d::CCPoint pos = m_textLabel->getPosition() + cocos2d::CCPoint(xPos, yPos - 1.0f);
        m_cursor->setPosition(pos);
        m_cursor->setAnchorPoint(cocos2d::CCPoint(0.5f, m_textLabel->getAnchorPoint().y));
    }
    else if (m_textArea)
    {
        // Implementación para TextArea
    }
}

void CCTextInputNode::updateDefaultFontValues(const std::string& font)
{
    if (font.find("chatFont.fnt") != std::string::npos)
    {
        m_isChatFont = true;
        m_fontValue2 = 5.0f;
        m_fontValue1 = -0.5f;
    }
    else
    {
        m_isChatFont = false;
        m_fontValue2 = 20.0f;
        m_fontValue1 = 1.5f;
    }
}

void CCTextInputNode::textChanged()
{
    std::string text = m_textField->getString();
    updateLabel(text);
    
    if (m_delegate)
    {
        m_delegate->textChanged(this);
    }
}

void CCTextInputNode::visit()
{
    cocos2d::CCSize size = getContentSize();
    cocos2d::CCRect rect(-size.width * 0.5f, -size.height * 0.5f, size.width, size.height);
    cocos2d::CCLayer::visit();
}

CCTextInputNode::~CCTextInputNode()
{
}