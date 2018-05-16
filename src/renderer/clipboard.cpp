#include "clipboard.hpp"

Q_LOGGING_CATEGORY(clipboardCategory, "clipboard")

Clipboard::Clipboard(QObject * const parent)
    : QObject{parent}
{
    Q_CHECK_PTR(clipboard);
}

void Clipboard::setImage(QImage image)
{
    clipboard->setImage(image, QClipboard::Clipboard);
}
