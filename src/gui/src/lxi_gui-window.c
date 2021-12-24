/*
 * Copyright (c) 2021  Martin Lund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <lxi.h>
#include <ctype.h>
#include "lxi_gui-config.h"
#include "lxi_gui-window.h"
#include "screenshot.h"
#include "benchmark.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

static lxi_info_t info;

struct _LxiGuiWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GSettings           *settings;
  GtkListBox          *list_instruments;
  GMenuModel          *list_widget_menu_model;
  GtkWidget           *list_widget_popover_menu;
  GtkViewport         *list_viewport;
  GdkClipboard        *clipboard;
  GtkEntry            *entry_scpi;
  GtkTextView         *text_view_scpi;
  GtkTextView         *text_view_scpi_status;
  GtkImage            *image_screenshot;
  GtkToggleButton     *toggle_button_screenshot_live_view;
  GtkButton           *button_screenshot_grab;
  GtkButton           *button_screenshot_save;
  GThread             *screenshot_worker_thread;
  GThread             *search_worker_thread;
  GtkTextView         *text_view_screenshot_status;
  GtkTextView         *text_view_benchmark_status;
  GtkProgressBar      *progress_bar_benchmark;
  GThread             *benchmark_worker_thread;
  GtkToggleButton     *toggle_button_benchmark_start;
  GtkToggleButton     *toggle_button_search;
  GtkSpinButton       *spin_button_benchmark_requests;
  GtkLabel            *label_benchmark_result;
  GdkPixbuf           *pixbuf_screenshot;
  unsigned int        benchmark_requests_count;
  const char          *id;
  const char          *ip;
};

G_DEFINE_TYPE (LxiGuiWindow, lxi_gui_window, GTK_TYPE_APPLICATION_WINDOW)

static LxiGuiWindow *self_global;

GtkWidget* find_child_by_name(GtkWidget* parent, const gchar* name)
{
  GtkWidget *child;
  GList *children = NULL;

  g_assert(GTK_IS_WIDGET(parent) == true);

  if (g_strcmp0(gtk_widget_get_name(parent), name) == 0)
    return parent;

  for (child = gtk_widget_get_first_child(parent);
       child != NULL;
       child = gtk_widget_get_next_sibling(child))
  {
    children = g_list_append(children, child);
  }

  while (children != NULL)
  {
    GtkWidget* widget = find_child_by_name(children->data, name);

    if (widget != NULL)
      return widget;

    children = children->next;
  }

  return NULL;
}

static void
pressed_cb (GtkGestureClick *gesture,
            guint            n_press,
            double           x,
            double           y,
            LxiGuiWindow     *self)
{
  GtkWidget *child;
  GtkListBoxRow *row = gtk_list_box_get_row_at_y(self->list_instruments, y);

  UNUSED(gesture);
  UNUSED(n_press);

  if (row != NULL)
  {
    // If left click
    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_PRIMARY)
    {
      child = find_child_by_name(GTK_WIDGET(row), "list-title");
      if (child != NULL)
      {
        // Set active IP
        self->ip = gtk_label_get_text(GTK_LABEL(child));
      }
    }

    // If right click
    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
      child = find_child_by_name(GTK_WIDGET(row), "list-subtitle");
      if (child != NULL)
        self->id = gtk_label_get_text(GTK_LABEL(child));

      child = find_child_by_name(GTK_WIDGET(row), "list-title");
      if (child != NULL)
        self->ip = gtk_label_get_text(GTK_LABEL(child));

      /* We are placing our menu at the point where
       * the click happened, before popping it up.
       */

      // Adjust y value to account for scrolling offset
      GtkAdjustment* adjustment = gtk_list_box_get_adjustment(self->list_instruments);
      double adjustment_y = gtk_adjustment_get_value(adjustment);

      y = y + adjustment_y;

      gtk_popover_set_pointing_to (GTK_POPOVER (self->list_widget_popover_menu),
          &(const GdkRectangle){ x, y, 1, 1 });
      gtk_popover_popup (GTK_POPOVER (self->list_widget_popover_menu));
    }
  }
}

