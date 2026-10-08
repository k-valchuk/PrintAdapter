#include "Request.h"
#include <QJsonDocument>
#include <QJsonObject>

rest::Query& rest::Request::query()
{
    return m_impl->m_query;
}

rest::Response rest::Request::response() const
{
    std::unique_lock lock{m_impl->m_lock};
    return m_impl->m_response;
}

bool rest::Request::exec(std::function<void(CancellationToken)> onCtCreate, bool cancellableByCancelAll)
{
    auto executor = Executor();
    
    std::unique_lock lock{m_impl->m_lock};
    CancellationToken ct;
    m_impl->m_cancellationTokens.append(ct);
    auto query = m_impl->m_query;
    if(onCtCreate)
        onCtCreate(ct);
    lock.unlock();
    
    auto response = executor->Execute(query, ct, cancellableByCancelAll);
    
    lock.lock();
    m_impl->m_response = response;
    return m_impl->m_response.isSuccess();
}

bool rest::Request::exec(bool cancellableByCancelAll)
{
    return exec(nullptr, cancellableByCancelAll);
}

void rest::Request::Cancel()
{
    QList<CancellationToken> ctsCopy;
    
    {std::unique_lock lock{m_impl->m_lock};
        ctsCopy = m_impl->m_cancellationTokens;
        m_impl->m_cancellationTokens.clear();
    }
    
    for(auto& ct : ctsCopy)
        ct.Cancel();
    m_impl->m_cancellationTokens.clear();
}

void rest::Request::CancelAndWait()
{
    QList<CancellationToken> ctsCopy;
    
    {std::unique_lock lock{m_impl->m_lock};
        ctsCopy = m_impl->m_cancellationTokens;
        m_impl->m_cancellationTokens.clear();
    }
    
    for(auto& ct : ctsCopy)
        ct.CancelAndWait();
    m_impl->m_cancellationTokens.clear();
}

void rest::Request::Wait()
{
    QList<CancellationToken> ctsCopy;
    
    {std::unique_lock lock{m_impl->m_lock};
        ctsCopy = m_impl->m_cancellationTokens;
        m_impl->m_cancellationTokens.clear();
    }
    
    for(auto& ct : ctsCopy)
        ct.Wait();
}

bool rest::Request::isSuccess()
{
    std::unique_lock lock{m_impl->m_lock};
    return m_impl->m_response.isSuccess();
}

rest::StatusCode rest::Request::getStatusCode()
{
    std::unique_lock lock{m_impl->m_lock};
    return m_impl->m_response.code();
}

QString rest::Request::getErrorText() const
{
    std::unique_lock lock{m_impl->m_lock};
    if(!m_impl->m_response.isSuccess()){
        auto js = QJsonDocument::fromJson(m_impl->m_response.answerString().toUtf8()).object();
        if(js.contains("error"))
            return js.value("error").toString();
        else
            return QString();
    } else
        return QString();
}

rest::Problem rest::Request::getProblem() const
{
    std::unique_lock lock{m_impl->m_lock};
    rest::Problem problem{};
    if(!m_impl->m_response.isSuccess()){
        auto js = QJsonDocument::fromJson(m_impl->m_response.answerString().toUtf8()).object();
        if(js.contains("problem")){
            problem.parse(js.value("problem").toObject());
        }
    }
    return problem;
}

void rest::whenAllRequestsAsync(QList<QSharedPointer<Request>> requests, QObject* context,
                                std::function<void (QList<QSharedPointer<Request>>)> onFinish)
{
    auto counter = QSharedPointer<std::atomic_int>::create(0);
    (*counter) = requests.size();
    
    for(auto req : requests)
    {
        req->execAsync<Request>(context, [counter, requests, onFinish](Request){
            if(counter->fetch_sub(1) == 1)
                onFinish(requests);
        });
    }
}

void rest::whenAllRequestsAsync(QList<QSharedPointer<Request> > requests, std::function<void (QList<QSharedPointer<Request> >)> onFinish)
{
    auto counter = QSharedPointer<std::atomic_int>::create(0);
    (*counter) = requests.size();
    
    for(auto req : requests)
    {
        req->execAsync<Request>([counter, requests, onFinish](Request){
            if(counter->fetch_sub(1) == 1)
                onFinish(requests);
        });
    }
}
