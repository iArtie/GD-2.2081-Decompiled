#include "Delegates/TextInputDelegate.h"
#include "Cocos/CCTextInputNode.h"

bool TextInputDelegate::allowTextInput(CCTextInputNode* node)
{
    return true;
}

void TextInputDelegate::enterPressed(CCTextInputNode* node)
{
}

void TextInputDelegate::textChanged(CCTextInputNode* node)
{
}

void TextInputDelegate::textInputClosed(CCTextInputNode* node)
{
}

void TextInputDelegate::textInputOpened(CCTextInputNode* node)
{
}

void TextInputDelegate::textInputReturn(CCTextInputNode* node)
{
}

void TextInputDelegate::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
}