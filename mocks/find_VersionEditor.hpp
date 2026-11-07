#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace mocks::version_editor {

struct FileSpec {
    QString key;
    QString relative_path;
    QString content_key;
    QString group;
    // Arquivos opcionais são processados somente quando estiverem disponíveis.
    bool optional = false;
};

struct FileStatus {
    QString key;
    QString group;
    QString relative_path;
    QString path;
    bool exists = false;
    bool optional = false;
    QStringList found;
    QStringList missing;
};

struct UpdateResult {
    QString key;
    QString group;
    QString relative_path;
    bool ok = false;
    QString message;
};

struct UpdateContext {
    QString pt_version;
    QString pt_date;
    QString en_version;
    QString en_date;
    QString pt_readme_version;
    QString en_readme_version;
};

/// Retorna os arquivos monitorados e sincronizados pelo VersionEditor. Além dos
/// assets institucionais e do código-fonte, a lista inclui, quando disponíveis:
/// - CODE_OF_CONDUCT.md, CONTRIBUTING.md, MANUAL.md, OBSERVACAO.md, README.md e SECURITY.md;
/// - docs/architecture.md, docs/cli-reference.md, docs/compliance.md,
///   docs/licensing.md, docs/matriz_rastreabilidade_normativa.md, docs/release-v*.md, docs/security-model.md,
///   docs/suggested-issues.md e docs/user-guide.md; e
/// - repo/Concrete_Mix_Designer_msvc.iss, repo/Concrete_Mix_Designer.iss e
///   repo/Concrete_Mix_Designer_mingw.iss.
///
/// Arquivos opcionais ausentes ou sem metadados de versão/data são relatados sem
/// falha. Os assets source/assets/RELEASE/RELEASE NOTES_{pt_BR,en_US}.txt são
/// deliberadamente excluídos da lista e nunca são modificados por este editor.
const QList<FileSpec> &file_specs();
QList<FileSpec> file_specs(const QString &base_dir);
QList<FileStatus> check_expected_lines(const QString &base_dir);
QList<UpdateResult> apply_updates(const UpdateContext &context, const QString &base_dir);
bool apply_line_updates(const QString &key, QStringList *lines, const UpdateContext &context);
QString detect_project_root();
QString normalize_dir(const QString &path);

class VersionEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit VersionEditorWidget(QWidget *parent = nullptr);

private:
    void selectBaseDirectory();
    void checkFiles();
    void saveChanges();
    void setStatusLines(const QStringList &lines);
    void setStatusGroups(const QList<QPair<QString, QStringList>> &groups);

    QString selected_base_dir_;

    QLineEdit *folder_path_ = nullptr;

    QLineEdit *pt_version_ = nullptr;
    QSpinBox *pt_day_ = nullptr;
    QComboBox *pt_month_ = nullptr;
    QSpinBox *pt_year_ = nullptr;

    QLineEdit *en_version_ = nullptr;
    QSpinBox *en_day_ = nullptr;
    QComboBox *en_month_ = nullptr;
    QSpinBox *en_year_ = nullptr;

    QPushButton *select_folder_btn_ = nullptr;
    QPushButton *check_btn_ = nullptr;
    QPushButton *save_btn_ = nullptr;
    QPushButton *refresh_btn_ = nullptr;

    QTabWidget *status_tabs_ = nullptr;
};

}  // namespace mocks::version_editor
