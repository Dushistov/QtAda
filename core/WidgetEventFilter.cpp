#include "WidgetEventFilter.hpp"

#include <QString>
#include <QObject>
#include <QMouseEvent>
#include <map>

#include <QAbstractButton>
#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QDial>
#include <QListView>
#include <QTableView>
#include <QCalendarWidget>
#include <QMenu>
#include <QAction>
#include <QTabBar>
#include <QTreeView>

#include "utils/Common.hpp"
#include "utils/FilterUtils.hpp"

//! TODO: remove
#include <iostream>

namespace QtAda::core::filters {
/*
 * Число - это максимальная степень вложенности нужного класса относительно изначально
 * обрабатываемого. Если обрабатываемый класс, например, - кнопка, то она и вызывает
 * eventFilter, поэтому и при обработке исследуемого указателя должны считать, что он
 * и является кнопкой, следовательно и ставим число 1. А если компонент более сложный,
 * то есть имеет несколько потомков, то eventFilter может вызвать один из его потомков.
 * Следовательно, для его правильной обработки нам нужно до него "добраться" проверив
 * N потомков.
 */
static const std::map<WidgetClass, std::pair<QLatin1String, size_t>> s_widgetMetaMap = {
    { Button, { QLatin1String("QAbstractButton"), 1 } },
    { RadioButton, { QLatin1String("QRadioButton"), 1 } },
    { CheckBox, { QLatin1String("QCheckBox"), 1 } },
    { Slider, { QLatin1String("QAbstractSlider"), 1 } },
    { ComboBox, { QLatin1String("QComboBox"), 4 } },
    { SpinBox, { QLatin1String("QAbstractSpinBox"), 1 } },
    { Menu, { QLatin1String("QMenu"), 1 } },
    { TabBar, { QLatin1String("QTabBar"), 1 } },
    { ItemView, { QLatin1String("QAbstractItemView"), 2 } },
    { TreeView, { QLatin1String("QTreeView"), 2 } },
    { Calendar, { QLatin1String("QCalendarView"), 2 } },
};

QString qMouseEventFilter(const QString &path, const QWidget *widget,
                          const QMouseEvent *event) noexcept
{
    if (path.isEmpty() || widget == nullptr || event == nullptr) {
        return QString();
    }

    const auto clickPosition = widget->mapFromGlobal(event->globalPos());
    return QStringLiteral("%1('%2', '%3', %4, %5)")
        .arg(event->type() == QEvent::MouseButtonDblClick ? "mouseDblClick" : "mouseClick")
        .arg(path)
        .arg(utils::mouseButtonToString(event->button()))
        .arg(clickPosition.x())
        .arg(clickPosition.y());
}

//! TODO: нужна ли обработка зажатия кастомной кнопки?
static QString qButtonFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    //! TODO: скорее всего нужно будет уточнять какие именно классы, а не просто QAbstractButton
    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::Button));
    if (widget == nullptr) {
        return QString();
    }

    auto *button = qobject_cast<const QAbstractButton *>(widget);
    assert(button != nullptr);
    const auto buttonRect = button->rect();
    const auto clickPos = button->mapFromGlobal(event->globalPos());

    return QStringLiteral("%1Button('%2')%3")
        .arg(buttonRect.contains(clickPos) ? "click" : "press")
        .arg(utils::objectPath(widget))
        .arg(button->text().isEmpty()
                 ? ""
                 : QStringLiteral(" // Button text: '%1'").arg(button->text()));
}

