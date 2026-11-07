#pragma once

#include <QDialog>
#include <QString>
#include <QVector>
#include <QPair>

class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTextBrowser;
class QToolButton;

namespace ConcreteMix::Gui {

class SobreDialog : public QDialog {
    Q_OBJECT
public:
    explicit SobreDialog(QWidget* parent = nullptr);

    void changeEvent(QEvent* event) override;
    void retranslateUi();

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchNext();
    void onSearchPrevious();
    void onTabChanged(int index);

private:
    void setupUi();
    void loadContents();
    void configureBrowser(QTextBrowser* browser);
    void performSearch(int direction);
    QString loadAssetFile(const QString& relativePath);

    QLabel* headerIconLabel_ = nullptr;
    QLabel* headerTextLabel_ = nullptr;
    QLabel* searchLabel_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    QToolButton* searchPrevBtn_ = nullptr;
    QToolButton* searchNextBtn_ = nullptr;
    QTabWidget* tabWidget_ = nullptr;
    QPushButton* closeButton_ = nullptr;

    QTextBrowser* browserDetails_ = nullptr;
    QTextBrowser* browserHistory_ = nullptr;
    QTextBrowser* browserLicense_ = nullptr;
    QTextBrowser* browserNotices_ = nullptr;
    QTextBrowser* browserPrivacy_ = nullptr;
    QTextBrowser* browserReleaseNotes_ = nullptr;

    QTextBrowser* currentBrowser_ = nullptr;
};

} // namespace ConcreteMix::Gui
