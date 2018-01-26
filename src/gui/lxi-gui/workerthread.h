#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QPixmap>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    void run() override;
    void startLiveUpdate(const QString IP, int timeout);
    bool live = true;

private:
    QString IP;
    int timeout;

signals:
    void requestUpdateUI(QPixmap pixmap, QString format, QString filename);
};

#endif // WORKERTHREAD_H