static QString qRadioButtonFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::RadioButton));
    if (widget == nullptr) {
        return QString();
    }

    //! TODO: не лучший вариант проверки нажатия, нужно придумать лучше
    const auto fitSize = widget->minimumSizeHint();
    const auto checkBoxSize = widget->size();
    if (fitSize.isValid() && checkBoxSize.isValid()) {
        const auto clickableArea
            = QRect(QPoint(0, 0), QSize(std::min(fitSize.width(), checkBoxSize.width()),
                                        std::min(fitSize.height(), checkBoxSize.height())));
        const auto clickPos = widget->mapFromGlobal(event->globalPos());
        if (clickableArea.contains(clickPos)) {
            auto *radioButton = qobject_cast<const QRadioButton *>(widget);
            assert(radioButton != nullptr);
            return QStringLiteral("clickButton('%1')%2")
                .arg(utils::objectPath(widget))
                .arg(radioButton->text().isEmpty()
                         ? ""
                         : QStringLiteral(" // Button text: '%1'").arg(radioButton->text()));
        }
    }
    return qMouseEventFilter(utils::objectPath(widget), widget, event);
}

static QString qCheckBoxFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::CheckBox));
    if (widget == nullptr) {
        return QString();
    }

    //! TODO: не лучший вариант проверки нажатия, нужно придумать лучше
    const auto fitSize = widget->minimumSizeHint();
    const auto checkBoxSize = widget->size();
    if (fitSize.isValid() && checkBoxSize.isValid()) {
        const auto clickableArea
            = QRect(QPoint(0, 0), QSize(std::min(fitSize.width(), checkBoxSize.width()),
                                        std::min(fitSize.height(), checkBoxSize.height())));
        const auto clickPos = widget->mapFromGlobal(event->globalPos());
        if (clickableArea.contains(clickPos)) {
            auto *checkBox = qobject_cast<const QCheckBox *>(widget);
            assert(checkBox != nullptr);
            //! TODO: разобраться с tristate
            return QStringLiteral("checkButton('%1', %2)%3")
                .arg(utils::objectPath(widget))
                .arg(checkBox->isChecked() ? "false" : "true")
                .arg(checkBox->text().isEmpty()
                         ? ""
                         : QStringLiteral(" // Button text: '%1'").arg(checkBox->text()));
            ;
        }
    }
    return qMouseEventFilter(utils::objectPath(widget), widget, event);
}

static QString qComboBoxFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    size_t iteration;
    std::tie(widget, iteration) = utils::searchSpecificWidgetWithIteration(
        widget, s_widgetMetaMap.at(WidgetClass::ComboBox));
    if (widget == nullptr) {
        return QString();
    }
    if (iteration <= 2) {
        return QStringLiteral("// Looks like QComboBox container clicked\n// %1")
            .arg(qMouseEventFilter(utils::objectPath(widget), widget, event));
    }

    auto *comboBox = qobject_cast<const QComboBox *>(widget);
    assert(comboBox != nullptr);

    //! TODO: нужно проверить, если выполнение кода дошло до этого места, то точно ли
    //! был нажат элемент списка
    auto *comboBoxView = comboBox->view();
    const auto containerRect = comboBoxView->rect();
    const auto clickPos = comboBoxView->mapFromGlobal(event->globalPos());

    if (containerRect.contains(clickPos)) {
        return QStringLiteral("selectItem('%1', '%2')")
            .arg(utils::objectPath(comboBox))
            .arg(utils::widgetIdInView(comboBox, comboBoxView->currentIndex().row(),
                                       WidgetClass::ComboBox));
    }
    /*
     * Отпускание мыши не приведет к закрытию QListView, и если мы зарегестрируем событие
     * обычного "клика", то при воспроизведении это приведет к закрытию QListView. Поэтому
     * возвращаем комментарий, так как в противном случае, если пользователь выберет элемент
     * из списка, то это может приведет к ошибке.
     */
    return QStringLiteral(
               "// 'Release' event is outside of QComboBox, so it is still opened\n// %1")
        .arg(qMouseEventFilter(utils::objectPath(comboBox), comboBox, event));
}

