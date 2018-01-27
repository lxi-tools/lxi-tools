#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workerthread.h"
#include <QString>
#include <QClipboard>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QThread>
#include <QInputDialog>
#include <iostream>
#include <lxi.h>
#include "../../include/config.h"
#include "../../include/scpi.h"
#include "../../include/benchmark.h"
#include "../../include/screenshot.h"

extern void lxi_discover_(int timeout, lxi_discover_t type);
extern void benchmark_progress(void);

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create message box for error messages etc.
    messageBox = new QMessageBox(this);

    // Setup instrument table widget
    ui->tableWidget_InstrumentList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_InstrumentList->verticalHeader()->setVisible(false);
    ui->tableWidget_InstrumentList->setShowGrid(false);
    ui->tableWidget_InstrumentList->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* copyIDAction = new QAction("Copy ID", this);
    QAction* copyIPAction = new QAction("Copy IP", this);
    QAction* openBrowserAction = new QAction("Open in browser", this);
    connect(copyIDAction, SIGNAL(triggered()), this, SLOT(InstrumentList_copyID()));
    connect(copyIPAction, SIGNAL(triggered()), this, SLOT(InstrumentList_copyIP()));
    connect(openBrowserAction, SIGNAL(triggered()), this, SLOT(InstrumentList_openBrowser()));
    ui->tableWidget_InstrumentList->addAction(copyIDAction);
    ui->tableWidget_InstrumentList->addAction(copyIPAction);
    ui->tableWidget_InstrumentList->addAction(openBrowserAction);

    // Set up SCPI send action for line edit box
    lineEdit = ui->comboBox_SCPI_Command->lineEdit();
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SCPIsendCommand()));

    // Setup Screenshot page
    ui->graphicsView_Screenshot->setStyleSheet("background: transparent");
    q_pixmap = new QPixmap(":/images/photo-camera.png");
    scene = new QGraphicsScene();
    ui->graphicsView_Screenshot->setScene(scene);
    scene->addPixmap(*q_pixmap);
    ui->graphicsView_Screenshot->show();
    live_view_active = false;

    // Set up About page labels
    ui->label_10->setText("<a href=\"https://lxi-tools.github.io/\"><span style=\"color:darkorange;\">Website</span></a>");
    ui->label_10->setTextFormat(Qt::RichText);
    ui->label_10->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_10->setOpenExternalLinks(true);
    QString string_version;
    string_version.sprintf("%s (BETA)", VERSION);
    ui->label_11->setText(string_version);

    // Set search button icon
    //ui->pushButton->setIcon(ui->pushButton->style()->standardIcon(QStyle::SP_DialogApplyButton));

    // Setup Data Recorder page
    datarecorder_chart = new QChart();
    datarecorder_chart->legend()->hide();
    line_series0 = new QLineSeries();
    line_series1 = new QLineSeries();
    datarecorder_chart->addSeries(line_series0);
    datarecorder_chart->addSeries(line_series1);
    datarecorder_chart->createDefaultAxes();
    //datarecorder_chart->setTitle("Data recorder chart");
    ui->chartView->setChart(datarecorder_chart);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(DataRecorder_Update()));
    data_recorder_active = false;
    data_recorder_sample_counter = 0;
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->chartView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* zoomInAction = new QAction("Zoom In", this);
    QAction* zoomOutAction = new QAction("Zoom Out", this);
    QAction* zoomResetAction = new QAction("Zoom Reset", this);
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(DataRecorder_zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(DataRecorder_zoomOut()));
    connect(zoomResetAction, SIGNAL(triggered()), this, SLOT(DataRecorder_zoomReset()));
    ui->chartView->addAction(zoomInAction);
    ui->chartView->addAction(zoomOutAction);
    ui->chartView->addAction(zoomResetAction);

    // Register screenshot plugins
    screenshot_register_plugins();
}

// Handle resize events
void MainWindow::resizeEvent(QResizeEvent *event)
{
    resize();
    QMainWindow::resizeEvent(event);
}

