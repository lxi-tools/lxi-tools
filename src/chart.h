#pragma once

#include <gtk/gtk.h>

#define CHART_TYPE_WIDGET (chart_get_type ())
G_DECLARE_FINAL_TYPE (Chart, chart, CHART, WIDGET, GtkWidget)

GtkWidget * chart_new (void);

enum chart_type_t
{
  CHART_TYPE_LINE,
  CHART_TYPE_SCATTER,
};

void chart_set_handle(Chart *chart, int handle);
int chart_get_handle(Chart *chart);

void chart_set_type(Chart *chart, int type);
void chart_set_title(Chart *chart, const char *title);
void chart_set_x_label(Chart *chart, const char *x_label);
void chart_set_y_label(Chart *chart, const char *y_label);
void chart_set_x_max(Chart *chart, double x_max);
void chart_set_y_max(Chart *chart, double y_max);
void chart_set_width(Chart *chart, int width);
void chart_add_data_point(Chart *chart, double x, double y);

