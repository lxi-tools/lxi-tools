#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void add_instrument(char *id, char *address);
    void update_statusbar(const char *message);
    void update_progressbar();
    void pushButton_reset();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void copyID();
    void copyIP();
    void SCPIsendCommand();
    void on_tableWidget_cellClicked(int row, int column);
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    QLineEdit *lineEdit;
    QString IP;
    QPixmap *q_pixmap;
    QString screenshotImageFormat;
    QString screenshotImageFilename;
};

#endif // MAINWINDOW_H
