#include "GJUserMessage.h"
#include "LevelTools.h"
#include "GameToolbox.h"

GJUserMessage* GJUserMessage::create() {
    auto message = new GJUserMessage();
    if (message) {
        message->autorelease();
    }
    return message;
}

GJUserMessage::~GJUserMessage() {
}