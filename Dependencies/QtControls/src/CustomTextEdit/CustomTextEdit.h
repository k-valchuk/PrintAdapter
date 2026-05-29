#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <QPointer>
#include <QSharedPointer>
#include <QMap>
#include <QSet>

struct TextWidget
{
    QPointer<QWidget> Widget;
    QVariant Data;
    int BorderRadius = 10;
    
    TextWidget(QWidget* wgt) : Widget(wgt) {}
    TextWidget(QVariant data) : Data(data) {}
};

Q_DECLARE_METATYPE(QSharedPointer<TextWidget>);

class CustomTextEdit : public QTextEdit
{
    Q_OBJECT
    
    QPoint m_lastCursorPosition;
    QPointer<QWidget> m_lastWidgetUnderCursor;
    QPointer<QWidget> m_focusedWidget;
    QPointer<QWidget> m_mouseGrabber;
    
    QPointF childTopLeftR(QWidget* wgt, QWidget* parent);
    
    bool SendEventToWidget(QEvent* ev);
    
    QSet<QSharedPointer<TextWidget>> m_createdWidgets;
    QSet<QWidget*> m_widgetsUnderSelection;
    
    QObject* handler = nullptr;
    
    friend class TextWidgetObjectHandler;
    friend class CustomTextEditViewportEventsFilter;
    friend class CustomTextEditMainEventsFilter;
    
    void setupWidget(QWidget* wgt);
    QHash<QWidget*, QRectF> widgets;
    bool m_mouseCapturedByEdit = false;
    
    QObject* m_mouseEventsFilter = nullptr;
    QObject* m_keyEventsFilter = nullptr;
    double m_baseFntSize;
    double m_currentScale = 1.0;
    
    void setEditorActive(bool active);
public:
    enum EnterDirection
    {
        PreviousChar,
        NextChar,
        PreviousLine,
        NextLine
    };
    
    CustomTextEdit(QWidget* parent = nullptr);
    ~CustomTextEdit();
    
    void SetFocusOnTextWidget(QWidget* wgt);
    
    void insertWidget(QTextCursor& cur, QWidget* wgt);
    void insertWidget(QTextCursor& cur, const QVariant& data);
    
    static QTextCharFormat GetNextCharacterFormat(const QTextCursor& cur);
    
    QSharedPointer<TextWidget> GetWidget(const QTextCursor& cur) const;
    static QSharedPointer<TextWidget> GetWidgetFromFormat(const QTextCharFormat& charFormat);
    
    //ВЫЗЫВАТЬ ЕСЛИ ВЫ ЗАБЛОЧИЛИ АВТОМАТИЧЕСКУЮ ПРОВЕРКУ ПРИ СИГНАЛЕ textChanged, НО ПОМЕНЯЛИ ТЕКСТ!!!!
    void UpdateInternalWidgetsData();
protected:
    virtual QMimeData* createMimeDataFromSelection() const override;
    
    void insertFromMimeData(const QMimeData *source) override;
    
    void wheelEvent(QWheelEvent *ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    
    void restoreInsertedWidgets(QTextDocument* doc, int startPosition, const QMimeData* source);
    
protected: //функции которые требуется переопределить для сереализации-восстановления виджетов
    virtual QWidget* createWidget(const QVariant& mimeData) const;
    virtual QVariant serializeWidget(QWidget* wgt, const QVariant& existingData) const;
    virtual void destroyWidget(QWidget* wgt);
    virtual void mergeWidgetFormatWithOther(QTextCharFormat& wf, const QTextCharFormat& other);
    
public slots:
    void onWidgetLeave(CustomTextEdit::EnterDirection to);
signals:
    void CTEScaleChanged(double);
    void maxWidthChanged(int);
};


#endif // CUSTOMTEXTEDIT_H
