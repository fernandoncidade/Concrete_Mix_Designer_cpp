#include "TabMortarContent.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QEvent>

namespace ConcreteMix {

TabMortarContent::TabMortarContent(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    updateUIFromData();
}

void TabMortarContent::setupUI() {
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scrollArea);
    auto mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    // 1. Grupo de Configurações do Ensaio Piloto
    m_groupConfig = new QGroupBox(container);
    auto hLayout = new QHBoxLayout(m_groupConfig);
    hLayout->setSpacing(12);

    m_lblPilotMTitle = new QLabel(m_groupConfig);
    hLayout->addWidget(m_lblPilotMTitle);
    m_spinPilotM = new QDoubleSpinBox(m_groupConfig);
    m_spinPilotM->setRange(0.0, 15.0);
    m_spinPilotM->setValue(5.0); // Padrão clássico IPT 1:5
    hLayout->addWidget(m_spinPilotM);

    m_lblBatchGravelTitle = new QLabel(m_groupConfig);
    hLayout->addWidget(m_lblBatchGravelTitle);
    m_spinBatchGravel = new QDoubleSpinBox(m_groupConfig);
    m_spinBatchGravel->setRange(0.0, 500.0);
    m_spinBatchGravel->setValue(43.2);
    hLayout->addWidget(m_spinBatchGravel);

    m_lblBatchWaterTitle = new QLabel(m_groupConfig);
    hLayout->addWidget(m_lblBatchWaterTitle);
    m_spinBatchWater = new QDoubleSpinBox(m_groupConfig);
    m_spinBatchWater->setRange(0.0, 100.0);
    m_spinBatchWater->setValue(6.0);
    hLayout->addWidget(m_spinBatchWater);

    hLayout->addStretch();

    m_lblSelectedAlphaTitle = new QLabel(m_groupConfig);
    hLayout->addWidget(m_lblSelectedAlphaTitle);
    m_spinSelectedAlpha = new QDoubleSpinBox(m_groupConfig);
    m_spinSelectedAlpha->setRange(0.0, 80.0);
    m_spinSelectedAlpha->setSuffix(" %");
    m_spinSelectedAlpha->setValue(52.0);
    QFont fAlpha = m_spinSelectedAlpha->font();
    fAlpha.setBold(true);
    fAlpha.setPointSize(fAlpha.pointSize() + 2);
    m_spinSelectedAlpha->setFont(fAlpha);
    hLayout->addWidget(m_spinSelectedAlpha);

    mainLayout->addWidget(m_groupConfig);

    // 2. Tabela de Variação Experimental de Alfa
    m_groupTable = new QGroupBox(container);
    auto vTableLayout = new QVBoxLayout(m_groupTable);

    m_tableAlpha = new QTableWidget(15, 10, m_groupTable);
    m_tableAlpha->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableAlpha->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableAlpha->verticalHeader()->setVisible(false);
    m_tableAlpha->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableAlpha->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableAlpha->setMinimumHeight(240);
    m_tableAlpha->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vTableLayout->addWidget(m_tableAlpha, 1);

    m_lblSelectedSummary = new QLabel(m_groupTable);
    QFont fSummary = m_lblSelectedSummary->font();
    fSummary.setBold(true);
    fSummary.setPointSize(fSummary.pointSize() + 1);
    m_lblSelectedSummary->setFont(fSummary);
    m_lblSelectedSummary->setFrameShape(QFrame::StyledPanel);
    m_lblSelectedSummary->setFrameShadow(QFrame::Plain);
    m_lblSelectedSummary->setMargin(8);
    vTableLayout->addWidget(m_lblSelectedSummary);

    m_lblHelp = new QLabel(m_groupTable);
    vTableLayout->addWidget(m_lblHelp);

