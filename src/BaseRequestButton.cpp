#include "BaseRequestButton.h"

BaseRequestButton::BaseRequestButton(
    QWidget* pwgt, 
    ServerRequester* server_requester, 
    QLayout* layout, 
    const QString& buttonName
): QPushButton(pwgt), server_requester(server_requester) {
    setText(buttonName);
    layout->addWidget(this);
}