static QString qSliderFilter(const QWidget *widget, const QMouseEvent *event,
                             const ExtraInfoForDelayed &extra)
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::Slider));
    if (widget == nullptr) {
        return QString();
    }

    assert(extra.changeType.has_value());
    if (*extra.changeType == QAbstractSlider::SliderNoAction) {
        return QString();
    }

    auto *slider = qobject_cast<const QAbstractSlider *>(widget);
    assert(slider != nullptr);

    // Рассматриваем отдельно, так как любое зарегестрированное нажатие
    // на QDial приводит к установке значения "под курсором"
    if (qobject_cast<const QDial *>(widget)) {
        return utils::setValueStatement(widget, slider->value());
    }

    //! TODO: надо удостовериться, возможно ли вызвать SliderSingleStepAdd(Sub) нажатием мыши,
    //! если нет - то убрать эти типы из проверки
    switch (*extra.changeType) {
    case QAbstractSlider::SliderSingleStepAdd:
        return utils::changeValueStatement(widget, "SingleStepAdd");
    case QAbstractSlider::SliderSingleStepSub:
        return utils::changeValueStatement(widget, "SingleStepSub");
    case QAbstractSlider::SliderPageStepAdd:
        return utils::changeValueStatement(widget, "PageStepAdd");
    case QAbstractSlider::SliderPageStepSub:
        return utils::changeValueStatement(widget, "PageStepSub");
    case QAbstractSlider::SliderToMinimum:
        return utils::changeValueStatement(widget, "ToMinimum");
    case QAbstractSlider::SliderToMaximum:
        return utils::changeValueStatement(widget, "ToMaximum");
    case QAbstractSlider::SliderMove:
        return utils::setValueStatement(widget, slider->value());
    }
    Q_UNREACHABLE();
}

static QString qSpinBoxFilter(const QWidget *widget, const QMouseEvent *event,
                              const ExtraInfoForDelayed &extra) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::SpinBox));
    if (widget == nullptr) {
        return QString();
    }

    if (auto *dateEdit = qobject_cast<const QDateEdit *>(widget)) {
        return utils::setValueStatement(widget, dateEdit->date().toString(Qt::ISODate));
    }
    else if (auto *timeEdit = qobject_cast<const QTimeEdit *>(widget)) {
        return utils::setValueStatement(widget, timeEdit->time().toString(Qt::ISODate));
    }
    else if (auto *dateTimeEdit = qobject_cast<const QDateTimeEdit *>(widget)) {
        return utils::setValueStatement(widget, dateTimeEdit->dateTime().toString(Qt::ISODate));
    }

    bool isDblClick = event->type() == QEvent::MouseButtonDblClick;
    if (extra.isContinuous || isDblClick) {
        auto handleSpinBox = [&](auto *spinBox) {
            //! TODO: костыль, так как spinBoxWidget->value() при
            //! MouseButtonDblClick почему-то не соответствует действительности, что
            //! странно, так как значение изменяется при событии нажатия, а не
            //! отпускания, а на этапе обработки MouseButtonDblClick значение должно
            //! было измениться два раза
            return utils::setValueStatement(widget, spinBox->value()
                                                        + (isDblClick ? spinBox->singleStep() : 0));
        };
        if (auto *spinBox = qobject_cast<const QSpinBox *>(widget)) {
            return handleSpinBox(spinBox);
        }
        else if (auto *doubleSpinBox = qobject_cast<const QDoubleSpinBox *>(widget)) {
            return handleSpinBox(doubleSpinBox);
        }
        Q_UNREACHABLE();
    }
    else {
        const QRect upButtonRect(0, 0, widget->width(), widget->height() / 2);
        const QRect downButtonRect(0, widget->height() / 2, widget->width(), widget->height() / 2);

        if (upButtonRect.contains(event->pos())) {
            return utils::changeValueStatement(widget, "Up");
        }
        else if (downButtonRect.contains(event->pos())) {
            return utils::changeValueStatement(widget, "Down");
        }
    }

    return QString();
}

