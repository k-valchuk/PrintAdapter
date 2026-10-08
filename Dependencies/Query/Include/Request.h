#ifndef REQUEST_H
#define REQUEST_H

#include "QueryExecutor.h"
#include <atomic>
#include <QSharedPointer>

#include "IAsyncTask.h"

namespace rest {

// Запрос
class Request
{
    struct Impl
    {
        Query m_query;
        Response m_response;
        
        QList<CancellationToken> m_cancellationTokens;
        std::recursive_mutex m_lock;
    };
    QSharedPointer<Impl> m_impl;
    
public:
    Request() { m_impl = QSharedPointer<Impl>::create(); }
    
    Query& query();
    Response response() const;
    
    bool exec(std::function<void(CancellationToken)> onCtCreate, bool cancellableByCancelAll = true);
    
    bool exec(bool cancellableByCancelAll = true);
    
    template <class TRequest>
    void execAsync(std::function<void(TRequest)> onFinish, std::function<void()> onCancel)
    {
        auto executor = Executor();
        
        std::unique_lock lock{m_impl->m_lock};
        auto impl = m_impl;
        CancellationToken ct;
        m_impl->m_cancellationTokens.append(ct);
        TRequest req = *(static_cast<TRequest*>(this));
        executor->ExecuteAsync(m_impl->m_query, [req, impl, onFinish](Response answ)
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response = answ;
                if(onFinish)
                    onFinish(req);
            }, ct, onCancel);
    }
    
    void execAsync() {
        execAsync<Request>((std::function<void(Request)>)nullptr);
    }
    
    template <class TRequest>
    void execAsync(std::function<void(TRequest)> onFinish, std::function<void(TRequest)> onCancel = nullptr)
    {
        auto executor = Executor();
        
        std::unique_lock lock{m_impl->m_lock};
        auto impl = m_impl;
        CancellationToken ct;
        m_impl->m_cancellationTokens.append(ct);
        TRequest req = *(static_cast<TRequest*>(this));
        if(onCancel == nullptr)
            onCancel = onFinish;
        executor->ExecuteAsync(m_impl->m_query, [req, impl, onFinish](Response answ)
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response = answ;
                if(onFinish)
                    onFinish(req);
            }, ct,
            [req, impl, onCancel]()
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response.setAnswerString("");
                impl->m_response.setCode(StatusCode::RequestTimeout);
                if(onCancel)
                    onCancel(req);
            });
    }
    
    //safely calls or not calls callback slots if QObject* context was deleted
    //so you can safely capture context in lambda for example
    template <class TRequest>
    void execAsync(QObject* context, std::function<void(TRequest)> onFinishSlot, std::function<void()> onCancelSlot)
    {
        auto executor = Executor();
        
        std::unique_lock lock{m_impl->m_lock};
        auto impl = m_impl;
        CancellationToken ct;
        m_impl->m_cancellationTokens.append(ct);
        TRequest req = *(static_cast<TRequest*>(this));
        executor->ExecuteAsync(m_impl->m_query, context, [req, impl, onFinishSlot](Response answ)
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response = answ;
                if(onFinishSlot)
                    onFinishSlot(req);
            }, ct, onCancelSlot);
    }
    
    //safely calls or not calls callback slots if QObject* context was deleted
    //so you can safely capture context in lambda for example
    template <class TRequest>
    void execAsync(QObject* context, std::function<void(TRequest)> onFinishSlot, std::function<void(TRequest)> onCancelSlot = nullptr)
    {
        auto executor = Executor();
        
        std::unique_lock lock{m_impl->m_lock};
        auto impl = m_impl;
        CancellationToken ct;
        m_impl->m_cancellationTokens.append(ct);
        TRequest req = *(static_cast<TRequest*>(this));
        if(onCancelSlot == nullptr)
            onCancelSlot = onFinishSlot;
        executor->ExecuteAsync(m_impl->m_query, context, [req, impl, onFinishSlot](Response answ)
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response = answ;
                if(onFinishSlot)
                    onFinishSlot(req);
            }, ct,
            [req, impl, onCancelSlot]()
            {
                std::unique_lock lock{impl->m_lock};
                impl->m_response.setAnswerString("");
                impl->m_response.setCode(StatusCode::RequestTimeout);
                if(onCancelSlot)
                    onCancelSlot(req);
            });
    }
    
    void Cancel();
    void CancelAndWait();
    void Wait();
    
    bool isSuccess();
    rest::StatusCode getStatusCode();
    QString getErrorText() const;
    Problem getProblem() const;
    
    template <class TRequest, class TResult>
    QSharedPointer<IAsyncTask<TResult>> GetAsyncTask(std::function<TResult(TRequest)> resultConverter);
};

