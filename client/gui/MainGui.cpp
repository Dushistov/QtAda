#include "MainGui.hpp"

#include <QMessageBox>
#include <QSettings>
#include <QStandardItemModel>
#include <QString>
#include <QFileDialog>
#include <set>
#include <memory>

#include "ui_MainGui.h"
#include "Paths.hpp"
#include "GuiTools.hpp"

namespace QtAda::gui {
class CustomStandardItem : public QStandardItem {
public:
    enum Roles {
        ScriptRole = Qt::UserRole + 1,
        SourceRole,
        ProjectRole,
        TestAppRole,
        DirRole,
        None,
    };

    CustomStandardItem(const QString &name, const QIcon &icon, bool isSelectable = true) noexcept
        : QStandardItem(name)
    {
        this->setIcon(icon);
        this->setSelectable(isSelectable);
    }

    CustomStandardItem(const QString &name, const QVariant &value, const QIcon &icon, int role,
                       bool isSelectable = true) noexcept
        : CustomStandardItem(name, icon, isSelectable)
    {
        role_ = role;
        this->setData(value, role_);
    }

    int role() const noexcept
    {
        return role_;
    }

private:
    int role_ = Roles::None;
};

static CustomStandardItem *initFileItem(const QString &fileName, const QString &filePath,
                                        bool isScript) noexcept
{
    return new CustomStandardItem(
        fileName, filePath, isScript ? QIcon(":/icons/script.svg") : QIcon(":/icons/source.svg"),
        isScript ? CustomStandardItem::ScriptRole : CustomStandardItem::SourceRole);
}

static CustomStandardItem *initDirItem(const QString &dirName, const QString &dirPath,
                                       bool isSourceDir, bool isRootDir = false) noexcept
{
    if (!dirPath.isEmpty() && !isSourceDir) {
        return new CustomStandardItem(
            dirName, dirPath, isRootDir ? QIcon(":/icons/root_dir.svg") : QIcon(":/icons/dir.svg"),
            CustomStandardItem::DirRole, false);
    }
    else {
        return new CustomStandardItem(dirName, QIcon(":/icons/source_dir.svg"), false);
    }
}

static void handleSubDirectories(const QString &projectDirPath, CustomStandardItem *rootItem,
                                 bool isScriptsTree,
                                 QMap<QString, CustomStandardItem *> *projectSubDirs,
                                 const QFileInfo &fileInfo) noexcept
{
    // Указатель на элемент папки в модели
    CustomStandardItem *subDirItem = nullptr;
    const auto fileDirPath = fileInfo.dir().absolutePath();

    // Проверка существования папки по такому пути
    if (!projectSubDirs->contains(fileDirPath)) {
        // Получаем список относительных директорий
        const auto dirParts = fileDirPath.right(fileDirPath.length() - projectDirPath.length())
                                  .split(QDir::separator(), Qt::SkipEmptyParts);
        assert(!dirParts.isEmpty());

        // Устанавливаем "корневой" указатель, относительно которого будут создаваться новые папки
        subDirItem = rootItem;
        // Устанавливаем "корневой" путь, к которому постепенно будут добавляться поддиректории
        auto relativeDirPath = projectDirPath;
        // Для надежности используем эту переменную, которая показывает, что неизвестная папка
        // была найдена, соответственно, в projectSubDirs не может быть найден указатель на элемент
        // папки
        bool unknownDirFound = false;
        for (const auto &dirPart : dirParts) {
            assert(subDirItem != nullptr);
            // Получаем путь к следующей папке
            relativeDirPath += QStringLiteral("%1%2").arg(QDir::separator()).arg(dirPart);
            if (!projectSubDirs->contains(relativeDirPath)) {
                // Если папка не найдена, то до "конца" dirParts создаем новые папки
                unknownDirFound = true;

                auto *newDir = initDirItem(dirPart, relativeDirPath, false);
                subDirItem->appendRow(newDir);
                subDirItem = newDir;
                (*projectSubDirs)[relativeDirPath] = subDirItem;
            }
            else {
                // Если папка найдена, то меняем "корневой" указатель на эту папку
                assert(!unknownDirFound);
                subDirItem = projectSubDirs->value(relativeDirPath, nullptr);
            }
        }
    }
    else {
        subDirItem = projectSubDirs->value(fileDirPath, nullptr);
    }
    assert(subDirItem != nullptr);
    // Добавляем к элементу папки новый файл
    subDirItem->appendRow(
        initFileItem(fileInfo.fileName(), fileInfo.absoluteFilePath(), isScriptsTree));
}

MainGui::MainGui(const QString &projectPath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGui)
{
    ui->setupUi(this);

    // Так как в Qt Designer нельзя добавить Spacer в QToolBar, то
    // делаем это вручную
    auto *toolSpacer = new QWidget(this);
    toolSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->toolBar->addWidget(toolSpacer);
    ui->toolBar->addAction(ui->actionRunCurrent);
    ui->toolBar->addAction(ui->actionRunAll);

    auto *fileNotOpenedLabel = new QLabel(this);
    fileNotOpenedLabel->setAlignment(Qt::AlignCenter);
    fileNotOpenedLabel->setText(
        "No files are open.\nSelect the file in the project tree on the left.");
    auto *contentLayout = new QVBoxLayout(ui->contentWidget);
    contentLayout->addWidget(fileNotOpenedLabel);

    configureProject(projectPath);

    connect(ui->actionNewScript, &QAction::triggered, this,
            [this]() { addNewFileToProject(true, true); });
    connect(ui->actionNewSource, &QAction::triggered, this,
            [this]() { addNewFileToProject(true, false); });
    connect(ui->actionAddScript, &QAction::triggered, this,
            [this]() { addNewFileToProject(false, true); });
    connect(ui->actionAddSource, &QAction::triggered, this,
            [this]() { addNewFileToProject(false, false); });

    connect(ui->projectFilesView, &QTreeView::customContextMenuRequested, this,
            &MainGui::showProjectTreeContextMenu);
}

MainGui::~MainGui()
{
    if (project_ != nullptr && saveProjectFileOnExit_) {
        saveGuiParamsToProjectFile();
        project_->deleteLater();
        project_ = nullptr;
    }

    delete ui;
}

bool MainGui::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        if (uiInitialized_) {
            updateProjectFileView(true);
        }
        else {
            // Сразу после .show() этого делать нельзя, так как
            // событие WindowActivate может возникнуть позже ->
            // будет "лишний" вызов функции
            uiInitialized_ = true;
        }
    }
    return QMainWindow::event(event);
}

