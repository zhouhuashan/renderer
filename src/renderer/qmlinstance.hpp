#pragma once

#include <QtQml>

template< typename Type >
QObject * instance(QQmlEngine * engine, QJSEngine * scriptEngine)
{
    Q_UNUSED(scriptEngine);
    const auto o = new (std::nothrow) Type{engine};
    Q_CHECK_PTR(o);
    o->setObjectName(QString::fromLatin1(Type::staticMetaObject.className()));
    return o;
}
