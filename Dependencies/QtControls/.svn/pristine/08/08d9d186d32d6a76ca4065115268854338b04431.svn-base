#ifndef CUSTOMICONBUTTONCORNER_H
#define CUSTOMICONBUTTONCORNER_H

#include <QFrame>
#include <QStackedLayout>
#include <QPushButton>
#include <QLabel>

class CustomIconButtonCorner : public QFrame
{
    Q_OBJECT
    
    QStackedLayout* m_layout;
    QPushButton* m_button;
    QLabel* m_label;
public:
    explicit CustomIconButtonCorner(QWidget *parent = nullptr);
    ~CustomIconButtonCorner();
    
    void SetText(const QString& text);
    void SetIcon(const QIcon& icon, double scaleFactor = 0.6);
    void SetColor(QColor color);
    void SetTextColor(QColor color);
signals:
    void clicked();
};

#endif // CUSTOMICONBUTTONCORNER_H