void MainGui::configureProject(const QString &projectPath) noexcept
{
    if (project_ != nullptr) {
        saveGuiParamsToProjectFile();
        project_->deleteLater();
    }
    project_ = new QSettings(projectPath, QSettings::IniFormat);

    updateProjectFileView(false);
    setGuiParamsFromProjectFile();
}

void MainGui::updateProjectFileView(bool isExternal) noexcept
{
    assert(project_ != nullptr);

    const auto projectFileInfo = QFileInfo(project_->fileName());
    if (!tools::isExistingFileAccessible(projectFileInfo)) {
        saveProjectFileOnExit_ = false;
        QMessageBox::critical(this, paths::QTADA_ERROR_HEADER,
                              QStringLiteral("The project file not accessible."));
        //! TODO: нужно придумать как "филиграннее" обрабатывать эту ситуацию
        QApplication::exit(1);
        return;
    }

    const auto appPath = project_->value(paths::PROJECT_APP_PATH, "").toString().trimmed();
    const auto appPathCheck = tools::checkProjectAppPath(appPath);
    if (appPathCheck != AppPathCheck::Ok) {
        saveProjectFileOnExit_ = false;
        QMessageBox::critical(this, paths::QTADA_ERROR_HEADER,
                              QStringLiteral("The testing application not accessible."));
        //! TODO: нужно придумать как "филиграннее" обрабатывать эту ситуацию
        QApplication::exit(1);
        return;
    }

    bool needToUpdateModel = false;
    const auto currentScripts = getAccessiblePaths(projectFileInfo, true);
    if (currentScripts != lastScripts_) {
        needToUpdateModel = true;
        lastScripts_ = std::move(currentScripts);
    }
    const auto currentSources = getAccessiblePaths(projectFileInfo, false);
    if (currentSources != lastSources_) {
        needToUpdateModel = true;
        lastSources_ = std::move(currentSources);
    }

    if (!needToUpdateModel && uiInitialized_) {
        return;
    }

    if (isExternal) {
        QMessageBox::warning(this, paths::QTADA_WARNING_HEADER,
                             QStringLiteral("Project resources have been changed externally."));
    }

    auto *projectFilesModel = new QStandardItemModel(this);

    const auto projectDir = projectFileInfo.dir();
    auto *rootProjectDir
        = initDirItem(projectDir.dirName(), projectDir.absolutePath(), false, true);
    projectFilesModel->appendRow(rootProjectDir);

    rootProjectDir->appendRow(
        new CustomStandardItem(projectFileInfo.fileName(), projectFileInfo.absoluteFilePath(),
                               QIcon(":/icons/project.svg"), CustomStandardItem::ProjectRole));

    const auto projectDirPath = projectDir.absolutePath();

    auto *scriptsDirItem = initDirItem(QStringLiteral("Scripts"), QString(), true);
    configureSubTree(scriptsDirItem, projectDirPath, true);
    rootProjectDir->appendRow(scriptsDirItem);

    auto *sourceDirItem = initDirItem(QStringLiteral("Sources"), QString(), true);
    configureSubTree(sourceDirItem, projectDirPath, false);
    rootProjectDir->appendRow(sourceDirItem);

    const auto appFileInfo = QFileInfo(appPath);
    projectFilesModel->appendRow(new CustomStandardItem(
        appFileInfo.fileName(), appFileInfo.absoluteFilePath(), QIcon(":/icons/test_app.svg"),
        CustomStandardItem::TestAppRole, false));

    tools::deleteModels(ui->projectFilesView);
    ui->projectFilesView->setModel(projectFilesModel);

    //! TODO: позже нужно "не трогать" раскрытие узлов дерева при добавлении
    //! новых файлов в проект внутри приложения
    // Раскрываем: <ProjectDir>, Scripts, Sources
    for (int row = 0; row < projectFilesModel->rowCount(); row++) {
        const auto index = projectFilesModel->index(row, 0);
        ui->projectFilesView->expand(index);

        for (int subRow = 0; subRow < projectFilesModel->itemFromIndex(index)->rowCount();
             subRow++) {
            ui->projectFilesView->expand(projectFilesModel->index(subRow, 0, index));
        }
    }
}

