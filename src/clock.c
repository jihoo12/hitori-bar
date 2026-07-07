#include "clock.h"
#include <time.h>

static gboolean update_clock(gpointer user_data) {
    GtkWidget *label = GTK_WIDGET(user_data);
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%a %b %d  %H:%M", tm);
    gtk_label_set_text(GTK_LABEL(label), buf);
    return G_SOURCE_CONTINUE;
}

void clock_init(GtkWidget *label) {
    update_clock(label);
    g_timeout_add_seconds(60, update_clock, label);
}
