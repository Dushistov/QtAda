#pragma once

#include <QObject>
#include <vector>
#include <set>

namespace QtAda::core {
class Probe : public QObject {
    Q_OBJECT

public:
    explicit Probe(QObject *parent = nullptr) noexcept;
    ~Probe() noexcept;

    static bool initialized() noexcept;
    static void initProbe() noexcept;
    static void addObject(QObject *obj) noexcept;
    static void removeObject(QObject *obj) noexcept;

    //    bool eventFilter(QObject *watched, QEvent *event) override ;

private slots:
    void kill() noexcept;

private:
    // Очень важно, что построение дерева объектов должно происходить
    // в одном потоке из экземпляров, которые мы сохраняем в knownObjects_
    struct QueuedObject {
        QObject *obj;
        enum Type { Create, Destroy } type;

        QueuedObject(QObject *o, Type t)
            : obj(o)
            , type(t)
        {
        }
    };
    std::vector<QueuedObject> queuedObjects_;

    static QAtomicPointer<Probe> s_probeInstance;
    std::set<const QObject *> knownObjects_;

    static Probe *probeInstance() noexcept;

    void discoverObject(QObject *obj) noexcept;
    void findObjectsFromCoreApp() noexcept;

    void addObjectCreationToQueue(QObject *obj) noexcept;
    void addObjectDestroyToQueue(QObject *obj) noexcept;

    void installEventFilter() noexcept;
    bool isIternalObjectCreated(QObject *obj) const noexcept;
};
} // namespace QtAda::core
