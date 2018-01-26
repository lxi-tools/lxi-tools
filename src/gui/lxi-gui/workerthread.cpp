#include "mainwindow.h"
#include "workerthread.h"
#include <QString>
#include <QPixmap>
#include <iostream>
#include "../../include/screenshot.h"

void WorkerThread::run()
{
    char image_buffer[0x200000];
    int image_size = 0;
    char image_format[10];
    char image_filename[1000];
    QPixmap pixmap;

    while (live)
    {
        // Capture screenshot
        // TODO: Optmize so we avoid plugin autodetect overhead at every capture
        screenshot(IP.toUtf8().data(), "", "", timeout, false, image_buffer, &image_size, image_format, image_filename);

        pixmap.loadFromData((const uchar*) image_buffer, image_size, "", Qt::AutoColor);

        emit requestUpdateUI(pixmap, QString(image_format), QString(image_filename));
    }
}

void WorkerThread::startLiveUpdate(const QString IP, int timeout)
{
    this->IP = IP;
    this->timeout = timeout;
    this->live = true;
    this->start();
}
