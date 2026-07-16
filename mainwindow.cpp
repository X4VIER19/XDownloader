#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadtaskwidget.h"
#include <QAbstractButton>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QPixmap>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("XDownloader");
    currentTaskIndex = -1;

    // ── Inicializar procesos y managers ──
    ytdlpProcess   = new QProcess(this);
    downloadProcess = new QProcess(this);
    networkManager  = new QNetworkAccessManager(this);

    // ── Ruta de destino por defecto ──
    QString defaultDest = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/XDownloader";
    QDir().mkpath(defaultDest);
    ui->destLineEdit->setText(defaultDest);

    // ── Layout de la cola ──
    queueLayout = qobject_cast<QVBoxLayout*>(ui->queueContentWidget->layout());
    queueLayout->setContentsMargins(8, 8, 8, 8);
    queueLayout->setSpacing(8);
    queueLayout->addStretch();

    // ── Estilos ──
    setStyleSheet(R"(
        QMainWindow { background-color: #050505; }
        QWidget { background-color: transparent; color: #f0f0f0; font-family: 'Segoe UI', sans-serif; }
        QFrame#sidebarFrame  { background-color: #050505; border: none; border-right: 1px solid #222; }
        QFrame#headerFrame   { background-color: #0a0a0a; border: none; border-bottom: 1px solid #222; }
        QFrame#contentFrame  { background-color: #050505; border: none; }
        QFrame#workspaceFrame { background-color: #050505; border: none; }
        QFrame#queueFrame    { background-color: #0a0a0a; border: none; border-left: 1px solid #222; }
        QFrame#mediaInfoFrame, QFrame#formatFrame, QFrame#optionsFrame {
            background-color: #121212; border: 1px solid #222; border-radius: 6px; margin: 6px;
        }
        QFrame#modeFrame { background-color: transparent; border: none; margin: 6px; }
        QScrollArea { background-color: #050505; border: none; }
        QScrollArea > QWidget > QWidget { background-color: #050505; }
        QScrollBar:vertical { background: #050505; width: 8px; }
        QScrollBar::handle:vertical { background: #333; border-radius: 4px; }
        QScrollBar::handle:vertical:hover { background: #ff003c; }
        QComboBox { background-color: #0a0a0a; border: 1px solid #333; border-radius: 4px; padding: 4px 8px; color: #f0f0f0; }
        QComboBox::drop-down { border: none; width: 20px; }
        QComboBox QAbstractItemView { background-color: #121212; border: 1px solid #333; selection-background-color: #1a1a1a; }
        QLineEdit { background-color: #0a0a0a; border: 1px solid #333; border-radius: 4px; padding: 4px 8px; color: #f0f0f0; }
        QLineEdit:focus { border-color: #ff003c; }
        QPushButton { background-color: #1a1a1a; border: 1px solid #333; border-radius: 4px; padding: 4px 12px; color: #f0f0f0; }
        QPushButton:hover { background-color: #222; border-color: #555; }
        QPushButton#analyzeButton { background-color: transparent; border: 1px solid #ff003c; color: #ff003c; padding: 4px 16px; font-weight: bold; }
        QPushButton#analyzeButton:hover { background-color: #ff003c; color: #000; }
        QPushButton#addToQueueButton { background-color: #ff003c; color: #000; font-weight: bold; border: none; border-radius: 4px; padding: 8px 24px; }
        QPushButton#btnModeFull, QPushButton#btnModeVA, QPushButton#btnModeV, QPushButton#btnModeA {
            background-color: #0a0a0a; border: 2px solid transparent; border-radius: 6px; color: #f0f0f0; font-weight: 600; font-size: 13px;
        }
        QPushButton#btnModeFull:hover, QPushButton#btnModeVA:hover, QPushButton#btnModeV:hover, QPushButton#btnModeA:hover { border-color: #444; }
        QPushButton#btnModeFull:checked, QPushButton#btnModeVA:checked, QPushButton#btnModeV:checked, QPushButton#btnModeA:checked {
            border-color: #ff003c; background-color: rgba(255,0,60,0.1); color: #ff003c;
        }
        QLabel#queueTitleLabel { font-weight: bold; font-size: 13px; padding: 8px; border-bottom: 1px solid #222; }
        QLabel#formatTitleLabel, QLabel#optionsTitleLabel {
            color: #666; font-size: 11px; font-weight: 600; padding-bottom: 6px; border-bottom: 1px solid #222;
        }
        QCheckBox { background-color: transparent; }
    )");

    // ── Modo de descarga ──
    modeButtonGroup = new QButtonGroup(this);
    modeButtonGroup->setExclusive(true);
    modeButtonGroup->addButton(ui->btnModeFull);
    modeButtonGroup->addButton(ui->btnModeVA);
    modeButtonGroup->addButton(ui->btnModeV);
    modeButtonGroup->addButton(ui->btnModeA);
    ui->btnModeFull->setChecked(true);

    connect(modeButtonGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton *btn) {
        bool showVideo = (btn == ui->btnModeFull || btn == ui->btnModeVA || btn == ui->btnModeV);
        bool showAudio = (btn != ui->btnModeV);
        bool showSubs  = (btn == ui->btnModeFull);
        ui->videoOptionsWidget->setVisible(showVideo);
        ui->audioOptionsWidget->setVisible(showAudio);
        ui->containerOptionsWidget->setVisible(showVideo);
        ui->subsWidget->setVisible(showSubs);
    });

    // ── Thumbnail placeholder ──
    ui->thumbnailLabel->setText("Sin miniatura");
    ui->thumbnailLabel->setAlignment(Qt::AlignCenter);
    ui->thumbnailLabel->setStyleSheet("color: #333; font-size: 12px; background-color: #0a0a0a; border: 1px solid #222; border-radius: 4px; min-width: 225px; max-width: 225px; min-height: 140px; max-height: 140px;");

    // ── ComboBoxes iniciales ──
    ui->videoQualityComboBox->addItems({"4K (2160p)", "Full HD (1080p)", "HD (720p)", "480p", "360p"});
    ui->videoQualityComboBox->setCurrentIndex(1);
    ui->audioQualityComboBox->addItems({"Original / Automático", "Español", "Inglés", "Japonés"});
    ui->containerComboBox->addItems({"MKV", "MP4", "WEBM"});
    ui->subsLangComboBox->addItems({"Español", "Inglés", "Japonés", "Francés", "Portugués"});
    ui->subsLangComboBox->setEnabled(false);

    // ── Signals & Slots ──
    connect(ui->subsCheckBox, &QCheckBox::toggled, ui->subsLangComboBox, &QComboBox::setEnabled);

    connect(ui->browseButton, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta de destino",
                                                        QDir::homePath(), QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty()) ui->destLineEdit->setText(dir);
    });

    connect(ui->analyzeButton, &QPushButton::clicked, this, [this]() {
        QString url = ui->urlLineEdit->text().trimmed();
        if (url.isEmpty()) return;
        ui->analyzeButton->setEnabled(false);
        ui->analyzeButton->setText("Analizando...");
        ytdlpProcess->start("yt-dlp", QStringList() << "-J" << "--no-playlist" << url);
    });

    connect(ytdlpProcess, &QProcess::finished, this, [this](int exitCode) {
        ui->analyzeButton->setEnabled(true);
        ui->analyzeButton->setText("Analizar");
        if (exitCode != 0) return;

        QByteArray output = ytdlpProcess->readAllStandardOutput();
        QJsonDocument doc = QJsonDocument::fromJson(output);
        if (doc.isNull()) return;

        QJsonObject obj = doc.object();
        ui->titleLabel->setText(obj["title"].toString());
        ui->channelLabel->setText(obj["uploader"].toString());

        int totalSecs = obj["duration"].toInt();
        ui->durationLabel->setText(QString("Duración: %1:%2:%3")
                                       .arg(totalSecs / 3600, 2, 10, QChar('0'))
                                       .arg((totalSecs % 3600) / 60, 2, 10, QChar('0'))
                                       .arg(totalSecs % 60, 2, 10, QChar('0')));

        // Ordenar formatos
        QVector<QJsonObject> formatsVec;
        for (const QJsonValue &f : obj["formats"].toArray())
            formatsVec.append(f.toObject());
        std::sort(formatsVec.begin(), formatsVec.end(), [](const QJsonObject &a, const QJsonObject &b) {
            return a["height"].toInt() > b["height"].toInt();
        });

        // Poblar video
        ui->videoQualityComboBox->clear();
        QSet<QString> resVistas;
        for (const QJsonObject &fmt : formatsVec) {
            if (fmt["vcodec"].toString() == "none") continue;
            QString res = fmt["resolution"].toString();
            if (res.isEmpty() || res == "audio only" || resVistas.contains(res)) continue;
            resVistas.insert(res);
            ui->videoQualityComboBox->addItem(res, fmt["format_id"].toString());
        }

        // Poblar audio
        ui->audioQualityComboBox->clear();
        QSet<QString> langVistas;
        for (const QJsonObject &fmt : formatsVec) {
            if (fmt["acodec"].toString() == "none" || fmt["vcodec"].toString() != "none") continue;
            QString lang = fmt["language"].toString();
            if (lang.isEmpty()) lang = "Desconocido";
            if (langVistas.contains(lang)) continue;
            langVistas.insert(lang);
            QString label = QString("%1 (%2 kbps)").arg(lang).arg(fmt["abr"].toDouble(), 0, 'f', 0);
            ui->audioQualityComboBox->addItem(label, fmt["format_id"].toString());
        }

        // Miniatura
        QString thumbUrl = obj["thumbnail"].toString();
        if (!thumbUrl.isEmpty()) {
            QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(thumbUrl)));
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap px;
                    px.loadFromData(reply->readAll());
                    ui->thumbnailLabel->setPixmap(px.scaled(ui->thumbnailLabel->size(),
                                                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
                reply->deleteLater();
            });
        }
    });

    connect(ui->addToQueueButton, &QPushButton::clicked, this, [this]() {
        QString title      = ui->titleLabel->text();
        QString resolution = ui->videoQualityComboBox->currentText();
        QString audio      = ui->audioQualityComboBox->currentText();
        QString dest       = ui->destLineEdit->text().trimmed();
        QString url        = ui->urlLineEdit->text().trimmed();

        if (title.isEmpty() || title == "Título del video") return;

        if (dest.isEmpty() || !QDir(dest).exists()) {
            if (!QDir().mkpath(dest)) {
                ui->destLineEdit->setStyleSheet("border: 1px solid #ff003c;");
                return;
            }
        }
        ui->destLineEdit->setStyleSheet("");

        // Crear widget de tarea
        DownloadTaskWidget *task = new DownloadTaskWidget(title, resolution, audio, ui->queueContentWidget);
        queueLayout->insertWidget(queueLayout->count() - 1, task);
        taskWidgets.append(task);
        task->setStatus("En cola...");

        connect(task, &DownloadTaskWidget::cancelRequested, this, [this, task]() {
            if (taskWidgets[currentTaskIndex] == task) {
                QProcess::execute("taskkill", QStringList() << "/F" << "/T" << "/PID"
                                                            << QString::number(downloadProcess->processId()));
                downloadProcess->waitForFinished(3000);

                QDir dir(task->getDestFolder());
                QStringList partFiles = dir.entryList({"*.part", "*.ytdl", "*.tmp"}, QDir::Files);
                for (const QString &file : partFiles)
                    dir.remove(file);

                task->setError("Cancelado por el usuario");
                startNextDownload();
            } else {
                // Tarea en cola, solo marcarla como cancelada
                task->setError("Cancelado por el usuario");
            }
        });

        // Iniciar descarga
        QString formatId = ui->videoQualityComboBox->currentData().toString();
        QString audioId  = ui->audioQualityComboBox->currentData().toString();

        QStringList args;
        args << "-f"  << QString("%1+%2").arg(formatId, audioId)
             << "--merge-output-format" << ui->containerComboBox->currentText().toLower()
             << "--output" << QString("%1/%(title)s.%(ext)s").arg(dest)
             << url;

        task->setArgs(args);

        task->setDestFolder(dest);

        // Solo iniciar si no hay descarga en curso
        if (currentTaskIndex == -1)
            startNextDownload();
    });

    connect(downloadProcess, &QProcess::finished, this, [this](int exitCode) {
        if (currentTaskIndex < 0 || currentTaskIndex >= taskWidgets.size()) return;

        if (currentTaskIndex < 0 || currentTaskIndex >= taskWidgets.size()) return;
        DownloadTaskWidget *task = taskWidgets[currentTaskIndex];
        if (exitCode == 0)
            task->setCompleted();
        else
            task->setError(downloadProcess->readAllStandardError());

        startNextDownload();
    });

    connect(downloadProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        if (currentTaskIndex < 0 || currentTaskIndex >= taskWidgets.size()) return;
        DownloadTaskWidget *task = taskWidgets[currentTaskIndex];

        QString output = QString::fromUtf8(downloadProcess->readAllStandardOutput());
        QStringList lines = output.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

        for (const QString &line : lines) {
            if (line.contains("[download]") && line.contains("%")) {
                QRegularExpression re(R"(\s*([\d.]+)%\s+of\s+([\d.]+\S+)\s+at\s+([\d.]+\S+)\s+ETA\s+(\S+))");
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch()) {
                    int percent = static_cast<int>(match.captured(1).toDouble());
                    task->updateProgress(percent, match.captured(3), match.captured(4), match.captured(2));
                }
            } else if (line.contains("[Merger]")) {
                task->setStatus("Fusionando pistas...");
            } else if (line.contains("[download] Destination") && line.contains("audio")) {
                task->setStatus("Descargando audio...");
            } else if (line.contains("[download] Destination") && line.contains("video")) {
                task->setStatus("Descargando video...");
            }
        }
    });
}

void MainWindow::startNextDownload() {
    for (int i = 0; i < taskWidgets.size(); i++) {
        if (taskWidgets[i]->isPending()) {
            currentTaskIndex = i;
            taskWidgets[i]->setStatus("Descargando...");
            downloadProcess->start("yt-dlp", taskWidgets[i]->getArgs());
            return;
        }
    }
    currentTaskIndex = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}