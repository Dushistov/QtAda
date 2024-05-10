#include "CommonFilters.hpp"

namespace QtAda::core::filters {
static QString clickType(const QEvent *event, bool isReleaseInside) noexcept
{
    return isReleaseInside
               ? (event->type() == QEvent::MouseButtonDblClick ? QStringLiteral("DblClick")
                                                               : QStringLiteral("Click"))
               : QStringLiteral("Press");
}

QString buttonEventCommand(const QString &path, const QEvent *event, bool isReleaseInside,
                           const QString &buttonText) noexcept
{
    return QStringLiteral("%1button%2('%3');%4")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(clickType(event, isReleaseInside))
        .arg(path)
        .arg(buttonText.isEmpty()
                 ? ""
                 : QStringLiteral(" // Button text: '%1'").arg(buttonText.simplified()));
}

QString mouseAreaEventCommand(const QString &path, const QEvent *event,
                              bool isReleaseInside) noexcept
{
    return QStringLiteral("%1mouseArea%2('%3');")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(clickType(event, isReleaseInside))
        .arg(path);
}

QString checkButtonCommand(const QString &path, bool isChecked, bool isDoubleCheck,
                           const QString &buttonText) noexcept
{
    auto generate = [&path, &buttonText](bool isChecked) {
        return QStringLiteral("%1checkButton('%2', %3);%4")
            .arg(SCRIPT_COMMAND_PREFIX)
            .arg(path)
            .arg(isChecked ? "true" : "false")
            .arg(buttonText.isEmpty()
                     ? ""
                     : QStringLiteral(" // Button text: %1").arg(buttonText.simplified()));
    };

    if (isDoubleCheck) {
        return QStringLiteral("%1\n%2").arg(generate(!isChecked)).arg(generate(isChecked));
    }
    else {
        return generate(isChecked);
    }
}

QString selectItemCommand(const QString &path, const QString &statement) noexcept
{
    return QStringLiteral("%1selectItem('%2', %3);")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(path)
        .arg(statement);
}

QString setDelayProgressCommand(const QString &path, double progress) noexcept
{
    return QStringLiteral("%1setDelayProgress('%2', %3);")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(path)
        .arg(progress);
}

QString selectTabCommand(const QString &path, const QString &statement) noexcept
{
    return QStringLiteral("%1selectTabItem('%2', %3);")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(path)
        .arg(statement);
}

QString treeViewCommand(const QString &path, bool isExpand, const QString &indexPath,
                        const QString &delegateText) noexcept
{
    return QStringLiteral("%1%2Delegate('%3', %4);%5")
        .arg(SCRIPT_COMMAND_PREFIX)
        .arg(isExpand ? "expand" : "collapse")
        .arg(path)
        .arg(indexPath)
        .arg(delegateText.isEmpty()
                 ? ""
                 : QStringLiteral(" // Delegate text: '%1'").arg(delegateText.simplified()));
}
} // namespace QtAda::core::filters