static void
action_cb (GtkWidget  *widget,
         const char *action_name,
         GVariant   *parameter)
{
  LxiGuiWindow *self = LXI_GUI_WINDOW (widget);

  UNUSED(parameter);

  if (g_str_equal (action_name, "action.copy_ip"))
    gdk_clipboard_set (self->clipboard, G_TYPE_STRING, self->ip);

  if (g_str_equal (action_name, "action.copy_id"))
    gdk_clipboard_set (self->clipboard, G_TYPE_STRING, self->id);

  if (g_str_equal (action_name, "action.open_browser") && self->ip != NULL)
  {
    gchar *uri = g_strconcat("http://", self->ip, NULL);
    gtk_show_uri(GTK_WINDOW(self), uri, GDK_CURRENT_TIME);
    g_free(uri);
  }
}


/* Add instrument to list */
static void
list_add_instrument (LxiGuiWindow *self, const char *ip, const char *id)
{
  GtkWidget *list_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *list_text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *list_title = gtk_label_new(ip);
  GtkWidget *list_subtitle = gtk_label_new (id);

  // Set height of list box
  gtk_widget_set_size_request(list_box, -1, 60);

  // Add image to list box
  GtkWidget *image = gtk_image_new_from_resource("/io/github/lxi-tools/lxi-gui/icons/lxi-instrument.svg");
  gtk_widget_set_margin_start(image, 2);
  gtk_widget_set_margin_end(image, 2);
  gtk_image_set_pixel_size(GTK_IMAGE(image), 50);
  gtk_box_append(GTK_BOX(list_box), image);

  // Add title to text box
  gtk_widget_set_name(list_title, "list-title");
  gtk_widget_set_halign(list_title, GTK_ALIGN_START);
  gtk_box_append(GTK_BOX(list_text_box), list_title);

  // Add subtitle to text box
  gtk_widget_set_name(list_subtitle, "list-subtitle");
  gtk_widget_add_css_class(list_subtitle, "subtitle");
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (list_subtitle));
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (provider, "label.subtitle {opacity: 1; font-size: x-small;}", -1);
  gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_widget_set_vexpand(list_subtitle, true);
  gtk_widget_set_vexpand_set(list_subtitle, true);
  gtk_widget_set_valign(list_subtitle, GTK_ALIGN_START);
  gtk_label_set_wrap(GTK_LABEL(list_subtitle), true);
  gtk_label_set_wrap_mode(GTK_LABEL(list_subtitle), PANGO_WRAP_CHAR);
  gtk_box_append(GTK_BOX(list_text_box), list_subtitle);

  // Add text box to list box
  gtk_box_append(GTK_BOX(list_box), list_text_box);

  // Add list box to list (GtkListBoxRow automatically inserted inbetween)
  gtk_list_box_append(self->list_instruments, list_box);
}

gpointer search_worker_function(gpointer data)
{
  LxiGuiWindow *self = data;
  unsigned int timeout = g_settings_get_uint(self->settings, "timeout-discover");

  // Search for LXI devices
  lxi_discover(&info, timeout, DISCOVER_VXI11);

  gtk_toggle_button_set_active(self->toggle_button_search, false);
  gtk_widget_set_sensitive(GTK_WIDGET(self->toggle_button_search), true);

  return NULL;
}

static void
button_clicked_search (LxiGuiWindow *self, GtkToggleButton *button)
{
  UNUSED(button);

  GtkWidget *child;

  // Clear instrument list
  child = gtk_widget_get_first_child (GTK_WIDGET(self->list_instruments));
  while (child != NULL)
  {
    gtk_list_box_remove (GTK_LIST_BOX (self->list_instruments), child);
    child = gtk_widget_get_first_child (GTK_WIDGET(self->list_instruments));
  }

  self->ip = NULL;
  self->id = NULL;

  // Start thread which searches for LXI instruments
  self->search_worker_thread = g_thread_new("search_worker", search_worker_function, (gpointer) self);

  // Only allow one search activity at a time
  gtk_widget_set_sensitive(GTK_WIDGET(self->toggle_button_search), false);
}

static void scroll_to_end(GtkTextView *text_view)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(buffer,&iter);
  GtkTextMark *mark = gtk_text_buffer_create_mark(buffer,NULL,&iter,FALSE);
  gtk_text_view_scroll_mark_onscreen(text_view, mark);
  gtk_text_buffer_delete_mark(buffer, mark);
}