template <class TRequest, class TResult>
class AsyncRequestResult : public IAsyncTask<TResult>
{
    friend class Request;
    friend class QSharedPointer<AsyncRequestResult<TRequest, TResult>>;
    friend class std::shared_ptr<AsyncRequestResult<TRequest, TResult>>;
    
    struct Impl
    {
        bool m_set = false;
        TRequest m_request;
        std::function<TResult(TRequest)> m_resultConverter;
        std::recursive_mutex m_lockObj;
        TResult m_result;
    };
    
    QSharedPointer<Impl> m_impl = QSharedPointer<Impl>::create();
    
    AsyncRequestResult(std::function<TResult(TRequest)> resultConverter, TRequest req)
    {
        m_impl->m_request = req;
        m_impl->m_resultConverter = resultConverter;
    }
public:
    
    void OnFinished(std::function<void(TResult)> callback, std::function<void()> failCallback) override
    {
        std::unique_lock lock{m_impl->m_lockObj};
        m_impl->m_set = true;
        auto impl = m_impl;
        m_impl->m_request.template execAsync<TRequest>([impl, callback, failCallback](TRequest req){
            impl->m_result = impl->m_resultConverter(req);
            if(req.isSuccess()){
                if(callback) callback(impl->m_result);
            }
            else {
                if(failCallback) failCallback();
            }
        });
    }
    void OnFinished(QObject* context, std::function<void(TResult)> slot, std::function<void()> failCallback) override
    {
        std::unique_lock lock{m_impl->m_lockObj};
        m_impl->m_set = true;
        auto impl = m_impl;
        m_impl->m_request.template execAsync<TRequest>(context, [impl, slot, failCallback](TRequest req){
            impl->m_result = impl->m_resultConverter(req);
            if(req.isSuccess()){
                if(slot) slot(impl->m_result);
            }
            else {
                if(failCallback) failCallback();
            }
        });
    }
    void Cancel() override {
        std::unique_lock lock{m_impl->m_lockObj};
        m_impl->m_request.Cancel();
    }
    void CancelAndWait() override {
        std::unique_lock lock{m_impl->m_lockObj};
        m_impl->m_request.CancelAndWait();
    }
    void Wait() override {
        std::unique_lock lock{m_impl->m_lockObj};
        if(m_impl->m_set)
            m_impl->m_request.Wait();
        else
            m_impl->m_request.exec();
        m_impl->m_result = m_impl->m_resultConverter(m_impl->m_request);
    }
    TResult Result() override {
        return m_impl->m_result;
    }
};

template <class TRequest, class TResult>
QSharedPointer<IAsyncTask<TResult>> rest::Request::GetAsyncTask(std::function<TResult(TRequest)> resultConverter)
{
    return QSharedPointer<AsyncRequestResult<TRequest, TResult>>::create(resultConverter, *((TRequest*)this));
}

//Syncronous execution
template <class TRequest, class ...TArgs>
Response makeRequest(TArgs... args)
{
    TRequest req(args...);
    
    req.exec();
    return req.response();
}

template <class TRequest, class ...TArgs>
TRequest makeRequestR(TArgs... args)
{
    TRequest req(args...);
    
    req.exec();
    return req;
}

//Asynchronous execution
//you are responsive for lambda captured objects to be alive at the moment of callbacks calling
template <class TRequest, class ...TArgs>
TRequest makeRequestAsyncC(std::function<void(TRequest)> onFinish, std::function<void()> onCancel, TArgs... args)
{
    TRequest req(args...);
    req.execAsync(onFinish, onCancel);
    return req;
}

template <class TRequest, class ...TArgs>
TRequest makeRequestAsyncC(std::function<void(TRequest)> onFinish, std::function<void(TRequest)> onCancel, TArgs... args)
{
    TRequest req(args...);
    req.execAsync(onFinish, onCancel);
    return req;
}

template <class TRequest, class ...TArgs>
TRequest makeRequestAsync(std::function<void(TRequest)> onFinish, TArgs... args)
{
    return makeRequestAsyncC(onFinish, onFinish, args...);
}


// next functions safely calls or not calls callback slots if QObject* context was deleted
//so you can safely capture context obj in lambda for example

template <class TRequest, class ...TArgs>
TRequest makeRequestAsyncC(QObject* context, std::function<void(TRequest)> onFinish, std::function<void()> onCancel, TArgs... args)
{
    TRequest req(args...);
    req.execAsync(context, onFinish, onCancel);
    return req;
}

template <class TRequest, class ...TArgs>
TRequest makeRequestAsyncC(QObject* context, std::function<void(TRequest)> onFinish, std::function<void(TRequest)> onCancel, TArgs... args)
{
    TRequest req(args...);
    req.execAsync(context, onFinish, onCancel);
    return req;
}

template <class TRequest, class ...TArgs>
TRequest makeRequestAsync(QObject* context, std::function<void(TRequest)> onFinish, TArgs... args)
{
    return makeRequestAsyncC(context, onFinish, onFinish, args...);
}


