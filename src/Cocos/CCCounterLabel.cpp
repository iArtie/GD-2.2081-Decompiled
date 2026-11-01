#include "Cocos/CCCounterLabel.h"
#include "Utils/GameToolbox.h"
#include "Platform/cocos2d.h"

void CCCounterLabel::calculateStepSize(int steps) {
    float stepFloat = static_cast<float>(steps);
    float diff = static_cast<float>(m_targetCount - m_currentCount);
    float absDiff = fabsf(diff);

    if (absDiff < static_cast<float>(steps)) {
        stepFloat = absDiff;
    }

    m_stepSize = diff / stepFloat;
}

CCCounterLabel* CCCounterLabel::create(int count, const char* font, FormatterType type) {
    CCCounterLabel* label = new CCCounterLabel();
    if (label && label->init(count, font, type)) {
        label->autorelease();
        return label;
    }
    delete label;
    return nullptr;
}

void CCCounterLabel::disableCounter() {
    if (m_counterEnabled) {
        m_counterEnabled = false;
        if (!m_dontSchedule) {
            this->unscheduleAllSelectors();
        }
    }
}

void CCCounterLabel::enableCounter() {
    if (!m_counterEnabled) {
        m_counterEnabled = true;
        if (!m_dontSchedule) {
            this->schedule(schedule_selector(CCCounterLabel::updateCounter));
        }
    }
}

void CCCounterLabel::fastUpdateCounter() {
    m_currentCount = m_targetCount;
    updateString();
}

int CCCounterLabel::getTargetCount() {
    return m_targetCount;
}

bool CCCounterLabel::init(int count, const char* font, FormatterType type) {
    if (!cocos2d::CCLabelBMFont::initWithString(" ", font, -1.0f, cocos2d::kCCTextAlignmentLeft, cocos2d::CCPoint(0, 0))) {
        return false;
    }

    m_formatterType = type;
    m_targetCount = count;
    fastUpdateCounter();

    return true;
}

void CCCounterLabel::setTargetCount(int count) {
    m_targetCount = count;
    calculateStepSize(m_stepCount);
    enableCounter();
}

void CCCounterLabel::updateCounter(float dt) {
    if (!m_counterEnabled) {
        return;
    }

    if (m_currentCount < m_targetCount) {
        float diff = static_cast<float>(m_targetCount - m_currentCount);
        if (diff > m_stepSize) {
            m_currentCount = static_cast<int>(static_cast<float>(m_currentCount) + m_stepSize);
        } else {
            m_currentCount = static_cast<int>(static_cast<float>(m_currentCount) + diff);
        }
        updateString();
    } else if (m_currentCount > m_targetCount) {
        float diff = static_cast<float>(m_targetCount - m_currentCount);
        if (diff < m_stepSize) {
            m_currentCount = static_cast<int>(static_cast<float>(m_currentCount) + m_stepSize);
        } else {
            m_currentCount = static_cast<int>(static_cast<float>(m_currentCount) + diff);
        }
        updateString();
    } else {
        disableCounter();
    }
}

void CCCounterLabel::updateString() {
    if (m_formatterType == FormatterType_Float) {
        CCString* str = CCString::createWithFormat("%.2f", static_cast<float>(m_currentCount) / 100.0f);
        this->setString(str->getCString());
    } else {
        std::string str = GameToolbox::intToString(m_currentCount);
        this->setString(str.c_str());
    }
}

CCCounterLabel::~CCCounterLabel() {
    // Destructor
}