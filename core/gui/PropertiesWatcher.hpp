#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
class QTreeView;
class QStandardItemModel;
class QStandardItem;
QT_END_NAMESPACE

namespace QtAda::core::gui {
class PropertiesWatcher : public QWidget {
    Q_OBJECT
public:
    PropertiesWatcher(QWidget *parent) noexcept;

public slots:
    void setSelectedObject(const QObject *object) noexcept;
    void setFrame(const QObject *frame) noexcept
    {
        frame_ = frame;
    }
    void removeFrame() noexcept
    {
        frame_ = nullptr;
    }

private:
    QPushButton *selectAll = nullptr;
    QPushButton *clearSelection = nullptr;
    QPushButton *acceptSelection = nullptr;

    QTreeView *treeView_ = nullptr;
    QStandardItemModel *selectedObjectModel_ = nullptr;
    const QObject *frame_ = nullptr;

    void addObjectToModel(const QObject *object, QStandardItem *parentViewItem) noexcept;

    void initButton(QPushButton *button, const QString &text) noexcept;
    void initFirstViewItem(QStandardItem *viewItem) noexcept;
};
} // namespace QtAda::core::gui
