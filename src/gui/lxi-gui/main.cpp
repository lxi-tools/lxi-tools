#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <lxi.h>

static MainWindow *main_window;
static lxi_info_t lxi_info;

void benchmark_progress(void)
{
    main_window->update_progressbar();
}

void broadcast(__attribute__((unused)) char *address, char *interface)
{
    std::string broadcast_message = "Broadcasting on interface "
                                    + std::string(interface);
    main_window->update_statusbar(broadcast_message.c_str());
}

void device(char *address, char *id)
{
    main_window->add_instrument(id, address);
}

void lxi_setup(void)
{
    // Initialize LXI library
    lxi_init();

    // Set up search information callbacks
    lxi_info.broadcast = &broadcast;
    lxi_info.device = &device;
}

void lxi_discover_(void)
{
    // Search for LXI devices, 1 second timeout
    lxi_discover(&lxi_info, 1000, DISCOVER_VXI11);

    // Reset push button text
    main_window->pushButton_reset();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    lxi_setup();
    main_window = &w;

    w.show();

    return a.exec();
}
