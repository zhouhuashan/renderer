#pragma once

#include <QtGui>

Q_DECLARE_LOGGING_CATEGORY(rendererInterfaceCategory)

class RendererInterface
        : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool autoRefresh MEMBER autoRefresh NOTIFY autoRefreshChanged)
    Q_PROPERTY(float dt MEMBER dt NOTIFY dtChanged)

public :

    using QObject::QObject;

    Q_INVOKABLE
    void updateProperty(QString name, QVariant value)
    {
        if (!setProperty(qPrintable(name), value)) {
            qCCritical(rendererInterfaceCategory);
        }
    }

    Q_INVOKABLE
    QVariant readProperty(QString name) const
    {
        return property(qPrintable(name));
    }

Q_SIGNALS :

    void autoRefreshChanged(bool autoRefresh);
    void dtChanged(float dt);

private :

    bool autoRefresh = false;
    float dt = 0.0f;

};
