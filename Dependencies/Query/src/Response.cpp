
#include "Response.h"

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

