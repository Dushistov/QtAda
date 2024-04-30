#pragma once

#include <QDir>

namespace QtAda::paths {
static const auto QTADA_CONFIG = QStringLiteral("%1/.config/qtada.conf").arg(QDir::homePath());
static constexpr char PROJECT_SUFFIX[] = "qtada";
static constexpr char PROJECT_TMP_SUFFIX[] = "qtada_tmp";

static constexpr char CONFIG_RECENT_PROJECTS[] = "recentProjects";

static const auto QTADA_HEADER = QStringLiteral("QtAda");
static const auto QTADA_ERROR_HEADER = QStringLiteral("%1 | Error").arg(QTADA_HEADER);
static const auto QTADA_PROJECT_ERROR_HEADER
    = QStringLiteral("%1 | Project Error").arg(QTADA_HEADER);
static const auto QTADA_INIT_PROJECT_HEADER = QStringLiteral("%1 | Init Project").arg(QTADA_HEADER);
static const auto QTADA_OPEN_PROJECT_HEADER = QStringLiteral("%1 | Open Project").arg(QTADA_HEADER);
static const auto QTADA_NEW_PROJECT_HEADER = QStringLiteral("%1 | New Project").arg(QTADA_HEADER);
static const auto QTADA_SELECT_EXE_HEADER
    = QStringLiteral("%1 | Select Executable").arg(QTADA_HEADER);
static const auto QTADA_RECORD_CONTROLLER_HEADER
    = QStringLiteral("%1 | Record Controller").arg(QTADA_HEADER);

static constexpr char PROJECT_APP_PATH[] = "appPath";
static constexpr char PROJECT_SIZES_GROUP[] = "LastUserSizes";
static constexpr char PROJECT_CONTENT_SIZES[] = "contentSizes";
static constexpr char PROJECT_MAIN_SIZES[] = "mainSizes";

static constexpr char REMOTE_OBJECT_PATH[] = "local:QTADA_REMOTE_OBJECT";
//! TODO: Костыль, см. InprocessController::startInitServer().
static const auto INIT_CONNECTION_SERVER
    = QStringLiteral("%1/QTADA_INIT_CONNECTION_SERVER").arg(QDir::tempPath());
} // namespace QtAda::paths
