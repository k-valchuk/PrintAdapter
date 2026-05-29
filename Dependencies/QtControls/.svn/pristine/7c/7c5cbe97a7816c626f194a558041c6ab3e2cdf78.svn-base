#ifndef PROPERTY_CLASS_H
#define PROPERTY_CLASS_H

#include <QMap>
#include <QList>
#include <QString>
#include <QMetaType>
#include <QVariant>
#include <optional>
#include <type_traits>

template <class Map>
void qMapsUnite(Map& dest, const Map& itemsToAdd)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 2)
    dest.unite(itemsToAdd);
#else
    dest.insert(itemsToAdd);
#endif
}

#define PROP(Name, Type, DefaultValue) \
std::optional<Type> Name; \
Type Get##Name() const {return Name.value_or(DefaultValue);} \
static QString Get##Name##Key() {return #Name;} \
void Set##Name(const Type& val) {Name = val;} private:\
static bool _initF##Name() {qRegisterMetaType<Type>();\
m_properties[typeid(CurrentClass).hash_code()].append(\
PropertyClass::PropertyInfo{QString(#Name), true, QString(QMetaType::typeName(qMetaTypeId<Type>())), false});\
m_valCheckers[typeid(CurrentClass).hash_code()][#Name] = [](const PropertyClass* obj){\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return curObj->Name.has_value(); else {Q_ASSERT(false); return false;}};\
m_setters[typeid(CurrentClass).hash_code()][#Name] \
= [](PropertyClass* obj, const QVariant& val, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<CurrentClass*>(obj)) curObj->Set##Name(val.value<Type>());\
else Q_ASSERT(false);};\
m_getters[typeid(CurrentClass).hash_code()][#Name] \
= [](const PropertyClass* obj, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name());\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
inline static bool _init##Name = _initF##Name(); public:

#define VALPROP(Name, Type) \
Type Name; \
    Type Get##Name() const {return Name;} \
    static QString Get##Name##Key() {return #Name;} \
    void Set##Name(const Type& val) {Name = val;} private:\
    static bool _initF##Name() {qRegisterMetaType<Type>();\
        m_properties[typeid(CurrentClass).hash_code()].append(\
                PropertyClass::PropertyInfo{QString(#Name), true, QString(QMetaType::typeName(qMetaTypeId<Type>())), false});\
        m_valCheckers[typeid(CurrentClass).hash_code()][#Name] = [](const PropertyClass* obj){\
        if(std::is_convertible<Type, bool>::value){\
            if(const auto curObj = dynamic_cast<const CurrentClass*>(obj)) return (bool)curObj->Name; else {Q_ASSERT(false); return false;}}\
        else return true;};\
        m_setters[typeid(CurrentClass).hash_code()][#Name] \
        = [](PropertyClass* obj, const QVariant& val, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<CurrentClass*>(obj)) curObj->Set##Name(val.value<Type>());\
else Q_ASSERT(false);};\
        m_getters[typeid(CurrentClass).hash_code()][#Name] \
        = [](const PropertyClass* obj, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name());\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
    inline static bool _init##Name = _initF##Name(); public:

#define CSPROP(Name, Type, GetExpr, SetExpr) \
Type Get##Name() const {return GetExpr;}\
void Set##Name(const Type& value) {SetExpr;}\
static QString Get##Name##Key() {return #Name;} private:\
static bool _initF##Name() {qRegisterMetaType<Type>();\
m_properties[typeid(CurrentClass).hash_code()].append(\
PropertyClass::PropertyInfo{QString(#Name), true, QString(QMetaType::typeName(qMetaTypeId<Type>())), true});\
m_setters[typeid(CurrentClass).hash_code()][#Name] \
    = [](PropertyClass* obj, const QVariant& val, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<CurrentClass*>(obj)) curObj->Set##Name(val.value<Type>());\
else Q_ASSERT(false);};\
m_getters[typeid(CurrentClass).hash_code()][#Name] \
    = [](const PropertyClass* obj, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name());\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
inline static bool _init##Name = _initF##Name(); public:

#define CPROP(Name, Type, GetExpr) \
Type Get##Name() const {return GetExpr;}\
static QString Get##Name##Key() {return #Name;}\
static bool _initF##Name() {qRegisterMetaType<Type>();\
m_properties[typeid(CurrentClass).hash_code()].append(\
     PropertyClass::PropertyInfo{QString(#Name), false, QString(QMetaType::typeName(qMetaTypeId<Type>())), true});\
m_getters[typeid(CurrentClass).hash_code()][#Name] \
    = [](const PropertyClass* obj, const QList<QVariant>& args){\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name());\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
inline static bool _init##Name = _initF##Name(); public:


#define CSPROP_ARG(Name, Type, GetExpr, SetExpr, ArgType, ArgName) \
Type Get##Name(const ArgType& ArgName) const {return GetExpr;}\
void Set##Name(const Type& value, const ArgType& ArgName) {SetExpr;}\
static QString Get##Name##Key() {return #Name;} private:\
static bool _initF##Name() {qRegisterMetaType<Type>(); qRegisterMetaType<ArgType>();\
m_properties[typeid(CurrentClass).hash_code()].append(\
PropertyClass::PropertyInfo{QString(#Name), true, QString(QMetaType::typeName(qMetaTypeId<Type>())), true});\
m_setters[typeid(CurrentClass).hash_code()][#Name] \
= [](PropertyClass* obj, const QVariant& val, const QList<QVariant>& args){\
if(args.size() < 1) {Q_ASSERT(false); return;}\
if(auto curObj = dynamic_cast<CurrentClass*>(obj)) curObj->Set##Name(val.value<Type>(), args.first().value<ArgType>());\
else Q_ASSERT(false);};\
m_getters[typeid(CurrentClass).hash_code()][#Name] \
= [](const PropertyClass* obj, const QList<QVariant>& args){\
if(args.size() < 1) {Q_ASSERT(false); return QVariant();}\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name(args.first().value<ArgType>()));\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
inline static bool _init##Name = _initF##Name(); public:


#define CPROP_ARG(Name, Type, GetExpr, SetExpr, ArgType, ArgName) \
Type Get##Name(const ArgType& ArgName) const {return GetExpr;}\
static QString Get##Name##Key() {return #Name;} private:\
static bool _initF##Name() {qRegisterMetaType<Type>(); qRegisterMetaType<ArgType>();\
m_properties[typeid(CurrentClass).hash_code()].append(\
PropertyClass::PropertyInfo{QString(#Name), false, QString(QMetaType::typeName(qMetaTypeId<Type>())), true});\
m_getters[typeid(CurrentClass).hash_code()][#Name] \
= [](const PropertyClass* obj, const QList<QVariant>& args){\
if(args.size() < 1) {Q_ASSERT(false); return QVariant();}\
if(auto curObj = dynamic_cast<const CurrentClass*>(obj)) return QVariant::fromValue(curObj->Get##Name(args.first().value<ArgType>()));\
else {Q_ASSERT(false); return QVariant();}}; return true;} \
inline static bool _init##Name = _initF##Name(); public:


#define INHERITS(Derieved, Base) \
static bool _initF##Derieved##Base() {\
m_properties[typeid(Derieved).hash_code()].append(m_properties[typeid(Base).hash_code()]);\
qMapsUnite(m_setters[typeid(Derieved).hash_code()], m_setters[typeid(Base).hash_code()]);\
qMapsUnite(m_getters[typeid(Derieved).hash_code()], m_getters[typeid(Base).hash_code()]);\
qMapsUnite(m_valCheckers[typeid(Derieved).hash_code()], m_valCheckers[typeid(Base).hash_code()]);\
return true;\
} inline static bool _init##Derieved##Base = _initF##Derieved##Base(); public:

class PropertyClass
{
public:
    struct PropertyInfo{
        QString Key;
        bool Settable;
        QString Type;
        bool IsProxy; //true если вычисляемая и не имеет за собой настоящих переменных
    };
protected:
    inline static QHash<std::size_t, QList<PropertyInfo>> m_properties;
    inline static QHash<std::size_t, QHash<QString, std::function<void(PropertyClass*, const QVariant&, const QList<QVariant>&)>>> m_setters;
    inline static QHash<std::size_t, QHash<QString, std::function<QVariant(const PropertyClass*, const QList<QVariant>&)>>> m_getters;
    inline static QHash<std::size_t, QHash<QString, std::function<bool(const PropertyClass*)>>> m_valCheckers;
    
public:
    template<typename T>
    bool SetValueByKey(const QString& key, const T& val)
    {
        auto type = typeid(*this).hash_code();
        if(!m_setters.contains(type))
            return false;
        if(!m_setters[type].contains(key))
            return false;
        
        auto setter = m_setters[type][key];
        QVariant variantVal = QVariant::fromValue(val);
        std::invoke(setter, this, std::forward<const QVariant&>(variantVal), QList<QVariant>{});
        
        return true;
    }
    bool SetValueByKey(const QString& key, const QVariant& val, const QList<QVariant>& args = {})
    {
        auto type = typeid(*this).hash_code();
        if(!m_setters.contains(type))
            return false;
        if(!m_setters[type].contains(key))
            return false;
        
        auto setter = m_setters[type][key];
        std::invoke(setter, this, std::forward<const QVariant&>(val), std::forward<const QList<QVariant>&>(args));
        
        return true;
    }
    
    QVariant GetValueByKey(const QString& key, const QList<QVariant>& args = {}) const
    {
        auto type = typeid(*this).hash_code();
        if(!m_getters.contains(type))
            return QVariant();
        if(!m_getters[type].contains(key))
            return QVariant();
        
        auto getter = m_getters[type][key];
        return std::invoke(getter, this, std::forward<const QList<QVariant>&>(args));
    }
    
    bool HasValue(const QString& propertyKey) const 
    {
        auto type = typeid(*this).hash_code();
        if(!m_valCheckers.contains(type))
            return false;
        if(!m_valCheckers[type].contains(propertyKey))
            return false;
        
        auto getter = m_valCheckers[type][propertyKey];
        return std::invoke(getter, this);
    }
    
    QList<PropertyInfo> GetProperties() const 
    {
        auto type = typeid(*this).hash_code();
        if(!m_properties.contains(type))
            return {};
        return m_properties[type];
    }
    
    virtual ~PropertyClass() = default;
};


#endif //PROPERTY_CLASS_H
