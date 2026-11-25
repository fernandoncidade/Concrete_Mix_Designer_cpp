#include "TabExperimentalMixes.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QEvent>
#include <QStyledItemDelegate>
#include <QPainter>

namespace ConcreteMix {

class InputCellDelegate : public QStyledItemDelegate {
public:
    explicit InputCellDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent), m_isDark(false) {}

    void setDarkMode(bool dark) { m_isDark = dark; }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        const bool isEditable = (index.flags() & Qt::ItemIsEditable);
        const bool isSelected = (option.state & QStyle::State_Selected);
        const bool hasFocus   = (option.state & QStyle::State_HasFocus);
        const bool isHover    = (option.state & QStyle::State_MouseOver);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        if (isEditable) {
            // Fundo discreto da célula de entrada (mesma paleta dos campos de entrada)
            QColor bgColor = m_isDark ? QColor(0x2b, 0x2b, 0x2b) : QColor(0xff, 0xff, 0xff);
            if (isSelected) {
                bgColor = m_isDark ? QColor(0x2f, 0x3d, 0x4c) : QColor(0xdc, 0xe8, 0xf5);
            }
            painter->fillRect(option.rect, bgColor);

            // Moldura fina (1px), nítida e sóbria — alinhada a meio pixel para não borrar
            QColor borderColor = m_isDark ? QColor(0x5f, 0x7f, 0x9e) : QColor(0x6a, 0x8b, 0xb5);
            if (hasFocus) {
                borderColor = m_isDark ? QColor(0x8f, 0xb3, 0xd6) : QColor(0x2f, 0x5f, 0x9a);
            } else if (isHover) {
                borderColor = m_isDark ? QColor(0x7a, 0x9a, 0xba) : QColor(0x4f, 0x74, 0xa3);
            }
            const QRectF borderRect = QRectF(option.rect).adjusted(2.5, 2.5, -2.5, -2.5);
            painter->setPen(QPen(borderColor, 1.0));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(borderRect, 3.0, 3.0);
        } else {
            // Célula calculada / somente leitura: plana, sem moldura
            QColor roBg = m_isDark ? QColor(0x25, 0x25, 0x25) : QColor(0xf0, 0xf0, 0xf0);
            if (isSelected) {
                roBg = m_isDark ? QColor(0x33, 0x33, 0x33) : QColor(0xe0, 0xe0, 0xe0);
            }
            painter->fillRect(option.rect, roBg);
        }
        painter->restore();

        // O fundo já foi pintado acima; o estilo base desenha apenas o texto
        QStyleOptionViewItem opt = option;
        opt.state &= ~(QStyle::State_Selected | QStyle::State_HasFocus);
        opt.backgroundBrush = Qt::NoBrush;
        if (isEditable) {
            opt.palette.setColor(QPalette::Text, m_isDark ? QColor(0xf0, 0xf0, 0xf0) : QColor(0x1a, 0x1a, 0x1a));
        } else {
            opt.palette.setColor(QPalette::Text, m_isDark ? QColor(0x9a, 0x9a, 0x9a) : QColor(0x5c, 0x5c, 0x5c));
        }
        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    bool m_isDark = false;
};

TabExperimentalMixes::TabExperimentalMixes(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    updateUIFromData();
}

