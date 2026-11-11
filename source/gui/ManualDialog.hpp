#pragma once

#include <QDialog>
#include <QString>
#include <QHash>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QTextBrowser;
class QToolButton;

namespace ConcreteMix::Gui {

/**
 * @brief Diálogo interativo modeless do Manual do Usuário (PT-BR / EN-US).
 * Documenta a navegação pelos módulos de cálculo, o submenu Configurações > Idioma,
 * a seleção de Cenários da Planilha e as diretrizes de finalidade estritamente
 * acadêmica, conferência manual obrigatória e isenção de responsabilidade legal.
 */
class ManualDialog : public QDialog {
    Q_OBJECT
public:
    explicit ManualDialog(QWidget* parent = nullptr);

    void changeEvent(QEvent* event) override;
    void retranslateUi();

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchNext();
    void onSearchPrevious();
    void onTocItemClicked(QListWidgetItem* item);

private:
    void setupUi();
    void loadManualContent();
    void performSearch(int direction);

    QLabel* searchLabel_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    QToolButton* searchPrevBtn_ = nullptr;
    QToolButton* searchNextBtn_ = nullptr;
    QSplitter* splitter_ = nullptr;
    QListWidget* tocList_ = nullptr;
    QTextBrowser* browser_ = nullptr;
    QPushButton* closeButton_ = nullptr;

    QHash<QString, QString> sectionAnchors_;
};

} // namespace ConcreteMix::Gui
