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

void broadcast(__attribute__((unused)) const char *address, const char *interface)
{
    std::string broadcast_message = "Broadcasting on interface "
                                    + std::string(interface);
    main_window->update_statusbar(broadcast_message.c_str());
}

void device(const char *address, const char *id)
{
    main_window->add_instrument(id, address);
}

void lxi_setup(void)
{
    // Initialize LXI library
    lxi_init();

    // Setup search information callbacks
    lxi_info.broadcast = &broadcast;
    lxi_info.device = &device;
}

void lxi_discover_(int timeout, lxi_discover_t type)
{
    // Search for LXI devices
    lxi_discover(&lxi_info, timeout, type);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Setup lxi library
    lxi_setup();

    main_window = &w;

    w.show();

    // Make sure things are resized correctly at startup
    w.resize();

    return a.exec();
}