void TabExperimentalMixes::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // =========================================================================
    // LADO A LADO NA HORIZONTAL: 1. Dados dos 3 Traços + 2. Equações Ajustadas
    // =========================================================================
    auto topHBox = new QHBoxLayout();
    topHBox->setSpacing(8);

    // 1. Tabela dos 3 Traços Experimentais
    m_groupInput = new QGroupBox(tr("1. Dados dos 3 Traços Experimentais Confeccionados (Laboratório)"), this);
    auto vInput = new QVBoxLayout(m_groupInput);
    vInput->setContentsMargins(8, 8, 8, 8);
    vInput->setSpacing(6);

    m_tableMixes = new QTableWidget(3, 7, m_groupInput);
    m_cellDelegate = new InputCellDelegate(this);
    m_tableMixes->setItemDelegate(m_cellDelegate);
    m_tableMixes->setMouseTracking(true);
    m_tableMixes->viewport()->setAttribute(Qt::WA_Hover, true);
    m_tableMixes->setHorizontalHeaderLabels({
        tr("Identificação"), tr("Traço m (1:m)"), tr("Relação a/c"),
        tr("fcj 28d (MPa)"), tr("Slump (mm)"), tr("M.E. Fresco (kg/dm³)"), tr("Consumo C (kg/m³)")
    });
    m_tableMixes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableMixes->verticalHeader()->setVisible(false);
    m_tableMixes->setFixedHeight(120);
    vInput->addWidget(m_tableMixes);

    auto hBtn = new QHBoxLayout();
    m_btnRecalculate = new QPushButton(tr("⚡ Ajustar Curvas e Regressões dos 4 Quadrantes"), m_groupInput);
    connect(m_btnRecalculate, &QPushButton::clicked, this, [this]() {
        calculateAndRefresh(true);
    });
    hBtn->addStretch();
    hBtn->addWidget(m_btnRecalculate);
    vInput->addLayout(hBtn);

    topHBox->addWidget(m_groupInput, 3);

    // 2. Grupo de Equações Ajustadas e Resultados da Otimização
    m_groupResults = new QGroupBox(tr("2. Equações Matemáticas Ajustadas e Valores do Traço de Projeto"), this);
    auto gridRes = new QGridLayout(m_groupResults);
    gridRes->setContentsMargins(8, 8, 8, 8);
    gridRes->setSpacing(6);

    QFont fEq("Consolas");
    fEq.setBold(true);

    m_lblAbramsTitle = new QLabel(tr("<b>1º Quad (Abrams):</b>"), m_groupResults);
    gridRes->addWidget(m_lblAbramsTitle, 0, 0);
    m_lblAbramsEq = new QLabel("-", m_groupResults);
    m_lblAbramsEq->setFont(fEq);
    gridRes->addWidget(m_lblAbramsEq, 0, 1);

    m_lblLyseTitle = new QLabel(tr("<b>2º Quad (Lyse):</b>"), m_groupResults);
    gridRes->addWidget(m_lblLyseTitle, 1, 0);
    m_lblLyseEq = new QLabel("-", m_groupResults);
    m_lblLyseEq->setFont(fEq);
    gridRes->addWidget(m_lblLyseEq, 1, 1);

    m_lblMolinariTitle = new QLabel(tr("<b>3º Quad (Molinari):</b>"), m_groupResults);
    gridRes->addWidget(m_lblMolinariTitle, 2, 0);
    m_lblMolinariEq = new QLabel("-", m_groupResults);
    m_lblMolinariEq->setFont(fEq);
    gridRes->addWidget(m_lblMolinariEq, 2, 1);

    auto makeBadge = [](QLabel*& titleLbl, const QString& titleText, QLabel*& valLbl, QWidget* parent) -> QWidget* {
        auto w = new QFrame(parent);
        w->setFrameShape(QFrame::StyledPanel);
        w->setFrameShadow(QFrame::Plain);
        auto l = new QVBoxLayout(w);
        l->setContentsMargins(6, 4, 6, 4);
        l->setSpacing(2);
        titleLbl = new QLabel(titleText, w);
        QFont fTitle = titleLbl->font();
        fTitle.setBold(true);
        titleLbl->setFont(fTitle);
        valLbl = new QLabel("-", w);
        QFont fVal = valLbl->font();
        fVal.setBold(true);
        fVal.setPointSize(fVal.pointSize() + 1);
        valLbl->setFont(fVal);
        l->addWidget(titleLbl);
        l->addWidget(valLbl);
        return w;
    };

    gridRes->addWidget(makeBadge(m_lblBadgeAcTitle, tr("Relação a/c Ótima"), m_lblOptimalAc, m_groupResults), 0, 2);
    gridRes->addWidget(makeBadge(m_lblBadgeMTitle, tr("Traço Seco Total m"), m_lblOptimalM, m_groupResults), 1, 2);
    gridRes->addWidget(makeBadge(m_lblBadgeCTitle, tr("Consumo C (kg/m³)"), m_lblOptimalC, m_groupResults), 2, 2);

    topHBox->addWidget(m_groupResults, 2);
    mainLayout->addLayout(topHBox);

    // =========================================================================
    // 3. Gráficos Interativos (QuadChartWidget) - Ocupando todo o espaço inferior
    // =========================================================================
    m_quadCharts = new QuadChartWidget(this);
    mainLayout->addWidget(m_quadCharts, 1);

    connect(m_tableMixes, &QTableWidget::cellChanged, this, [this](int row, int col) {
        if (col == 1 || col == 2 || col == 5) {
            auto itM = m_tableMixes->item(row, 1);
            auto itAc = m_tableMixes->item(row, 2);
            auto itDens = m_tableMixes->item(row, 5);
            if (itM && itAc && itDens) {
                double mVal = itM->text().toDouble();
                double acVal = itAc->text().toDouble();
                double densVal = itDens->text().toDouble();
                double denom = 1.0 + mVal + acVal;
                if (denom > 0.0 && densVal > 0.0) {
                    double cVal = (densVal / denom) * 1000.0;
                    m_tableMixes->blockSignals(true);
                    auto item6 = new QTableWidgetItem(QString::asprintf("%.2f", cVal));
                    item6->setFlags(item6->flags() & ~Qt::ItemIsEditable);
                    m_tableMixes->setItem(row, 6, item6);
                    m_tableMixes->blockSignals(false);
                }
            }
        }
        calculateAndRefresh(true);
    });

}

