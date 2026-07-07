#include "volume.h"
#include <stdio.h>
#include <string.h>

bool volume_available(void) {
    return g_find_program_in_path("wpctl") != NULL;
}

static int get_volume(void) {
    char *output = NULL;
    GError *error = NULL;
    if (!g_spawn_command_line_sync("wpctl get-volume @DEFAULT_AUDIO_SINK@",
                                   &output, NULL, NULL, &error)) {
        g_warning("volume: %s", error->message);
        g_error_free(error);
        return -1;
    }

    int vol = -1;
    if (output) {
        float f;
        if (sscanf(output, "Volume: %f", &f) == 1)
            vol = (int)(f * 100.0f);
        g_free(output);
    }
    return vol;
}

static bool is_muted(void) {
    char *output = NULL;
    GError *error = NULL;
    if (!g_spawn_command_line_sync("wpctl get-volume @DEFAULT_AUDIO_SINK@",
                                   &output, NULL, NULL, &error)) {
        g_warning("volume muted check: %s", error->message);
        g_error_free(error);
        return false;
    }

    bool muted = false;
    if (output) {
        muted = (strstr(output, "MUTED") != NULL);
        g_free(output);
    }
    return muted;
}

static gboolean update_volume(gpointer user_data) {
    GtkWidget *label = GTK_WIDGET(user_data);
    int vol = get_volume();
    if (vol < 0) {
        gtk_label_set_text(GTK_LABEL(label), "");
        return G_SOURCE_CONTINUE;
    }

    if (is_muted()) {
        gtk_label_set_text(GTK_LABEL(label), "  Muted");
        return G_SOURCE_CONTINUE;
    }

    const char *icon;
    if (vol >= 70) icon = "";
    else if (vol >= 30) icon = "";
    else icon = "";

    char buf[32];
    snprintf(buf, sizeof(buf), "%s %d%%", icon, vol);
    gtk_label_set_text(GTK_LABEL(label), buf);
    return G_SOURCE_CONTINUE;
}

void volume_init(GtkWidget *label) {
    update_volume(label);
    g_timeout_add_seconds(5, update_volume, label);
}