static QString qCalendarFilter(const QWidget *widget, const QMouseEvent *event,
                               const ExtraInfoForDelayed &extra) noexcept
{
    Q_UNUSED(extra);
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::Calendar));
    if (widget == nullptr) {
        return QString();
    }

    //! TODO:
    //! 1. QCalendarWidget не предоставляет возможности получить его модель данных,
    //! а она необходима, так как на этапе отпускания мыши значение меняется только в
    //! модели, но не в QCalendarWidget. Поэтому приходится работать с моделью напрямую,
    //! а сам QCalendarWidget получать через родителя модели, что не совсем хорошо...
    //!
    //! 2. Надо ли генерировать строку, если было произведено нажатие на уже выбранную
    //! дату?
    const auto *calendar = qobject_cast<const QCalendarWidget *>(widget->parentWidget());
    assert(calendar != nullptr);
    const auto *calendarView = qobject_cast<const QAbstractItemView *>(widget);
    assert(calendarView != nullptr);

    const auto currentCellIndex = calendarView->currentIndex();

    const auto selectedCellIndexes = calendarView->selectionModel()->selectedIndexes();
    assert(selectedCellIndexes.size() <= 1);
    const auto selectedCellIndex = selectedCellIndexes.first();
    const auto clickPos = calendarView->mapFromGlobal(event->globalPos());
    const auto dateChanged
        = calendarView->rect().contains(clickPos)
          && ((currentCellIndex != selectedCellIndex && event->type() == QEvent::MouseButtonRelease)
              || event->type() == QEvent::MouseButtonDblClick);

    assert(currentCellIndex.isValid());
    assert(currentCellIndex.data().canConvert<int>());

    const int day = currentCellIndex.data().toInt();
    int month = calendar->monthShown();
    int year = calendar->yearShown();

    QModelIndex repeatingDayIndex;
    auto *calendarModel = calendarView->model();
    for (int row = 0; row < calendarModel->rowCount(); ++row) {
        for (int column = 0; column < calendarModel->columnCount(); ++column) {
            QModelIndex index = calendarModel->index(row, column);
            assert(index.isValid());
            assert(index.data().canConvert<int>());
            if (index.data().toInt() == day && index != currentCellIndex) {
                repeatingDayIndex = index;
                break;
            }
        }

        if (repeatingDayIndex.isValid()) {
            break;
        }
    }

    if (repeatingDayIndex.isValid()) {
        if (repeatingDayIndex.row() < currentCellIndex.row()
            || (repeatingDayIndex.row() == currentCellIndex.row()
                && repeatingDayIndex.column() < currentCellIndex.column())) {
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
        else {
            month--;
            if (month < 1) {
                month = 12;
                year--;
            }
        }
    }
    auto currentDate = calendar->calendar().dateFromParts(year, month, day);
    assert(currentDate.isValid());

    return QStringLiteral("%1%2")
        .arg(dateChanged ? "" : "// Looks like this date was not selected\n// ")
        .arg(utils::setValueStatement(calendar, currentDate.toString(Qt::ISODate)));
}

static QString qTreeViewFilter(const QWidget *widget, const QMouseEvent *event,
                               const ExtraInfoForDelayed &extra) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::TreeView));
    if (widget == nullptr) {
        return QString();
    }

    // В этом фильтре обрабатываем только Expanded и Collapsed события для QTreeView,
    // для остальных событий будет вызван фильтр qItemViewFilter
    assert(extra.changeType.has_value());
    assert(*extra.changeType == ExtraInfoForDelayed::TreeViewExtra::Collapsed
           || *extra.changeType == ExtraInfoForDelayed::TreeViewExtra::Expanded);
    assert(extra.changeIndex.isValid());

    auto *view = qobject_cast<const QAbstractItemView *>(widget);
    assert(view != nullptr);

    const auto currentItem = view->model()->data(extra.changeIndex);
    const auto currentItemText
        = currentItem.canConvert<QString>() ? currentItem.toString() : QString();
    return QStringLiteral("%1Delegate('%2')%3")
        .arg(extra.changeType == ExtraInfoForDelayed::TreeViewExtra::Expanded ? "expand"
                                                                              : "collapse")
        .arg(utils::objectPath(widget))
        .arg(currentItemText.isEmpty()
                 ? ""
                 : QStringLiteral(" // Delegate text: '%1'").arg(currentItemText));
}