//next functions work with method pointers (the call onFinish will be done from context object: context->onFinish(req))
//if TCaller : QObject it will work safely for context object deletion before callbacks, because call is made by signal connection
//if TCaller is not derieved from QObject you are responsible for context object to be alive at the moment of callbacks calling

template <class TRequest, class TCaller, class ...TArgs>
TRequest makeRequestAsyncC(TCaller* context, void(TCaller::*onFinish)(TRequest), void(TCaller::*onCancel)(), TArgs... args)
{
    TRequest req(args...);
    if(std::is_base_of<QObject, TCaller>())
        req.execAsync(context,
            onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
            onCancel ? (std::function<void()>)std::bind(onCancel, context) : nullptr);
    else
        req.execAsync(onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel ? (std::function<void()>)std::bind(onCancel, context) : nullptr);
    return req;
}

template <class TRequest, class TCaller, class ...TArgs>
TRequest makeRequestAsyncC(TCaller* context, void(TCaller::*onFinish)(TRequest), void(TCaller::*onCancel)(TRequest), TArgs... args)
{
    TRequest req(args...);
    if(std::is_base_of<QObject, TCaller>())
        req.execAsync(context,
                      onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel ? (std::function<void(TRequest)>)std::bind(onCancel, context, std::placeholders::_1) : nullptr);
    else
        req.execAsync(onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel ? (std::function<void(TRequest)>)std::bind(onCancel, context, std::placeholders::_1) : nullptr);
    return req;
}


//2 overloads with independent of context object onCancel callback
template <class TRequest, class TCaller, class ...TArgs>
TRequest makeRequestAsyncC(TCaller* context, void(TCaller::*onFinish)(TRequest), std::function<void()> onCancel, TArgs... args)
{
    TRequest req(args...);
    if(std::is_base_of<QObject, TCaller>())
        req.execAsync(context,
                      onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel);
    else
        req.execAsync(onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel);
    return req;
}

template <class TRequest, class TCaller, class ...TArgs>
TRequest makeRequestAsyncC(TCaller* context, void(TCaller::*onFinish)(TRequest), std::function<void(TRequest)> onCancel, TArgs... args)
{
    TRequest req(args...);
    if(std::is_base_of<QObject, TCaller>())
        req.execAsync(context,
                      onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel);
    else
        req.execAsync(onFinish ? (std::function<void(TRequest)>)std::bind(onFinish, context, std::placeholders::_1) : nullptr,
                      onCancel);
    return req;
}

template <class TRequest, class TCaller, class ...TArgs>
TRequest makeRequestAsync(TCaller* context, void(TCaller::*onFinish)(TRequest), TArgs... args)
{
    return makeRequestAsyncC(context, onFinish, onFinish/*(void(TCaller::*)())nullptr*/, args...);
}


//вызывает onFinish только когда все запросы завершатся, туда аргументом кидает поданный список реквестов
//можно давать реквесты разных типов (а в колбеке преобразовывать обратно)
//onFinish вызывается безопасно через QueuedConnection от объекта QObject* context
void whenAllRequestsAsync(QList<QSharedPointer<Request>> requests, QObject* context,
                                           std::function<void(QList<QSharedPointer<Request>>)> onFinish);

//перегрузка БЕЗ безопасного вызова через QObject
void whenAllRequestsAsync(QList<QSharedPointer<Request>> requests,
                                           std::function<void(QList<QSharedPointer<Request>>)> onFinish);

//шаблонные варианты позволяющие работать без QSharedPointer и списка, а передавать разные Request'ы аргументами
//осторожно, при вызове надо явно преобразовать первый аргумент к QObject*, иначе не дедукнется какую вызывать - эту или следующую
template<class ...TRequests, class TCallback>
void whenAllRequestsAsync(QObject* context, TCallback onFinish, TRequests ...args){
    auto counter = QSharedPointer<std::atomic_int>::create(0);
    const auto count = sizeof...(TRequests);
    (*counter) = count;
    
    std::tuple<TRequests...> requests = std::forward_as_tuple(args...);
    (args.template execAsync<Request>(context, [counter, requests, onFinish](Request){
        if(counter->fetch_sub(1) == 1)
            std::apply(onFinish, requests);
    }), ...);
}

template<class ...TRequests, class TCallback>
void whenAllRequestsAsync(TCallback onFinish, TRequests ...args){
    auto counter = QSharedPointer<std::atomic_int>::create(0);
    const auto count = sizeof...(TRequests);
    (*counter) = count;
    
    std::tuple<TRequests...> requests = std::forward_as_tuple(args...);
    (args.template execAsync<Request>([counter, requests, onFinish](Request){
        if(counter->fetch_sub(1) == 1)
            std::apply(onFinish, requests);
    }), ...);
}

}

#endif // REQUEST_H
