#include <gtk/gtk.h>
#include <gtk4-layer-shell.h>

#include "clock.h"
#include "battery.h"
#include "volume.h"

#define BAR_HEIGHT 36

static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        "window {"
        "  background-color: rgba(30, 30, 46, 0.95);"
        "  color: #cdd6f4;"
        "  font-size: 13px;"
        "  min-height: 36px;"
        "}"
        "box.bar-box {"
        "  padding: 0 12px;"
        "  min-height: 36px;"
        "}"
        "label.bar-label {"
        "  padding: 0 8px;"
        "  color: #cdd6f4;"
        "  font-weight: 500;"
        "}"
        "label.bar-clock {"
        "  padding: 0 8px;"
        "  font-weight: 700;"
        "  font-size: 13px;"
        "  color: #b4befe;"
        "}"
        "label.bar-battery {"
        "  padding: 0 8px;"
        "  color: #a6e3a1;"
        "}"
        "label.bar-volume {"
        "  padding: 0 8px;"
        "  color: #89b4fa;"
        "}";
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static void on_logo_clicked(GtkGestureClick *gesture, gint n_press,
                            gdouble x, gdouble y, gpointer user_data) {
    (void)gesture;
    (void)n_press;
    (void)x;
    (void)y;
    (void)user_data;
    GError *error = NULL;
    if (!g_spawn_command_line_async("hitori", &error)) {
        g_warning("launch hitori: %s", error->message);
        g_error_free(error);
    }
}

static GtkWidget *make_left_section(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(box, GTK_ALIGN_START);

    GtkWidget *icon = gtk_label_new("");
    gtk_widget_add_css_class(icon, "bar-label");
    gtk_widget_set_cursor_from_name(icon, "pointer");

    GtkGesture *click = gtk_gesture_click_new();
    g_signal_connect(click, "pressed", G_CALLBACK(on_logo_clicked), NULL);
    gtk_widget_add_controller(icon, GTK_EVENT_CONTROLLER(click));

    gtk_box_append(GTK_BOX(box), icon);

    return box;
}

static GtkWidget *make_center_section(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(box, TRUE);

    GtkWidget *clock_label = gtk_label_new("");
    gtk_widget_add_css_class(clock_label, "bar-clock");
    gtk_box_append(GTK_BOX(box), clock_label);
    clock_init(clock_label);

    return box;
}

static GtkWidget *make_right_section(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(box, GTK_ALIGN_END);

    if (battery_find()) {
        GtkWidget *battery_label = gtk_label_new("");
        gtk_widget_add_css_class(battery_label, "bar-battery");
        gtk_box_append(GTK_BOX(box), battery_label);
        battery_init(battery_label);
    }

    if (volume_available()) {
        GtkWidget *volume_label = gtk_label_new("");
        gtk_widget_add_css_class(volume_label, "bar-volume");
        gtk_box_append(GTK_BOX(box), volume_label);
        volume_init(volume_label);
    }

    return box;
}

static void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    load_css();

    GtkWidget *window = gtk_application_window_new(app);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), BAR_HEIGHT);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);

    gtk_widget_set_size_request(window, -1, BAR_HEIGHT);

    GtkWidget *bar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(bar_box, "bar-box");
    gtk_window_set_child(GTK_WINDOW(window), bar_box);

    GtkWidget *left = make_left_section();
    GtkWidget *center = make_center_section();
    GtkWidget *right = make_right_section();

    gtk_box_append(GTK_BOX(bar_box), left);
    gtk_box_append(GTK_BOX(bar_box), center);
    gtk_box_append(GTK_BOX(bar_box), right);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.hitori.bar",
                                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
