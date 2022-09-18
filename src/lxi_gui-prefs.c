/*
 * Copyright (c) 2021-2022  Martin Lund
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

#include <gtk/gtk.h>
#include <adwaita.h>

#include "lxi_gui-application.h"
#include "lxi_gui-window.h"
#include "lxi_gui-prefs.h"
#include "misc.h"

struct _LxiGuiPrefs
{
    GtkDialog parent;

    GSettings *settings;
    GtkWidget *spin_button_timeout_discover;
    GtkWidget *spin_button_timeout_scpi;
    GtkWidget *spin_button_timeout_screenshot;
    GtkWidget *switch_show_sent_scpi;
    GtkWidget *switch_use_mdns_discovery;
    GtkComboBoxText *combo_box_text_com_protocol;
    GtkWidget *spin_button_raw_port;
    GtkSwitch *switch_prefer_dark_theme;
    GtkSwitch *switch_scpi_show_message_timestamp;
    GtkSwitch *switch_scpi_show_message_ip;
    GtkSwitch *switch_scpi_show_message_type;
};

G_DEFINE_TYPE (LxiGuiPrefs, lxi_gui_prefs, ADW_TYPE_PREFERENCES_WINDOW)

static void switch_activate_prefer_dark_theme (LxiGuiPrefs *self, GtkSwitch *Switch)
{
    UNUSED(Switch);
    AdwStyleManager *adw_style_manager = adw_style_manager_get_default();

    // Manage dark theme setting
    bool prefer_dark_theme = gtk_switch_get_active(self->switch_prefer_dark_theme);
    if (prefer_dark_theme)
    {
        adw_style_manager_set_color_scheme(adw_style_manager, ADW_COLOR_SCHEME_PREFER_DARK);
    }
    else
    {
        adw_style_manager_set_color_scheme(adw_style_manager, ADW_COLOR_SCHEME_DEFAULT);
    }
}

static void lxi_gui_prefs_init (LxiGuiPrefs *prefs)
{
    gtk_widget_init_template (GTK_WIDGET (prefs));
    prefs->settings = g_settings_new ("io.github.lxi-tools.lxi-gui");
    g_settings_bind (prefs->settings, "timeout-discover",
            prefs->spin_button_timeout_discover, "value",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "timeout-scpi",
            prefs->spin_button_timeout_scpi, "value",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "timeout-screenshot",
            prefs->spin_button_timeout_screenshot, "value",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "show-sent-scpi",
            prefs->switch_show_sent_scpi, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "use-mdns-discovery",
            prefs->switch_use_mdns_discovery, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "com-protocol",
            prefs->combo_box_text_com_protocol, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "raw-port",
            prefs->spin_button_raw_port, "value",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "prefer-dark-theme",
            prefs->switch_prefer_dark_theme, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "scpi-show-message-timestamp",
            prefs->switch_scpi_show_message_timestamp, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "scpi-show-message-ip",
            prefs->switch_scpi_show_message_ip, "active",
            G_SETTINGS_BIND_DEFAULT);
    g_settings_bind (prefs->settings, "scpi-show-message-type",
            prefs->switch_scpi_show_message_type, "active",
            G_SETTINGS_BIND_DEFAULT);
}

static void lxi_gui_prefs_dispose (GObject *object)
{
    LxiGuiPrefs *prefs;

    prefs = LXI_GUI_PREFS (object);

    g_clear_object (&prefs->settings);

    G_OBJECT_CLASS (lxi_gui_prefs_parent_class)->dispose (object);
}

static void lxi_gui_prefs_class_init (LxiGuiPrefsClass *class)
{
    G_OBJECT_CLASS (class)->dispose = lxi_gui_prefs_dispose;
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

    gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
            "/io/github/lxi-tools/lxi-gui/lxi_gui-prefs.ui");

    // Bind widgets
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, spin_button_timeout_discover);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, spin_button_timeout_scpi);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, spin_button_timeout_screenshot);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_show_sent_scpi);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_use_mdns_discovery);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, combo_box_text_com_protocol);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, spin_button_raw_port);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_prefer_dark_theme);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_scpi_show_message_timestamp);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_scpi_show_message_ip);
    gtk_widget_class_bind_template_child (widget_class, LxiGuiPrefs, switch_scpi_show_message_type);

    // Bind signals
    gtk_widget_class_bind_template_callback (widget_class, switch_activate_prefer_dark_theme);
}

LxiGuiPrefs * lxi_gui_prefs_new (LxiGuiWindow *win)
{
    return g_object_new (LXI_GUI_PREFS_TYPE, "transient-for", win, NULL);
}
