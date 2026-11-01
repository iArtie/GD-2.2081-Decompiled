#include "Models/GJUserMessage.h"
#include "Utils/LevelTools.h"
#include "Utils/GameToolbox.h"

GJUserMessage* GJUserMessage::create() {
    auto message = new GJUserMessage();
    if (message) {
        message->autorelease();
    }
    return message;
}

GJUserMessage::~GJUserMessage() {
}