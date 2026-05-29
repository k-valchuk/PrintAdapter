
#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>

#include <QObject>

class ElidedLabel : public QLabel 
{
    Q_OBJECT
public:
    using QLabel::QLabel;
    // Set the elide mode used for displaying text.
    void setElideMode(Qt::TextElideMode elideMode);
    // Get the elide mode currently used to display text.
    Qt::TextElideMode elideMode() const { return m_elideMode; }
    void setTextAndElide(const QString& text);
    QString Text() {return m_text;}
    QSize sizeHint() const override;
protected:
    void resizeEvent(QResizeEvent *e) override;
private:
    void updateElidedText();
    
private:
    Qt::TextElideMode m_elideMode = Qt::ElideRight;
    QString m_elidedText;
    QString m_text;
};

#endif // ELIDEDLABEL_H