static QString qItemViewFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::ItemView));
    if (widget == nullptr) {
        return QString();
    }

    auto *view = qobject_cast<const QAbstractItemView *>(widget);
    assert(view != nullptr);
    const auto clickPos = widget->mapFromGlobal(event->globalPos());

    //! TODO:
    //! 1. Основная проблема в том, что пока что нет четкого понимания, как работать с моделями.
    //! Чаще бывает так, что данные в модели - далеко не постоянная величина. Также часто бывает,
    //! что эти данные нельзя представить в виде текста. Поэтому на текущий момент было принято
    //! решение обращаться к этим данным только по индексу.
    //!
    //! 2. Также не очень понятно, обработка каких действий может быть в принципе полезна для
    //! тестирования. Основное обрабатываемое действие - `клик` по элементу, потому что обычно
    //! именно это событие и используется в тестируемых приложениях. На всякий случай мы
    //! пока что также обрабатываем `выбор` элементов, но скорее всего это будет лишним.

    const auto *selectionModel = view->selectionModel();
    //! TODO: возможна ли ситуация `selectionModel == nullptr`?
    assert(selectionModel != nullptr);
    const auto currentIndex = view->currentIndex();
    const auto selectedIndexes = selectionModel->selectedIndexes();
    const auto selectedIndex
        = selectedIndexes.size() == 1 ? selectedIndexes.first() : QModelIndex();
    if ((view->selectionMode() == QAbstractItemView::NoSelection || currentIndex == selectedIndex)
        && view->rect().contains(clickPos) && currentIndex.isValid()) {
        const auto currentItem = view->model()->data(currentIndex);
        const auto currentItemText
            = currentItem.canConvert<QString>() ? currentItem.toString() : QString();
        return QStringLiteral("%1Delegate('%2', (%3, %4))%5")
            .arg(event->type() == QEvent::MouseButtonDblClick ? "doubleClick" : "click")
            .arg(utils::objectPath(view))
            .arg(currentIndex.row())
            .arg(currentIndex.column())
            .arg(currentItemText.isEmpty()
                     ? ""
                     : QStringLiteral(" // Delegate text: '%1'").arg(currentItemText));
    }

    const auto selectedCellsData = utils::selectedCellsData(selectionModel);
    //! TODO: на этапе обработки записанных действий скорее всего придется переделать
    //! запись выбранных ячеек
    return selectedCellsData.isEmpty()
               ? QStringLiteral("clearSelection('%1')").arg(utils::objectPath(widget))
               : QStringLiteral("let selectionData = [%1];\nsetSelection('%2', selectionData)")
                     .arg(selectedCellsData)
                     .arg(utils::objectPath(widget));
}

static QString qMenuFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::Menu));
    if (widget == nullptr) {
        return QString();
    }

    auto *menu = qobject_cast<const QMenu *>(widget);
    assert(menu != nullptr);

    const auto clickPos = widget->mapFromGlobal(event->globalPos());
    auto *action = menu->actionAt(clickPos);

    if (action == nullptr) {
        const auto menuText = menu->title();
        return QStringLiteral("activateMenu('%1')%2")
            .arg(utils::objectPath(widget))
            .arg(menuText.isEmpty() ? "" : QStringLiteral(" // Menu title: '%1'").arg(menuText));
    }
    else {
        const auto actionText = action->text();
        return QStringLiteral("%1activateMenuAction('%2', '%3'%4)%5")
            .arg(action->isSeparator() ? "// Looks like QMenu::Separator clicked\n// " : "")
            .arg(utils::objectPath(widget))
            .arg(utils::widgetIdInView(menu, menu->actions().indexOf(action), WidgetClass::Menu))
            .arg(action->isCheckable()
                     ? QStringLiteral(", %1").arg(action->isChecked() ? "false" : "true")
                     : "")
            .arg(actionText.isEmpty() ? ""
                                      : QStringLiteral(" // Action text: '%1'").arg(actionText));
    }
}