QStringList MainGui::getAccessiblePaths(const QFileInfo &projectInfo, bool isScripts) noexcept
{
    assert(project_ != nullptr);

    // Считываем пути к файлам
    const auto rawFilesPaths
        = project_->value(isScripts ? paths::PROJECT_SCRIPTS : paths::PROJECT_SOURCES)
              .toStringList();
    if (rawFilesPaths.isEmpty()) {
        return QStringList();
    }

    // "Засовывая" rawFilesPaths в std::set мы одновременно избавляемся от дубликатов
    // и сортируем строки по алфавиту
    auto filesPaths = std::set<QString>(rawFilesPaths.begin(), rawFilesPaths.end());
    // Чтобы исключить дублирование файла проекта в дереве
    filesPaths.erase(projectInfo.absoluteFilePath());

    // Список файлов, которые "подходят" для нашего проекта
    QStringList acceptedFiles;
    // Список файлов, которые "не подходят" для нашего проекта, не существуют, не читаемы,
    // не доступны для записи или не являющиеся файлами
    QStringList discardFiles;

    const auto projectDirPath = projectInfo.dir().absolutePath();
    for (auto filePath : filesPaths) {
        filePath = filePath.trimmed();
        if (filePath.isEmpty()) {
            continue;
        }

        QFileInfo fileInfo(filePath);
        if (!tools::isExistingFileAccessible(fileInfo)
            || (isScripts && fileInfo.suffix() != "js")) {
            discardFiles.append(filePath);
            continue;
        }
        acceptedFiles.append(filePath);
    }

    project_->setValue(isScripts ? paths::PROJECT_SCRIPTS : paths::PROJECT_SOURCES,
                       acceptedFiles.isEmpty() ? QStringList("") : acceptedFiles);
    if (!discardFiles.isEmpty()) {
        QMessageBox::warning(
            this, paths::QTADA_WARNING_HEADER,
            QStringLiteral("These files are not applicable to the project, "
                           "so they have been removed from the project file:\n-- %1")
                .arg(discardFiles.join("\n-- ")));
    }

    return acceptedFiles;
}

