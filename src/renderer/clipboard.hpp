#pragma once

#include <QtGui>

Q_DECLARE_LOGGING_CATEGORY(clipboardCategory)

class Clipboard
        : public QObject
{

    Q_OBJECT

public :

    explicit Clipboard(QObject * const parent = Q_NULLPTR);

public Q_SLOTS :

    void setImage(QImage image);

private :

    QClipboard * const clipboard = QGuiApplication::clipboard();

};
