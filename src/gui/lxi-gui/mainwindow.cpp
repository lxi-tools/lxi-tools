#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QClipboard>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QTimer>
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
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* copyIDAction = new QAction("Copy ID", this);
    QAction* copyIPAction = new QAction("Copy IP", this);
    QAction* openBrowserAction = new QAction("Open in browser", this);
    connect(copyIDAction, SIGNAL(triggered()), this, SLOT(copyID()));
    connect(copyIPAction, SIGNAL(triggered()), this, SLOT(copyIP()));
    connect(openBrowserAction, SIGNAL(triggered()), this, SLOT(openBrowser()));
    ui->tableWidget->addAction(copyIDAction);
    ui->tableWidget->addAction(copyIPAction);
    ui->tableWidget->addAction(openBrowserAction);

    // Set up SCPI send action for line edit box
    lineEdit = ui->comboBox->lineEdit();
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SCPIsendCommand()));

    // Set up background of screenshot view
    ui->graphicsView->setStyleSheet("background: transparent");

    // Set up About page labels
    ui->label_10->setText("<a href=\"https://lxi-tools.github.io/\"><span style=\"color:darkorange;\">Website</span></a>");
    ui->label_10->setTextFormat(Qt::RichText);
    ui->label_10->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_10->setOpenExternalLinks(true);
    QString string_version;
    string_version.sprintf("%s (BETA)", VERSION);
    ui->label_11->setText(string_version);

    // Add screenshot camera image
    q_pixmap = new QPixmap(":/images/photo-camera.png");
    QGraphicsScene* scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->addPixmap(*q_pixmap);
    ui->graphicsView->show();

    // Set search button icon
    //ui->pushButton->setIcon(ui->pushButton->style()->standardIcon(QStyle::SP_DialogApplyButton));

    // Set up Data Recorder stuff
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
    connect(timer, SIGNAL(timeout()), this, SLOT(data_recorder_update()));
    data_recorder_active = false;
    data_recorder_sample_counter = 0;
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->chartView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* zoomInAction = new QAction("Zoom In", this);
    QAction* zoomOutAction = new QAction("Zoom Out", this);
    QAction* zoomResetAction = new QAction("Zoom Reset", this);
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(zoomResetAction, SIGNAL(triggered()), this, SLOT(zoomReset()));
    ui->chartView->addAction(zoomInAction);
    ui->chartView->addAction(zoomOutAction);
    ui->chartView->addAction(zoomResetAction);

    // Register screenshot plugins
    screenshot_register_plugins();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width()*4/5);
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width()/5-1);

    QMainWindow::resizeEvent(event);
}

void MainWindow::resize()
{
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width()*4/5);
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width()/5-1);
}

// Connect
int MainWindow::LXI_connect()
{
    char *ip = (char *) IP.toUtf8().data();
    int timeout = ui->spinBox_SCPITimeout->value() * 1000;

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
    int length;
    char response_buffer[10000] = "";
    char command_buffer[10000] = "";

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
    int status;
    QString response;
    int timeout = ui->spinBox_SCPITimeout->value() * 1000;

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
        ui->textBrowser->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
        ui->textBrowser->insertPlainText(response);

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

    QString command(ui->comboBox->currentText());
    SCPIsendCommand(&command);
}

void MainWindow::SCPIsendCommand(const char *command)
{
    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    QString *q_command = new QString(command);
    SCPIsendCommand(q_command);
}

void MainWindow::copyID()
{
    QClipboard *clipboard = QApplication::clipboard();
    QTableWidgetItem *item;

    item = ui->tableWidget->item(ui->tableWidget->currentRow(), 0);
    clipboard->setText(item->text());
}

void MainWindow::copyIP()
{
    QClipboard *clipboard = QApplication::clipboard();
    QTableWidgetItem *item;

    item = ui->tableWidget->item(ui->tableWidget->currentRow(), 1);
    clipboard->setText(item->text());
}

void MainWindow::openBrowser()
{
    QTableWidgetItem *item;
    item = ui->tableWidget->item(ui->tableWidget->currentRow(), 1);

    QString URL = "http://" + item->text();
    QDesktopServices::openUrl(QUrl(URL));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Search button
void MainWindow::on_pushButton_clicked()
{
    int timeout = ui->spinBox_SearchTimeout->value() * 1000;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->pushButton->setText("Searching");
    ui->pushButton->repaint();
    lxi_discover_(timeout, ui->checkBox_mDNS->isChecked() ? DISCOVER_MDNS : DISCOVER_VXI11);
    ui->statusBar->clearMessage();
    ui->pushButton->setText("Search");
    IP.clear();
}

void MainWindow::add_instrument(char *id, char *address)
{
    QString instrument_id = QString::fromStdString(id);
    QString instrument_address = QString::fromStdString(address);

    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0,0, new QTableWidgetItem(instrument_id));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(instrument_address));
    ui->tableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
}

void MainWindow::update_statusbar(const char *message)
{
    QString status_message = QString::fromStdString(message);
    ui->statusBar->showMessage(status_message);
}

