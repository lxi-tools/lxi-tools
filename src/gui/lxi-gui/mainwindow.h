#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
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
    void add_instrument(const char *id, const char *address);
    void update_statusbar(const char *message);
    void update_progressbar();
    void pushButton_reset();
    void resizeEvent(QResizeEvent *event);
    void resize();

private slots:
    void on_tableWidget_InstrumentList_cellClicked(int row, int column);
    void InstrumentList_copyID();
    void InstrumentList_copyIP();
    void InstrumentList_openBrowser();
    int LXI_connect();
    int LXI_send_receive(QString *command, QString *response, int timeout);
    int LXI_disconnect();
    void DataRecorder_zoomOut();
    void DataRecorder_zoomIn();
    void DataRecorder_zoomReset();
    void DataRecorder_Update();
    void on_pushButton_Search_clicked();
    void on_pushButton_Benchmark_Start_clicked();
    void on_pushButton_Screenshot_TakeScreenshot_clicked();
    void on_pushButton_Screenshot_Save_clicked();
    void on_pushButton_Screenshot_LiveView_clicked();
    void Screenshot_UpdateUI(QPixmap pixmap, QString image_format, QString image_filename);
    void SCPIsendCommand(const char *command);
    void SCPIsendCommand(QString *command);
    void SCPIsendCommand();
    void on_pushButton_SCPI_Send_clicked();
    void on_pushButton_SCPI_Clear_clicked();
    void on_pushButton_SCPI_CLS_clicked();
    void on_pushButton_SCPI_ESE_clicked();
    void on_pushButton_SCPI_ESEQuestion_clicked();
    void on_pushButton_SCPI_ESRQuestion_clicked();
    void on_pushButton_SCPI_IDNQuestion_clicked();
    void on_pushButton_SCPI_OPC_clicked();
    void on_pushButton_SCPI_OPCQuestion_clicked();
    void on_pushButton_SCPI_Blank_clicked();
    void on_pushButton_SCPI_RST_clicked();
    void on_pushButton_SCPI_SRE_clicked();
    void on_pushButton_SCPI_SREQuestion_clicked();
    void on_pushButton_SCPI_STBQuestion_clicked();
    void on_pushButton_SCPI_TSTQuestion_clicked();
    void on_pushButton_SCPI_WAI_clicked();
    void on_pushButton_DataRecorder_Start_clicked();
    void on_pushButton_DataRecorder_Save_clicked();
    void on_pushButton_SCPI_SystemVersionQuery_clicked();
    void on_pushButton_SCPI_SystemErrorQuery_clicked();
    void on_pushButton_SCPI_SystemErrorNextQuery_clicked();
    void on_pushButton_SCPIP_StatusOperationQuery_clicked();
    void on_pushButton_SCPI_StatusPreset_clicked();

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
    QTime time;
    bool live_view_active;
    bool data_recorder_active;
    int data_recorder_sample_counter;
    double data_recorder_time_slice;
    bool data_recorder_first_sample = true;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem = NULL;
    QValueAxis *axisX;
    QValueAxis *axisY;

};

#endif // MAINWINDOW_H
