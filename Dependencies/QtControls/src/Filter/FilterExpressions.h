#ifndef FILTEREXPRESSIONS_H
#define FILTEREXPRESSIONS_H

#include <QWidget>
#include <QLayout>
#include <QSet>
#include <QJsonObject>
#include "FilterEllipse.h"

class IExpression;
class ICustomExpressionData
{
protected:
    IExpression* m_exp;
public:
    void SetExp(IExpression* exp) {m_exp = exp;}
    virtual ~ICustomExpressionData() {}
};

class IExpression;
Q_DECLARE_METATYPE(QSharedPointer<IExpression>);
class IExpression
{
    
protected:
    
    IExpression* m_parent = nullptr;
    ICustomExpressionData* m_data = nullptr;
public:
    IExpression(IExpression* parent)
        : m_parent(parent)
    {
        qRegisterMetaType<QSharedPointer<IExpression>>();
    }
    virtual void Delete() = 0;
    IExpression* Parent() {return m_parent;}
    
    void SetData(ICustomExpressionData* data)
    {
        m_data = data;
        if(data)
            data->SetExp(this);
    }
    
    ICustomExpressionData* GetData()
    {
        return m_data;
    }
    
    template <class T>
    T* GetData()
    {
        if(m_data)
            return dynamic_cast<T*>(m_data);
        else
            return nullptr;
    }
    
    void SetParent(IExpression* parent)
    {
        m_parent = parent;
    }
    
    virtual QList<IExpression*> Children()
    {
        return {};
    }
    
    virtual void SwapChild(IExpression* old, IExpression* now) { }
    
    virtual ~IExpression()
    {
        if(m_data)
            delete m_data;
    }
    
    void Tree(std::function<void(IExpression*)> functor)
    {
        for(auto child : Children())
        {
            if(child)
                child->Tree(functor);
        }
        functor(this);
    }
    
    void Tree(std::function<void(IExpression*)> functor, std::function<bool(IExpression*)> goDown)
    {
        for(auto child : Children())
        {
            if(child && goDown(child))
                child->Tree(functor, goDown);
        }
        functor(this);
    }
    
    void Tree(std::function<void(IExpression*)> functor,
              std::function<void()> onDown, std::function<void()> onUp, bool inverted = false)
    {
        onDown();
        auto children = Children();
        for(int i = 0; i < children.size(); i++)
        {
            auto child = inverted ? children[children.size() - i - 1] : children[i];
            if(child)
                child->Tree(functor, onDown, onUp, inverted);
        }
        onUp();
        functor(this);
    }
    
    virtual QJsonObject ToJson() = 0;
   
    
    virtual void FillFromJson(QJsonObject obj) {}
    static IExpression* FromJson(QJsonObject obj);
    
    virtual IExpression* Copy() = 0;
};

class IUnaryExpression : public IExpression
{
protected:
    IExpression* m_exp = nullptr;
public:
    IUnaryExpression(IExpression* parent)
        : IExpression(parent)
    {
        
    }
    void SetChild(IExpression* exp)
    {
        m_exp = exp;
    }
    IExpression* GetChild()
    {
        return m_exp;
    }
    
    void SwapChild(IExpression* old, IExpression* now) override
    {
        m_exp = now;
    }
    
    QList<IExpression*> Children() override
    {
        return {m_exp};
    }
    
    void Delete() override
    {
        Q_ASSERT_X(false, "filter widget", "can not delete root node");
    }
    
    QJsonObject ToJson() override
    {
        if(m_exp)
            return m_exp->ToJson();
        else
            return QJsonObject();
    }
    
    virtual ~IUnaryExpression()
    {
        if(m_exp)
            delete m_exp;
    }
};

class RootExpression : public IUnaryExpression
{
public:
    RootExpression() : IUnaryExpression(nullptr)
    {
        
    }
    
    
    IExpression* Copy() override
    {
        auto copy = new RootExpression();
        copy->m_exp = this->m_exp->Copy();
        copy->m_exp->SetParent(copy);
        return copy;
    }
    IExpression* CopyOfChild()
    {
        if(m_exp)
            return m_exp->Copy();
        else
            return nullptr;
    }
};

class BracketsExpression : public IUnaryExpression
{
public:
    BracketsExpression(IExpression* parent)
        : IUnaryExpression(parent)
    {
        
    }
    
    QJsonObject ToJson() override
    {
        QJsonObject obj;
        obj["nodeType"] = "brackets";
        if(m_exp)
            obj["child"] = m_exp->ToJson();
        else
            Q_ASSERT_X(false, "filter widget", "to JSON - brackets child is null");
        return obj;
    }
    
