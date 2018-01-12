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
    void pushButton_reset();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void copyID();
    void copyIP();
    void SCPIsendCommand();
    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;
    QLineEdit *lineEdit;
    QString IP;
};

#endif // MAINWINDOW_H
