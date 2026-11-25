#pragma once

#include <QString>
#if __has_include(<QIcon>)
#include <QIcon>
#define CONCRETE_HAS_QICON 1
#endif

QString get_app_base_path();
QString getAppBasePath();

QString get_text_file_path(const QString& filename, const QString& folder = QString());
QString getTextFilePath(const QString& filename, const QString& folder = QString());

QString load_text_file(const QString& filename, const QString& folder = QString(), const QString& encoding = QStringLiteral("utf-8"));
QString loadTextFile(const QString& filename, const QString& folder = QString(), const QString& encoding = QStringLiteral("utf-8"));

QString get_app_icon_path();
#ifdef CONCRETE_HAS_QICON
QIcon get_app_icon();
#endif