    void FillFromJson(QJsonObject obj) override
    {
        if(obj.contains("child"))
        {
            m_exp = FromJson(obj["child"].toObject());
            m_exp->SetParent(this);
        }
        else
            m_exp = nullptr;
    }
    
    IExpression* Copy() override
    {
        auto copy = new BracketsExpression(nullptr);
        copy->m_exp = this->m_exp->Copy();
        copy->m_exp->SetParent(copy);
        return copy;
    }
};


class IBinaryOperator : public IExpression
{
protected:
    IExpression* m_left = nullptr;
    IExpression* m_right = nullptr;
    
    virtual QString Key() = 0;
public:
    IBinaryOperator(IExpression* parent, IExpression* left, IExpression* right)
        : m_left(left), m_right(right), IExpression(parent)
    {
        if(left)
            left->SetParent(this);
        if(right)
            right->SetParent(this);
    }
    void Delete() override
    {
        Q_ASSERT_X(false, "filter widget", "can not delete operator itself");
    }
    void SetLeft(IExpression* left)
    {
        m_left = left;
    }
    void SetRight(IExpression* right)
    {
        m_right = right;
    }

    
    void SwapChild(IExpression* old, IExpression* now) override
    {
        if(old == m_left)
            m_left = now;
        else
            m_right = now;
    }
    
    IExpression* Left() {return m_left;}
    IExpression* Right() {return m_right;}
    
    QList<IExpression*> Children() override
    {
        return {m_left, m_right};
    }
    
    QJsonObject ToJson() override
    {
        QJsonObject obj;
        obj["nodeType"] = "binaryOperator";
        obj["key"] = Key();
        
        if(m_left)
            obj["left"] = m_left->ToJson();
        else
            Q_ASSERT_X(false, "filter widget", "to JSON - binary op left is null");
        
        if(m_right)
            obj["right"] = m_right->ToJson();
        else
            Q_ASSERT_X(false, "filter widget", "to JSON - binary op right is null");
        
        return obj;
    }
    
    void FillFromJson(QJsonObject obj) override
    {
        if(obj.contains("left"))
        {
            m_left = FromJson(obj["left"].toObject());
            m_left->SetParent(this);
        }
        else
            m_left = nullptr;
        
        if(obj.contains("right"))
        {
            m_right = FromJson(obj["right"].toObject());
            m_right->SetParent(this);
        }
        else
            m_right = nullptr;
    }
    
    virtual ~IBinaryOperator()
    {
        for(auto child : {m_left, m_right})
            if(child)
                delete child;
    }
};

class AndOperator : public IBinaryOperator
{
protected:
    QString Key() override {return OperatorKey();}
public:
    static QString OperatorKey() {return "and";}
    AndOperator(IExpression* parent, IExpression* left, IExpression* right)
        : IBinaryOperator(parent, left, right)
    {
        
    }
    
    IExpression* Copy() override
    {
        auto copy = new AndOperator(nullptr, nullptr, nullptr);
        copy->m_left = this->m_left->Copy();
        copy->m_left->SetParent(copy);
        copy->m_right = this->m_right->Copy();
        copy->m_right->SetParent(copy);
        return copy;
    }
};

class OrOperator : public IBinaryOperator
{
protected:
    QString Key() override {return OperatorKey();}
public:
    static QString OperatorKey() {return "or";}
    OrOperator(IExpression* parent, IExpression* left, IExpression* right)
        : IBinaryOperator(parent, left, right)
    {
        
    }
    
    IExpression* Copy() override
    {
        auto copy = new OrOperator(nullptr, nullptr, nullptr);
        copy->m_left = this->m_left->Copy();
        copy->m_left->SetParent(copy);
        copy->m_right = this->m_right->Copy();
        copy->m_right->SetParent(copy);
        return copy;
    }
};

class FilterExpression : public IExpression
{
    QVariant m_filterKey;
    QVariant m_filterOperatorKey;
    QVariant m_filterValue;
public:
    FilterExpression(IExpression* parent)
        : IExpression(parent)
    {
        
    }
    FilterExpression(QVariant filterKey, QVariant filterOperatorKey, QVariant filterValue, IExpression* parent = nullptr)
        : IExpression(parent), m_filterKey(filterKey), m_filterOperatorKey(filterOperatorKey), m_filterValue(filterValue)
    {
        
    }
    
