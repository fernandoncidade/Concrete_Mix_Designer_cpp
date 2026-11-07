#include "find_VersionEditor.hpp"

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtGlobal>

#include <optional>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace mocks::version_editor {
namespace {

const QStringList kPtMonths = {
    QStringLiteral("Janeiro"),
    QStringLiteral("Fevereiro"),
    QStringLiteral("Março"),
    QStringLiteral("Abril"),
    QStringLiteral("Maio"),
    QStringLiteral("Junho"),
    QStringLiteral("Julho"),
    QStringLiteral("Agosto"),
    QStringLiteral("Setembro"),
    QStringLiteral("Outubro"),
    QStringLiteral("Novembro"),
    QStringLiteral("Dezembro"),
};

const QStringList kPtMonthsLower = {
    QStringLiteral("janeiro"),
    QStringLiteral("fevereiro"),
    QStringLiteral("março"),
    QStringLiteral("abril"),
    QStringLiteral("maio"),
    QStringLiteral("junho"),
    QStringLiteral("julho"),
    QStringLiteral("agosto"),
    QStringLiteral("setembro"),
    QStringLiteral("outubro"),
    QStringLiteral("novembro"),
    QStringLiteral("dezembro"),
};

const QStringList kEnMonths = {
    QStringLiteral("January"),
    QStringLiteral("February"),
    QStringLiteral("March"),
    QStringLiteral("April"),
    QStringLiteral("May"),
    QStringLiteral("June"),
    QStringLiteral("July"),
    QStringLiteral("August"),
    QStringLiteral("September"),
    QStringLiteral("October"),
    QStringLiteral("November"),
    QStringLiteral("December"),
};

const QString &group_about()
{
    static const QString value = QStringLiteral("Sobre");
    return value;
}

const QString &group_clc()
{
    static const QString value = QStringLiteral("CLC");
    return value;
}

const QString &group_copyright()
{
    static const QString value = QStringLiteral("Copyright");
    return value;
}

const QString &group_eula()
{
    static const QString value = QStringLiteral("EULA");
    return value;
}

const QString &group_notices()
{
    static const QString value = QStringLiteral("Notices");
    return value;
}

const QString &group_privacy()
{
    static const QString value = QStringLiteral("Política de Privacidade");
    return value;
}

const QString &group_source()
{
    static const QString value = QStringLiteral("Código Fonte (C++)");
    return value;
}

const QString &group_docs()
{
    static const QString value = QStringLiteral("Documentação");
    return value;
}

const QString &group_installers()
{
    static const QString value = QStringLiteral("Instaladores (Inno Setup)");
    return value;
}

const QStringList &group_order()
{
    static const QStringList order = {
        group_about(),
        group_clc(),
        group_copyright(),
        group_eula(),
        group_notices(),
        group_privacy(),
        group_source(),
        group_docs(),
        group_installers(),
    };
    return order;
}

QString build_version_from_date(const QDate &date = QDate::currentDate())
{
    const QDate safe_date = date.isValid() ? date : QDate::currentDate();
    if (!safe_date.isValid()) {
        return QStringLiteral("2026.10.22.0");
    }

    return QStringLiteral("%1.%2.%3.0")
        .arg(safe_date.year())
        .arg(safe_date.month())
        .arg(safe_date.day());
}

QString readme_version_token(const QString &version)
{
    const QString trimmed = version.trimmed();
    if (trimmed.isEmpty()) {
        return QStringLiteral("v0.0.0.0");
    }

    return trimmed.startsWith(QLatin1Char('v'), Qt::CaseInsensitive) ? trimmed : QStringLiteral("v%1").arg(trimmed);
}

QString spec_content_key(const FileSpec &spec)
{
    return spec.content_key.isEmpty() ? spec.key : spec.content_key;
}

QString version_number(const QString &version)
{
    QString normalized = version.trimmed();
    if (normalized.startsWith(QLatin1Char('v'), Qt::CaseInsensitive)) {
        normalized.remove(0, 1);
    }
    return normalized;
}

bool replace_project_version_tokens(QString *line, const QString &version)
{
    if (line == nullptr) {
        return false;
    }

    const QString normalized_version = version_number(version);
    if (normalized_version.isEmpty()) {
        return false;
    }

    // Limita a substituição às versões do aplicativo (quatro componentes), sem
    // alterar versões de normas, Qt ou Contributor Covenant.
    const QRegularExpression version_rx(
        QStringLiteral(R"(\b([vV]?)(\d{4}\.\d{1,2}\.\d{1,2}\.\d+)\b)"));
    QRegularExpressionMatchIterator matches = version_rx.globalMatch(*line);
    if (!matches.hasNext()) {
        return false;
    }

    QString updated;
    int offset = 0;
    do {
        const QRegularExpressionMatch match = matches.next();
        updated += line->mid(offset, match.capturedStart(0) - offset);
        updated += match.captured(1);
        updated += normalized_version;
        offset = match.capturedEnd(0);
    } while (matches.hasNext());
    updated += line->mid(offset);

    const bool changed = updated != *line;
    if (changed) {
        *line = updated;
    }
    return changed;
}

bool replace_markdown_date(QString *line, const QString &date, bool english)
{
    if (line == nullptr || date.trimmed().isEmpty()) {
        return false;
    }

    const QRegularExpression date_rx(
        english
            ? QStringLiteral(R"(^(\s*(?:[-*]\s*)?\*\*(?:Release Date|Date|Last updated):\*\*\s*).*$)")
            : QStringLiteral(R"(^(\s*(?:[-*]\s*)?\*\*(?:Data(?: de Lançamento)?|Última atualização):\*\*\s*).*$)"));
    const QRegularExpressionMatch match = date_rx.match(*line);
    if (!match.hasMatch()) {
        return false;
    }

    const QString trailing_markdown_break = line->endsWith(QStringLiteral("  ")) ? QStringLiteral("  ") : QString();
    const QString updated = match.captured(1) + date.trimmed() + trailing_markdown_break;
    if (updated == *line) {
        return false;
    }

    *line = updated;
    return true;
}

QString find_release_doc_relative_path(const QString &base_dir)
{
    const QString root = base_dir.isEmpty() ? detect_project_root() : base_dir;
    const QString preferred = QStringLiteral("docs/release-v2026.10.22.0.md");
    if (QFileInfo::exists(QDir(root).filePath(preferred))) {
        return preferred;
    }

    const QDir docs_dir(QDir(root).filePath(QStringLiteral("docs")));
    const QStringList files = docs_dir.entryList({QStringLiteral("release-v*.md")}, QDir::Files, QDir::Name);
    if (!files.isEmpty()) {
        return QStringLiteral("docs/%1").arg(files.first());
    }
    return preferred;
}

const QMap<QString, QStringList> &patterns()
{
    static const QMap<QString, QStringList> data = {
        {QStringLiteral("about_pt"), {QStringLiteral(R"(^(Versão:\s*[\d\.]+))")}},
        {QStringLiteral("about_en"), {QStringLiteral(R"(^(Version:\s*[\d\.]+))")}},
        {QStringLiteral("history_pt"), {QStringLiteral(R"((História e Justificativa do Desenvolvimento|HISTÓRICO DO DESENVOLVIMENTO))")}},
        {QStringLiteral("history_en"), {QStringLiteral(R"((Development History and Rationale|DEVELOPMENT HISTORY))")}},
        {QStringLiteral("clc_pt"),
         {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*[\d\.]+|Vers(?:ã|a)o\s+[\d\.]+,?\s+.*)$)"),
          QStringLiteral(R"(^(Data(?:\s+de\s+Vig[êe]ncia)?:\s*.*)$)")}},
        {QStringLiteral("clc_en"),
         {QStringLiteral(R"(^(Version:\s*[\d\.]+|Version\s+[\d\.]+,?\s+.*)$)"),
          QStringLiteral(R"(^((?:Effective\s+)?Date:\s*.*)$)")}},
        {QStringLiteral("copyright_pt"),
         {QStringLiteral(R"(^(Vers(?:ã|a)o.*))"),
          QStringLiteral(R"(^(Copyright.*))")}},
        {QStringLiteral("copyright_en"),
         {QStringLiteral(R"(^(Version.*))"),
          QStringLiteral(R"(^(Copyright.*))")}},
        {QStringLiteral("eula_pt"), {QStringLiteral(R"(^(Versão\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("eula_en"), {QStringLiteral(R"(^(Version\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("notice_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o.*))")}},
        {QStringLiteral("notice_en"), {QStringLiteral(R"(^(Version.*))")}},
        {QStringLiteral("privacy_pt"),
         {QStringLiteral(R"(^(Última atualiza(?:çã|ca)o:\s*.*)$)"),
          QStringLiteral(R"(^(Vers(?:ã|a)o:\s*[\d\.]+))")}},
        {QStringLiteral("privacy_en"),
         {QStringLiteral(R"(^(Last updated:\s*.*)$)"),
          QStringLiteral(R"(^(Version:\s*[\d\.]+))")}},
        {QStringLiteral("main_root"),
         {QStringLiteral(R"(((?:QApplication::)?setApplicationVersion\((?:QStringLiteral\()?"[\d\.]+"(?:\))?\);))")}},
        {QStringLiteral("about_dialog"),
         {QStringLiteral(R"((tr\("Versão"\)\s*\+\s*":</b>\s*[\d\.]+))")}},
        {QStringLiteral("project_markdown"),
         {QStringLiteral(R"(((?:v|V)?\d{4}\.\d{1,2}\.\d{1,2}\.\d+))")}},
        {QStringLiteral("release_doc"),
         {QStringLiteral(R"(((?:v|V)?\d{4}\.\d{1,2}\.\d{1,2}\.\d+))"),
          QStringLiteral(R"((^\s*-\s*\*\*Data de Lançamento:\*\*\s*.+$))"),
          QStringLiteral(R"((^\s*-\s*\*\*Release Date:\*\*\s*.+$))")}},
        {QStringLiteral("iss_msvc"),
         {QStringLiteral(R"((#define\s+MyAppVersion\s+"[\d\.]+"))"),
          QStringLiteral(R"((OutputBaseFilename=\S+))")}},
        {QStringLiteral("iss_unified"),
         {QStringLiteral(R"((#define\s+MyAppVersion\s+"[\d\.]+"))")}},
        {QStringLiteral("iss_mingw"),
         {QStringLiteral(R"((#define\s+MyAppVersion\s+"[\d\.]+"))"),
          QStringLiteral(R"((OutputBaseFilename=\S+))")}},
    };

    return data;
}

QString resolve_file_path(const QString &base_dir, const QString &relative_path)
{
    const QDir dir(base_dir);
    return QFileInfo(dir.filePath(relative_path)).absoluteFilePath();
}

std::optional<QString> read_file_text(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QByteArray data = file.readAll();
    if (data.startsWith("\xEF\xBB\xBF")) {
        data = data.mid(3);
    }

    QString utf8 = QString::fromUtf8(data);
    if (!utf8.contains(QChar::ReplacementCharacter)) {
        return utf8;
    }

    QString latin1 = QString::fromLatin1(data);
    return latin1;
}

bool write_file_text(const QString &path, const QString &text, QString *error_message)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    const qint64 written = file.write(text.toUtf8());
    if (written < 0) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    return true;
}

QString detect_line_ending(const QString &text)
{
    return text.contains(QStringLiteral("\r\n")) ? QStringLiteral("\r\n") : QStringLiteral("\n");
}

QStringList split_lines_preserving_empty(const QString &text)
{
    return text.split(QRegularExpression(QStringLiteral(R"(\r\n|\n|\r)")), Qt::KeepEmptyParts);
}

QString join_lines(const QStringList &lines, const QString &separator)
{
    return lines.join(separator);
}

QStringList first_lines(const QString &text, int limit)
{
    QStringList lines = split_lines_preserving_empty(text);
    if (lines.size() > limit) {
        lines = lines.mid(0, limit);
    }
    return lines;
}

QString mock_windows_icon_name()
{
    return QStringLiteral("concrete_mix_design.ico");
}

QString find_icon_path(const QString &project_root, const QString &preferred_name = QString())
{
    const QString primary_icon_name = preferred_name.trimmed().isEmpty() ? mock_windows_icon_name() : preferred_name.trimmed();
    const QStringList candidate_names = {
        primary_icon_name,
        QStringLiteral("concrete_mix_design.ico"),
        QStringLiteral("concrete_mix_design.png")
    };

    const QDir root(project_root);
    const QDir app_dir(QCoreApplication::applicationDirPath());
    const QDir current_dir(QDir::currentPath());
    const QDir source_dir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());

    for (const QString &icon_name : candidate_names) {
        if (icon_name.isEmpty()) {
            continue;
        }
        QStringList candidates = {
            app_dir.filePath(QStringLiteral("icons/%1").arg(icon_name)),
            app_dir.filePath(icon_name),
            current_dir.filePath(QStringLiteral("icons/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("mocks/icons/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("icons/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("../source/assets/icons/%1").arg(icon_name)),
        };

        if (!project_root.isEmpty()) {
            candidates.prepend(root.filePath(QStringLiteral("source/assets/icons/%1").arg(icon_name)));
            candidates.prepend(root.filePath(QStringLiteral("mocks/icons/%1").arg(icon_name)));
            candidates.prepend(root.filePath(QStringLiteral("icons/%1").arg(icon_name)));
        }

        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
        }
    }

    return QString();
}

}  // namespace

QString normalize_dir(const QString &path)
{
    if (path.trimmed().isEmpty()) {
        return QString();
    }

    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

QString detect_project_root()
{
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString current_dir = QDir::currentPath();
    const QString source_file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
    QString renamed_source_dir = source_file_dir;
    renamed_source_dir.replace(QStringLiteral("Concrete_Mix_Design_cpp"), QStringLiteral("Concrete_Mix_Designer_cpp"));

    QStringList seeds = {app_dir, current_dir, source_file_dir, renamed_source_dir};
    QStringList checked;

    auto is_project_root = [](const QString &candidate) {
        const QDir dir(candidate);
        return dir.exists(QStringLiteral("mocks")) && dir.exists(QStringLiteral("source"))
               && QFileInfo(dir.filePath(QStringLiteral("CMakeLists.txt"))).exists();
    };

    for (const QString &seed : seeds) {
        QDir dir(seed);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = normalize_dir(dir.absolutePath());
            if (!checked.contains(candidate)) {
                checked.append(candidate);
                if (is_project_root(candidate)) {
                    return candidate;
                }
            }

            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return normalize_dir(QDir(source_file_dir).filePath(QStringLiteral("..")));
}

QList<FileSpec> file_specs(const QString &base_dir)
{
    QList<FileSpec> data;

    auto make_key = [](const QString &relative_path) {
        QString key;
        bool last_was_separator = false;
        for (const QChar ch : relative_path.toCaseFolded()) {
            if (ch.isLetterOrNumber()) {
                key.append(ch);
                last_was_separator = false;
            } else if (!last_was_separator) {
                key.append(QLatin1Char('_'));
                last_was_separator = true;
            }
        }

        while (key.endsWith(QLatin1Char('_'))) {
            key.chop(1);
        }
        return key;
    };

    auto append = [&](const QString &group,
                      const QString &relative_path,
                      const QString &content_key,
                      bool optional = false) {
        data.append({make_key(relative_path), relative_path, content_key, group, optional});
    };

    // 1. Arquivos Sobre (ABOUT)
    append(group_about(), QStringLiteral("source/assets/ABOUT/ABOUT_pt_BR.txt"), QStringLiteral("about_pt"));
    append(group_about(), QStringLiteral("source/assets/ABOUT/ABOUT_en_US.txt"), QStringLiteral("about_en"));
    append(group_about(), QStringLiteral("source/assets/ABOUT/History_APP_pt_BR.txt"), QStringLiteral("history_pt"));
    append(group_about(), QStringLiteral("source/assets/ABOUT/History_APP_en_US.txt"), QStringLiteral("history_en"));

    // 2. Contratos Comerciais (CLC)
    append(group_clc(), QStringLiteral("source/assets/CLC/CLC_pt_BR.txt"), QStringLiteral("clc_pt"));
    append(group_clc(), QStringLiteral("source/assets/CLC/CLC_en_US.txt"), QStringLiteral("clc_en"));

    // 3. Copyright & Marca
    append(group_copyright(), QStringLiteral("source/assets/COPYRIGHT/AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt"), QStringLiteral("copyright_pt"));
    append(group_copyright(), QStringLiteral("source/assets/COPYRIGHT/COPYRIGHT AND TRADEMARK NOTICE_en_US.txt"), QStringLiteral("copyright_en"));

    // 4. Licenças EULA
    append(group_eula(), QStringLiteral("source/assets/EULA/EULA_pt_BR.txt"), QStringLiteral("eula_pt"));
    append(group_eula(), QStringLiteral("source/assets/EULA/EULA_en_US.txt"), QStringLiteral("eula_en"));

    // 5. Notices & Terceiros
    append(group_notices(), QStringLiteral("source/assets/NOTICES/NOTICE_pt_BR.txt"), QStringLiteral("notice_pt"));
    append(group_notices(), QStringLiteral("source/assets/NOTICES/NOTICE_en_US.txt"), QStringLiteral("notice_en"));

    // 6. Políticas de Privacidade
    append(group_privacy(), QStringLiteral("source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt"), QStringLiteral("privacy_pt"));
    append(group_privacy(), QStringLiteral("source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt"), QStringLiteral("privacy_en"));

    // 7. Código Fonte Principal (C++)
    append(group_source(), QStringLiteral("main.cpp"), QStringLiteral("main_root"));
    append(group_source(), QStringLiteral("source/gui/SobreDialog.cpp"), QStringLiteral("about_dialog"));

    // 8. Documentação Técnica e de Comunidade (Markdown). Esses arquivos podem
    // não expor metadados de produto; nesse caso são apenas relatados, sem falha.
    append(group_docs(), QStringLiteral("CODE_OF_CONDUCT.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("CONTRIBUTING.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("MANUAL.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("OBSERVACAO.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("README.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("SECURITY.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/architecture.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/cli-reference.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/compliance.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/licensing.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/matriz_rastreabilidade_normativa.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), find_release_doc_relative_path(base_dir), QStringLiteral("release_doc"), true);
    append(group_docs(), QStringLiteral("docs/security-model.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/suggested-issues.md"), QStringLiteral("project_markdown"), true);
    append(group_docs(), QStringLiteral("docs/user-guide.md"), QStringLiteral("project_markdown"), true);

    // 9. Instaladores (Inno Setup)
    append(group_installers(), QStringLiteral("repo/Concrete_Mix_Designer_msvc.iss"), QStringLiteral("iss_msvc"), true);
    append(group_installers(), QStringLiteral("repo/Concrete_Mix_Designer.iss"), QStringLiteral("iss_unified"), true);
    append(group_installers(), QStringLiteral("repo/Concrete_Mix_Designer_mingw.iss"), QStringLiteral("iss_mingw"), true);

    return data;
}

const QList<FileSpec> &file_specs()
{
    static const QList<FileSpec> static_specs = file_specs(detect_project_root());
    return static_specs;
}

QList<FileStatus> check_expected_lines(const QString &base_dir)
{
    QList<FileStatus> statuses;

    const auto specs = file_specs(base_dir);
    for (const FileSpec &spec : specs) {
        FileStatus status;
        status.key = spec.key;
        status.group = spec.group.isEmpty() ? group_source() : spec.group;
        status.relative_path = spec.relative_path;
        status.path = resolve_file_path(base_dir, spec.relative_path);
        status.optional = spec.optional;

        const std::optional<QString> text = read_file_text(status.path);
        if (!text.has_value()) {
            status.exists = false;
            statuses.append(status);
            continue;
        }

        status.exists = true;
        const QStringList pattern_list = patterns().value(spec_content_key(spec));
        if (pattern_list.isEmpty()) {
            status.found = first_lines(text.value(), 3);
            statuses.append(status);
            continue;
        }

        for (const QString &pattern : pattern_list) {
            const QRegularExpression rx(pattern, QRegularExpression::MultilineOption);
            const QRegularExpressionMatch match = rx.match(text.value());
            if (match.hasMatch()) {
                status.found.append(match.captured(1).trimmed());
            } else {
                status.missing.append(pattern);
            }
        }

        statuses.append(status);
    }

    return statuses;
}

bool apply_line_updates(const QString &key, QStringList *lines, const UpdateContext &context)
{
    if (lines == nullptr) {
        return false;
    }

    bool changed = false;
    bool english_section = false;
    for (QString &line : *lines) {
        const QString original = line;

        if (line.startsWith(QStringLiteral("## Português (Brasil)"))) {
            english_section = false;
        } else if (line.startsWith(QStringLiteral("## English (United States)"))) {
            english_section = true;
        }

        if (key == QStringLiteral("about_pt") || key == QStringLiteral("copyright_pt") || key == QStringLiteral("notice_pt")) {
            if (line.startsWith(QStringLiteral("Versão:")) || line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Versão ")) || line.startsWith(QStringLiteral("Versao "))) {
                line = QStringLiteral("Versão %1, %2").arg(context.pt_version, context.pt_date);
            }
        } else if (key == QStringLiteral("about_en") || key == QStringLiteral("copyright_en") || key == QStringLiteral("notice_en")) {
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Version "))) {
                line = QStringLiteral("Version %1, %2").arg(context.en_version, context.en_date);
            }
        } else if (key == QStringLiteral("clc_pt") || key == QStringLiteral("eula_pt")) {
            const QRegularExpression rx_ver_date(QStringLiteral(R"(^Vers(?:ã|a)o\s+[\d\.]+,?\s*.*$)"));
            const QRegularExpression rx_close_date(QStringLiteral(R"(^(Brasil,\s+Paran(?:á|a),\s+.*,\s+).*\.$)"));
            if (line.startsWith(QStringLiteral("Versão:")) || line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data de Vigência:"))) {
                line = QStringLiteral("Data de Vigência: %1").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Versão ")) || line.startsWith(QStringLiteral("Versao "))
                || rx_ver_date.match(line).hasMatch()) {
                line = QStringLiteral("Versão %1, %2").arg(context.pt_version, context.pt_date);
            } else {
                const QRegularExpressionMatch match = rx_close_date.match(line);
                if (match.hasMatch()) {
                    line = QStringLiteral("%1%2.").arg(match.captured(1), context.pt_date);
                }
            }
        } else if (key == QStringLiteral("clc_en") || key == QStringLiteral("eula_en")) {
            const QRegularExpression rx_ver_date(QStringLiteral(R"(^Version\s+[\d\.]+,?\s*.*$)"));
            const QRegularExpressionMatch rx_close_date_match =
                QRegularExpression(QStringLiteral(R"(^(Brazil,\s+Paran(?:á|a),\s+.*,\s+).*\.$)")).match(line);
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Effective Date:"))) {
                line = QStringLiteral("Effective Date: %1").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Version "))
                || rx_ver_date.match(line).hasMatch()) {
                line = QStringLiteral("Version %1, %2").arg(context.en_version, context.en_date);
            } else if (rx_close_date_match.hasMatch()) {
                line = QStringLiteral("%1%2.").arg(rx_close_date_match.captured(1), context.en_date);
            }
        } else if (key == QStringLiteral("privacy_pt")) {
            if (line.startsWith(QStringLiteral("Última atualização:")) || line.startsWith(QStringLiteral("Ultima atualizacao:"))) {
                line = QStringLiteral("Última atualização: %1").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Versão:")) || line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            }
        } else if (key == QStringLiteral("privacy_en")) {
            if (line.startsWith(QStringLiteral("Last updated:"))) {
                line = QStringLiteral("Last updated: %1").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            }
        } else if (key == QStringLiteral("main_root")) {
            const QRegularExpression rx_app_ver(QStringLiteral(R"(((?:QApplication::)?setApplicationVersion\((?:QStringLiteral\()?")([^"]+)("(?:\))?\);))"));
            const QRegularExpressionMatch match_app_ver = rx_app_ver.match(line);
            if (match_app_ver.hasMatch()) {
                line = match_app_ver.captured(1) + context.pt_version + match_app_ver.captured(3);
            }
        } else if (key == QStringLiteral("about_dialog")) {
            const QRegularExpression rx_dialog(QStringLiteral(R"((tr\("Versão"\)\s*\+\s*":</b>\s*)[^|]+(\s*\|\s*C\+\+20))"));
            const QRegularExpressionMatch match_dialog = rx_dialog.match(line);
            if (match_dialog.hasMatch()) {
                line = line.left(match_dialog.capturedStart(0)) + match_dialog.captured(1)
                       + context.pt_version + match_dialog.captured(2)
                       + line.mid(match_dialog.capturedEnd(0));
            }
        } else if (key == QStringLiteral("project_markdown") || key == QStringLiteral("release_doc")) {
            const QString &version = english_section ? context.en_version : context.pt_version;
            const QString &date = english_section ? context.en_date : context.pt_date;
            replace_project_version_tokens(&line, version);
            replace_markdown_date(&line, date, english_section);
        } else if (key == QStringLiteral("iss_msvc") || key == QStringLiteral("iss_unified") || key == QStringLiteral("iss_mingw")) {
            const QRegularExpression rx_iss_ver(QStringLiteral(R"((#define\s+MyAppVersion\s+")[^"]+("))"));
            const QRegularExpressionMatch match_iss_ver = rx_iss_ver.match(line);
            if (match_iss_ver.hasMatch()) {
                line = line.left(match_iss_ver.capturedStart(0)) + match_iss_ver.captured(1)
                       + context.pt_version + match_iss_ver.captured(2)
                       + line.mid(match_iss_ver.capturedEnd(0));
            }

            const QRegularExpression rx_iss_out(QStringLiteral(R"((OutputBaseFilename=.*_v)[\d\.]+(.*))"));
            const QRegularExpressionMatch match_iss_out = rx_iss_out.match(line);
            if (match_iss_out.hasMatch()) {
                line = line.left(match_iss_out.capturedStart(0)) + match_iss_out.captured(1)
                       + context.pt_version + match_iss_out.captured(2)
                       + line.mid(match_iss_out.capturedEnd(0));
            }
        }

        if (line != original) {
            changed = true;
        }
    }

    return changed;
}

QList<UpdateResult> apply_updates(const UpdateContext &context, const QString &base_dir)
{
    QList<UpdateResult> results;

    const auto specs = file_specs(base_dir);
    for (const FileSpec &spec : specs) {
        UpdateResult result;
        result.key = spec.key;
        result.group = spec.group.isEmpty() ? group_source() : spec.group;
        result.relative_path = spec.relative_path;

        const QString path = resolve_file_path(base_dir, spec.relative_path);
        const std::optional<QString> text = read_file_text(path);
        if (!text.has_value()) {
            result.ok = spec.optional;
            result.message = spec.optional ? QStringLiteral("arquivo não disponível")
                                         : QStringLiteral("arquivo inexistente");
            results.append(result);
            continue;
        }

        const QString line_ending = detect_line_ending(text.value());
        QStringList lines = split_lines_preserving_empty(text.value());
        const bool changed = apply_line_updates(spec_content_key(spec), &lines, context);

        if (!changed) {
            result.ok = spec.optional;
            result.message = spec.optional ? QStringLiteral("sem versão/data disponível para atualizar")
                                         : QStringLiteral("padrões não encontrados / sem alteração");
            results.append(result);
            continue;
        }

        QString target_path = path;
        QString target_rel_path = spec.relative_path;

        if (spec.content_key == QStringLiteral("release_doc")) {
            const QString new_rel_path = QStringLiteral("docs/release-v%1.md").arg(context.pt_version);
            const QString new_path = resolve_file_path(base_dir, new_rel_path);
            target_path = new_path;
            target_rel_path = new_rel_path;
        }

        QString error_message;
        if (write_file_text(target_path, join_lines(lines, line_ending), &error_message)) {
            if (target_path != path) {
                QFile::remove(path);
            }
            result.relative_path = target_rel_path;
            result.ok = true;
            result.message = QStringLiteral("atualizado");
        } else {
            result.ok = false;
            result.message = QStringLiteral("erro escrita: %1").arg(error_message);
        }

        results.append(result);
    }

    return results;
}

VersionEditorWidget::VersionEditorWidget(QWidget *parent) : QWidget(parent), selected_base_dir_(detect_project_root())
{
    setWindowTitle(QStringLiteral("Editor de Versões / Datas - Módulo de Dosagem de Concreto (PT/EN)"));
    resize(1020, 680);

    const QDate today = QDate::currentDate();
    const QString default_version = build_version_from_date(today);
    const int default_day = today.isValid() ? today.day() : 1;
    const int default_month_index = today.isValid() ? qMax(0, today.month() - 1) : 0;
    const int default_year = today.isValid() ? today.year() : 2026;

    const QString icon_path = find_icon_path(selected_base_dir_, QStringLiteral("concrete_mix_design.ico"));
    if (!icon_path.isEmpty()) {
        const QIcon icon(icon_path);
        QApplication::setWindowIcon(icon);
        setWindowIcon(icon);
    }

    auto *layout = new QVBoxLayout(this);

    auto *folder_layout = new QHBoxLayout();
    folder_layout->addWidget(new QLabel(QStringLiteral("Pasta dos arquivos:"), this));

    folder_path_ = new QLineEdit(selected_base_dir_, this);
    folder_path_->setReadOnly(true);
    folder_layout->addWidget(folder_path_);

    select_folder_btn_ = new QPushButton(QStringLiteral("Selecionar..."), this);
    folder_layout->addWidget(select_folder_btn_);
    layout->addLayout(folder_layout);

    auto *pt_group = new QGroupBox(QStringLiteral("Português (pt-BR)"), this);
    auto *pt_layout = new QHBoxLayout(pt_group);
    pt_layout->addWidget(new QLabel(QStringLiteral("Versão:"), pt_group));
    pt_version_ = new QLineEdit(default_version, pt_group);
    pt_layout->addWidget(pt_version_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Dia:"), pt_group));
    pt_day_ = new QSpinBox(pt_group);
    pt_day_->setRange(1, 31);
    pt_day_->setValue(default_day);
    pt_layout->addWidget(pt_day_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Mês:"), pt_group));
    pt_month_ = new QComboBox(pt_group);
    pt_month_->addItems(kPtMonths);
    pt_month_->setCurrentIndex(default_month_index);
    pt_layout->addWidget(pt_month_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Ano:"), pt_group));
    pt_year_ = new QSpinBox(pt_group);
    pt_year_->setRange(1900, 3000);
    pt_year_->setValue(default_year);
    pt_layout->addWidget(pt_year_);
    layout->addWidget(pt_group);

    auto *en_group = new QGroupBox(QStringLiteral("English (en-US)"), this);
    auto *en_layout = new QHBoxLayout(en_group);
    en_layout->addWidget(new QLabel(QStringLiteral("Version:"), en_group));
    en_version_ = new QLineEdit(default_version, en_group);
    en_layout->addWidget(en_version_);

    en_layout->addWidget(new QLabel(QStringLiteral("Day:"), en_group));
    en_day_ = new QSpinBox(en_group);
    en_day_->setRange(1, 31);
    en_day_->setValue(default_day);
    en_layout->addWidget(en_day_);

    en_layout->addWidget(new QLabel(QStringLiteral("Month:"), en_group));
    en_month_ = new QComboBox(en_group);
    en_month_->addItems(kEnMonths);
    en_month_->setCurrentIndex(default_month_index);
    en_layout->addWidget(en_month_);

    en_layout->addWidget(new QLabel(QStringLiteral("Year:"), en_group));
    en_year_ = new QSpinBox(en_group);
    en_year_->setRange(1900, 3000);
    en_year_->setValue(default_year);
    en_layout->addWidget(en_year_);
    layout->addWidget(en_group);

    auto *btn_layout = new QHBoxLayout();
    check_btn_ = new QPushButton(QStringLiteral("Verificar arquivos"), this);
    save_btn_ = new QPushButton(QStringLiteral("Salvar alterações"), this);
    refresh_btn_ = new QPushButton(QStringLiteral("Recarregar verificação"), this);

    const QString check_icon = find_icon_path(selected_base_dir_, QStringLiteral("concrete_mix_design.png"));
    if (!check_icon.isEmpty()) {
        const QIcon icon(check_icon);
        check_btn_->setIcon(icon);
        save_btn_->setIcon(icon);
        refresh_btn_->setIcon(icon);
        select_folder_btn_->setIcon(icon);
    }

    btn_layout->addWidget(check_btn_);
    btn_layout->addWidget(save_btn_);
    btn_layout->addWidget(refresh_btn_);
    layout->addLayout(btn_layout);

    status_tabs_ = new QTabWidget(this);
    status_tabs_->setDocumentMode(true);
    status_tabs_->setUsesScrollButtons(true);
    status_tabs_->setElideMode(Qt::ElideRight);
    layout->addWidget(status_tabs_, 1);

    connect(select_folder_btn_, &QPushButton::clicked, this, [this]() { selectBaseDirectory(); });
    connect(check_btn_, &QPushButton::clicked, this, [this]() { checkFiles(); });
    connect(save_btn_, &QPushButton::clicked, this, [this]() { saveChanges(); });
    connect(refresh_btn_, &QPushButton::clicked, this, [this]() { checkFiles(); });

    checkFiles();
}

void VersionEditorWidget::selectBaseDirectory()
{
    const QString selected = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Selecione a pasta onde os arquivos estão"), selected_base_dir_);
    if (selected.isEmpty()) {
        return;
    }

    selected_base_dir_ = normalize_dir(selected);
    folder_path_->setText(selected_base_dir_);
    checkFiles();
}

void VersionEditorWidget::checkFiles()
{
    const QList<FileStatus> statuses = check_expected_lines(selected_base_dir_);
    QMap<QString, QStringList> grouped_lines;
    for (const QString &group : group_order()) {
        grouped_lines[group].append(QStringLiteral("Base: %1").arg(selected_base_dir_));
        grouped_lines[group].append(QString());
    }

    for (const FileStatus &status : statuses) {
        QStringList &lines = grouped_lines[status.group];
        lines.append(QStringLiteral("%1").arg(status.relative_path));
        lines.append(QStringLiteral("  chave: %1").arg(status.key));
        lines.append(QStringLiteral("  arquivo: %1").arg(
            status.exists ? QStringLiteral("existe")
                          : (status.optional ? QStringLiteral("não disponível (opcional)")
                                             : QStringLiteral("ausente"))));

        if (status.exists) {
            if (!status.found.isEmpty()) {
                lines.append(QStringLiteral("  encontrado:"));
                for (const QString &entry : status.found) {
                    lines.append(QStringLiteral("    - %1").arg(entry));
                }
            }

            if (!status.missing.isEmpty()) {
                lines.append(status.optional ? QStringLiteral("  não disponível no conteúdo (opcional):")
                                             : QStringLiteral("  ausente no conteúdo:"));
                for (const QString &entry : status.missing) {
                    lines.append(QStringLiteral("    - %1").arg(entry));
                }
            }
        }

        lines.append(QString());
    }

    QList<QPair<QString, QStringList>> groups;
    for (const QString &group : group_order()) {
        groups.append({group, grouped_lines.value(group)});
    }
    setStatusGroups(groups);
}

void VersionEditorWidget::saveChanges()
{
    const QString pt_version = pt_version_->text().trimmed();
    const int pt_day = pt_day_->value();
    const int pt_month_index = pt_month_->currentIndex();
    const int pt_year = pt_year_->value();

    const QString en_version = en_version_->text().trimmed();
    const int en_day = en_day_->value();
    const int en_month_index = en_month_->currentIndex();
    const int en_year = en_year_->value();

    const UpdateContext context = {
        pt_version,
        QStringLiteral("%1 de %2 de %3")
            .arg(QString::number(pt_day), kPtMonthsLower.value(pt_month_index), QString::number(pt_year)),
        en_version,
        QStringLiteral("%1 %2, %3")
            .arg(kEnMonths.value(en_month_index), QString::number(en_day), QString::number(en_year)),
        readme_version_token(pt_version),
        readme_version_token(en_version),
    };

    const QList<UpdateResult> results = apply_updates(context, selected_base_dir_);

    QMap<QString, QStringList> grouped_lines;
    for (const QString &group : group_order()) {
        grouped_lines[group].append(QStringLiteral("Resultados:"));
        grouped_lines[group].append(QString());
    }

    for (const UpdateResult &result : results) {
        QStringList &lines = grouped_lines[result.group];
        lines.append(QStringLiteral("%1: %2 - %3")
                         .arg(result.relative_path,
                              result.ok ? QStringLiteral("OK") : QStringLiteral("FAIL"),
                              result.message));
    }

    QList<QPair<QString, QStringList>> groups;
    for (const QString &group : group_order()) {
        groups.append({group, grouped_lines.value(group)});
    }
    setStatusGroups(groups);
}

void VersionEditorWidget::setStatusLines(const QStringList &lines)
{
    setStatusGroups({{group_source(), lines}});
}

void VersionEditorWidget::setStatusGroups(const QList<QPair<QString, QStringList>> &groups)
{
    if (status_tabs_ == nullptr) {
        return;
    }

    const QString current_title =
        status_tabs_->currentIndex() >= 0 ? status_tabs_->tabText(status_tabs_->currentIndex()) : QString();

    while (status_tabs_->count() > 0) {
        QWidget *page = status_tabs_->widget(0);
        status_tabs_->removeTab(0);
        if (page != nullptr) {
            page->deleteLater();
        }
    }

    int target_index = 0;
    for (const auto &group : groups) {
        auto *editor = new QTextEdit(status_tabs_);
        editor->setReadOnly(true);
        editor->setLineWrapMode(QTextEdit::NoWrap);
        QStringList lines = group.second;
        if (lines.isEmpty()) {
            lines.append(QStringLiteral("Nenhum arquivo neste grupo."));
        }
        editor->setPlainText(lines.join(QLatin1Char('\n')));

        const int index = status_tabs_->addTab(editor, group.first);
        if (!current_title.isEmpty() && group.first == current_title) {
            target_index = index;
        }
    }

    if (status_tabs_->count() > 0) {
        status_tabs_->setCurrentIndex(qBound(0, target_index, status_tabs_->count() - 1));
    }
}

}  // namespace mocks::version_editor

int main(int argc, char *argv[])
{
    if (argc > 1) {
        const QString arg = QString::fromLocal8Bit(argv[1]).trimmed().toLower();
        if (arg == QStringLiteral("--check") || arg == QStringLiteral("-c") || arg == QStringLiteral("--status")) {
#if defined(Q_OS_WIN)
            if (AttachConsole(ATTACH_PARENT_PROCESS)) {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == nullptr || hOut == INVALID_HANDLE_VALUE || GetFileType(hOut) == FILE_TYPE_UNKNOWN) {
                    FILE *fp = nullptr;
                    freopen_s(&fp, "CONOUT$", "w", stdout);
                    freopen_s(&fp, "CONOUT$", "w", stderr);
                }
            }
#endif
            QTextStream out(stdout);
            const QString base_dir = (argc > 2) ? mocks::version_editor::normalize_dir(QString::fromLocal8Bit(argv[2]))
                                                : mocks::version_editor::detect_project_root();
            const auto statuses = mocks::version_editor::check_expected_lines(base_dir);
            bool all_ok = true;
            for (const auto &status : statuses) {
                if (!status.exists) {
                    if (status.optional) {
                        out << "[INDISPONÍVEL] " << status.relative_path << "\n";
                    } else {
                        all_ok = false;
                        out << "[AUSENTE] " << status.relative_path << "\n";
                    }
                } else if (!status.missing.isEmpty()) {
                    if (status.optional) {
                        out << "[SEM METADADOS] " << status.relative_path << "\n";
                    } else {
                        all_ok = false;
                        out << "[PADRAO AUSENTE] " << status.relative_path << "\n";
                    }
                } else {
                    out << "[OK] " << status.relative_path << "\n";
                }
            }
            out << (all_ok ? "TODOS OS ARQUIVOS VERIFICADOS COM SUCESSO.\n"
                           : "FALHAS NA VERIFICACAO DE ARQUIVOS.\n");
            return all_ok ? 0 : 1;
        } else if (arg == QStringLiteral("--apply") || arg == QStringLiteral("--save") || arg == QStringLiteral("--update")) {
#if defined(Q_OS_WIN)
            if (AttachConsole(ATTACH_PARENT_PROCESS)) {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == nullptr || hOut == INVALID_HANDLE_VALUE || GetFileType(hOut) == FILE_TYPE_UNKNOWN) {
                    FILE *fp = nullptr;
                    freopen_s(&fp, "CONOUT$", "w", stdout);
                    freopen_s(&fp, "CONOUT$", "w", stderr);
                }
            }
#endif
            QTextStream out(stdout);
            const QString base_dir = (argc > 2) ? mocks::version_editor::normalize_dir(QString::fromLocal8Bit(argv[2]))
                                                : mocks::version_editor::detect_project_root();
            const QDate today = QDate::currentDate();
            const QString version = (argc > 3) ? QString::fromLocal8Bit(argv[3]).trimmed()
                                               : mocks::version_editor::build_version_from_date(today);
            int day = today.day();
            int month_idx = qMax(0, today.month() - 1);
            int year = today.year();

            const QRegularExpression rx_cal_ver(QStringLiteral(R"(^(\d{4})\.(\d{1,2})\.(\d{1,2})(?:\.\d+)?$)"));
            const QRegularExpressionMatch match_cal = rx_cal_ver.match(version);
            if (match_cal.hasMatch()) {
                const int parsed_year = match_cal.captured(1).toInt();
                const int parsed_month = match_cal.captured(2).toInt();
                const int parsed_day = match_cal.captured(3).toInt();
                const QDate parsed_date(parsed_year, parsed_month, parsed_day);
                if (parsed_date.isValid()) {
                    year = parsed_year;
                    month_idx = qMax(0, parsed_month - 1);
                    day = parsed_day;
                }
            }

            const mocks::version_editor::UpdateContext ctx = {
                version,
                QStringLiteral("%1 de %2 de %3").arg(QString::number(day), mocks::version_editor::kPtMonthsLower.value(month_idx), QString::number(year)),
                version,
                QStringLiteral("%1 %2, %3").arg(mocks::version_editor::kEnMonths.value(month_idx), QString::number(day), QString::number(year)),
                mocks::version_editor::readme_version_token(version),
                mocks::version_editor::readme_version_token(version),
            };

            const auto results = mocks::version_editor::apply_updates(ctx, base_dir);
            bool all_ok = true;
            for (const auto &res : results) {
                if (!res.ok) {
                    all_ok = false;
                    out << "[FALHA] " << res.relative_path << " - " << res.message << "\n";
                } else {
                    out << "[ATUALIZADO] " << res.relative_path << "\n";
                }
            }
            out << (all_ok ? "TODOS OS ARQUIVOS ATUALIZADOS COM SUCESSO.\n"
                           : "FALHAS NA ATUALIZACAO DE ARQUIVOS.\n");
            return all_ok ? 0 : 1;
        }
    }

    QApplication app(argc, argv);
    mocks::version_editor::VersionEditorWidget widget;
    widget.show();
    return app.exec();
}
