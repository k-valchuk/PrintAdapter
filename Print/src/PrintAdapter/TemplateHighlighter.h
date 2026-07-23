#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class TemplateHighlighter: public QSyntaxHighlighter {
    Q_OBJECT

    public:
        TemplateHighlighter(QTextDocument* parent = nullptr);
    
        
    protected:
        void highlightBlock(const QString &text) override;
    
    private:

    void highlightHtmlPart(const QString &text);

    enum HighlightState {
        State_Html  = -1,
        State_InCss  = 1
    };

    QTextCharFormat cssSelectorFormat;
    QTextCharFormat cssPropertyFormat;
    QTextCharFormat cssValueFormat;
    QTextCharFormat cssAtRuleFormat;

    QTextCharFormat bracketFormat;
    QTextCharFormat doctypeFormat;
    QTextCharFormat tagAttributesFormat;
    QTextCharFormat tagNameFormat;
    QTextCharFormat customTagFormat;
    QTextCharFormat customKeywordFormat;
    QTextCharFormat fieldFormat;

    QRegularExpression htmlTagRegex;
    QRegularExpression doctypeRegex;
    QRegularExpression fieldRegex;
    QRegularExpression customTagRegex;

    QRegularExpression cssSelectorRegex;
    QRegularExpression cssPropertyRegex;
    QRegularExpression cssValueRegex;
    QRegularExpression cssAtRuleRegex;


};