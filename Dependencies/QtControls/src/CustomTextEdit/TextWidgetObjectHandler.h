#ifndef TEXTWIDGETOBJECTHANDLER_H
#define TEXTWIDGETOBJECTHANDLER_H

#include "QObject"
#include "QTextObjectInterface"

class CustomTextEdit;
class TextWidgetObjectHandler : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
    
    CustomTextEdit* m_edit = nullptr;
    QHash<QWidget*, QPixmap> m_pixmapCache;
    bool m_paintWidget = false;
    int m_documentWidth = 0;
public:
    TextWidgetObjectHandler(CustomTextEdit* textEdit);
    
    QSizeF intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat& format) override;
    void drawObject(QPainter* painter, const QRectF& wholeRect, QTextDocument* doc, int posInDocument, const QTextFormat& format) override;
    
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // TEXTWIDGETOBJECTHANDLER_H