    m_groupTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_groupTable, 1);

    scrollArea->setWidget(container);
    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(scrollArea);

    retranslateUi();

    // Conexões
    connect(m_spinPilotM, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) {
            m_data->pilotM = val;
            m_data->updatePilotBatchFromMixerVolume();
            m_spinBatchGravel->blockSignals(true);
            m_spinBatchWater->blockSignals(true);
            m_spinBatchGravel->setValue(m_data->pilotBatchGravelKg);
            m_spinBatchWater->setValue(m_data->pilotBatchWaterKg);
            m_spinBatchGravel->blockSignals(false);
            m_spinBatchWater->blockSignals(false);
        }
        updateTable();
        emit mortarContentChanged();
    });
    connect(m_spinBatchGravel, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) m_data->pilotBatchGravelKg = val;
        updateTable();
        emit mortarContentChanged();
    });
    connect(m_spinBatchWater, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) m_data->pilotBatchWaterKg = val;
        updateTable();
        emit mortarContentChanged();
    });
    connect(m_spinSelectedAlpha, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) m_data->selectedAlphaPercent = val;
        updateTable();
        emit mortarContentChanged();
    });

    connect(m_tableAlpha, &QTableWidget::cellClicked, this, [this](int row, int) {
        auto item = m_tableAlpha->item(row, 0);
        if (item) {
            double alphaVal = item->text().replace("%", "").trimmed().toDouble();
            if (alphaVal > 0.0) {
                m_spinSelectedAlpha->blockSignals(true);
                m_spinSelectedAlpha->setValue(alphaVal);
                m_spinSelectedAlpha->blockSignals(false);
                if (m_data) m_data->selectedAlphaPercent = alphaVal;
                updateTable();
                emit mortarContentChanged();
            }
        }
    });

    updateTable();
}

void TabMortarContent::updateTable() {
    double m = m_spinPilotM->value();
    double bMass = m_spinBatchGravel->value();
    double wMass = m_spinBatchWater->value();
    double selAlpha = m_spinSelectedAlpha->value();

    if (m <= 0.0 || selAlpha <= 0.0) {
        m_tableAlpha->clearContents();
        m_lblSelectedSummary->setText(tr("Teor de Argamassa: - | Aguardando dados do usuário"));
        return;
    }

    auto rows = MortarContentOptimizer::generateTable(m, bMass, wMass, selAlpha);
    const bool isDark = (palette().color(QPalette::Window).lightness() < 128);
    const QColor selBg = isDark ? QColor(30, 70, 32) : QColor(212, 237, 218);
    const QColor selFg = isDark ? QColor(144, 238, 144) : QColor(21, 87, 36);

    for (int r = 0; r < static_cast<int>(rows.size()); ++r) {
        const auto& row = rows[r];

        auto makeItem = [selBg, selFg](const QString& txt, bool isSelected) {
            auto item = new QTableWidgetItem(txt);
            item->setTextAlignment(Qt::AlignCenter);
            if (isSelected) {
                item->setBackground(selBg);
                item->setForeground(selFg);
                item->setFont(QFont("Segoe UI", 9, QFont::Bold));
            }
            return item;
        };

        QString alphaTxt = QString::asprintf("%.0f %%", row.alphaPercent);
        if (row.isSelected) alphaTxt += " ★";

        m_tableAlpha->setItem(r, 0, makeItem(alphaTxt, row.isSelected));
        m_tableAlpha->setItem(r, 1, makeItem("1", row.isSelected));
        m_tableAlpha->setItem(r, 2, makeItem(QString::asprintf("%.2f", row.sandRatio), row.isSelected));
        m_tableAlpha->setItem(r, 3, makeItem(QString::asprintf("%.2f", row.gravelRatio), row.isSelected));
        m_tableAlpha->setItem(r, 4, makeItem(QString::asprintf("%.2f", row.totalSandMass), row.isSelected));
        m_tableAlpha->setItem(r, 5, makeItem(r == 0 ? "-" : QString::asprintf("+%.2f", row.incrementalSand), row.isSelected));
        m_tableAlpha->setItem(r, 6, makeItem(QString::asprintf("%.2f", row.totalCementMass), row.isSelected));
        m_tableAlpha->setItem(r, 7, makeItem(r == 0 ? "-" : QString::asprintf("+%.2f", row.incrementalCement), row.isSelected));
        m_tableAlpha->setItem(r, 8, makeItem(QString::asprintf("%.2f", row.totalWaterMass), row.isSelected));
        m_tableAlpha->setItem(r, 9, makeItem(QString::asprintf("%.2f", row.totalGravelMass), row.isSelected));
    }

    double sandRatio = MortarContentOptimizer::calculateSandRatio(selAlpha, m);
    double gravelRatio = m - sandRatio;
    m_lblSelectedSummary->setText(
        tr("Teor de Argamassa Selecionado: α = %1% | Traço Piloto Correspondente: 1 : %2 : %3")
        .arg(selAlpha, 0, 'f', 1)
        .arg(sandRatio, 0, 'f', 2)
        .arg(gravelRatio, 0, 'f', 2)
    );
}

