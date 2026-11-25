#pragma once

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include "../core/ProjectData.hpp"

namespace ConcreteMix {

class TabTechnicalReport : public QWidget {
    Q_OBJECT

public:
    explicit TabTechnicalReport(ProjectData *data, QWidget *parent = nullptr);

    void refreshReport();
    void retranslateUi();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void exportPdf();
    void exportMarkdown();
    void exportHtml();

    ProjectData *m_data = nullptr;
    QPushButton *m_btnCopy = nullptr;
    QPushButton *m_btnMd = nullptr;
    QPushButton *m_btnHtml = nullptr;
    QPushButton *m_btnPdf = nullptr;
    QTextBrowser *m_textBrowser = nullptr;
};

} // namespace ConcreteMix