void MainGui::configureSubTree(CustomStandardItem *rootItem, const QString &projectDirPath,
                               bool isScriptsTree) noexcept
{
    assert(rootItem != nullptr);
    // Файлы, которые лежат в корневой папке проекта. Выносим их отдельно, чтобы
    // добавить в конце инициализации, после всех подпапок
    QVector<QFileInfo> projectDirFilesInfo;
    // Файлы, которые не относятся к корневой папке проекта
    QVector<QFileInfo> otherPathsInfo;
    // Структура данных для папок корневой директории проекта, где:
    // {путь к папке} -> {указатель на элемент модели}
    auto projectSubDirs = std::make_unique<QMap<QString, CustomStandardItem *>>();

    for (const auto &filePath : (isScriptsTree ? lastScripts_ : lastSources_)) {
        QFileInfo fileInfo(filePath);

        const auto fileDirPath = fileInfo.dir().absolutePath();
        if (!fileInfo.absoluteFilePath().startsWith(projectDirPath)) {
            // Если начало абсолютного пути к папке в которой лежит файл не соответствует
            // пути к корневой папке проекта, то файл лежит вне корневой папки проекта
            otherPathsInfo.push_back(fileInfo);
            continue;
        }

        // Проверка лежит ли файл в корневой папке проекта
        if (fileDirPath == projectDirPath) {
            projectDirFilesInfo.append(fileInfo);
            continue;
        }

        handleSubDirectories(projectDirPath, rootItem, isScriptsTree, projectSubDirs.get(),
                             fileInfo);
    }

    // Добавляем в конец rootItem файлы, лежащие в корневой папке проекта
    for (const auto &fileInfo : projectDirFilesInfo) {
        rootItem->appendRow(
            initFileItem(fileInfo.fileName(), fileInfo.absoluteFilePath(), isScriptsTree));
    }

    if (!otherPathsInfo.isEmpty()) {
        // Если были обнаружены файлы, лежащие вне корневой папки проекта, то в конец
        // добавляем папку, в которой будут расположены все "внешние" файлы
        projectSubDirs->clear();
        auto *otherPathsDirItem = initDirItem("<Other paths>", QString(), true);
        for (const auto &otherPathFileInfo : otherPathsInfo) {
            handleSubDirectories(QString(), otherPathsDirItem, isScriptsTree, projectSubDirs.get(),
                                 otherPathFileInfo);
        }
        rootItem->appendRow(otherPathsDirItem);
    }
}

