#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>

bool battery_find(void);
void battery_init(GtkWidget *label);
