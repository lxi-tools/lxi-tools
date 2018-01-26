#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QThread>

QT_CHARTS_USE_NAMESPACE

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
    void resizeEvent(QResizeEvent *event);
    void resize();

private slots:
    void copyID();
    void copyIP();
    void openBrowser();
    void SCPIsendCommand(const char *command);
    void SCPIsendCommand(QString *command);
    void SCPIsendCommand();
    int LXI_connect();
    int LXI_send_receive(QString *command, QString *response, int timeout);
    int LXI_disconnect();
    void zoomOut();
    void zoomIn();
    void zoomReset();
    void data_recorder_update();
    void on_tableWidget_cellClicked(int row, int column);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_Screenshot_TakeScreenshot_clicked();
    void on_pushButton_Screenshot_Save_clicked();
    void on_pushButton_Screenshot_LiveView_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_21_clicked();
    void updateUI(QPixmap pixmap, QString image_format, QString image_filename);

private:
    Ui::MainWindow *ui;
    QMessageBox *messageBox;
    QLineEdit *lineEdit;
    QString IP;
    QPixmap *q_pixmap;
    QString screenshotImageFormat;
    QString screenshotImageFilename;
    QLineSeries *line_series0;
    QLineSeries *line_series1;
    QChart *datarecorder_chart;
    int lxi_device;
    QTimer *timer;
    bool live_view_active;
    bool data_recorder_active;
    int data_recorder_sample_counter;
    double data_recorder_time_slice;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem = NULL;

};

#endif // MAINWINDOW_H
