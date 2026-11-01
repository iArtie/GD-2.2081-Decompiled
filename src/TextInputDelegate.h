#pragma once

class CCTextInputNode;

class TextInputDelegate
{
public:
    virtual bool allowTextInput(CCTextInputNode* node);
    virtual void enterPressed(CCTextInputNode* node);
    virtual void textChanged(CCTextInputNode* node);
    virtual void textInputClosed(CCTextInputNode* node);
    virtual void textInputOpened(CCTextInputNode* node);
    virtual void textInputReturn(CCTextInputNode* node);
    virtual void textInputShouldOffset(CCTextInputNode* node, float yOffset);
    
    virtual ~TextInputDelegate() {}
};