void TabExperimentalMixes::updateUIFromData() {
    if (!m_data) return;

    m_tableMixes->blockSignals(true);
    for (int r = 0; r < 3; ++r) {
        if (r < static_cast<int>(m_data->experimentalMixes.size())) {
            const auto& mix = m_data->experimentalMixes[r];
            if (mix.dryRatioM <= 0.0 && mix.waterCementRatio <= 0.0 && mix.freshDensity <= 0.0) {
                m_tableMixes->setItem(r, 0, new QTableWidgetItem(mix.label.empty() ? "" : QString::fromStdString(mix.label)));
                m_tableMixes->setItem(r, 1, new QTableWidgetItem(""));
                m_tableMixes->setItem(r, 2, new QTableWidgetItem(""));
                m_tableMixes->setItem(r, 3, new QTableWidgetItem(""));
                m_tableMixes->setItem(r, 4, new QTableWidgetItem(""));
                m_tableMixes->setItem(r, 5, new QTableWidgetItem(""));
                auto item6 = new QTableWidgetItem("-");
                item6->setFlags(item6->flags() & ~Qt::ItemIsEditable);
                m_tableMixes->setItem(r, 6, item6);
            } else {
                m_tableMixes->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(mix.label)));
                m_tableMixes->setItem(r, 1, new QTableWidgetItem(QString::asprintf("%.2f", mix.dryRatioM)));
                m_tableMixes->setItem(r, 2, new QTableWidgetItem(QString::asprintf("%.4f", mix.waterCementRatio)));
                m_tableMixes->setItem(r, 3, new QTableWidgetItem(QString::asprintf("%.1f", mix.compressiveStrength28d)));
                m_tableMixes->setItem(r, 4, new QTableWidgetItem(QString::asprintf("%.0f", mix.slumpMm)));
                m_tableMixes->setItem(r, 5, new QTableWidgetItem(QString::asprintf("%.3f", mix.freshDensity)));
                auto item6 = new QTableWidgetItem(QString::asprintf("%.2f", mix.cementConsumption));
                item6->setFlags(item6->flags() & ~Qt::ItemIsEditable);
                m_tableMixes->setItem(r, 6, item6);
            }
        }
    }
    m_tableMixes->blockSignals(false);

    calculateAndRefresh();
}

void TabExperimentalMixes::updateDataFromUI() {
    if (!m_data) return;

    m_data->experimentalMixes.clear();
    for (int r = 0; r < 3; ++r) {
        ExperimentalMixRecord mix;
        auto it0 = m_tableMixes->item(r, 0);
        auto it1 = m_tableMixes->item(r, 1);
        auto it2 = m_tableMixes->item(r, 2);
        auto it3 = m_tableMixes->item(r, 3);
        auto it4 = m_tableMixes->item(r, 4);
        auto it5 = m_tableMixes->item(r, 5);
        auto it6 = m_tableMixes->item(r, 6);

        if (it0) mix.label = it0->text().toStdString();
        if (it1) mix.dryRatioM = it1->text().toDouble();
        if (it2) mix.waterCementRatio = it2->text().toDouble();
        if (it3) mix.compressiveStrength28d = it3->text().toDouble();
        if (it4) mix.slumpMm = it4->text().toDouble();
        if (it5) mix.freshDensity = it5->text().toDouble();

        const double denom = 1.0 + mix.dryRatioM + mix.waterCementRatio;
        if (mix.freshDensity > 0.0 && denom > 0.0) {
            mix.cementConsumption = (mix.freshDensity / denom) * 1000.0;
        } else if (it6) {
            mix.cementConsumption = it6->text().toDouble();
        }

        m_data->experimentalMixes.push_back(mix);
    }
}