void MainGui::saveGuiParamsToProjectFile() noexcept
{
    assert(project_ != nullptr);

    project_->beginGroup(paths::PROJECT_GUI_GROUP);

    QVariantList contentSizes;
    for (const auto &width : ui->contentSplitter->sizes()) {
        contentSizes.append(width);
    }
    assert(contentSizes.size() == 3);
    project_->setValue(paths::PROJECT_CONTENT_SIZES, contentSizes);

    QVariantList mainSizes;
    for (const auto &height : ui->mainSplitter->sizes()) {
        mainSizes.append(height);
    }
    assert(mainSizes.size() == 2);
    project_->setValue(paths::PROJECT_MAIN_SIZES, mainSizes);

    project_->setValue(paths::PROJECT_TOOL_BAR_POSITION, this->toolBarArea(ui->toolBar));

    project_->endGroup();
    project_->sync();
}

void MainGui::setGuiParamsFromProjectFile() noexcept
{
    assert(project_ != nullptr);

    bool toolBarPosIsOk = false;
    auto *toolBar = ui->toolBar;
    assert(toolBar != nullptr);

    project_->beginGroup(paths::PROJECT_GUI_GROUP);
    const auto toolBarPos
        = project_->value(paths::PROJECT_TOOL_BAR_POSITION, Qt::ToolBarArea::TopToolBarArea)
              .toInt(&toolBarPosIsOk);
    const auto contentProjectSizes = project_->value(paths::PROJECT_CONTENT_SIZES, {}).toList();
    const auto mainProjectSizes = project_->value(paths::PROJECT_MAIN_SIZES, {}).toList();
    project_->endGroup();

    if (toolBarPosIsOk && toolBarPos >= Qt::ToolBarArea::LeftToolBarArea
        && toolBarPos <= Qt::ToolBarArea::BottomToolBarArea) {
        this->addToolBar(static_cast<Qt::ToolBarArea>(toolBarPos), toolBar);
    }
    else {
        this->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
    }

    if (contentProjectSizes.isEmpty() || contentProjectSizes.size() != 3) {
        ui->contentSplitter->setSizes({ -1, ui->contentWidget->maximumWidth(), -1 });
    }
    else {
        QList<int> contentSizes;
        for (const auto &width : contentProjectSizes) {
            if (width.canConvert<int>()) {
                contentSizes.append(width.toInt());
            }
            else {
                contentSizes.append(-1);
            }
        }
        ui->contentSplitter->setSizes(contentSizes);
    }

    if (mainProjectSizes.isEmpty() || mainProjectSizes.size() != 2) {
        ui->mainSplitter->setSizes({ ui->contentSplitter->maximumHeight(), -1 });
    }
    else {
        QList<int> mainSizes;
        for (const auto &height : mainProjectSizes) {
            if (height.canConvert<int>()) {
                mainSizes.append(height.toInt());
            }
            else {
                mainSizes.append(-1);
            }
        }
        ui->mainSplitter->setSizes(mainSizes);
    }
}