    void Delete() override
    {
        if(auto op = dynamic_cast<IBinaryOperator*>(m_parent))
        {
            auto other = op->Left() == this ? op->Right() : op->Left();
            
            if(auto brackets = dynamic_cast<BracketsExpression*>(op->Parent()))
            {
                if(dynamic_cast<FilterExpression*>(other))
                {
                    brackets->Parent()->SwapChild(brackets, op);
                    op->SetParent(brackets->Parent());
                    
                    brackets->SetChild(nullptr);
                    delete brackets;
                }
            }
            
            if(auto parentOp = dynamic_cast<IBinaryOperator*>(op->Parent()))
            {
                if(parentOp->Left() == op)
                    parentOp->SetLeft(other);
                else
                    parentOp->SetRight(other);
                
                other->SetParent(parentOp);
            }
            else if(auto root = dynamic_cast<IUnaryExpression*>(op->Parent()))
            {
                root->SetChild(other);
                other->SetParent(root);
            }
            else
                Q_ASSERT_X(false, "filter widget", "unknown type");
            
            op->SetLeft(nullptr); op->SetRight(nullptr);
            delete op;
            delete this;
        }
        else if(auto root = dynamic_cast<RootExpression*>(m_parent))
        {
            root->SetChild(nullptr);
            delete this;
        }
    }
    
    void SetFilter(QVariant filterKey, QVariant filterOperatorKey)
    {
        m_filterKey = filterKey;
        m_filterOperatorKey = filterOperatorKey;
    }
    
    void SetFilterValue(QVariant value)
    {
        m_filterValue = value;
    }
    
    QVariant GetFilterKey() {return m_filterKey;}
    QVariant GetFilterOperatorKey() {return m_filterOperatorKey;}
    QVariant GetFilterValue() {return m_filterValue;}
    
    QJsonObject ToJson() override
    {
        QJsonObject obj;
        obj["nodeType"] = "filter";

        obj["key"] = m_filterKey.toJsonValue();
        obj["filterOperator"] = m_filterOperatorKey.toJsonValue();
        obj["value"] = m_filterValue.toJsonValue();
        
        return obj;
    }
    
    void FillFromJson(QJsonObject obj) override
    {
        if(obj.contains("key") && obj.contains("filterOperator"))
            this->SetFilter(obj["key"].toVariant(), obj["filterOperator"].toVariant());
        if(obj.contains("value"))
            this->SetFilterValue(obj["value"].toVariant());
    }
    
    IExpression* Copy() override
    {
        auto copy = new FilterExpression(nullptr);
        copy->m_filterKey = this->m_filterKey;
        copy->m_filterOperatorKey = this->m_filterOperatorKey;
        copy->m_filterValue = this->m_filterValue;
        return copy;
    }
};

template<class BinaryOp> //where BinaryOp : IBinaryOperator
IExpression* Concat(IExpression* left, IExpression* right)
{
    if(!left && right)
        return right;
    if(!right && left)
        return left;
    if(!right && !left)
        return nullptr;
    
    auto op = new BinaryOp(nullptr, nullptr, nullptr);
    auto bin = dynamic_cast<IBinaryOperator*>(op);
    if(!bin)
        Q_ASSERT_X(false, "concat expressions", "template must be subclass of IBinaryOperator");
    
    bin->SetLeft(left);
    bin->SetRight(right);
    return op;
}

template<class BinaryOp> //where BinaryOp : IBinaryOperator
QSharedPointer<IExpression> Concat(QSharedPointer<IExpression> left, QSharedPointer<IExpression> right)
{
    if(!left && right)
        return QSharedPointer<IExpression>(right->Copy());
    if(!right && left)
        return QSharedPointer<IExpression>(left->Copy());
    if(!right && !left)
        return nullptr;
    
    auto op = new BinaryOp(nullptr, nullptr, nullptr);
    auto bin = dynamic_cast<IBinaryOperator*>(op);
    if(!bin)
        Q_ASSERT_X(false, "concat expressions", "template must be subclass of IBinaryOperator");
    
    bin->SetLeft(left->Copy());
    bin->SetRight(right->Copy());
    return QSharedPointer<IExpression>(op);
}

template<class BinaryOp, class ...Args> //where BinaryOp : IBinaryOperator
IExpression* Concat(IExpression* left, IExpression* right, Args... args)
{
    if(!left && right)
        return right;
    if(!right && left)
        return left;
    if(!right && !left)
        return nullptr;
    
    auto op = new BinaryOp(args...);
    auto bin = dynamic_cast<IBinaryOperator*>(op);
    if(!bin)
        Q_ASSERT_X(false, "concat expressions", "template must be subclass of IBinaryOperator");
    
    bin->SetLeft(left);
    bin->SetRight(right);
    return op;
}


void TransformNodes(QSharedPointer<IExpression>& root,
                    std::function<bool(IExpression*)> predicate,
                    std::function<IExpression*(IExpression*)> transform);

#endif // FILTEREXPRESSIONS_H
