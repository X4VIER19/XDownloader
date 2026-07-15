#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractButton>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("XDownloader");

    // Estilos base
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
        QFrame#videoOptionsFrame, QFrame#audioOptionsFrame,
        QFrame#containerOptionsFrame, QFrame#destFrame, QFrame#subsFrame {
            background-color: transparent; border: none;
        }
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
        QListWidget { background-color: #0a0a0a; border: none; }
        QLabel#queueTitleLabel { background-color: transparent; font-weight: bold; font-size: 13px; padding: 8px; border-bottom: 1px solid #222; }
        QCheckBox { background-color: transparent; }

        QLabel#formatTitleLabel, QLabel#optionsTitleLabel {
            color: #666666;
            font-size: 11px;
            font-weight: 600;
            letter-spacing: 1px;
            background-color: transparent;
            padding-bottom: 6px;
            border-bottom: 1px solid #222;
        }
        QLabel#thumbnailLabel {
            background-color: #0a0a0a;
            border: 1px solid #333;
            border-radius: 4px;
            min-width: 160px;
            max-width: 160px;
            min-height: 100px;
        }
        QPushButton#addToQueueButton {
            background-color: #ff003c;
            color: #000000;
            font-weight: bold;
            border: none;
            border-radius: 4px;
            padding: 8px 24px;
        }
        QPushButton#btnModeFull, QPushButton#btnModeVA,
        QPushButton#btnModeV, QPushButton#btnModeA {
            background-color: #0a0a0a;
            border: 2px solid transparent;
            border-radius: 6px;
            color: #f0f0f0;
            font-weight: 600;
            font-size: 13px;
        }
        QPushButton#btnModeFull:hover, QPushButton#btnModeVA:hover,
        QPushButton#btnModeV:hover, QPushButton#btnModeA:hover {
            border-color: #444;
        }
        QPushButton#btnModeFull:checked, QPushButton#btnModeVA:checked,
        QPushButton#btnModeV:checked, QPushButton#btnModeA:checked {
            border-color: #ff003c;
            background-color: rgba(255,0,60,0.1);
            color: #ff003c;
        }
        QFrame#modeFrame {
            background-color: transparent;
            border: none;
            margin: 6px;
        }

    )");

    modeButtonGroup = new QButtonGroup(this);
    modeButtonGroup->setExclusive(true);
    modeButtonGroup->addButton(ui->btnModeFull);
    modeButtonGroup->addButton(ui->btnModeVA);
    modeButtonGroup->addButton(ui->btnModeV);
    modeButtonGroup->addButton(ui->btnModeA);
    ui->btnModeFull->setChecked(true);

    ui->thumbnailLabel->setText("Sin miniatura");
    ui->thumbnailLabel->setAlignment(Qt::AlignCenter);
    ui->thumbnailLabel->setStyleSheet("color: #333; font-size: 12px; background-color: #0a0a0a; border: 1px solid #222; border-radius: 4px; min-width: 225px; max-width: 225px; min-height: 140px; max-height: 140px;");

    // Calidad de video
    ui->videoQualityComboBox->addItems({"4K (2160p)", "Full HD (1080p)", "HD (720p)", "480p", "360p"});
    ui->videoQualityComboBox->setCurrentIndex(1);
    // Calidad de audio
    ui->audioQualityComboBox->addItems({"Original / Automático", "Español", "Inglés", "Japonés"});
    // Contenedor
    ui->containerComboBox->addItems({"MKV", "MP4", "WEBM"});
    // Subtítulos
    ui->subsLangComboBox->addItems({"Español", "Inglés", "Japonés", "Francés", "Portugués"});

    ui->subsLangComboBox->setEnabled(false);
    connect(ui->subsCheckBox, &QCheckBox::toggled, ui->subsLangComboBox, &QComboBox::setEnabled);

    connect(modeButtonGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton *btn) {
        bool showVideo = (btn == ui->btnModeFull || btn == ui->btnModeVA || btn == ui->btnModeV);
        bool showAudio = (btn != ui->btnModeV);
        bool showSubs  = (btn == ui->btnModeFull);

        ui->videoOptionsWidget->setVisible(showVideo);
        ui->audioOptionsWidget->setVisible(showAudio);
        ui->containerOptionsWidget->setVisible(showVideo);
        ui->subsWidget->setVisible(showSubs);
    });

    connect(ui->browseButton, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta de destino", QDir::homePath(), QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty())
            ui->destLineEdit->setText(dir);
    });

    ytdlpProcess = new QProcess(this);

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
        int h = totalSecs / 3600;
        int m = (totalSecs % 3600) / 60;
        int s = totalSecs % 60;
        ui->durationLabel->setText(QString("Duración: %1:%2:%3")
                                       .arg(h, 2, 10, QChar('0'))
                                       .arg(m, 2, 10, QChar('0'))
                                       .arg(s, 2, 10, QChar('0')));
        QJsonArray formats = obj["formats"].toArray();

        QVector<QJsonObject> formatsVec;
        for (const QJsonValue &f : formats)
            formatsVec.append(f.toObject());

        std::sort(formatsVec.begin(), formatsVec.end(), [](const QJsonObject &a, const QJsonObject &b) {
            return a["height"].toInt() > b["height"].toInt();
        });

        ui->videoQualityComboBox->clear();
        QSet<QString> resolucionesVistas;

        for (const QJsonObject &fmt : formatsVec) {
            QString vcodec = fmt["vcodec"].toString();
            QString resolution = fmt["resolution"].toString();

            if (vcodec == "none" || resolution.isEmpty() || resolution == "audio only")
                continue;

            if (!resolucionesVistas.contains(resolution)) {
                resolucionesVistas.insert(resolution);
                ui->videoQualityComboBox->addItem(resolution, fmt["format_id"].toString());
            }
        }

        ui->audioQualityComboBox->clear();
        QSet<QString> idiomasVistos;

        for (const QJsonObject &fmt : formatsVec) {
            QString acodec = fmt["acodec"].toString();
            QString vcodec = fmt["vcodec"].toString();

            // Solo formatos de audio puro
            if (acodec == "none" || vcodec != "none")
                continue;

            QString lang = fmt["language"].toString();
            if (lang.isEmpty()) lang = "Desconocido";

            QString abr = QString::number(fmt["abr"].toDouble(), 'f', 0) + " kbps";
            QString label = QString("%1 (%2)").arg(lang, abr);

            if (!idiomasVistos.contains(lang)) {
                idiomasVistos.insert(lang);
                ui->audioQualityComboBox->addItem(label, fmt["format_id"].toString());
            }
        }

        QString thumbnailUrl = obj["thumbnail"].toString();
        if (!thumbnailUrl.isEmpty()) {
            QNetworkRequest request(thumbnailUrl);
            QNetworkReply *reply = networkManager->get(request);
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    ui->thumbnailLabel->setPixmap(
                        pixmap.scaled(ui->thumbnailLabel->size(),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation)
                        );
                }
                reply->deleteLater();
            });
        }

    });

    networkManager = new QNetworkAccessManager(this);

    connect(ui->addToQueueButton, &QPushButton::clicked, this, [this]() {
        QString title = ui->titleLabel->text();
        QString resolution = ui->videoQualityComboBox->currentText();
        QString audio = ui->audioQualityComboBox->currentText();

        if (title.isEmpty() || title == "Título del video") return;

        QString itemText = QString("%1\n%2 | %3").arg(title, resolution, audio);
        ui->queueListWidget->addItem(itemText);
    });

    downloadProcess = new QProcess(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}