static QString qTabBarFilter(const QWidget *widget, const QMouseEvent *event) noexcept
{
    if (!utils::mouseEventCanBeFiltered(widget, event)) {
        return QString();
    }

    widget = utils::searchSpecificWidget(widget, s_widgetMetaMap.at(WidgetClass::TabBar));
    if (widget == nullptr) {
        return QString();
    }

    auto *tabBar = qobject_cast<const QTabBar *>(widget);
    assert(tabBar != nullptr);

    const auto currentIndex = tabBar->currentIndex();
    const auto currentText = tabBar->tabText(currentIndex);
    return QStringLiteral("selectTabItem('%1', '%2')%3")
        .arg(utils::objectPath(widget))
        .arg(utils::widgetIdInView(tabBar, currentIndex, WidgetClass::TabBar))
        .arg(currentText.isEmpty() ? ""
                                   : QStringLiteral(" // Tab item text: '%1'").arg(currentText));
}
} // namespace QtAda::core::filters

namespace QtAda::core {
WidgetEventFilter::WidgetEventFilter(QObject *parent) noexcept
    : QObject{ parent }
{
    filterFunctions_ = {
        filters::qRadioButtonFilter,
        filters::qCheckBoxFilter,
        filters::qComboBoxFilter,
        filters::qMenuFilter,
        filters::qTabBarFilter,
        filters::qItemViewFilter,
        // Обязательно в таком порядке:
        filters::qButtonFilter,
    };

    delayedFilterFunctions_ = {
        { WidgetClass::Slider, filters::qSliderFilter },
        { WidgetClass::SpinBox, filters::qSpinBoxFilter },
        { WidgetClass::Calendar, filters::qCalendarFilter },
        { WidgetClass::TreeView, filters::qTreeViewFilter },
    };
}

QString WidgetEventFilter::callWidgetFilters(const QWidget *widget, const QMouseEvent *event,
                                             bool isContinuous) noexcept
{
    const auto delayedResult = callDelayedFilter(widget, event, isContinuous);
    if (delayedResult.has_value() && !(*delayedResult).isEmpty()) {
        return *delayedResult;
    }

    QString result;
    for (auto &filter : filterFunctions_) {
        result = filter(widget, event);
        if (!result.isEmpty()) {
            return result;
        }
    }
    return result;
}

void WidgetEventFilter::findAndSetDelayedFilter(const QWidget *widget,
                                                const QMouseEvent *event) noexcept
{
    if (widget == delayedWidget_ && event == causedEvent_
        && causedEventType_ == QEvent::MouseButtonPress
        && event->type() == QEvent::MouseButtonDblClick) {
        return;
    }

    destroyDelay();
    WidgetClass foundWidgetClass = WidgetClass::None;
    std::vector<QMetaObject::Connection> connections;
    if (auto *foundWidget
        = utils::searchSpecificWidget(widget, filters::s_widgetMetaMap.at(WidgetClass::SpinBox))) {
        auto slot = [this] { emit this->signalDetected(); };
        foundWidgetClass = WidgetClass::SpinBox;
        QMetaObject::Connection spinBoxConnection = utils::connectIfType<QSpinBox>(
            widget, this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), slot);
        if (!spinBoxConnection) {
            spinBoxConnection = utils::connectIfType<QDoubleSpinBox>(
                foundWidget, this,
                static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), slot);
        }
        if (!spinBoxConnection) {
            spinBoxConnection = utils::connectIfType<QDateTimeEdit>(
                foundWidget, this,
                static_cast<void (QDateTimeEdit::*)(const QDateTime &)>(
                    &QDateTimeEdit::dateTimeChanged),
                slot);
        }
        connections.push_back(spinBoxConnection);
    }
    else if (auto *foundWidget = utils::searchSpecificWidget(
                 widget, filters::s_widgetMetaMap.at(WidgetClass::Slider))) {
        foundWidgetClass = WidgetClass::Slider;
        connections.push_back(utils::connectIfType<QAbstractSlider>(
            foundWidget, this,
            static_cast<void (QAbstractSlider::*)(int)>(&QAbstractSlider::actionTriggered),
            [this](int type) {
                this->delayedExtra_.changeType = type;
                emit this->signalDetected();
            }));
    }
    else if (auto *foundWidget = utils::searchSpecificWidget(
                 widget, filters::s_widgetMetaMap.at(WidgetClass::Calendar))) {
        auto *itemView = qobject_cast<const QAbstractItemView *>(foundWidget);
        assert(itemView != nullptr);
        foundWidgetClass = WidgetClass::Calendar;
        connections.push_back(utils::connectIfType<QItemSelectionModel>(
            itemView->selectionModel(), this,
            static_cast<void (QItemSelectionModel::*)(const QModelIndex &, const QModelIndex &)>(
                &QItemSelectionModel::currentChanged),
            [this] { emit this->signalDetected(); }));
    }
    else if (auto *foundWidget = utils::searchSpecificWidget(
                 widget, filters::s_widgetMetaMap.at(WidgetClass::TreeView))) {
        foundWidgetClass = WidgetClass::TreeView;
        connections.push_back(utils::connectIfType<QTreeView>(
            foundWidget, this,
            static_cast<void (QTreeView::*)(const QModelIndex &)>(&QTreeView::expanded),
            [this](const QModelIndex &index) {
                this->delayedExtra_.changeIndex = index;
                this->delayedExtra_.changeType = ExtraInfoForDelayed::TreeViewExtra::Expanded;
                emit this->signalDetected();
            }));
        connections.push_back(utils::connectIfType<QTreeView>(
            foundWidget, this,
            static_cast<void (QTreeView::*)(const QModelIndex &)>(&QTreeView::collapsed),
            [this](const QModelIndex &index) {
                this->delayedExtra_.changeIndex = index;
                this->delayedExtra_.changeType = ExtraInfoForDelayed::TreeViewExtra::Collapsed;
                emit this->signalDetected();
            }));
    }

    if (foundWidgetClass != WidgetClass::None) {
        assert(connectionIsInit(connections) == true);
        initDelay(widget, event, delayedFilterFunctions_.at(foundWidgetClass), connections);
    }
}

