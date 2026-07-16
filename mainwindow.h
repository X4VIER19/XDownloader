#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "downloadtaskwidget.h"
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QButtonGroup *modeButtonGroup;
    QProcess *ytdlpProcess;
    QNetworkAccessManager *networkManager;
    QProcess *downloadProcess;
    QVBoxLayout *queueLayout;
    QList<DownloadTaskWidget*> taskWidgets;
    int currentTaskIndex;
    void startNextDownload();

};
#endif // MAINWINDOW_H
