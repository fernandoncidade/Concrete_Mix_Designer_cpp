#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <vector>
#include "../core/MixDesignCalculator.hpp"

namespace ConcreteMix {

class IptUnifiedDiagramWidget : public QWidget {
    Q_OBJECT

public:
    explicit IptUnifiedDiagramWidget(QWidget *parent = nullptr);

    void setData(const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& mixes);
    void setTheme(bool isDark);
    bool exportImage(const QString& filePath, int targetWidth = 1920, int targetHeight = 1080);
    void copyToClipboard();
    void retranslateUi();

protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void drawDiagram(QPainter &painter, const QRect &rect);

    bool m_hasData = false;
    bool m_isDark = false;
    MixDesignResult m_result;
    std::vector<ExperimentalMixRecord> m_mixes;
};

} // namespace ConcreteMix
