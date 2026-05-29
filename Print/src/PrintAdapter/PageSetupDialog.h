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

enum class Headers {
    TITLE,
    PAGE_NUMBER,
    PAGE_NUMBER_TOTAL,
    COUNT_PAGES,
    LONG_DATE,
    SHORT_DATE,
    TIME,
    EMPTY
};

class PageSetupDialog: public BaseDialog {
    Q_OBJECT

    private:
        QPrinter* printer;
        QPrintPreviewWidget* previewWidget;
        QLabel* previewLabel;
        QTextDocument *doc;
        QList<QLineEdit*> marginEdits;
        QList<QComboBox*> headerComboBoxes;
        QComboBox* paperComboBox;
        QComboBox* orientationComboBox;
        QString headerTitle;


        void setCancelButtonStyle(QPushButton* btn);
        void setApplyButtonStyle(QPushButton* btn);
        void setComboBoxStyle(QComboBox* cbox, const QStringList elements, int distance);
        void setGroupBoxStyle(QGroupBox* gbox);
        void addParam(const QString label, QFormLayout* layout, QWidget* pwgt, int distance);
        void setLineEditStyle(QLineEdit* lineEdit);
        void updateThumbnail();

        void computeHeaders();

    public:
        PageSetupDialog(QWidget* pwgt, QPrinter* printer_, QPrintPreviewWidget* previewWidget_, QTextDocument *doc_, QString headerTitle_, QList<QList<Headers>> headersValues);
    
    signals:
        void headersChanged(QStringList, QStringList, QList<QList<Headers>>);
};