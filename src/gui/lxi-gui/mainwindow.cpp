#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QClipboard>
#include <QAction>
#include <QMessageBox>
#include <iostream>
#include <lxi.h>
#include "../../include/scpi.h"
#include "../../include/benchmark.h"
#include "../../include/screenshot.h"

extern void lxi_discover_(void);
extern void benchmark_progress(void);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup table widget
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setColumnWidth(0, 559);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* copyIDAction = new QAction("Copy ID", this);
    QAction* copyIPAction = new QAction("Copy IP", this);
    connect(copyIDAction, SIGNAL(triggered()), this, SLOT(copyID()));
    connect(copyIPAction, SIGNAL(triggered()), this, SLOT(copyIP()));
    ui->tableWidget->addAction(copyIDAction);
    ui->tableWidget->addAction(copyIPAction);

    lineEdit = ui->comboBox->lineEdit();
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SCPIsendCommand()));

    ui->graphicsView->setStyleSheet("background: transparent");

    // Register screenshot plugins
    screenshot_register_plugins();
}

// SCPI Send action
void MainWindow::SCPIsendCommand()
{
    QMessageBox messageBox(this);
    QString q_response;
    int device, length;
    char response[10000];
    char command[10000];
    char *ip = (char *) IP.toUtf8().data();

    if (IP.size() == 0)
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    if (ui->comboBox->currentText().size() > 0)
    {
        // Connect
        device = lxi_connect(ip, 0, NULL, 1000, VXI11);
        if (device == LXI_ERROR)
        {
            messageBox.critical(this, "Error", "Failed to connect!");
            return;
        }

        // Prepare SCPI command string
        strcpy(command, ui->comboBox->currentText().toUtf8().constData());
        strip_trailing_space(command);

        // Send command
        lxi_send(device, command, strlen(command), 1000);

        // If command is a question then receive response
        if (question(command))
        {
            length = lxi_receive(device, response, 10000, 3000);
            if (length < 0)
            {
                messageBox.critical(this, "Error", "Failed to receive message!");
                return;
            }

            // Print response
            q_response = QString::fromStdString(response);
            ui->textBrowser->append(q_response.left(length));
        }

        lxi_disconnect(device);
    }
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

MainWindow::~MainWindow()
{
    delete ui;
}

// Search button
void MainWindow::on_pushButton_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->pushButton->setText("Searching");
    ui->pushButton->repaint();
    lxi_discover_();
    ui->statusBar->clearMessage();
    ui->pushButton->setText("Search");
}

void MainWindow::add_instrument(char *id, char *address)
{
    QString instrument_id = QString::fromStdString(id);
    QString instrument_address = QString::fromStdString(address);

    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0,0, new QTableWidgetItem(instrument_id));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(instrument_address));
}

void MainWindow::pushButton_reset()
{

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

void MainWindow::on_tableWidget_cellClicked(int row, int column)
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

void MainWindow::on_pushButton_3_clicked()
{
    double result;
    QString q_result;
    QMessageBox messageBox(this);

    if (IP.size() == 0)
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    // Reset
    ui->label_6->clear();
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(ui->spinBox->value());

    // Run benchmark
    benchmark(IP.toUtf8().data(), 0, 1000, VXI11, ui->spinBox->value(), false, &result, benchmark_progress);

    // Print result
    q_result = QString::number(result, 'f', 1);
    ui->label_6->setText(q_result + " requests/second");
}

void MainWindow::on_pushButton_4_clicked()
{
    char image_buffer[0x200000];
    int image_size = 0;

    QMessageBox messageBox(this);

    if (IP.size() == 0)
    {
        messageBox.warning(this, "Warning", "Please select instrument!");
        return;
    }

    // Take screenshot
    screenshot(IP.toUtf8().data(), "", "", 1000, false, image_buffer, &image_size);

    int width = ui->graphicsView->geometry().width();
    int height = ui->graphicsView->geometry().height();

    QPixmap *q_pixmap = new QPixmap;
    q_pixmap->loadFromData((const uchar*) image_buffer, image_size, "", Qt::AutoColor);
    q_pixmap->scaled(QSize(width, height), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QGraphicsScene* scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->addPixmap(*q_pixmap);
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    ui->graphicsView->show();
}
