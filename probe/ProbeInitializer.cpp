#include "ProbeInitializer.hpp"

#include "Probe.hpp"

#include <QCoreApplication>
#include <QThread>
#include <QDebug>

namespace QtAda::probe {
ProbeInitializer::ProbeInitializer() noexcept
{
    moveToThread(QCoreApplication::instance()->thread());
    QMetaObject::invokeMethod(this, "initProbe", Qt::QueuedConnection);

    if (qgetenv("QTADA_NEED_TO_UNSET_PRELOAD") == "1")
        qputenv("LD_PRELOAD", "");
}

void ProbeInitializer::initProbe() noexcept
{
    if (!qApp) {
        deleteLater();
        return;
    }

    assert(QThread::currentThread() == qApp->thread());

    core::Probe::initProbe();
    assert(core::Probe::initialized());

    deleteLater();
}
} // namespace QtAda::probe
