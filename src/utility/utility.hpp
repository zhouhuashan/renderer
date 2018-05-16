#pragma once

#include <QtCore>

template< typename Type >
QString toString(const Type & value)
{
    QString string;
    QDebug{&string}.noquote().nospace() << value;
    return string;
}
