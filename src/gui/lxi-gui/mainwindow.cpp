#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

extern void lxi_discover_(void);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup table widget
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setColumnWidth(0, 518);
    ui->tableWidget->setColumnWidth(1, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setText("Searching");
    ui->pushButton->setEnabled(false);
    lxi_discover_();
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
    ui->pushButton->setText("Search");
    ui->pushButton->setEnabled(true);
}
