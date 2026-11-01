#include "SetupInstantCollisionTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "GameToolbox.h"
#include "CCTextInputNode.h"

void SetupInstantCollisionTriggerPopup::updateDefaultTriggerValues()
{
    SetupTriggerPopup::updateDefaultTriggerValues();
    updateSpecialNodes();
}

void SetupInstantCollisionTriggerPopup::updateSpecialNodes()
{
    bool bothPlayers = getValue(201) != 0.0f;
    float targetP1 = getValue(138);
    float targetP2 = getValue(200);

    CCTextInputNode* p1Input = (CCTextInputNode*)m_inputNodes->objectForKey("p1");
    CCTextInputNode* p2Input = (CCTextInputNode*)m_inputNodes->objectForKey("p2");

    if (p1Input)
    {
        if (bothPlayers)
        {
            p1Input->setString("P1");
        }
        else if (targetP2 == 0.0f)
        {
            if (targetP1 == 0.0f)
                p1Input->setString(GameToolbox::intToString((int)getValue(80)));
            else
                p1Input->setString("P1");
        }
        else
        {
            if (targetP1 == 0.0f)
                p1Input->setString("P2");
            else
                p1Input->setString("P");
        }
    }

    if (p2Input)
    {
        if (bothPlayers)
        {
            p2Input->setString("P2");
        }
        else
        {
            p2Input->setString(GameToolbox::intToString((int)getValue(95)));
        }
    }
}

void SetupInstantCollisionTriggerPopup::valueDidChange(int tag, float value)
{
    if (tag == 138 || (tag >= 138 && (unsigned int)(tag - 200) <= 1))
        updateSpecialNodes();
}