static void text_view_add_buffer(GtkTextView *view, const char *buffer)
{
  GtkTextIter iter;
  GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
  gtk_text_buffer_get_end_iter(text_buffer, &iter);
  gtk_text_buffer_insert (text_buffer, &iter, buffer, strlen(buffer));
}

static void text_view_add_buffer_in_dimgray(GtkTextView *view, const char *buffer)
{
  GtkTextIter iter;
  char markup_buffer[65536];

  snprintf(markup_buffer, sizeof(markup_buffer), "<span foreground=\"dimgray\">%s</span>", buffer);
  GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
  gtk_text_buffer_get_end_iter(text_buffer, &iter);
  gtk_text_buffer_insert_markup (text_buffer, &iter, markup_buffer, strlen(markup_buffer));
}


static void text_view_clear_buffer(GtkTextView *view)
{
  GtkTextIter start, end;
  GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
  gtk_text_buffer_get_bounds(text_buffer, &start, &end);
  gtk_text_buffer_delete(text_buffer, &start, &end);
}

static void print_error(const char *buffer)
{
  text_view_clear_buffer(self_global->text_view_scpi_status);
  text_view_add_buffer(self_global->text_view_scpi_status, buffer);
}

static void print_error_screenshot(const char *buffer)
{
  text_view_clear_buffer(self_global->text_view_screenshot_status);
  text_view_add_buffer(self_global->text_view_screenshot_status, buffer);
}

static void print_error_benchmark(const char *buffer)
{
  text_view_clear_buffer(self_global->text_view_benchmark_status);
  text_view_add_buffer(self_global->text_view_benchmark_status, buffer);
}


static void strip_trailing_space(char *line)
{
    int i = strlen(line) - 1;

    while (i >= 0)
    {
        if ( isspace(line[i]) )
            line[i] = '\0';
        else
            break;
        i--;
    }
}

static int question(const char *string)
{
    int i;

    for (i = 0; string[i] != '\0'; i++)
    {
        if (string[i] == '?')
            return true;
    }

    return false;
}

static void
entry_scpi_enter_pressed (LxiGuiWindow *self, GtkEntry *entry)
{
  int device = 0, protocol = VXI11;
  const char *input_buffer;
  char *tx_buffer;
  char rx_buffer[65536];
  int rx_bytes;
  unsigned int timeout = g_settings_get_uint(self->settings, "timeout-scpi");
  bool show_sent_scpi = g_settings_get_boolean(self->settings, "show-sent-scpi");

  // Clear text in status text view
  text_view_clear_buffer(self->text_view_scpi_status);

  if (self->ip == NULL)
  {
    print_error("No instrument selected");
    return;
  }

  // Prepare buffer to send
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
  input_buffer = gtk_entry_buffer_get_text(entry_buffer);
  if (strlen(input_buffer) == 0)
    return;
  tx_buffer = strdup(input_buffer);
  strip_trailing_space(tx_buffer);

  device = lxi_connect(self->ip, 0, NULL, timeout, protocol);
  if (device == LXI_ERROR)
  {
    print_error("Error connecting");
    goto error_connect;
  }

  if (lxi_send(device, tx_buffer, strlen(tx_buffer), timeout) == LXI_ERROR)
  {
    print_error("Error sending");
    goto error_send;
  }

  if (question(tx_buffer))
  {
    rx_bytes = lxi_receive(device, rx_buffer, sizeof(rx_buffer), timeout);
    if (rx_bytes == LXI_ERROR)
    {
      print_error("Error receiving");
      goto error_receive;
    }

    // Terminate received string/data
    rx_buffer[rx_bytes] = 0;

    if (show_sent_scpi)
    {
      // Add sent command to output view
      text_view_add_buffer_in_dimgray(self->text_view_scpi, tx_buffer);
      text_view_add_buffer(self->text_view_scpi, "\n");
    }

    // Add received response to text view
    text_view_add_buffer(self->text_view_scpi, rx_buffer);
    scroll_to_end(self->text_view_scpi);
  }

  // Clear text in text input entry
  gtk_entry_buffer_delete_text(entry_buffer, 0, -1);

error_send:
error_receive:
  lxi_disconnect(device);
error_connect:
  free(tx_buffer);
}

