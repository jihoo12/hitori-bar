#include "battery.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>

static char battery_path[256] = {0};

bool battery_find(void) {
    DIR *dir = opendir("/sys/class/power_supply");
    if (!dir) return false;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "BAT") == entry->d_name) {
            snprintf(battery_path, sizeof(battery_path),
                     "/sys/class/power_supply/%s", entry->d_name);
            closedir(dir);
            return true;
        }
    }
    closedir(dir);
    return false;
}

static int read_int(const char *file) {
    char path[300];
    snprintf(path, sizeof(path), "%s/%s", battery_path, file);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    int val = -1;
    if (fscanf(f, "%d", &val) != 1) val = -1;
    fclose(f);
    return val;
}

static gboolean update_battery(gpointer user_data) {
    GtkWidget *label = GTK_WIDGET(user_data);
    int capacity = read_int("capacity");
    if (capacity < 0) {
        gtk_label_set_text(GTK_LABEL(label), "");
        return G_SOURCE_CONTINUE;
    }

    int status = read_int("status");
    const char *icon;
    if (capacity >= 90) icon = "";
    else if (capacity >= 60) icon = "";
    else if (capacity >= 30) icon = "";
    else if (capacity >= 10) icon = "";
    else icon = "";

    if (status == 1)
        icon = "";

    char buf[32];
    snprintf(buf, sizeof(buf), "%s %d%%", icon, capacity);
    gtk_label_set_text(GTK_LABEL(label), buf);
    return G_SOURCE_CONTINUE;
}

void battery_init(GtkWidget *label) {
    update_battery(label);
    g_timeout_add_seconds(60, update_battery, label);
}
