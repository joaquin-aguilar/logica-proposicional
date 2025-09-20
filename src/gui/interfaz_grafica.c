#include "interfaz_grafica.h"

void al_activar(GtkApplication* programa, gpointer informacion_recuperada)
{
    GtkWidget *ventana = gtk_application_window_new(programa);

    gtk_window_set_default_size(GTK_WINDOW(ventana), 600, 300);

    gtk_window_set_title(GTK_WINDOW(ventana), "ventana de pruebas");

    gtk_window_present(GTK_WINDOW(ventana));
}