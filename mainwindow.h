#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>


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
};
#endif // MAINWINDOW_H