void TabMortarContent::updateUIFromData() {
    if (!m_data) return;

    if (!m_spinPilotM->hasFocus()) {
        m_spinPilotM->blockSignals(true);
        m_spinPilotM->setValue(m_data->pilotM);
        m_spinPilotM->blockSignals(false);
    }

    if (!m_spinSelectedAlpha->hasFocus()) {
        m_spinSelectedAlpha->blockSignals(true);
        m_spinSelectedAlpha->setValue(m_data->selectedAlphaPercent);
        m_spinSelectedAlpha->blockSignals(false);
    }

    if (!m_spinBatchGravel->hasFocus()) {
        m_spinBatchGravel->blockSignals(true);
        m_spinBatchGravel->setValue(m_data->pilotBatchGravelKg);
        m_spinBatchGravel->blockSignals(false);
    }

    if (!m_spinBatchWater->hasFocus()) {
        m_spinBatchWater->blockSignals(true);
        m_spinBatchWater->setValue(m_data->pilotBatchWaterKg);
        m_spinBatchWater->blockSignals(false);
    }

    updateTable();
}

void TabMortarContent::updateDataFromUI() {
    if (!m_data) return;
    m_data->pilotM = m_spinPilotM->value();
    m_data->pilotBatchGravelKg = m_spinBatchGravel->value();
    m_data->pilotBatchWaterKg = m_spinBatchWater->value();
    m_data->selectedAlphaPercent = m_spinSelectedAlpha->value();
}

void TabMortarContent::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    } else if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        updateTable();
    }
    QWidget::changeEvent(event);
}

void TabMortarContent::retranslateUi() {
    if (m_groupConfig) m_groupConfig->setTitle(tr("1. Parâmetros do Ensaio Piloto de Teor de Argamassa (Método IPT)"));
    if (m_groupTable) m_groupTable->setTitle(tr("2. Sequência de Adições Experimentais na Betoneira (40% a 68%)"));

    if (m_lblPilotMTitle) m_lblPilotMTitle->setText(tr("Traço Piloto (m = a + p):"));
    if (m_lblBatchGravelTitle) m_lblBatchGravelTitle->setText(tr("Massa Fixa de Brita (kg):"));
    if (m_lblBatchWaterTitle) m_lblBatchWaterTitle->setText(tr("Massa de Água (kg):"));
    if (m_lblSelectedAlphaTitle) m_lblSelectedAlphaTitle->setText(tr("<b>Teor Ideal Adotado α (%):</b>"));

    if (m_lblHelp) {
        m_lblHelp->setText(tr("<i>Dica técnica: Selecione qualquer linha da tabela para definir o teor de argamassa (α) do estudo de dosagem. O teor de argamassa ideal confere trabalhabilidade satisfatória ao concreto sem exsudação e sem excesso de pasta de cimento.</i>"));
    }

    if (m_tableAlpha) {
        m_tableAlpha->setHorizontalHeaderLabels({
            tr("α (%)"), tr("c"), tr("a (Areia)"), tr("b (Brita)"),
            tr("Areia Total (kg)"), tr("Acréscimo Areia (kg)"),
            tr("Cimento Total (kg)"), tr("Acréscimo Cimento (kg)"),
            tr("Água Total (kg)"), tr("Brita Fixa (kg)")
        });
    }

    updateTable();
}

} // namespace ConcreteMix
