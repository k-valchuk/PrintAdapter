#pragma once

#include <QDialog>
#include <QComboBox>
#include <QTextBrowser>

class QComboBox;


enum class ContentType { HTML, TEXT };

class RequestDialog: public QDialog {
    Q_OBJECT
    

    private:
        QTextBrowser* browser;
    
    public:
        RequestDialog(QWidget* pwgt = nullptr);

    
    public slots:
        void changeBrowserContent(QString content, ContentType content_type);
};