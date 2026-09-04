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
#include <QTimer>
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

        QTimer* resizeTimer;      
        QPixmap cachedPreviewPixmap;

        QGroupBox* previewGroupBox;

        void setComboBoxStyle(QComboBox* cbox, const QStringList elements, int distance);
        void addParam(const QString label, QFormLayout* layout, QWidget* pwgt, int distance);
        void updateThumbnail();

        void computeHeaders();
    
    protected:
    
        void resizeEvent(QResizeEvent* event) override;
   

    public:
        PageSetupDialog(QWidget* pwgt, QPrinter* printer_, QPrintPreviewWidget* previewWidget_, QTextDocument *doc_, QString headerTitle_, QList<QList<Headers>> headersValues);
    
    private slots:
    
        void renderThumbnail(); 
        
        void scaleCachedThumbnail();

    signals:
        void headersChanged(QStringList, QStringList, QList<QList<Headers>>);
    
};