// Handle resize
void MainWindow::resize()
{
    ui->tableWidget_InstrumentList->setColumnWidth(0, ui->tableWidget_InstrumentList->width()*4/5);
    ui->tableWidget_InstrumentList->setColumnWidth(1, ui->tableWidget_InstrumentList->width()/5-1);
}

// Connect
int MainWindow::LXI_connect()
{
    int timeout = ui->spinBox_SCPITimeout->value() * 1000;
    char *ip = (char *) IP.toUtf8().data();

    // Connect
    lxi_device = lxi_connect(ip, 0, NULL, timeout, VXI11);
    if (lxi_device == LXI_ERROR)
    {
        messageBox->critical(this, "Error", "Failed to connect!");
        return -1;
    }

    return 0;
}

// Send command
int MainWindow::LXI_send_receive(QString *command, QString *response, int timeout)
{
    char response_buffer[10000] = "";
    char command_buffer[10000] = "";
    int length;

    if (command->size() > 0)
    {
        // Prepare SCPI command string
        strcpy(command_buffer, command->toUtf8().constData());
        strip_trailing_space(command_buffer);

        // Send command
        lxi_send(lxi_device, command_buffer, strlen(command_buffer), timeout);

        // If command is a question then receive response
        if (question(command_buffer))
        {
            length = lxi_receive(lxi_device, response_buffer, 10000, timeout);
            if (length < 0)
            {
                messageBox->critical(this, "Error", "Failed to receive message!");
                lxi_disconnect(lxi_device);
                return -1;
            }

            // Return response
            *response = QString::fromStdString(response_buffer);
        }
    }

    return 0;
}

// Disconnect
int MainWindow::LXI_disconnect()
{
    if (lxi_disconnect(lxi_device) != LXI_OK)
        return -1;
    else
        return 0;
}

void MainWindow::SCPIsendCommand(QString *command)
{
    int timeout = ui->spinBox_SCPITimeout->value() * 1000;
    QString response;
    int status;

    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    LXI_connect();

    status = LXI_send_receive(command, &response, timeout);
    if (status == 0)
    {
        // Print response
        ui->textBrowser_SCPI_Response->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
        ui->textBrowser_SCPI_Response->insertPlainText(response);

        LXI_disconnect();
    }
}

void MainWindow::SCPIsendCommand()
{
    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    QString command(ui->comboBox_SCPI_Command->currentText());
    SCPIsendCommand(&command);
}

void MainWindow::SCPIsendCommand(const char *command)
{
    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    QString *command_ = new QString(command);
    SCPIsendCommand(command_);
}

void MainWindow::InstrumentList_copyID()
{
    QClipboard *clipboard = QApplication::clipboard();
    QTableWidgetItem *item = ui->tableWidget_InstrumentList->item(ui->tableWidget_InstrumentList->currentRow(), 0);
    clipboard->setText(item->text());
}

void MainWindow::InstrumentList_copyIP()
{
    QClipboard *clipboard = QApplication::clipboard();
    QTableWidgetItem *item = ui->tableWidget_InstrumentList->item(ui->tableWidget_InstrumentList->currentRow(), 1);
    clipboard->setText(item->text());
}