static void
button_clicked_scpi (LxiGuiWindow *self, GtkButton *button)
{
  // Insert button label at entry cursor position

  const char *button_label = gtk_button_get_label(button);
  int cursor_position = gtk_editable_get_position(GTK_EDITABLE(self->entry_scpi));
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(self->entry_scpi);

  gtk_entry_buffer_insert_text(entry_buffer, cursor_position, button_label, strlen(button_label));
  gtk_entry_set_buffer(self->entry_scpi, entry_buffer);

  cursor_position = cursor_position + strlen(button_label);

  gtk_editable_set_position(GTK_EDITABLE(self->entry_scpi), cursor_position);
}

static bool grab_screenshot(LxiGuiWindow *self)
{
  char *plugin_name = (char *) "";
  char *filename = (char *) "";
  char *image_buffer;
  int image_size = 0;
  char image_format[10];
  char image_filename[1000];
  unsigned int timeout = g_settings_get_uint(self->settings, "timeout-screenshot");
  GdkPixbufLoader *loader;
  int status;

  // Check for instrument
  if (self->ip == NULL)
  {
    print_error_screenshot("No instrument selected");
    return 1;
  }

  // Allocate 20 MB for image data
  image_buffer = g_malloc(0x100000*20);
  if (image_buffer == NULL)
  {
    print_error_screenshot("Failure allocating memory for image data");
    return 1;
  }

  // Capture screenshot
  status = screenshot((char *)self->ip, plugin_name, filename, timeout, false, image_buffer, &image_size, image_format, image_filename);
  if (status != 0)
  {
    print_error_screenshot("Failure grabbing screenshot");
    g_free(image_buffer);
    return 1;
  }

  // Show screenshot
  loader = gdk_pixbuf_loader_new ();
  gdk_pixbuf_loader_write (loader, (const guchar *)image_buffer, image_size, NULL);
  self->pixbuf_screenshot = gdk_pixbuf_loader_get_pixbuf (loader);
  gtk_widget_set_valign(GTK_WIDGET(self->image_screenshot), GTK_ALIGN_FILL);
  gtk_widget_set_halign(GTK_WIDGET(self->image_screenshot), GTK_ALIGN_FILL);
  gtk_image_set_pixel_size(self->image_screenshot, -1);
  gtk_image_set_from_pixbuf(self->image_screenshot, self->pixbuf_screenshot);

  g_free(image_buffer);

  return 0;
}

static void
button_clicked_scpi_send (LxiGuiWindow *self, GtkButton *button)
{
  UNUSED(button);
  entry_scpi_enter_pressed(self, self->entry_scpi);
}

static void
button_clicked_scpi_clear (LxiGuiWindow *self, GtkButton *button)
{
  UNUSED(button);
  GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(self->entry_scpi);
  gtk_entry_buffer_delete_text(entry_buffer, 0, -1);
  gtk_entry_set_buffer(self->entry_scpi, entry_buffer);
}

gpointer screenshot_worker_function(gpointer data)
{
  LxiGuiWindow *self = data;

  while ((gtk_toggle_button_get_active(self->toggle_button_screenshot_live_view) == true))
  {
    if (grab_screenshot(self) != 0)
      continue;
  }

  return NULL;
}

static void
button_clicked_screenshot_live_view (LxiGuiWindow *self, GtkToggleButton *button)
{
  text_view_clear_buffer(self->text_view_screenshot_status);

  if (self->ip == NULL)
  {
    print_error_screenshot("No instrument selected");
    gtk_toggle_button_set_active(button, false);
    return;
  }

  if (gtk_toggle_button_get_active(button) == true)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_grab), false);
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_save), false);
    self->screenshot_worker_thread = g_thread_new("screenshot_worker", screenshot_worker_function, (gpointer) self);
  }
  else
  {
    g_thread_join(self->screenshot_worker_thread);
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_grab), true);

    // Activate screenshot "Save" button if image data is available
    if (gtk_image_get_storage_type(self->image_screenshot) != GTK_IMAGE_EMPTY)
      gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_save), true);
  }
}

static void
button_clicked_screenshot_grab (LxiGuiWindow *self, GtkButton *button)
{
  UNUSED(button);

  text_view_clear_buffer(self->text_view_screenshot_status);
  grab_screenshot(self);

  // Activate screenshot "Save" button if image data is available
  if (gtk_image_get_storage_type(self->image_screenshot) != GTK_IMAGE_EMPTY)
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_save), true);
}

