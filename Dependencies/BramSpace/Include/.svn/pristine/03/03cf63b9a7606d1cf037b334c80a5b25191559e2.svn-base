#ifndef MENUACTION_H
#define MENUACTION_H

#include <QObject>
#include <functional>
#include <QAction>
#include <QMenu>
#include <QSharedPointer>
#include <QEvent>


#define LAMBDA(x) [](){return x;}
#define LAMBDA_THIS(x) [this](){return x;}

template <class... Args>
class MenuAction
{
    struct impl
    {
        class NonDeletableQAction : public QAction
        {
            bool m_deleting = false;
        public:
            
            bool event(QEvent* ev) override
            {
                if(ev->type() == QEvent::Type::DeferredDelete && !m_deleting)
                    return false;
                return QAction::event(ev);
            }
            void Destroy()
            {
                m_deleting = true;
                deleteLater();
            }
        };
        
        NonDeletableQAction* m_action = new NonDeletableQAction();
        std::function<QString()> m_nameGetter = nullptr;
        std::tuple<Args...> m_data;
        
        ~impl()
        {
            m_action->Destroy();
        }
    };
    
    QSharedPointer<impl> m_impl = nullptr;
public:
    MenuAction() {}
    
    MenuAction(std::function<QString()> name, std::function<void(Args...)> action)
    {
        m_impl = QSharedPointer<impl>::create();
        
        m_impl->m_nameGetter = name;
        UpdateName();
        auto ptr =  m_impl.data();
        QObject::connect( m_impl->m_action, &QAction::triggered, m_impl->m_action, [ptr, action](){
            std::apply(action,  ptr->m_data);
        });
    }
    
    void SetData(const std::tuple<Args...>& data)
    {
        m_impl->m_data = data;
    }
    void SetData(Args ...args)
    {
        m_impl->m_data = std::tuple<Args...>(args...);
    }
    
    void UpdateName()
    {
        if(m_impl && m_impl->m_nameGetter)
            m_impl->m_action->setText(m_impl->m_nameGetter());
    }
    
    std::function<QString()> NameGetter()
    {
        return m_impl->m_nameGetter;
    }
    
    QAction* Action()
    {
        return m_impl->m_action;
    }
    
    bool operator==(std::nullptr_t other)
    {
        if(m_impl == nullptr)
            return true;
        
        return false;
    }
};

template <class... Args>
class IMenuManager
{
public:
    virtual void AddAction(MenuAction<Args...>& action, QMenu* parentSubmenu = nullptr) = 0;
    virtual MenuAction<Args...> AddAction(std::function<QString()> name, std::function<void(Args...)> action, QMenu* parentSubmenu = nullptr) = 0;
    virtual QMenu* GetOrCreateSubmenu(QString key, std::function<QString()> name, QMenu* parentSubmenu = nullptr) = 0;
};

template <class... Args>
class IOrderedMenuManager : public IMenuManager<Args...>
{
public:
    virtual void AddSeparator(QMenu* parentSubmenu = nullptr) = 0;
};

#endif // MENUACTION_H
