#include "downloadtaskwidget.h"

DownloadTaskWidget::DownloadTaskWidget(const QString &title,
                                       const QString &resolution,
                                       const QString &audio,
                                       QWidget *parent)
    : QWidget{parent}
{
    setStyleSheet(R"(
        DownloadTaskWidget {
            background-color: #121212;
            border-left: 2px solid #ff003c;
            border-radius: 4px;
        }
        QLabel { background: transparent; font-size: 11px; color: #aaaaaa; }
        QLabel#titleLabel { font-size: 12px; font-weight: bold; color: #f0f0f0; }
        QLabel#statusLabel { color: #ff003c; font-size: 10px; }
        QProgressBar {
            background-color: #0a0a0a;
            border: none;
            border-radius: 3px;
            text-align: center;
            color: #f0f0f0;
            font-size: 10px;
        }
        QProgressBar::chunk { background-color: #ff003c; border-radius: 3px; }
        QPushButton {
            background: #1a1a1a;
            border: 1px solid #333;
            border-radius: 3px;
            color: #888;
            font-size: 12px;
            min-width: 20px;
            min-height: 20px;
        }
        QPushButton:hover {
            background: #ff003c;
            border-color: #ff003c;
            color: #fff;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(4);

    // Fila 1: título + botón cancelar
    QHBoxLayout *topRow = new QHBoxLayout();
    titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    titleLabel->setWordWrap(true);
    cancelButton = new QPushButton("✕", this);
    cancelButton->setFixedSize(15, 15);
    topRow->addWidget(titleLabel);
    topRow->addWidget(cancelButton);
    mainLayout->addLayout(topRow);

    // Fila 2: resolución y audio
    infoLabel = new QLabel(QString("%1  |  %2").arg(resolution, audio), this);
    mainLayout->addWidget(infoLabel);

    // Fila 3: barra de progreso con porcentaje
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFixedHeight(14);
    progressBar->setTextVisible(true);
    progressBar->setFormat("%p%");
    mainLayout->addWidget(progressBar);

    // Fila 4: estado
    statusLabel = new QLabel("En cola", this);
    statusLabel->setObjectName("statusLabel");
    mainLayout->addWidget(statusLabel);

    // Fila 5: velocidad y ETA
    QHBoxLayout *statsRow1 = new QHBoxLayout();
    speedLabel = new QLabel("Velocidad: --", this);
    etaLabel   = new QLabel("Tiempo restante: --", this);
    etaLabel->setAlignment(Qt::AlignRight);
    statsRow1->addWidget(speedLabel);
    statsRow1->addWidget(etaLabel);
    mainLayout->addLayout(statsRow1);

    // Fila 6: tamaño
    sizeLabel = new QLabel("Tamaño: --", this);
    mainLayout->addWidget(sizeLabel);

    connect(cancelButton, &QPushButton::clicked, this, &DownloadTaskWidget::cancelRequested);
}

void DownloadTaskWidget::updateProgress(int percent, const QString &speed,
                                        const QString &eta, const QString &size)
{
    progressBar->setValue(percent);
    speedLabel->setText("Velocidad: " + speed);
    etaLabel->setText("Tiempo restante: " + eta);
    sizeLabel->setText("Tamaño: " + size);
}

void DownloadTaskWidget::setStatus(const QString &status)
{
    statusLabel->setText(status);
}

void DownloadTaskWidget::setCompleted()
{
    pending = false;
    progressBar->setValue(100);
    progressBar->setStyleSheet("QProgressBar::chunk { background-color: #00cc66; border-radius: 3px; }");
    statusLabel->setText("✓ Completado");
    statusLabel->setStyleSheet("color: #00cc66;");
    cancelButton->setVisible(false);
    speedLabel->setText("");
    etaLabel->setText("");
}

void DownloadTaskWidget::setError(const QString &errorMsg)
{
    pending = false;
    statusLabel->setText("✗ Error: " + errorMsg);
    statusLabel->setStyleSheet("color: #ff003c;");
    progressBar->setStyleSheet("QProgressBar::chunk { background-color: #ff003c; border-radius: 3px; }");
    cancelButton->setVisible(false);
}

bool DownloadTaskWidget::isPending() const { return pending; }
void DownloadTaskWidget::setArgs(const QStringList &args) { downloadArgs = args; }
QStringList DownloadTaskWidget::getArgs() const { return downloadArgs; }