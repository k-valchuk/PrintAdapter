#include "Response.h"
#include <QJsonArray>

void rest::ProblemField::parse(const QJsonObject& o)
{
    if(o.contains("code"))
        code = o.value("code").toString();
    if(o.contains("translationKey"))
        trKey = o.value("translationKey").toString();
    if(o.contains("message"))
        message = o.value("message").toString();
    const auto vParams = o.value("params");
    if (vParams.isObject())
        params = vParams.toObject().toVariantMap();
}

void rest::Problem::parse(const QJsonObject& o)
{
    if(o.contains("code"))
        code = o.value("code").toString();
    if(o.contains("translationKey"))
        trKey = o.value("translationKey").toString();
    if(o.contains("message"))
        message = o.value("message").toString();
    const auto vParams = o.value("params");
    if (vParams.isObject())
        params = vParams.toObject().toVariantMap();
    const auto vFields = o.value("fields");
    if (vFields.isObject()) {
        const QJsonObject fieldsObj = vFields.toObject();
        for (auto it = fieldsObj.constBegin(); it != fieldsObj.constEnd(); ++it) {
            const auto v = it.value();
            if (!v.isObject()) continue;

            rest::ProblemField f;
            f.parse(v.toObject());
            fields.insert(it.key(), f);
        }
    }
}

rest::Response::Response()
{
    m_code = (StatusCode)0;
}

QString rest::Response::answerString() const
{
    return m_answerString;
}

rest::StatusCode rest::Response::code() const
{
    return m_code;
}

bool rest::Response::isSuccess() const
{
    if(m_code == StatusCode::OK)
        return true;
    return false;
}

rest::Response& rest::Response::setAnswerString(QString str)
{
    m_answerString = str;
    return *this;
}

rest::Response& rest::Response::setCode(StatusCode code)
{
    m_code = code;
    return *this;
}

