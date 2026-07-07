#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>

bool volume_available(void);
void volume_init(GtkWidget *label);
