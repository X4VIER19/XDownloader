#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractButton>
#include <QFileDialog>


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
}

MainWindow::~MainWindow()
{
    delete ui;
}