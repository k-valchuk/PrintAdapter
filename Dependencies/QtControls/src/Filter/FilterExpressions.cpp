#include "FilterExpressions.h"



IExpression* IExpression::FromJson(QJsonObject obj)
{
    auto nodeType = obj["nodeType"].toString();
    
    IExpression* result = nullptr;
    
    if(nodeType == "filter")
    {
        result = new FilterExpression(nullptr);
    }
    else if(nodeType == "binaryOperator")
    {
        auto opKey = obj["key"].toString();
        if(opKey == AndOperator::OperatorKey())
        {
            result = new AndOperator(nullptr, nullptr, nullptr);
        }
        else if(opKey == OrOperator::OperatorKey())
        {
            result = new OrOperator(nullptr, nullptr, nullptr);
        }
    }
    else if(nodeType == "brackets")
    {
        result = new BracketsExpression(nullptr);
    }
    
    if(result)
        result->FillFromJson(obj);
    
    return result;
}

void TransformNodes(QSharedPointer<IExpression>& root, std::function<bool (IExpression*)> predicate, std::function<IExpression* (IExpression*)> transform)
{
    if(!root)
        return;
    
    QList<IExpression*> expsToTransform;
    root->Tree([&expsToTransform, predicate](IExpression* node){
        if(predicate(node))
            expsToTransform.push_back(node);
    });
    for(auto exp : expsToTransform)
    {
        auto newExp = transform(exp);
        
        if(exp->Parent())
            exp->Parent()->SwapChild(exp, newExp);
        else
        {
            root = QSharedPointer<IExpression>(newExp);
            break;
        }
        delete exp;
    }
}
