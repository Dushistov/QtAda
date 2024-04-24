#pragma once

#include <QFrame>

namespace QtAda::inprocess::tools {
inline QFrame *generateSeparator(QWidget *parent, bool isHorizontal = false) noexcept
{
    QFrame *separator = new QFrame;
    separator->setFrameShape(isHorizontal ? QFrame::HLine : QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    return separator;
}

std::vector<QString> cutLine(const QString &line) noexcept;
} // namespace QtAda::inprocess::tools
