#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void pushButton_reset();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    void lxi_setup(void);
};

#endif // MAINWINDOW_H
