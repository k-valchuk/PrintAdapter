
#include "ElidedLabel.h"

void ElidedLabel::setElideMode(Qt::TextElideMode elideMode)
{
    m_elideMode = elideMode;
    updateElidedText();
}

void ElidedLabel::setTextAndElide(const QString& text)
{
    m_text = text;
    updateElidedText();
}

QSize ElidedLabel::sizeHint() const
{
    auto width = contentsMargins().left() + contentsMargins().right() + fontMetrics().horizontalAdvance(m_text);
    return QSize(
        width,
        heightForWidth(width)
        );
}

void ElidedLabel::resizeEvent(QResizeEvent *e)
{
    QLabel::resizeEvent(e);
    updateElidedText();
}

void ElidedLabel::updateElidedText()
{
    // setText() is not virtual ... :/
    const QFontMetrics fm(fontMetrics());
    m_elidedText = fm.elidedText(m_text,
                                    m_elideMode,
                                    width());
    // make sure to show at least the first character
    if (!m_elidedText.isEmpty())
    {
        const QString showFirstCharacter = m_text.at(0) + QStringLiteral("...");
        setMinimumWidth(fm.horizontalAdvance(showFirstCharacter) + 1);
    }
    QLabel::setText(m_elidedText);
}

