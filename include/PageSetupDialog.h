#pragma once

#include <QPrinter>
#include <QPrintPreviewWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QTextDocument>
#include <QEvent>
#include <QLineEdit>
#include "BaseDialog.h"

class PageSetupDialog: public BaseDialog {
    Q_OBJECT

    private:
        QPrinter* printer;
        QPrintPreviewWidget* previewWidget;
        QLabel* previewLabel;
        QTextDocument *doc;
        QList<QLineEdit*> marginEdits;
        QComboBox* paperComboBox;
        QComboBox* orientationComboBox;

        bool eventFilter(QObject *watched, QEvent *event) override;


        void setCancelButtonStyle(QPushButton* btn);
        void setApplyButtonStyle(QPushButton* btn);
        void setComboBoxStyle(QComboBox* cbox, const QStringList elements, int distance);
        void setGroupBoxStyle(QGroupBox* gbox);
        void addParam(const QString label, QFormLayout* layout, QWidget* pwgt, int distance);
        void setLineEditStyle(QLineEdit* lineEdit);
        void updateThumbnail();

    public:
        PageSetupDialog(QWidget* pwgt, QPrinter* printer_, QPrintPreviewWidget* previewWidget_, QTextDocument *doc_);
};