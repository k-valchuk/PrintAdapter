#include "BaseRequestButton.h"

BaseRequestButton::BaseRequestButton(
    QWidget* pwgt, 
    ServerRequester* server_requester, 
    const QString& buttonName
): QPushButton(pwgt), server_requester(server_requester) {
    setText(buttonName);
}

void BaseRequestButton::setCurrentButton(ActionId buttonId) {
    server_requester->setCurrentButton(buttonId);
}