// SCPI Send button
void MainWindow::on_pushButton_2_clicked()
{
    SCPIsendCommand();
}

void MainWindow::on_tableWidget_cellClicked(__attribute__((unused)) int row, __attribute__((unused)) int column)
{
    QTableWidgetItem *item;
    item = ui->tableWidget->item(ui->tableWidget->currentRow(), 1);

    // Update IP
    IP = item->text();
}

void MainWindow::update_progressbar()
{
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

// Benchmark start
void MainWindow::on_pushButton_3_clicked()
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
    ui->pushButton_3->setText("Testing");
    ui->pushButton_3->repaint();
    benchmark(IP.toUtf8().data(), 0, 1000, VXI11, ui->spinBox_BenchmarkRequests->value(), false, &result, benchmark_progress);
    ui->pushButton_3->setText("Start");
    ui->pushButton_3->repaint();

    // Print result
    q_result = QString::number(result, 'f', 1);
    ui->label_6->setText(q_result + " requests/second");
}

// Take screenshot
void MainWindow::on_pushButton_4_clicked()
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

    int width = ui->graphicsView->width();
    int height = ui->graphicsView->height() - 2;

    q_pixmap->loadFromData((const uchar*) image_buffer, image_size, "", Qt::AutoColor);
    *q_pixmap = q_pixmap->scaled(QSize(std::min(width, q_pixmap->width()), std::min(height, q_pixmap->height())), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QGraphicsScene* scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->addPixmap(*q_pixmap);

    ui->graphicsView->show();

    // Enable Save button
    ui->pushButton_5->setEnabled(true);
}

// Save screenshot
void MainWindow::on_pushButton_5_clicked()
{
    QString q_filename = QFileDialog::getSaveFileName(this, "Save file", screenshotImageFilename, "." + screenshotImageFormat);
    QFile q_file(q_filename);
    q_file.open(QIODevice::WriteOnly);
    q_pixmap->save(&q_file, screenshotImageFormat.toUtf8().constData());
    q_file.close();
}

// *CLS
void MainWindow::on_pushButton_6_clicked()
{
    SCPIsendCommand("*CLS");
}

// *ESE
void MainWindow::on_pushButton_7_clicked()
{
    SCPIsendCommand("*ESE");
}

// *ESE?
void MainWindow::on_pushButton_8_clicked()
{
    SCPIsendCommand("*ESE?");
}

// *ESR?
void MainWindow::on_pushButton_9_clicked()
{
    SCPIsendCommand("*ESR?");
}

// *IDN?
void MainWindow::on_pushButton_10_clicked()
{
    SCPIsendCommand("*IDN?");
}

// *OPC
void MainWindow::on_pushButton_11_clicked()
{
    SCPIsendCommand("*OPC");
}

// *OPC?
void MainWindow::on_pushButton_12_clicked()
{
    SCPIsendCommand("*OPC?");
}

// *OPT?
void MainWindow::on_pushButton_13_clicked()
{
    SCPIsendCommand("*OPT?");
}

// *RST
void MainWindow::on_pushButton_14_clicked()
{
    SCPIsendCommand("*RST");
}

// *SRE
void MainWindow::on_pushButton_15_clicked()
{
    SCPIsendCommand("*SRE");
}

// *SRE?
void MainWindow::on_pushButton_16_clicked()
{
    SCPIsendCommand("*SRE?");
}

// *STB?
void MainWindow::on_pushButton_17_clicked()
{
    SCPIsendCommand("*STB?");
}

// *TST?
void MainWindow::on_pushButton_18_clicked()
{
    SCPIsendCommand("*TST?");
}

// *WAI
void MainWindow::on_pushButton_19_clicked()
{
    SCPIsendCommand("*WAI");
}

// Data recorder start
void MainWindow::on_pushButton_20_clicked()
{
    if (IP.isEmpty())
    {
        messageBox->warning(this, "Warning", "Please select instrument!");
        return;
    }

    if (data_recorder_active)
    {
        timer->stop();
        ui->pushButton_20->setText("Start");
        LXI_disconnect();

        // Enable inputs
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->pushButton_21->setEnabled(true);
        ui->spinBox_DataRecorderRate->setEnabled(true);
    }
    else
    {
        data_recorder_time_slice = 1000 / ui->spinBox_DataRecorderRate->value();
        timer->start(data_recorder_time_slice);
        ui->pushButton_20->setText("Stop");

        LXI_connect();

        // Disable inputs
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->pushButton_21->setEnabled(false);
        ui->spinBox_DataRecorderRate->setEnabled(false);
    }

    ui->pushButton_20->repaint();

    data_recorder_active = !data_recorder_active;
}

void MainWindow::data_recorder_update()
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
void MainWindow::on_pushButton_21_clicked()
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

void MainWindow::zoomOut()
{
    ui->chartView->chart()->zoomOut();
}

void MainWindow::zoomIn()
{
    ui->chartView->chart()->zoomIn();
}

void MainWindow::zoomReset()
{
    ui->chartView->chart()->zoomReset();
}
