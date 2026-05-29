#ifndef TREEGROUPINGSTYLEITEMDELEGATE_H
#define TREEGROUPINGSTYLEITEMDELEGATE_H

#include "styleItemDelegate.h"
#include "QTextDocument"

class RichTextData{
    QString m_str;
    inline static bool isRegistered = false;
public:
    inline RichTextData();
    inline RichTextData(const QString& richText);
    operator QString() const {return m_str;}
    operator QVariant() const {return QVariant::fromValue(*this);}
};
Q_DECLARE_METATYPE(RichTextData)
RichTextData::RichTextData(){
    if(!isRegistered){
        qRegisterMetaType<RichTextData>();
        QMetaType::registerConverter<RichTextData, QString>();
        isRegistered = true;
    }
}
RichTextData::RichTextData(const QString& richText)
    : m_str(richText)
{
    if(!isRegistered){
        qRegisterMetaType<RichTextData>();
        QMetaType::registerConverter<RichTextData, QString>();
        isRegistered = true;
    }
}


class TreeGroupingStyleItemDelegate : public StyleItemDelegate
{
    bool m_drawLines = false;
    
    class CustomTextDocument : public QTextDocument
    {
        int m_lineHeight = 20;
        QPixmap generatePixmap(const QString& path)
        {
            return QPixmap(path).scaledToHeight(m_lineHeight*0.7, Qt::SmoothTransformation);
        }
    public:
        CustomTextDocument(int lineHeight) : m_lineHeight(lineHeight) {}
    protected:
        
        QVariant loadResource(int type, const QUrl& name) override
        {
            if(type == QTextDocument::ResourceType::ImageResource){
                if(name.scheme() == "qrc")
                {
                    auto path = name.path();
                    if(path.startsWith('/'))
                        path.prepend(':');
                    else if(!path.startsWith(':'))
                        path.prepend(":/");
                    
                    auto pm = generatePixmap(path);
                    this->addResource(type, name, pm); //чтобы закешировать пиксмап, иначе будет оч сильно тупить грузя каждый раз
                    return pm;
                }
                
                auto localFile = name.toLocalFile();
                if(!localFile.isEmpty()){
                    QFile f(localFile);
                    if(f.exists())
                    {
                        auto pm = generatePixmap(localFile);
                        this->addResource(type, name, pm); //чтобы закешировать пиксмап, иначе будет оч сильно тупить грузя каждый раз
                        return pm;
                    }
                }
            }
            return QTextDocument::loadResource(type, name);
        }
    };
    
    struct CacheKey {
        QString text;
        int width;
        QFont fnt;
        bool operator==(const CacheKey &other) const {
            return text == other.text && width == other.width && fnt == other.fnt;
        }
    };
    friend uint qHash(const CacheKey&, uint);
    inline static QHash<CacheKey, int> m_richTextToWidthToCharsDeletedCache;
    inline static QHash<CacheKey, int> m_sizeHintCache;
public:
    TreeGroupingStyleItemDelegate(bool drawLines = false, QWidget* parent = nullptr);
    
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool drawLines(QPainter* painter, const QStyleOptionViewItem&, const QModelIndex&) const override;
    
    virtual void ModifyOption(const QModelIndex& index, QStyleOptionViewItem& opt) const {}
    virtual void PaintAfter(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    
    void setDrawSideLines(bool enable);
    
    static void DrawRichText(const QString& richText, QPainter* painter, const QStyleOptionViewItem& option,
                             const QVariant& highlightColor = QVariant());
    
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

inline uint qHash(const TreeGroupingStyleItemDelegate::CacheKey &key, uint seed = 0) {
    return qHash(key.text, seed) 
           ^ qHash(key.width, seed) 
           ^ qHash(key.fnt, seed);
}

#endif // TREEGROUPINGSTYLEITEMDELEGATE_H