static void
on_screenshot_file_save_response (GtkDialog *dialog,
                                  int        response)
{
  GError *error = NULL;
  gboolean status = true;

  if (response == GTK_RESPONSE_ACCEPT)
  {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

    g_autoptr(GFile) file = gtk_file_chooser_get_file (chooser);

    status = gdk_pixbuf_save(self_global->pixbuf_screenshot, g_file_get_path(file), "png", &error, NULL);
    if (status == false)
    {
      g_error ("Error: %s\n", error->message);
    }
  }

  gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
button_clicked_screenshot_save (LxiGuiWindow *self, GtkButton *button)
{
  UNUSED(button);
  GtkWidget *dialog;

  // Show file save as dialog
  dialog = gtk_file_chooser_dialog_new ("Select file",
                                        GTK_WINDOW (self),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                        "_Save", GTK_RESPONSE_ACCEPT,
                                        NULL);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_widget_show (dialog);

  g_signal_connect (dialog, "response",
                    G_CALLBACK (on_screenshot_file_save_response),
                    NULL);
}

void benchmark_progress_cb(unsigned int count)
{
  double ten_percent_count;
  double total_count = self_global->benchmark_requests_count;

  // Update progress for every 10% fraction reached
  ten_percent_count = total_count / 10;
  if ((++count % (unsigned int) ten_percent_count) == 0)
  {
    double fraction = count / ten_percent_count / 10;
    gtk_progress_bar_set_fraction(self_global->progress_bar_benchmark, fraction);

    // The following reduces the risk of "Trying to snapshot *some widget* without a current allocation" errors
    gtk_widget_queue_draw(GTK_WIDGET(self_global->progress_bar_benchmark));
  }
}

gpointer benchmark_worker_function(gpointer data)
{
  double result;
  char result_text[20];
  LxiGuiWindow *self = data;

  benchmark(self->ip, 0, 1000, VXI11, self->benchmark_requests_count, false, &result, benchmark_progress_cb);

  sprintf(result_text, "%.1f request/s", result);
  gtk_label_set_text(self->label_benchmark_result, result_text);

  gtk_toggle_button_set_active(self->toggle_button_benchmark_start, false);
  gtk_widget_set_sensitive(GTK_WIDGET(self->toggle_button_benchmark_start), true);

  return NULL;
}

static void
button_clicked_benchmark_start (LxiGuiWindow *self, GtkToggleButton *button)
{
  UNUSED(button);

  // Reset
  self->benchmark_requests_count = gtk_spin_button_get_value(self_global->spin_button_benchmark_requests);
  text_view_clear_buffer(self->text_view_benchmark_status);
  gtk_progress_bar_set_fraction(self->progress_bar_benchmark, 0);
  gtk_label_set_text(self->label_benchmark_result, " ");

  if (self->ip == NULL)
  {
    print_error_benchmark("No instrument selected");
    return;
  }

  gtk_widget_set_sensitive(GTK_WIDGET(button), false);
  self->benchmark_worker_thread = g_thread_new("benchmark_worker", benchmark_worker_function, (gpointer) self);
}

static void
button_clicked_add_instrument (LxiGuiWindow *self, GtkButton *button)
{
  UNUSED(button);

  g_print ("Add instrument pressed\n");

  list_add_instrument(self, "192.168.100.201", "Samsung Evo 970 Pro");
}

void broadcast(const char *address, const char *interface)
{
  g_print ("Broadcasting on interface %s using address %s\n", interface, address);
}

void device(const char *address, const char *id)
{
  list_add_instrument(self_global, address, id);
}

static void
lxi_gui_window_dispose (GObject *object)
{
  LxiGuiWindow *window = (LxiGuiWindow *)object;

  g_object_unref (window->settings);

  // Remove list view port as parent to list popover menu
  gtk_widget_unparent(GTK_WIDGET(window->list_widget_popover_menu));

  G_OBJECT_CLASS (lxi_gui_window_parent_class)->dispose (object);
}

static void
lxi_gui_window_class_init (LxiGuiWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = lxi_gui_window_dispose;

  gtk_widget_class_set_template_from_resource (widget_class, "/io/github/lxi-tools/lxi-gui/lxi_gui-window.ui");
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, list_instruments);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, list_viewport);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, entry_scpi);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, text_view_scpi);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, text_view_scpi_status);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, image_screenshot);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, toggle_button_screenshot_live_view);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, button_screenshot_grab);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, button_screenshot_save);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, text_view_screenshot_status);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, text_view_benchmark_status);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, progress_bar_benchmark);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, toggle_button_benchmark_start);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, spin_button_benchmark_requests);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, label_benchmark_result);
  gtk_widget_class_bind_template_child (widget_class, LxiGuiWindow, toggle_button_search);

  // Bind signal callbacks
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_search);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_add_instrument);
  gtk_widget_class_bind_template_callback (widget_class, entry_scpi_enter_pressed);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_scpi);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_scpi_send);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_scpi_clear);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_screenshot_live_view);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_screenshot_grab);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_screenshot_save);
  gtk_widget_class_bind_template_callback (widget_class, button_clicked_benchmark_start);

  /* These are the actions that we are using in the menu */
  gtk_widget_class_install_action (widget_class, "action.copy_ip", NULL, action_cb);
  gtk_widget_class_install_action (widget_class, "action.copy_id", NULL, action_cb);
  gtk_widget_class_install_action (widget_class, "action.open_browser", NULL, action_cb);

  // Initialize LXI library
  lxi_init();

  // Set up search information callbacks
  info.broadcast = &broadcast;
  info.device = &device;
}