void MainGui::addNewFileToProject(bool isNewFileMode, bool isScript) noexcept
{
    assert(project_ != nullptr);
    const auto projectInfo = QFileInfo(project_->fileName());
    const auto projectPath = projectInfo.absoluteFilePath();

    const auto defaultDir = projectInfo.dir().absolutePath();
    const auto filter
        = isScript ? QStringLiteral("JavaScript (*.js)") : QStringLiteral("All files (*)");
    const auto path
        = (isNewFileMode
               ? QFileDialog::getSaveFileName(
                   this, isScript ? paths::QTADA_NEW_SCRIPT_HEADER : paths::QTADA_NEW_SOURCE_HEADER,
                   defaultDir, filter)
               : QFileDialog::getOpenFileName(
                   this, isScript ? paths::QTADA_ADD_SCRIPT_HEADER : paths::QTADA_ADD_SOURCE_HEADER,
                   defaultDir, filter))
              .trimmed();

    if (path.isEmpty()) {
        return;
    }

    const auto newFileInfo = QFileInfo(path);
    if (!isNewFileMode && !tools::isExistingFileAccessible(newFileInfo)) {
        QMessageBox::warning(
            this, paths::QTADA_WARNING_HEADER,
            QStringLiteral("The %1 file is not accessible.").arg(isScript ? "script" : "source"));
        return;
    }

    if (isScript && newFileInfo.suffix() != "js") {
        QMessageBox::warning(
            this, paths::QTADA_WARNING_HEADER,
            QStringLiteral("The script must be a JavaScript file with a .js extension."));
        return;
    }

    const auto newFilePath = newFileInfo.absoluteFilePath();
    if (projectPath == newFilePath) {
        QMessageBox::warning(this, paths::QTADA_WARNING_HEADER,
                             isNewFileMode
                                 ? QStringLiteral("You can't rewrite current project file.")
                                 : QStringLiteral("You can't add current project file as source."));
        return;
    }

    if (isNewFileMode) {
        auto newFile = QFile(newFilePath);
        if (!newFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::warning(this, paths::QTADA_WARNING_HEADER,
                                 QStringLiteral("The %1 file can't be created.")
                                     .arg(isScript ? "script" : "source"));
            return;
        }
        newFile.close();
        assert(newFileInfo.exists());
    }

    auto paths = project_->value(isScript ? paths::PROJECT_SCRIPTS : paths::PROJECT_SOURCES, "")
                     .toStringList();
    paths.append(newFilePath);
    project_->setValue(isScript ? paths::PROJECT_SCRIPTS : paths::PROJECT_SOURCES, paths);
    updateProjectFileView(false);
}

void MainGui::showProjectTreeContextMenu(const QPoint &pos) noexcept
{
    const auto index = ui->projectFilesView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    const auto *model = qobject_cast<const QStandardItemModel *>(ui->projectFilesView->model());
    assert(model != nullptr);

    const auto *rawItem = model->itemFromIndex(index);
    const auto *item = static_cast<const CustomStandardItem *>(rawItem);
    assert(item != nullptr);

    const auto role = item->role();

    if (role == CustomStandardItem::None) {
        return;
    }

    const auto path = item->data(role).value<QString>();
    assert(!path.isEmpty());

    QMenu contextMenu;

    switch (role) {
    case CustomStandardItem::ScriptRole: {
        contextMenu.addAction(QStringLiteral("Run Test Script"), this,
                              [this, path] { runScript(path); });
        contextMenu.addSeparator();
    }
    case CustomStandardItem::SourceRole:
    case CustomStandardItem::ProjectRole: {
        contextMenu.addAction(QStringLiteral("Open in Editor"), this,
                              [this, path] { openInEditor(path); });
        if (role != CustomStandardItem::ProjectRole) {
            contextMenu.addAction(QStringLiteral("Remove From Project"), this,
                                  [this, path] { removeFromProject(path); });
        }
        contextMenu.addSeparator();
        contextMenu.addAction(QStringLiteral("Open Externally"), this,
                              [this, path] { openExternally(path); });
        break;
    }
    case CustomStandardItem::TestAppRole: {
        contextMenu.addAction(QStringLiteral("Execute"), this,
                              [this, path] { executeApplication(path); });
        contextMenu.addSeparator();
        break;
    }
    case CustomStandardItem::DirRole: {
        contextMenu.addAction(QStringLiteral("Open Folder"), this,
                              [this, path] { openFolder(path); });
        break;
    }
    default:
        Q_UNREACHABLE();
    }

    if (role != CustomStandardItem::DirRole) {
        contextMenu.addAction(QStringLiteral("Show in Folder"), this,
                              [this, path] { showInFolder(path); });
    }
    contextMenu.addSeparator();
    contextMenu.addAction(QStringLiteral("Rename"), this, [this, path] { renameFile(path); });
    if (role != CustomStandardItem::ProjectRole && role != CustomStandardItem::TestAppRole) {
        contextMenu.addAction(QStringLiteral("Delete"), this, [this, path] { deleteFile(path); });
    }

    contextMenu.exec(ui->projectFilesView->viewport()->mapToGlobal(pos));
}
} // namespace QtAda::gui
