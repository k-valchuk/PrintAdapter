#ifndef IASYNCTASK_H
#define IASYNCTASK_H

#include <QObject>
#include <functional>

template <class TResult>
class IAsyncTask
{
public:
    virtual void OnFinished(std::function<void(TResult)> callback, std::function<void()> failCallback = nullptr) = 0;
    virtual void OnFinished(QObject* context, std::function<void(TResult)> slot, std::function<void()> failCallback = nullptr) = 0;
    virtual void Cancel() = 0;
    virtual void CancelAndWait() = 0;
    virtual void Wait() = 0;
    virtual TResult Result() = 0;
};


#endif // IASYNCTASK_H