static void
lxi_gui_window_init (LxiGuiWindow *self)
{
  GtkGesture *list_widget_gesture = gtk_gesture_click_new();

  gtk_widget_init_template (GTK_WIDGET (self));

  // Load settings
  self->settings = g_settings_new ("io.github.lxi-tools.lxi-gui");

  // Load and apply CSS
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_resource (provider, "/io/github/lxi-tools/lxi-gui/lxi_gui.css");
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
                                              GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  // Load instrument popover menu from model
  GtkBuilder *builder = gtk_builder_new_from_resource ("/io/github/lxi-tools/lxi-gui/lxi_gui-window_list_widget_menu_model.ui");
  self->list_widget_menu_model = G_MENU_MODEL (gtk_builder_get_object (builder, "list-widget-menu-model"));

  // Load popover menu from menu model
  self->list_widget_popover_menu = gtk_popover_menu_new_from_model(self->list_widget_menu_model);
  gtk_popover_set_has_arrow(GTK_POPOVER(self->list_widget_popover_menu), false);

  // Add list view port as parent to list popover menu
  gtk_widget_set_parent (GTK_WIDGET(self->list_widget_popover_menu), GTK_WIDGET(self->list_viewport));

  // Add event controller to handle any click gesture on list item widget
  gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (list_widget_gesture), 0);
  g_signal_connect (list_widget_gesture, "pressed", G_CALLBACK (pressed_cb), self);
  gtk_widget_add_controller (GTK_WIDGET(self->list_viewport), GTK_EVENT_CONTROLLER (list_widget_gesture));

  // Set up clipboard
  GdkDisplay* gdk_display = gdk_display_get_default();
  self->clipboard = gdk_display_get_clipboard(gdk_display);

  // Set up SCPI command entry
  gtk_editable_set_enable_undo (GTK_EDITABLE (self->entry_scpi), TRUE);

  g_object_unref (builder);
  g_object_unref (provider);

  self->ip = NULL;
  self->id = NULL;

  // Register LXI screenshot plugins
  screenshot_register_plugins();

  // Set greeting image on screenshot page
  gtk_image_set_pixel_size(self->image_screenshot, 200);
  gtk_image_set_from_resource(self->image_screenshot, "/io/github/lxi-tools/lxi-gui/images/photo-camera.png");

  // Grab focus to SCPI input entry
  gtk_widget_grab_focus(GTK_WIDGET(self->entry_scpi));

  // Set application window minimum width and height
  gtk_widget_set_size_request(GTK_WIDGET(self), 900, 700);

  // Disable screenshot "Save" button until image is present
  gtk_widget_set_sensitive(GTK_WIDGET(self->button_screenshot_save), false);

  self_global = self;
}