bool WidgetEventFilter::delayedFilterCanBeCalledForWidget(const QWidget *widget) const noexcept
{
    return needToUseFilter_ && !connectionIsInit() && delayedFilter_.has_value()
           && delayedWidget_ != nullptr && delayedWidget_ == widget;
}

void WidgetEventFilter::initDelay(const QWidget *widget, const QMouseEvent *event,
                                  const DelayedWidgetFilterFunction &filter,
                                  std::vector<QMetaObject::Connection> &connections) noexcept
{
    causedEvent_ = event;
    causedEventType_ = event->type();
    delayedWidget_ = widget;
    delayedFilter_ = filter;
    connections_ = connections;
}

void WidgetEventFilter::destroyDelay() noexcept
{
    causedEventType_ = QEvent::None;
    delayedWidget_ = nullptr;
    delayedFilter_ = std::nullopt;
    delayedExtra_.clear();
    needToUseFilter_ = false;
    disconnectAll();
}

bool WidgetEventFilter::connectionIsInit(
    std::optional<std::vector<QMetaObject::Connection>> connections) const noexcept
{
    for (auto &connection : connections.has_value() ? *connections : connections_) {
        if (connection) {
            return true;
        }
    }
    return false;
}

void WidgetEventFilter::disconnectAll() noexcept
{
    for (auto &connection : connections_) {
        QObject::disconnect(connection);
    }
    connections_.clear();
}

std::optional<QString> WidgetEventFilter::callDelayedFilter(const QWidget *widget,
                                                            const QMouseEvent *event,
                                                            bool isContinuous) noexcept
{
    delayedExtra_.isContinuous = isContinuous;
    bool callable = delayedFilterCanBeCalledForWidget(widget);
    return callable ? std::make_optional((*delayedFilter_)(widget, event, delayedExtra_))
                    : std::nullopt;
}
} // namespace QtAda::core
