#ifndef DOWNLOADTASKWIDGET_H
#define DOWNLOADTASKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DownloadTaskWidget : public QWidget
{
    Q_OBJECT

public:
    bool isPending() const;
    void setArgs(const QStringList &args);
    QStringList getArgs() const;
    explicit DownloadTaskWidget(const QString &title,
                                const QString &resolution,
                                const QString &audio,
                                QWidget *parent = nullptr);

    void updateProgress(int percent, const QString &speed,
                        const QString &eta, const QString &size);
    void setStatus(const QString &status);
    void setCompleted();
    void setError(const QString &errorMsg);

signals:
    void cancelRequested();

private:
    QLabel      *titleLabel;
    QLabel      *infoLabel;
    QLabel      *statusLabel;
    QLabel      *speedLabel;
    QLabel      *etaLabel;
    QLabel      *sizeLabel;
    QProgressBar *progressBar;
    QPushButton  *cancelButton;
    QStringList downloadArgs;
    bool pending = true;
};

#endif // DOWNLOADTASKWIDGET_H