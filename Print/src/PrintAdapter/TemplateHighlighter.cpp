#include "TemplateHighlighter.h"

TemplateHighlighter::TemplateHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent) {

    doctypeFormat.setForeground(QColor("#5C6370"));
    doctypeFormat.setFontItalic(true);
    doctypeRegex = QRegularExpression("<!DOCTYPE\\s+html>", QRegularExpression::CaseInsensitiveOption);

    fieldFormat.setForeground(QColor("#C678DD")); 
    fieldRegex = QRegularExpression("#[A-Za-z0-9_.]+#");

    customTagFormat.setForeground(QColor("#98C379"));
    customKeywordFormat.setForeground(QColor("#ddcd54")); 

    customTagRegex = QRegularExpression("(</?#)(Repeat)(#)([A-Za-z0-9_.]*)(#>)");

    bracketFormat.setForeground(QColor("#5293b8"));
    tagNameFormat.setForeground(QColor("#68c1fc"));
    htmlTagRegex = QRegularExpression("(<)(/[a-zA-Z0-9]+|[a-zA-Z0-9]+)([^>]*)(>)");

    tagAttributesFormat.setForeground(QColor("#D19A66"));

    cssSelectorFormat.setForeground(QColor("#D19A66"));

    cssPropertyFormat.setForeground(QColor("#E06C75"));
    cssValueFormat.setForeground(QColor("#98C379"));
    cssAtRuleFormat.setForeground(QColor("#C678DD")); 

    cssSelectorRegex = QRegularExpression("([\\.#@]?[A-Za-z0-9_-]+)\\s*\\{");
    cssPropertyRegex = QRegularExpression("([A-Za-z0-9_-]+)\\s*:");
    cssValueRegex = QRegularExpression(":\\s*([^;]+);");
    cssAtRuleRegex = QRegularExpression("@[A-Za-z0-9_-]+");
}

void TemplateHighlighter::highlightHtmlPart(const QString &text) {
    QRegularExpressionMatchIterator htmlIt  = htmlTagRegex.globalMatch(text);
    while (htmlIt.hasNext()) {
        QRegularExpressionMatch match = htmlIt.next();
        setFormat(match.capturedStart(1), match.capturedLength(1), bracketFormat);
        setFormat(match.capturedStart(2), match.capturedLength(2), tagNameFormat);

        if (match.capturedLength(3) > 0) {
            setFormat(match.capturedStart(3), match.capturedLength(3), tagAttributesFormat);
        }

        setFormat(match.capturedStart(4), match.capturedLength(4), bracketFormat);
    }

    QRegularExpressionMatchIterator doctypeIt = doctypeRegex.globalMatch(text);
    while (doctypeIt.hasNext()) {
        QRegularExpressionMatch match = doctypeIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), doctypeFormat);
    }

    QRegularExpressionMatchIterator fieldIt = fieldRegex.globalMatch(text);
    while (fieldIt.hasNext()) {
        QRegularExpressionMatch match = fieldIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), fieldFormat);
    }

    QRegularExpressionMatchIterator customIt = customTagRegex.globalMatch(text);
    while (customIt.hasNext()) {
        QRegularExpressionMatch match = customIt.next();

        setFormat(match.capturedStart(1), match.capturedLength(1), customTagFormat);
        
        setFormat(match.capturedStart(2), match.capturedLength(2), customKeywordFormat);
        
        setFormat(match.capturedStart(3), match.capturedLength(3), customTagFormat);
        
        if (match.capturedLength(4) > 0) {
            setFormat(match.capturedStart(4), match.capturedLength(4), customTagFormat);
        }

        setFormat(match.capturedStart(5), match.capturedLength(5), customTagFormat);
    }
}

void TemplateHighlighter::highlightBlock(const QString &text) {
    
    int currentState = previousBlockState();
    if (currentState == -1) {
        currentState = State_Html;
    }

    int index = 0;

    if (currentState == State_Html) {
        int styleStartIndex  = text.indexOf("<style", 0, Qt::CaseInsensitive);

        if (styleStartIndex  != -1) {
            highlightHtmlPart(text);
            currentState = State_InCss;
        } else {
            highlightHtmlPart(text);
        }
    } else if (currentState == State_InCss) {

        int styleEndIndex = text.indexOf("</style>", 0, Qt::CaseInsensitive);

        if (styleEndIndex != -1) {
            highlightHtmlPart(text);
            currentState = State_Html;
        } else {
            QRegularExpressionMatchIterator selIt = cssSelectorRegex.globalMatch(text);
            while (selIt.hasNext()) {
                QRegularExpressionMatch match = selIt.next();
                setFormat(match.capturedStart(1), match.capturedLength(1), cssSelectorFormat);
            }

            QRegularExpressionMatchIterator propIt = cssPropertyRegex.globalMatch(text);
            while (propIt.hasNext()) {
                QRegularExpressionMatch match = propIt.next();
                setFormat(match.capturedStart(1), match.capturedLength(1), cssPropertyFormat);
            }

            QRegularExpressionMatchIterator valIt = cssValueRegex.globalMatch(text);
            while (valIt.hasNext()) {
                QRegularExpressionMatch match = valIt.next();
                setFormat(match.capturedStart(1), match.capturedLength(1), cssValueFormat);
            }

            QRegularExpressionMatchIterator atIt = cssAtRuleRegex.globalMatch(text);
            while (atIt.hasNext()) {
                QRegularExpressionMatch match = atIt.next();
                setFormat(match.capturedStart(), match.capturedLength(), cssAtRuleFormat);
            }
        }
    }

    setCurrentBlockState(currentState);

}