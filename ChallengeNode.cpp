#include "ChallengeNode.h"
#include "GJChallengeItem.h"
#include "ChallengesPage.h"
#include "FMODAudioEngine.h"
#include "cocos2d.h"

ChallengeNode* ChallengeNode::create(GJChallengeItem* challengeItem, ChallengesPage* challengesPage, bool isNew) {
    ChallengeNode* node = new ChallengeNode();
    if (node && node->init(challengeItem, challengesPage, isNew)) {
        node->autorelease();
        return node;
    }
    delete node;
    return nullptr;
}

void ChallengeNode::onClaimReward(cocos2d::CCObject* sender) {
    cocos2d::CCPoint pos = this->getPosition() + m_unkPoint;
    m_challengesPage->claimItem(this, m_challengeItem, pos);

    FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");

    sender->setVisible(false);
    sender->setEnabled(false);
}

void ChallengeNode::updateTimeLabel(std::string text) {
    if (m_countdownLabel) {
        m_countdownLabel->setString(text.c_str());
    }
}

ChallengeNode::~ChallengeNode() {
    // Destructor
}