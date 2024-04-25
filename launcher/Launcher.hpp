#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "LaunchOptions.hpp"

QT_BEGIN_NAMESPACE
class QLocalServer;
class QLocalSocket;
QT_END_NAMESPACE

namespace QtAda::launcher::injector {
class AbstractInjector;
}

namespace QtAda::inprocess {
class InprocessDialog;
}

namespace QtAda::launcher {
class Launcher final : public QObject {
    Q_OBJECT
public:
    explicit Launcher(const UserLaunchOptions &options, bool fromGui = false,
                      QObject *parent = nullptr) noexcept;
    ~Launcher() noexcept override;

    bool launch() noexcept;
    int exitCode() const noexcept
    {
        return options_.exitCode;
    }

signals:
    void launcherFinished();
    void stdOutMessage(const QString &msg);
    void stdErrMessage(const QString &msg);
    void launcherOutMessage(const QString &msg);
    void launcherErrMessage(const QString &msg);

private slots:
    void restartTimer() noexcept;
    void timeout() noexcept;
    void injectorFinished() noexcept;
    void handleNewConnection() noexcept;

private:
    LaunchOptions options_;
    std::unique_ptr<injector::AbstractInjector> injector_;

    inprocess::InprocessDialog *inprocessDialog_ = nullptr;

    // Используются для "лишнего" подтверждения успешного запуска и инъекции кода,
    // а также для получения предупреждений и ошибок при запуске тестового скрипта.
    QLocalServer *server_ = nullptr;
    QLocalSocket *probeSocket_ = nullptr;

    QTimer waitingTimer_;

    void checkIfLauncherIsFinished() noexcept;
    void handleLauncherFailure(int exitCode, const QString &errorMessage) noexcept;
    void destroyInprocessDialog() noexcept;
};
} // namespace QtAda::launcher