void TabExperimentalMixes::calculateAndRefresh(bool emitSignal) {
    updateDataFromUI();
    if (!m_data) return;

    auto res = m_data->calculate();

    if (res.targetWaterCement <= 0.0 || res.targetDryMixM <= 0.0 || res.targetCementConsumption <= 0.0) {
        m_lblAbramsEq->setText("-");
        m_lblLyseEq->setText("-");
        m_lblMolinariEq->setText("-");
        m_lblOptimalAc->setText("-");
        m_lblOptimalM->setText("-");
        m_lblOptimalC->setText("-");
    } else {
        m_lblAbramsEq->setText(QString::fromStdString(res.abramsPoly.toEquationString("fcj", "a/c")));
        m_lblLyseEq->setText(QString::fromStdString(res.lyseLinear.toEquationString("m", "a/c")));
        m_lblMolinariEq->setText(QString::fromStdString(res.molinariPoly.toEquationString("m", "C")));

        m_lblOptimalAc->setText(QString::asprintf("%.3f", res.targetWaterCement));
        m_lblOptimalM->setText(QString::asprintf("%.2f", res.targetDryMixM));
        m_lblOptimalC->setText(QString::asprintf("%.1f kg/m³", res.targetCementConsumption));
    }

    m_quadCharts->updateCharts(res, m_data->experimentalMixes);
    if (emitSignal) {
        emit experimentalMixesChanged();
    }
}

void TabExperimentalMixes::retranslateUi() {
    if (m_groupInput) {
        m_groupInput->setTitle(tr("1. Dados dos 3 Traços Experimentais Confeccionados (Laboratório)"));
    }
    if (m_tableMixes) {
        m_tableMixes->setHorizontalHeaderLabels({
            tr("Identificação"), tr("Traço m (1:m)"), tr("Relação a/c"),
            tr("fcj 28d (MPa)"), tr("Slump (mm)"), tr("M.E. Fresco (kg/dm³)"), tr("Consumo C (kg/m³)")
        });
    }
    if (m_btnRecalculate) {
        m_btnRecalculate->setText(tr("⚡ Ajustar Curvas e Regressões dos 4 Quadrantes"));
    }
    if (m_groupResults) {
        m_groupResults->setTitle(tr("2. Equações Matemáticas Ajustadas e Valores do Traço de Projeto"));
    }
    if (m_lblAbramsTitle) {
        m_lblAbramsTitle->setText(tr("<b>1º Quad (Abrams):</b>"));
    }
    if (m_lblLyseTitle) {
        m_lblLyseTitle->setText(tr("<b>2º Quad (Lyse):</b>"));
    }
    if (m_lblMolinariTitle) {
        m_lblMolinariTitle->setText(tr("<b>3º Quad (Molinari):</b>"));
    }
    if (m_lblBadgeAcTitle) {
        m_lblBadgeAcTitle->setText(tr("Relação a/c Ótima"));
    }
    if (m_lblBadgeMTitle) {
        m_lblBadgeMTitle->setText(tr("Traço Seco Total m"));
    }
    if (m_lblBadgeCTitle) {
        m_lblBadgeCTitle->setText(tr("Consumo C (kg/m³)"));
    }

    if (m_quadCharts) {
        m_quadCharts->retranslateUi();
    }

    if (m_data) {
        auto res = m_data->calculate();
        if (res.targetWaterCement <= 0.0 || res.targetDryMixM <= 0.0 || res.targetCementConsumption <= 0.0) {
            m_lblAbramsEq->setText("-");
            m_lblLyseEq->setText("-");
            m_lblMolinariEq->setText("-");
        } else {
            m_lblAbramsEq->setText(QString::fromStdString(res.abramsPoly.toEquationString("fcj", "a/c")));
            m_lblLyseEq->setText(QString::fromStdString(res.lyseLinear.toEquationString("m", "a/c")));
            m_lblMolinariEq->setText(QString::fromStdString(res.molinariPoly.toEquationString("m", "C")));
        }
    }
}

void TabExperimentalMixes::updateTheme(bool isDark) {
    if (m_cellDelegate) {
        m_cellDelegate->setDarkMode(isDark);
        if (m_tableMixes && m_tableMixes->viewport()) {
            m_tableMixes->viewport()->update();
        }
    }
    if (m_quadCharts) {
        m_quadCharts->updateTheme(isDark);
    }
}

void TabExperimentalMixes::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

} // namespace ConcreteMix
