#include "BaseRequestButton.h"

BaseRequestButton::BaseRequestButton(
    QWidget* pwgt, 
    ServerRequester* server_requester, 
    QLayout* layout, 
    const QString& buttonName,
    QButtonGroup* button_group,
    ActionId buttonId
): QPushButton(pwgt), server_requester(server_requester) {
    setText(buttonName);
    layout->addWidget(this);
    button_group->addButton(this, static_cast<int>(buttonId));
}