void MainWindow::InstrumentList_openBrowser()
{
    QTableWidgetItem *item = ui->tableWidget_InstrumentList->item(ui->tableWidget_InstrumentList->currentRow(), 1);
    QString URL = "http://" + item->text();
    QDesktopServices::openUrl(QUrl(URL));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Search button
void MainWindow::on_pushButton_Search_clicked()
{
    int timeout = ui->spinBox_SearchTimeout->value() * 1000;

    ui->tableWidget_InstrumentList->clearContents();
    ui->tableWidget_InstrumentList->setRowCount(0);
    ui->pushButton_Search->setText("Searching");
    ui->pushButton_Search->repaint();
    lxi_discover_(timeout, ui->checkBox_mDNS->isChecked() ? DISCOVER_MDNS : DISCOVER_VXI11);
    ui->statusBar->clearMessage();
    ui->pushButton_Search->setText("Search");
    IP.clear();
}

void MainWindow::add_instrument(char *id, char *address)
{
    QString instrument_id = QString::fromStdString(id);
    QString instrument_address = QString::fromStdString(address);

    ui->tableWidget_InstrumentList->insertRow(0);
    ui->tableWidget_InstrumentList->setItem(0,0, new QTableWidgetItem(instrument_id));
    ui->tableWidget_InstrumentList->setItem(0,1, new QTableWidgetItem(instrument_address));
    ui->tableWidget_InstrumentList->item(0,1)->setTextAlignment(Qt::AlignCenter);
}

void MainWindow::update_statusbar(const char *message)
{
    QString status_message = QString::fromStdString(message);
    ui->statusBar->showMessage(status_message);
}

// SCPI Send button
void MainWindow::on_pushButton_SCPI_Send_clicked()
{
    SCPIsendCommand();
}

void MainWindow::on_tableWidget_InstrumentList_cellClicked(__attribute__((unused)) int row, __attribute__((unused)) int column)
{
    QTableWidgetItem *item = ui->tableWidget_InstrumentList->item(ui->tableWidget_InstrumentList->currentRow(), 1);

    // Update IP
    IP = item->text();
}

void MainWindow::update_progressbar()
{
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

// Benchmark start
void MainWindow::on_pushButton_Benchmark_Start_clicked()
{
    double result;
    QString q_result;
    QMessageBox messageBox(this);

    if (IP.isEmpty())
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    // Reset
    ui->label_6->clear();
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(ui->spinBox_BenchmarkRequests->value());

    // Run benchmark
    ui->pushButton_Benchmark_Start->setText("Testing");
    ui->pushButton_Benchmark_Start->repaint();
    benchmark(IP.toUtf8().data(), 0, 1000, VXI11, ui->spinBox_BenchmarkRequests->value(), false, &result, benchmark_progress);
    ui->pushButton_Benchmark_Start->setText("Start");
    ui->pushButton_Benchmark_Start->repaint();

    // Print result
    q_result = QString::number(result, 'f', 1);
    ui->label_6->setText(q_result + " requests/second");
}

void MainWindow::Screenshot_UpdateUI(QPixmap pixmap, QString image_format, QString image_filename)
{
    // Update UI
    // TODO: Refactor common screenshot code

    int width = ui->graphicsView_Screenshot->width();
    int height = ui->graphicsView_Screenshot->height() - 2;

    screenshotImageFormat.clear();
    screenshotImageFormat.append(image_format);

    screenshotImageFilename.clear();
    screenshotImageFilename.append(image_filename);

    pixmap = pixmap.scaled(QSize(std::min(width, pixmap.width()), std::min(height, pixmap.height())), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    *q_pixmap = pixmap;

    if (!pixmapItem)
        pixmapItem = scene->addPixmap(pixmap);
    else
        pixmapItem->setPixmap(pixmap);

    ui->graphicsView_Screenshot->show();
}

// Live View
void MainWindow::on_pushButton_Screenshot_LiveView_clicked()
{
    static WorkerThread *workerthread;
    QMessageBox messageBox(this);

    if (IP.isEmpty())
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    if (live_view_active)
    {
        // Stop live view

        // Wait for worker thread to finish
        workerthread->live = false;
        workerthread->wait();
        delete workerthread;

        ui->pushButton_Screenshot_LiveView->setText("Live View");
        ui->pushButton_Screenshot_LiveView->repaint();

        // Enable buttons
        ui->pushButton_Screenshot_Save->setEnabled(true);
        ui->pushButton_Screenshot_TakeScreenshot->setEnabled(true);

        live_view_active = false;
    } else
    {
        // Start live view
        ui->pushButton_Screenshot_LiveView->setText("Stop");
        ui->pushButton_Screenshot_LiveView->repaint();

        // Disable buttons
        ui->pushButton_Screenshot_Save->setEnabled(false);
        ui->pushButton_Screenshot_TakeScreenshot->setEnabled(false);

        // Start worker thread
        int timeout = ui->spinBox_ScreenshotTimeout->value() * 1000;
        workerthread = new WorkerThread;
        connect(workerthread, SIGNAL(requestUpdateUI(QPixmap, QString, QString)), this, SLOT(Screenshot_UpdateUI(QPixmap, QString, QString)));
        workerthread->startLiveUpdate(IP, timeout);

        live_view_active = true;
    }
}

// Take screenshot
void MainWindow::on_pushButton_Screenshot_TakeScreenshot_clicked()
{
    char image_buffer[0x200000];
    int image_size = 0;
    char image_format[10];
    char image_filename[1000];
    int timeout = ui->spinBox_ScreenshotTimeout->value() * 1000;

    QMessageBox messageBox(this);

    if (IP.isEmpty())
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    // Capture screenshot
    screenshot(IP.toUtf8().data(), "", "", timeout, false, image_buffer, &image_size, image_format, image_filename);

    screenshotImageFormat.clear();
    screenshotImageFormat.append(image_format);

    screenshotImageFilename.clear();
    screenshotImageFilename.append(image_filename);

    int width = ui->graphicsView_Screenshot->width();
    int height = ui->graphicsView_Screenshot->height() - 2;

    q_pixmap->loadFromData((const uchar*) image_buffer, image_size, "", Qt::AutoColor);
    *q_pixmap = q_pixmap->scaled(QSize(std::min(width, q_pixmap->width()), std::min(height, q_pixmap->height())), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (!pixmapItem)
        pixmapItem = scene->addPixmap(*q_pixmap);
    else
        pixmapItem->setPixmap(*q_pixmap);

    ui->graphicsView_Screenshot->show();

    // Enable buttons
    ui->pushButton_Screenshot_Save->setEnabled(true);
}

// Save screenshot
void MainWindow::on_pushButton_Screenshot_Save_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save file", screenshotImageFilename, "." + screenshotImageFormat);
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    q_pixmap->save(&file, screenshotImageFormat.toUtf8().constData());
    file.close();
}

// *CLS
void MainWindow::on_pushButton_SCPI_CLS_clicked()
{
    SCPIsendCommand("*CLS");
}

// *ESE
void MainWindow::on_pushButton_SCPI_ESE_clicked()
{
    QString command;
    bool ok;
    int i = QInputDialog::getInt(this, tr("*ESE <value>"),
                                     tr("Value:"), 0, 0, 255, 1, &ok);
    if (ok)
    {
        command = tr("*ESE %1").arg(i);
        SCPIsendCommand(&command);
    }
}

// *ESE?
void MainWindow::on_pushButton_SCPI_ESEQuestion_clicked()
{
    SCPIsendCommand("*ESE?");
}

// *ESR?
void MainWindow::on_pushButton_SCPI_ESRQuestion_clicked()
{
    SCPIsendCommand("*ESR?");
}

// *IDN?
void MainWindow::on_pushButton_SCPI_IDNQuestion_clicked()
{
    SCPIsendCommand("*IDN?");
}

// *OPC
void MainWindow::on_pushButton_SCPI_OPC_clicked()
{
    SCPIsendCommand("*OPC");
}

// *OPC?
void MainWindow::on_pushButton_SCPI_OPCQuestion_clicked()
{
    SCPIsendCommand("*OPC?");
}

// *OPT?
void MainWindow::on_pushButton_SCPI_OPTQuestion_clicked()
{
    SCPIsendCommand("*OPT?");
}

// *RST
void MainWindow::on_pushButton_SCPI_RST_clicked()
{
    SCPIsendCommand("*RST");
}

// *SRE
void MainWindow::on_pushButton_SCPI_SRE_clicked()
{
    QString command;
    bool ok;
    int i = QInputDialog::getInt(this, tr("*SRE <value>"),
                                     tr("Value:"), 0, 0, 255, 1, &ok);
    if (ok)
    {
        command = tr("*SRE %1").arg(i);
        SCPIsendCommand(&command);
    }
}

// *SRE?
void MainWindow::on_pushButton_SCPI_SREQuestion_clicked()
{
    SCPIsendCommand("*SRE?");
}

// *STB?
void MainWindow::on_pushButton_SCPI_STBQuestion_clicked()
{
    SCPIsendCommand("*STB?");
}

// *TST?
void MainWindow::on_pushButton_SCPI_TSTQuestion_clicked()
{
    SCPIsendCommand("*TST?");
}

// *WAI
void MainWindow::on_pushButton_SCPI_WAI_clicked()
{
    SCPIsendCommand("*WAI");
}

// Data recorder start
void MainWindow::on_pushButton_DataRecorder_Start_clicked()
{
    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    if (data_recorder_active)
    {
        timer->stop();
        ui->pushButton_DataRecorder_Start->setText("Start");
        LXI_disconnect();

        // Enable inputs
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->pushButton_DataRecorder_Clear->setEnabled(true);
        ui->spinBox_DataRecorderRate->setEnabled(true);
    }
    else
    {
        data_recorder_time_slice = 1000 / ui->spinBox_DataRecorderRate->value();
        timer->start(data_recorder_time_slice);
        ui->pushButton_DataRecorder_Start->setText("Stop");

        LXI_connect();

        // Disable inputs
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->pushButton_DataRecorder_Clear->setEnabled(false);
        ui->spinBox_DataRecorderRate->setEnabled(false);
    }

    ui->pushButton_DataRecorder_Start->repaint();

    data_recorder_active = !data_recorder_active;
}

void MainWindow::DataRecorder_Update()
{
    QString response;

    if (!ui->lineEdit->text().isEmpty())
    {
        // Retrieve sample 1
        QString command = ui->lineEdit->text();
        LXI_send_receive(&command, &response, 1000);
        line_series0->append(data_recorder_sample_counter * data_recorder_time_slice / 1000, response.toDouble());
    }

    if (!ui->lineEdit_2->text().isEmpty())
    {
        // Retrieve sample 2
        QString command = ui->lineEdit_2->text();
        LXI_send_receive(&command, &response, 1000);
        line_series1->append(data_recorder_sample_counter * data_recorder_time_slice / 1000, response.toDouble());
    }

    datarecorder_chart->removeSeries(line_series0);
    datarecorder_chart->addSeries(line_series0);
    datarecorder_chart->removeSeries(line_series1);
    datarecorder_chart->addSeries(line_series1);
    datarecorder_chart->createDefaultAxes();
    ui->chartView->setChart(datarecorder_chart);

    data_recorder_sample_counter++;
}

// Data recorder clear
void MainWindow::on_pushButton_DataRecorder_Clear_clicked()
{
    datarecorder_chart->removeSeries(line_series0);
    line_series0->clear();
    datarecorder_chart->addSeries(line_series0);
    datarecorder_chart->removeSeries(line_series1);
    line_series1->clear();
    datarecorder_chart->addSeries(line_series1);
    datarecorder_chart->createDefaultAxes();
    ui->chartView->setChart(datarecorder_chart);
    data_recorder_sample_counter = 0;
}

void MainWindow::DataRecorder_zoomOut()
{
    ui->chartView->chart()->zoomOut();
}

void MainWindow::DataRecorder_zoomIn()
{
    ui->chartView->chart()->zoomIn();
}

void MainWindow::DataRecorder_zoomReset()
{
    ui->chartView->chart()->zoomReset();
}

void MainWindow::on_pushButton_SCPI_Clear_clicked()
{
    ui->comboBox_SCPI_Command->lineEdit()->clear();
}
